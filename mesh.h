#pragma once
#include"general.h"

#include"object.h"


START_NAMESPACE(NewRay)

struct Vertex
{
    
};

struct VertexPC:public Vertex
{
    MyGeo::Vec3f position;
    MyGeo::Vec3f color;
};



struct TriangleMesh: Object 
{
    std::vector<Triangle> triangles;
    virtual bool hit(const Ray& ray, float t0, float t1, std::optional<HitRecord>& rec) const override
    {
        return true;
    }


};




END_NAMESPACE