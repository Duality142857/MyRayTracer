#pragma once
#include"general.h"
#include"texture.h"
START_NAMESPACE(NewRay)

struct Material
{
    virtual bool scatter(const Ray& rayIn, const HitRecord& rec, MyGeo::Vec3f& attenuation, Ray& rayScatter) const =0;

    virtual MyGeo::Vec3f emitColor(float u, float v, const MyGeo::Vec3f& p) const 
    {
        return MyGeo::Vec3f{0,0,0};
    }
    
};

struct Lambertian: public Material
{
    std::shared_ptr<Texture> texture;
    Lambertian(std::shared_ptr<Texture> text):texture{text}
    {
    }
    virtual bool scatter(const Ray& rayIn, const HitRecord& rec, MyGeo::Vec3f& attenuation, Ray& rayScatter) const override
    {
        auto scatterDir=rec.normal+randomUnitVector<float>();
        if(scatterDir.nearZero()) scatterDir=rec.normal;
        rayScatter={rec.position,scatterDir.normalize()};
        attenuation=texture->getValue(rec.u,rec.v);
        return true;
    }
    
    virtual MyGeo::Vec3f emitColor(float u, float v, const MyGeo::Vec3f& p) const override
    {
        return 0.05f*texture->getValue(u,v);
    }
};


struct SimpleLightEmitMaterial : public Material
{
    std::shared_ptr<Texture> texture;
    SimpleLightEmitMaterial(std::shared_ptr<Texture> text):texture{text}{}
    virtual bool scatter(const Ray& rayIn, const HitRecord& rec, MyGeo::Vec3f& attenuation, Ray& rayOut) const override
    {
        return false;   
    }
    virtual MyGeo::Vec3f emitColor(float u, float v, const MyGeo::Vec3f& p) const override
    {
        return texture->getValue(u,v);
    }

};



END_NAMESPACE
