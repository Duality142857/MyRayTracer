#pragma once
#include<memory>
#include"general.h"
#include"material.h"

START_NAMESPACE(NewRay)

struct Object
{
    virtual bool hit(const Ray& ray, float t0, float t1, std::optional<HitRecord>& rec) const=0;
};

struct AABB : public Object
{
    MyGeo::Vec3f min,max;
    AABB(const MyGeo::Vec3f& min,const MyGeo::Vec3f& max): 
    min{min},max{max}
    {}

    virtual bool hit(const Ray& ray, float tmin, float tmax, std::optional<HitRecord>& rec) const override
    {
        for(int i=0;i!=3;++i)
        {
            auto recid=1.0f/ray.direction[i];

            auto t0=(min[i]-ray.source[i])*recid;
            auto t1=(max[i]-ray.source[i])*recid;

            if(recid<0) std::swap(t0,t1);
            
            if(t0>tmin) std::swap(t0,tmin);
            if(t1<tmax) std::swap(t1,tmax);
            if(tmin>=tmax) return false;
        }
        return true;
    }
    
    friend AABB getUnion(const AABB& b0,const AABB& b1)
    {
        return AABB{
            MyGeo::Vec3f{
                std::min(b0.min.x,b1.min.x),
                std::min(b0.min.y,b1.min.y),
                std::min(b0.min.z,b1.min.z)
                },
            MyGeo::Vec3f{
                std::max(b0.max.x,b1.max.x),
                std::max(b0.max.y,b1.max.y),
                std::max(b0.max.z,b1.max.z)
                }
        };
    }
};



struct ObjList: public Object
{
    std::vector<std::shared_ptr<Object>> objects;
    void add(std::shared_ptr<Object> obj)
    {
        objects.push_back(obj);
    }
    virtual bool hit(const Ray& ray, float t0, float t1, std::optional<HitRecord>& rec) const override
    {
        float curt=t1;
        bool hitflag=false;
        for(auto const& obj:objects)
        {
            if(obj->hit(ray,t0,curt,rec)==true)
            {
                hitflag=true;
                curt=t1;           
            }
        }
        return hitflag;
    }
};

struct Triangle: public Object
{
    MyGeo::Vec3f v0,v1,v2;
    std::shared_ptr<Material> material;
    Triangle(const MyGeo::Vec3f& v0,const MyGeo::Vec3f& v1,const MyGeo::Vec3f& v2,std::shared_ptr<Material> material):v0{v0},v1{v1},v2{v2}
    ,material{material}
    {}

    virtual bool hit(const Ray& ray, float t0, float t1, std::optional<HitRecord>& rec) const override
    {
        AABB bb=getBoundingBox();
        if(!bb.hit(ray,t0,t1,rec)) return false;
        MyGeo::Vec3f e1=v1-v0;
        MyGeo::Vec3f e2=v2-v0;
        MyGeo::Vec3f s=ray.source-v0;
        MyGeo::Vec3f s1=ray.direction.cross(e2);
        MyGeo::Vec3f s2=s.cross(e1);
        auto recidenom=1.0f/s1.dot(e1);
        auto t=s2.dot(e2)*recidenom;
        auto beta=s1.dot(s)*recidenom;
        auto gamma=s2.dot(ray.direction)*recidenom;
        if(t>t0 && t<t1 && beta>0 && gamma>0 && beta+gamma<1.0f)
        {
            if(!rec.has_value())
            {
                rec.emplace();
            }
            rec->t=t;
            rec->position=ray.at(t);
            rec->normal=e1.cross(e1);
            rec->material=material;
            rec->fixNormal(ray.direction);
            return true;
        }
        return false;
    }

    AABB getBoundingBox() const
    {
        return AABB{
            MyGeo::Vec3f{std::min({v0.x,v1.x,v2.x}),std::min({v0.y,v1.y,v2.y}),std::min({v0.z,v1.z,v2.z})},
            MyGeo::Vec3f{std::max({v0.x,v1.x,v2.x}),std::max({v0.y,v1.y,v2.y}),std::max({v0.z,v1.z,v2.z})}
        };
    }
};



// template<int X,int Y,int Z>
// struct Rect:public Object
// {
//     MyGeo::Vec2f min,max;
//     int taxis;
//     float k;
//     std::shared_ptr<Material> material;
//     Rect(){}
//     Rect(const MyGeo::Vec2f& min, const MyGeo::Vec2f& max, float k,std::shared_ptr<Material> material ):min{min},max{max},k{k},taxis{Z},material{material}
//     {
//     }
//     bool intersect(const Ray& ray, float tmin, float tmax, std::optional<HitRecord>& record) override
//     {
//         auto t=(k-ray.source[Z])/ray.direction[Z];
//         if(t<tmin || t> tmax) return false;
//         auto x=ray.source[X]+t*ray.direction[X];
//         auto y=ray.source[Y]+t*ray.direction[Y];
//         if(x<min.x || x>max.x || y<min.y || y>max.y) return false;
//         if(!record.has_value())
//         {
//             record.emplace();
//             record->t=std::numeric_limits<float>::max();
//         }
//         if(t>=record->t) return false;
//         record->u=(x-min.x)/(max.x-min.x);
//         record->v=(y-min.y)/(max.y-min.y);
//         record->t=t;
//     static MyGeo::Vec3f normalArray[3]={MyGeo::Vec3f{1,0,0},MyGeo::Vec3f{0,1,0},MyGeo::Vec3f{0,0,1}};
//         record->normal=normalArray[taxis];
//         if(record->normal.dot(ray.direction)>0) record->normal=-record->normal;
//         record->matPtr=matPtr;
//         record->position=ray.source+ray.direction*t;
//         return true;
//     }
// };


struct Sphere : Object
{
    MyGeo::Vec3f center;
    std::shared_ptr<Material> material;
    float radius,radius2;
    Sphere(const MyGeo::Vec3f& center,float r,std::shared_ptr<Material> material):center{center},radius{r},radius2{r*r},material{material}
    {}

    virtual bool hit(const Ray& ray, float t0, float t1, std::optional<HitRecord>& rec) const override
    {
        // std::optional<HitRecord> rec;
        AABB bb=getBoundingBox();
        if(!bb.hit(ray,t0,t1,rec)) return false;
    
        MyGeo::Vec3f l=ray.source-center;
        float a=ray.direction.norm2();
        float b=2*ray.direction.dot(l);
        float c=l.norm2()-radius2;
        auto solution=solveQuadratic(a,b,c);

        //不相交
        if(!solution.has_value()) return false;
        auto [x1,x2]=solution.value();

        if(x2<=t0) return false;
        auto t=x1<t0?x2:x1;
        if(t>t1) return false;
        if(!rec.has_value())
        {
            rec.emplace();
        }
        rec->t=t;
        rec->position=ray.at(t);
        rec->normal=(rec->position-center).normalize();
        rec->material=material;
        getUV(rec->position,rec->u,rec->v);
        rec->fixNormal(ray.direction);
        return true;
    }
    static void getUV(const MyGeo::Vec3f& p, float& u, float& v)
    {

    }
    AABB getBoundingBox() const
    {
        return AABB{center-MyGeo::Vec3f{radius,radius,radius},center+MyGeo::Vec3f{radius,radius,radius}};
    }

};



END_NAMESPACE