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
#include"mesh.h"
using namespace NewRay;

template<class T>
static T instanceRotate(T obj, const MyGeo::Vec3f& axis, const float& angle)
{
    return std::make_shared<InstanceRotation>(obj,axis,angle);
}
template<class T>
static T instanceTranslate(T obj, const MyGeo::Vec3f& displacement)
{
    return std::make_shared<InstanceTranslation>(obj,displacement);
}
auto lightColor=8.0f * MyGeo::Vec3f{0.747f+0.058f, 0.747f+0.258f, 0.747f} + 15.6f *  MyGeo::Vec3f{0.740f+0.287f,0.740f+0.160f,0.740f} + 18.4f * MyGeo::Vec3f{0.737f+0.642f,0.737f+0.159f,0.737f};

auto red=std::make_shared<Lambertian>(std::make_shared<SolidColor>(MyGeo::Vec3f{0.65,0.05,0.05}));
auto white=std::make_shared<Lambertian>(std::make_shared<SolidColor>(MyGeo::Vec3f{.73, .73, .73}));
auto green=std::make_shared<Lambertian>(std::make_shared<SolidColor>(MyGeo::Vec3f{.12, .45, .15}));
auto light=std::make_shared<SimpleLightEmitMaterial>(std::make_shared<SolidColor>(lightColor)); 


void cornellBox(int argc, char** argv)
{
    auto argmap=parseArgs(argc,argv);
    for(const auto& kv:argmap)
    {
        std::cout<<kv.first<<"->"<<kv.second<<std::endl;
    }
    int width=800,height=600,spp=16,raydepth=32,fov=25;
    int ifbvh=1;
    GetIntVar(width);
    GetIntVar(height);
    GetIntVar(spp);
    GetIntVar(raydepth);
    GetIntVar(ifbvh);
    GetIntVar(fov);

    NewRay::Canvas canvas(width,height);
    NewRay::Camera cam(MyGeo::Vec3f{278,278,-800},MyGeo::Vec3f{278,278,0},MyGeo::Vec3f{0,1,0},(float)width/height,fov,0.1f,10);
    canvas.camera=std::make_shared<NewRay::Camera>(cam);

    NewRay::ObjList objs;
    // std::shared_ptr<Object> rect1=std::make_shared<Rect>(MyGeo::Vec3f{300,0,0},MyGeo::Vec3f{0,300,0},white);

objs.add(std::make_shared<Rect>(MyGeo::Vec3f{0,275.5,275.5},MyGeo::Vec3f{0,555,0},MyGeo::Vec3f{0,0,555},green));

objs.add(std::make_shared<Rect>(MyGeo::Vec3f{555,275.5,275.5},MyGeo::Vec3f{0,555,0},MyGeo::Vec3f{0,0,555},red));

objs.add(std::make_shared<Rect>(MyGeo::Vec3f{275.5,0,275.5},MyGeo::Vec3f{555,0,0},MyGeo::Vec3f{0,0,555},white));

objs.add(std::make_shared<Rect>(MyGeo::Vec3f{275.5,555,275.5},MyGeo::Vec3f{555,0,0},MyGeo::Vec3f{0,0,555},white));

objs.add(std::make_shared<Rect>(MyGeo::Vec3f{275.5,275.5,555},MyGeo::Vec3f{555,0,0},MyGeo::Vec3f{0,555,0},white));

std::shared_ptr<Object> bigbox=std::make_shared<Cube>(white,MyGeo::Vec3f{82.5,165,82.5});//265,0,295
bigbox=instanceRotate(bigbox,MyGeo::Vec3f{0,1,0},15);
bigbox=instanceTranslate(bigbox,MyGeo::Vec3f{347.5,165,377.5});


std::shared_ptr<Object> smallbox=std::make_shared<Cube>(white,MyGeo::Vec3f{82.5,82.5,82.5});
smallbox=instanceRotate(smallbox,MyGeo::Vec3f{0,1,0},-18);
smallbox=instanceTranslate(smallbox,MyGeo::Vec3f{212.5,82.5,147.5});
objs.add(std::make_shared<Rect>(MyGeo::Vec3f{278,554,279.5},MyGeo::Vec3f{130,0,0},MyGeo::Vec3f{0,0,105},light));
objs.add(smallbox);
objs.add(bigbox);




// objs.add(std::make_shared<Rect>(MyGeo::Vec3f{0,0,0},MyGeo::Vec3f{0,555,555},green));
// objs.add(std::make_shared<Rect>(MyGeo::Vec3f{555,0,0},MyGeo::Vec3f{555,555,555},red));


// objs.add(std::make_shared<Rect>(MyGeo::Vec3f{213,554,227},MyGeo::Vec3f{343,554,332},light));


// objs.add(std::make_shared<Rect>(MyGeo::Vec3f{0,0,0},MyGeo::Vec3f{555,0,555},white));
// objs.add(std::make_shared<Rect>(MyGeo::Vec3f{0,555,0},MyGeo::Vec3f{555,555,555},white));
// objs.add(std::make_shared<Rect>(MyGeo::Vec3f{0,0,555},MyGeo::Vec3f{555,555,555},white));
    canvas.render(objs,spp,raydepth);return;

    
    WindowContext sdlcontext(width,height,"ray tracer");
    float deltax,deltay,deltaz;
    auto k=objs.objects.size()-1;
    for(int i=0;i!=30;i++)
    {
        deltax=std::sin(float(i)*0.3)*100;
        deltay=std::cos(float(i)*0.5)*100;
        deltaz=deltax+deltay;
        // objs.objects[0] =std::make_shared<InstanceTranslation>(objs.objects[0],MyGeo::Vec3f{0.5f*deltay,0.5f*deltaz,1.0f*deltax});
        objs.objects[k-1]=instanceRotate(smallbox,MyGeo::Vec3f{0,1,0},i*10);
        // objs.objects[1]=instanceTranslate(objs.objects[1],{200,200,200});

        sdlcontext.rendererClear();
        canvas.renderSDL(objs,spp,raydepth,sdlcontext);
        sdlcontext.renderPresent();
        // sdlcontext.sleep(1000);
    }


   


}

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
    std::shared_ptr<Object> lightsphere2=std::make_shared<Sphere>(MyGeo::Vec3f{500, 500, 500},50,light2);


    objs.add(lightsphere1);
    std::shared_ptr<Object> sphere1=std::make_shared<Sphere>(MyGeo::Vec3f{300,300,300},100,white);
    std::shared_ptr<Object> sphere2=std::make_shared<Sphere>(MyGeo::Vec3f{200,0,300},150,white);

    std::shared_ptr<Object> cube0=std::make_shared<Cube>(white,MyGeo::Vec3f{1,1,1});
    // cube0=instanceRotate(cube0,MyGeo::Vec3f{0,1,0},60);
    // cube0=instanceTranslate(cube0,{200,200,200});
    std::shared_ptr<Object> rect1=std::make_shared<Rect>(MyGeo::Vec3f{300,0,0},MyGeo::Vec3f{300,0,0},MyGeo::Vec3f{0,300,0},white);
    // rect1=instanceRotate(rect1,MyGeo::Vec3f{0,1,0},45);
    // rect1=instanceTranslate(rect1,MyGeo::Vec3f{300,300,300});
    

    objs.add(cube0);
    objs.add(lightsphere2);
    // objs.add(rect1);

    // objs.add(std::make_shared<Sphere>(MyGeo::Vec3f{0, 0, 0},50,light1));
    // objs.add(std::make_shared<Sphere>(MyGeo::Vec3f{500, 500, 500},50,light2));
    // objs.add(std::make_shared<Sphere>(MyGeo::Vec3f{300,300,300},100,white));
    // objs.add(sphere1);
    // objs.add(sphere2);
    // objs.add(std::make_shared<Triangle>(MyGeo::Vec3f{100,100,100},MyGeo::Vec3f{200,300,400},MyGeo::Vec3f{500,200,150},white));
    
    
    // canvas.render(objs,spp,raydepth);return;

    //real time

    WindowContext sdlcontext(width,height,"ray tracer");
    float deltax=0,deltay=0,deltaz=0;

    

    for(int i=0;i!=30;i++)
    {
        deltax=std::sin(float(i)*0.3)*100;
        deltay=std::cos(float(i)*0.5)*100;
        deltaz=deltax+deltay;
        // objs.objects[0] =std::make_shared<InstanceTranslation>(objs.objects[0],MyGeo::Vec3f{0.5f*deltay,0.5f*deltaz,1.0f*deltax});
    objs.objects[1]=instanceRotate(cube0,MyGeo::Vec3f{0,1,0},i*10);
    objs.objects[1]=instanceTranslate(objs.objects[1],{200,200,200});
    // rect1=instanceRotate(rect1,MyGeo::Vec3f{0,1,0},45);
    // rect1=instanceTranslate(rect1,MyGeo::Vec3f{300,300,300});

    objs.objects[2]=instanceRotate(rect1,MyGeo::Vec3f{0,1,0},i*30);
    objs.objects[2]=instanceTranslate(objs.objects[2],MyGeo::Vec3f{300,300,300});

        // objs.objects[1] =std::make_shared<InstanceTranslation>(objs.objects[1],MyGeo::Vec3f{0.5f*deltaz,0.5f*deltax,1.0f*deltay});
        // objs.objects[2] =std::make_shared<InstanceTranslation>(objs.objects[2],MyGeo::Vec3f{0.5f*deltax,0.5f*deltay,1.0f*deltaz});
        // objs.objects[2] =std::make_shared<InstanceTranslation>(objs.objects[2],MyGeo::Vec3f{2.0f*deltax,2.0f*deltay,2.0f*deltaz});
        // objs.objects[3] =std::make_shared<InstanceTranslation>(objs.objects[3],MyGeo::Vec3f{2.0f*deltay,2.0f*deltaz,2.0f*deltax});
        // objs.objects[3] =std::make_shared<InstanceTranslation>(objs.objects[3],MyGeo::Vec3f{2.0f*deltay,2.0f*deltaz,2.0f*deltax});

        sdlcontext.rendererClear();
        canvas.renderSDL(objs,spp,raydepth,sdlcontext);
        sdlcontext.renderPresent();
        // sdlcontext.sleep(1000);
    }
}


int main(int argc, char** argv) 
{
    // auto light1=std::make_shared<SimpleLightEmitMaterial>(std::make_shared<SolidColor>(MyGeo::Vec3f{0,40,50}));
    
    // Cube cube{light1,100.f};
    // cube.print();
    // exit(0);


    auto t0=mytime::now();
    // testScene(argc,argv);
    cornellBox(argc,argv);

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
