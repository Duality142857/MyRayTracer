#pragma once
#include"general.h"
#include"texture.h"
START_NAMESPACE(NewRay)

struct Material
{
    virtual std::optional<MyGeo::Vec3f> getColor(float u,float v) const 
    {
        return std::nullopt;
    }
    virtual MyGeo::Vec3f getKd() const 
    {
        return MyGeo::Vec3f{0.65,0.65,0.65};
    }
    // static const bool emitflag=false;
    virtual bool ifemit() const {return false;}

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

    std::optional<MyGeo::Vec3f> getColor(float u,float v) const override
    {
        return std::make_optional(texture->getValue(u,v));
    }

    virtual bool scatter(const Ray& rayIn, const HitRecord& rec, MyGeo::Vec3f& attenuation, Ray& rayScatter) const override
    {
        auto scatterDir=rec.normal+randomUnitVector<float>();
        if(scatterDir.nearZero()) scatterDir=rec.normal;
        rayScatter={rec.position,scatterDir.normalize()};
        attenuation=texture->getValue(rec.u,rec.v);
        return true;
    }
    virtual MyGeo::Vec3f getKd() const override
    {
        return texture->getValue(0,0);
    }
    virtual MyGeo::Vec3f emitColor(float u, float v, const MyGeo::Vec3f& p) const override
    {
        return 0.05f*texture->getValue(u,v);
    }
};


struct SimpleLightEmitMaterial : public Material
{
    std::shared_ptr<Texture> texture;
    // static const bool emitflag=true;
    virtual bool ifemit() const override {return true;}
    virtual MyGeo::Vec3f getKd() const override
    {
        return texture->kd;
    }
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
