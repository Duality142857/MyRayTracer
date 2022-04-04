#pragma once 
#include<mygeo/vec.h>
#include<vector>
#include<array>
#include<map>
#include<assert.h>
#include<memory>
#include<optional>
#include<math.h>
#include"myrandom.h"

#define START_NAMESPACE(name) namespace name {
#define END_NAMESPACE }
static constexpr float PI=3.14159;
static constexpr float RECI_PI=1.0/PI;

START_NAMESPACE(NewRay)
static void msg(const std::string& msg)
{
    std::cerr<<msg<<std::endl;
}

struct Material;

template<class T>
static T clamp(T min, T max, T x)
{
    return std::max(min,std::min(max,x));
}

float deg2rad(float deg)
{
    return deg*PI/180.f;
}

static std::optional<std::tuple<float,float>> solveQuadratic(const float& a, const float& b, const float& c)
{
    float d=b*b-4*a*c;
    if(d<0) return std::nullopt;
    float recidenom=0.5f/a;
    float m=-b*recidenom;
    float n=std::sqrt(d)*recidenom;
    float x1=m-n,x2=m+n;
    if(x1>x2) std::swap(x1,x2);
    return std::make_optional(std::make_tuple(x1,x2));
}

static inline MyGeo::Vec3f reflect(const MyGeo::Vec3f& l, const MyGeo::Vec3f& n)
{
    return l-n*l.dot(n)*2.f;
}

static MyGeo::Vec3f refract(const MyGeo::Vec3f& l, const MyGeo::Vec3f& N, const float& ior1,const float& ior2)
{
    auto n=N;
    float cos=l.dot(n);
    float etai=ior1,etat=ior2;
    if(cos<0) cos=-cos;
    else {std::swap(etai,etat);n=-n;}
    float eta=etai/etat;
    float k=1-eta*eta*(1-cos*cos);
    return k<0?MyGeo::Vec3f{0,0,0}:l*eta+n*(eta*cos-sqrtf(k));
}

static float fresnel(const MyGeo::Vec3f& l, const MyGeo::Vec3f& N,const float& ior1,const float& ior2)
{
    float cosi=l.dot(N);
    float etai=ior1,etat=ior2;
    if(cosi>0)
    {
        std::swap(etai,etat);
    }
    float sint=etai/etat*sqrtf(std::max(0.f,1-cosi*cosi));

    if(sint>=1) return 1.0f;
    float cost=sqrtf(std::max(0.f,1-sint*sint));
    cosi=fabsf(cosi);
    float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
    float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
    return (Rs * Rs + Rp * Rp) / 2;
}

/**
 * should normalize direction beforehand
 */ 
struct Ray
{
    MyGeo::Vec3f source;
    MyGeo::Vec3f direction;
    MyGeo::Vec3f at(float t) const
    {
        return source+t*direction;
    }
};



struct HitRecord
{
    MyGeo::Vec3f position;
    MyGeo::Vec3f normal
    ;
    float t;    
    float u,v;
    std::shared_ptr<Material> material;
    void fixNormal(const MyGeo::Vec3f& raydirection)
    {
        if(normal.dot(raydirection)>0) normal=-normal;
    }    
};

END_NAMESPACE
