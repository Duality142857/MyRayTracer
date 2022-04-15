#pragma once
#include"object.h"
#include<mygeo/mat.h>
#include<mygeo/geo.h>

START_NAMESPACE(NewRay)

struct InstanceTranslation : public Object
{
    std::shared_ptr<Object> obj;
    MyGeo::Vec3f offset;
    InstanceTranslation(std::shared_ptr<Object> obj, const MyGeo::Vec3f& offset):obj{obj},offset{offset}
    {}
    virtual bool hit(const Ray& ray, float t0, float t1, std::optional<HitRecord>& rec) const override
    {
        Ray translateRay{ray.source-offset,ray.direction};
        auto hitflag=obj->hit(translateRay,t0,t1,rec);
        if(hitflag) 
        {
            rec->position+=offset;
            rec->fixNormal(translateRay.direction);
        }
        return hitflag;
    }    
};

struct InstanceRotation : public Object
{
    std::shared_ptr<Object> obj;
    MyGeo::Mat3f rotation;
    MyGeo::Mat3f reverseRotation;
    AABB bbox;
    InstanceRotation(std::shared_ptr<Object> p, const MyGeo::Vec3f& rotAxis, const float& angle): obj{p},rotation{MyGeo::rotationMatrix3f(rotAxis,angle)},reverseRotation{rotation.transpose()}
    {
        // obj->boundingBox(0,1,bbox);
        MyGeo::Vec3f min{
                std::numeric_limits<float>::infinity(),
                std::numeric_limits<float>::infinity(),
                std::numeric_limits<float>::infinity()
                };
        MyGeo::Vec3f max{
                -std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity()
                };
        for(int i=0;i!=2;++i)
        for(int j=0;j!=2;++j)
        for(int k=0;k!=2;++k)
        {
            // std::cout<<i<<j<<k<<std::endl;
            MyGeo::Vec3f vert{
                i*bbox.max.x+(1-i)*bbox.min.x,
                j*bbox.max.y+(1-j)*bbox.min.y,
                k*bbox.max.z+(1-k)*bbox.min.z};
            
            vert=rotation*vert;
            for(int n=0;n!=3;++n)
            {
                min[n]=std::min(min[n],vert[n]);
                max[n]=std::max(max[n],vert[n]);
            }
        }
        // bbox=AABB{min,max};
    }
    virtual bool hit(const Ray& ray, float t0, float t1, std::optional<HitRecord>& rec) const override
    {
        Ray newray{reverseRotation*ray.source,reverseRotation*ray.direction};
        if(!obj->hit(newray,t0,t1,rec)) return false;

        rec->position=rotation*rec->position;
        rec->normal=rotation*rec->normal;
        rec->fixNormal(ray.direction);
        return true;
    }
};


END_NAMESPACE
