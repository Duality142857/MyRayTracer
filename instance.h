#pragma once
#include"object.h"

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
        if(hitflag) rec->position+=offset;
        return hitflag;
    }    

};




END_NAMESPACE
