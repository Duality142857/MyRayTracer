#include <iostream>
#include<type_traits>
#include<functional>
#include"object.h"
#include"general.h"
#include"canvas.h"
#include<memory>
#include<optional>
#include"util/argparser.h"
#include"myrandom.h"
#include"instance.h"
#include"mytime.h"
using namespace NewRay;
void testScene(int argc, char** argv)
{
    auto argmap=parseArgs(argc,argv);
    for(const auto& kv:argmap)
    {
        std::cout<<kv.first<<"->"<<kv.second<<std::endl;
    }
    int width=800,height=600,spp=16,raydepth=32;
    int ifbvh=1;
    GetIntVar(width);
    GetIntVar(height);
    GetIntVar(spp);
    GetIntVar(raydepth);
    GetIntVar(ifbvh);

    NewRay::Canvas canvas(width,height);
    // Camera(
    //     const MyGeo::Vec3f& position,
    //     const MyGeo::Vec3f& lookat,
    //     const MyGeo::Vec3f& up,
    //     float focuslen,
    //     float vfov,
    //     float aperture
    //     )
    NewRay::Camera cam(MyGeo::Vec3f{278,278,-800},MyGeo::Vec3f{278,278,0},MyGeo::Vec3f{0,1,0},4.0f/3.0f,45,0.1f,10);
    canvas.camera=std::make_shared<NewRay::Camera>(cam);

    NewRay::ObjList objs;
    auto light1=std::make_shared<SimpleLightEmitMaterial>(std::make_shared<SolidColor>(MyGeo::Vec3f{0,40,50}));
    auto light2=std::make_shared<SimpleLightEmitMaterial>(std::make_shared<SolidColor>(MyGeo::Vec3f{50,40,0}));

    auto white=std::make_shared<Lambertian>(std::make_shared<SolidColor>(MyGeo::Vec3f{1,1,1}));
    auto blue=std::make_shared<Lambertian>(std::make_shared<SolidColor>(MyGeo::Vec3f{0,0,1}));


    std::shared_ptr<Object> lightsphere1=std::make_shared<Sphere>(MyGeo::Vec3f{0, 0, 0},50,light1);
    // std::shared_ptr<Object> lightsphere2=std::make_shared<Sphere>(MyGeo::Vec3f{100, 100, 100},50,light2);



    objs.add(lightsphere1);
    // objs.add(lightsphere2);
    std::shared_ptr<Object> sphere1=std::make_shared<Sphere>(MyGeo::Vec3f{300,300,300},100,white);
    std::shared_ptr<Object> sphere2=std::make_shared<Sphere>(MyGeo::Vec3f{200,0,300},150,white);


    // objs.add(std::make_shared<Sphere>(MyGeo::Vec3f{0, 0, 0},50,light1));
    objs.add(std::make_shared<Sphere>(MyGeo::Vec3f{500, 500, 500},50,light2));
    // objs.add(std::make_shared<Sphere>(MyGeo::Vec3f{300,300,300},100,white));
    objs.add(sphere1);
    objs.add(sphere2);
    objs.add(std::make_shared<Triangle>(MyGeo::Vec3f{100,100,100},MyGeo::Vec3f{200,300,400},MyGeo::Vec3f{500,200,150},white));
    
    // canvas.render(objs,spp,raydepth);return;

    //real time

    WindowContext sdlcontext(width,height,"ray tracer");
    float deltax=0,deltay=0,deltaz=0;

    

    for(int i=0;i!=30;i++)
    {
        deltax=std::sin(float(i)*0.3)*100;
        deltay=std::cos(float(i)*0.5)*100;
        deltaz=deltax+deltay;
        objs.objects[0] =std::make_shared<InstanceTranslation>(objs.objects[0],MyGeo::Vec3f{0.5f*deltay,0.5f*deltaz,1.0f*deltax});
        objs.objects[1] =std::make_shared<InstanceTranslation>(objs.objects[1],MyGeo::Vec3f{0.5f*deltaz,0.5f*deltax,1.0f*deltay});
        // objs.objects[2] =std::make_shared<InstanceTranslation>(objs.objects[2],MyGeo::Vec3f{0.5f*deltax,0.5f*deltay,1.0f*deltaz});
        objs.objects[2] =std::make_shared<InstanceTranslation>(objs.objects[2],MyGeo::Vec3f{2.0f*deltax,2.0f*deltay,2.0f*deltaz});
        // objs.objects[3] =std::make_shared<InstanceTranslation>(objs.objects[3],MyGeo::Vec3f{2.0f*deltay,2.0f*deltaz,2.0f*deltax});
        objs.objects[4] =std::make_shared<InstanceTranslation>(objs.objects[4],MyGeo::Vec3f{2.0f*deltay,2.0f*deltaz,2.0f*deltax});

        sdlcontext.rendererClear();
        canvas.renderSDL(objs,spp,raydepth,sdlcontext);
        sdlcontext.renderPresent();
        // sdlcontext.sleep(1000);
    }
}



int main(int argc, char** argv) 
{
    auto t0=mytime::now();
    testScene(argc,argv);
    auto t1=mytime::now();
    std::cout<<mytime::getDuration(t0,t1)<<" ms"<<std::endl;
    exit(0);

}





// void typeTest()
// {
//     using namespace NewRay;
//         // std::result_of<Func(int,double)>::type a;
//     // std::cout<<sizeof(a)<<std::endl;
//     // std::cout<<typeid(a).name()<<std::endl;
//     Sphere o1;
//     Circle o2;
//     std::vector<std::shared_ptr<Object>> objs;
//     objs.push_back(std::make_shared<Sphere>());
//     objs.push_back(std::make_shared<Circle>());

//     auto flag=typeid(objs[0])==typeid(objs[1]);
//     std::cout<<flag<<std::endl;exit(0);

//     // std::cout<<std::is_same_v<decltype(objs[0]),decltype(objs[1])><<std::endl;
//     std::cout<<__func__<<std::endl;
//     // std::cout<<____PRETTY_FUNCTION__<<std::endl;
// }
