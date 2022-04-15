#pragma once
#include"general.h"

START_NAMESPACE(NewRay)

struct Texture
{
    virtual MyGeo::Vec3f getValue(float u,float v) const=0;
    MyGeo::Vec3f kd{0.65f,0.65f,0.65f};
};

struct SolidColor : public Texture
{
    SolidColor(const MyGeo::Vec3f& color):color{color}{}
    MyGeo::Vec3f color;
    // MyGeo::Vec3f kd{0.65f,0.65f,0.65f};
    MyGeo::Vec3f getValue(float u, float v) const override
    {
        return color;
    }
};



END_NAMESPACE