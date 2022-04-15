#pragma once
#include"general.h"

#include"object.h"


START_NAMESPACE(NewRay)

struct Vertex
{
    MyGeo::Vec3f position;
};

struct VertexC:public Vertex
{
    MyGeo::Vec3f color;
};

struct VertexNT:public Vertex
{
    MyGeo::Vec3f normal;
    float u,v;
    void print()
    {
        std::cout<<"position "<<position<<"  normal "<<normal<<std::endl;
    }
};




struct TriangleMesh: Object 
{
    std::vector<Triangle> triangles;
    // virtual bool hit(const Ray& ray, float t0, float t1, std::optional<HitRecord>& rec) const override
    // {
    //     return true;
    // }
};

template<class T>
static void vecAppend(std::vector<T>& v,std::initializer_list<T> il)
{
    for(const auto& x:il)
    {
        v.emplace_back(x);
    }
}


struct Rect: TriangleMesh
{
    std::shared_ptr<Material> material;
    std::vector<VertexNT> vertices;
    std::vector<uint32_t> indices;
    std::vector<Triangle> triangles;
    MyGeo::Vec3f up,left,center;
    float area;

    void sample(HitRecord& sampleRec) const override
    {
        auto x=getRand(0.f,1.f);
        int ind=0;
        if(x>0.5) ind=1;
        triangles[ind].sample(sampleRec);
    }
    virtual bool ifemit() const {return material->ifemit();}
    Rect(const MyGeo::Vec3f& center,const MyGeo::Vec3f& up, const MyGeo::Vec3f& left,std::shared_ptr<Material> material):
    center{center},up{up},left{left},material{material}
    {
        int ind=0;
        auto p=0.5*(up+left),q=0.5*(up-left);
        auto normal=p.cross(q).normalize();
        vecAppend(vertices,{{center-p,normal,0,0},{center+q,normal,0,0},{center-q,normal,0,0},{center+p,normal,0,0}});
        vecAppend(indices,{0u,1u,2u,2u,1u,3u});
        for(int i=0;i!=indices.size();i+=3)
        {
            triangles.push_back({vertices[indices[i]].position,vertices[indices[i+1]].position,vertices[indices[i+2]].position,vertices[indices[i]].normal,material});
        }
        area=up.cross(left).norm();
        std::cout<<"area "<<area<<std::endl;
    }
    virtual std::optional<float> getArea() const override
    {
        return std::make_optional(area);
    }
    virtual bool hit(const Ray& ray, float t0, float t1, std::optional<HitRecord>& rec) const override
    {
        float curt=t1;
        bool hitflag=false;
        for(auto const& triangle:triangles)
        {
            if(triangle.hit(ray,t0,curt,rec)==true)
            {
                hitflag=true;
                curt=rec->t;
            }
        }
        return hitflag;
    }

};




struct Cube: TriangleMesh
{
    std::shared_ptr<Material> material;
    std::vector<VertexNT> vertices;
    std::vector<uint32_t> indices;
    std::vector<Triangle> triangles;
    // float scale;
    MyGeo::Vec3f scale;
    virtual bool hit(const Ray& ray, float t0, float t1, std::optional<HitRecord>& rec) const override
    {
        float curt=t1;
        bool hitflag=false;
        for(auto const& triangle:triangles)
        {
            if(triangle.hit(ray,t0,curt,rec)==true)
            {
                hitflag=true;
                curt=rec->t;
            }
        }
        return hitflag;
    }

    AABB getBoundingBox() const 
    {
        return AABB{MyGeo::Vec3f{-1,-1,-1},MyGeo::Vec3f{1,1,1}};
    }

    Cube(std::shared_ptr<Material> material,MyGeo::Vec3f scale):material{material},scale{scale}
    {
        uint32_t curstart=0;
        //x=-1,1 faces
        for(int k=-1;k!=3;k+=2)
        {
            MyGeo::Vec3f n={(float)k,0,0};
            for(int i=-1;i!=3;i+=2)
            for(int j=-1;j!=3;j+=2)
            {
                vertices.emplace_back(VertexNT{MyGeo::Vec3f{(float)k*scale.x,(float)i*scale.y,(float)j*scale.z},n,0,0});
            }
        }
        vecAppend<uint32_t>(indices,{curstart,curstart+1,curstart+2});//12为公共边，所以刚好反过来
        vecAppend<uint32_t>(indices,{curstart+2,curstart+1,curstart+3});
        curstart+=4;
        vecAppend<uint32_t>(indices,{curstart,curstart+1,curstart+2});//12为公共边，所以刚好反过来
        vecAppend<uint32_t>(indices,{curstart+2,curstart+1,curstart+3});
        curstart+=4;
        //y=-1,1 faces
        for(int k=-1;k!=3;k+=2)
        {
            MyGeo::Vec3f n={0,(float)k,0};
            for(int i=-1;i!=3;i+=2)
            for(int j=-1;j!=3;j+=2)
            {
                vertices.emplace_back(VertexNT{MyGeo::Vec3f{(float)i*scale.x,(float)k*scale.y,(float)j*scale.z},n,0,0});
            }
        }
        vecAppend<uint32_t>(indices,{curstart,curstart+1,curstart+2});//12为公共边，所以刚好反过来
        vecAppend<uint32_t>(indices,{curstart+2,curstart+1,curstart+3});
        curstart+=4;
        vecAppend<uint32_t>(indices,{curstart,curstart+1,curstart+2});//12为公共边，所以刚好反过来
        vecAppend<uint32_t>(indices,{curstart+2,curstart+1,curstart+3});
        curstart+=4;

        //z=-1,1 faces
        for(int k=-1;k!=3;k+=2)
        {
            MyGeo::Vec3f n={0,0,(float)k};
            for(int i=-1;i!=3;i+=2)
            for(int j=-1;j!=3;j+=2)
            {
                vertices.emplace_back(VertexNT{MyGeo::Vec3f{(float)i*scale.x,(float)j*scale.y,(float)k*scale.z},n,0,0});
            }
        }
        vecAppend<uint32_t>(indices,{curstart,curstart+1,curstart+2});//12为公共边，所以刚好反过来
        vecAppend<uint32_t>(indices,{curstart+2,curstart+1,curstart+3});
        curstart+=4;
        vecAppend<uint32_t>(indices,{curstart,curstart+1,curstart+2});//12为公共边，所以刚好反过来
        vecAppend<uint32_t>(indices,{curstart+2,curstart+1,curstart+3});
        curstart+=4;
        std::cout<<"numtriangles: "<<indices.size()/3<<std::endl;
        print();
        for(int i=0;i!=indices.size();i+=3)
        {
            triangles.push_back({vertices[indices[i]].position,vertices[indices[i+1]].position,vertices[indices[i+2]].position,vertices[indices[i]].normal,material});
        }
    }

    void print()
    {
        for(auto& v:vertices)
        {
            v.print();
        }

        for(int i=0;i!=indices.size()/3;++i)
        {
            std::cout<<indices[3*i]<<','<<indices[3*i+1]<<','<<indices[3*i+2]<<std::endl;
        }
    }


};





END_NAMESPACE