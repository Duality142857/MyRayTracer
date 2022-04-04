#pragma once
#include"general.h"
#include"object.h"
START_NAMESPACE(NewRay)

struct Scene
{
    // std::vector<std::shared_ptr<Object>> objects;
    std::shared_ptr<ObjList> objlist;
};



END_NAMESPACE