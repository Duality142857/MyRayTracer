#pragma once
#include"general.h"
#include"object.h"
#include"scene.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include"sdlcontext.h"

START_NAMESPACE(NewRay)


struct Camera
{
    MyGeo::Vec3f position;
    float focuslen;
    float vfov;
    float aspectRatio;
    // float aperture;
    float lensRadius;
    MyGeo::Vec3f xaxis,yaxis,zaxis;
    MyGeo::Vec3f TopLeft;
    MyGeo::Vec3f Left,Up;

    Camera(
        const MyGeo::Vec3f& position,
        const MyGeo::Vec3f& lookat,
        const MyGeo::Vec3f& up,
        float aspectRatio,
        float vfov,
        float aperture,
        float focuslen
        )
    : position{position},zaxis{(position-lookat).normalize()},focuslen{focuslen},vfov{vfov},aspectRatio{aspectRatio},lensRadius{aperture*0.5}
    {
        xaxis=up.cross(zaxis).normalize();
        yaxis=zaxis.cross(xaxis);
        
        float theta=0.5*deg2rad(vfov);
        float tanTheta=std::tan(theta);
        float h=2*tanTheta;//*focuslen;
        float w=h*aspectRatio;

        Left=focuslen*w*xaxis;
        Up=focuslen*h*yaxis;
        TopLeft=position-Left*0.5+Up*0.5-zaxis*focuslen;
        printvec(Left,"left");
        printvec(Up,"up");
        printvec(TopLeft,"topleft");
        

    }
    static void printvec(const MyGeo::Vec3f& v,const std::string& msg)
    {
        std::cout<<msg<<": "<<v<<std::endl;
    }

    Ray getRay(float u, float v)
    {
        auto randomVecInLens=lensRadius*randomInUnitDisk<float>();
        auto offset=xaxis*randomVecInLens.x+yaxis*randomVecInLens.y;
        return {position+offset,(TopLeft+u*Left-v*Up-position-offset).normalize()};
    }
};



struct Canvas
{
    int width,height;
    std::vector<MyGeo::Vec3c> framebuffer;
    std::shared_ptr<Camera> camera;
    MyGeo::Vec3f backgroundColor{0,0,0};

    Canvas(int width,int height):width{width},height{height}
    {
        framebuffer.resize(width*height);
    }
    void setCamera(std::shared_ptr<Camera> cam)
    {
        camera=cam;
    }
    void setPixel(int i, int j, const MyGeo::Vec3f& color)
    {
        framebuffer[j*width+i]=
        MyGeo::Vec3c{(uint8_t)(255*clamp(0.f,0.999f,color.x)),
        (uint8_t)(255*clamp(0.f,0.999f,color.y)),
        (uint8_t)(255*clamp(0.f,0.999f,color.z))};
    }

    void render(ObjList objList,int spp,int maxdepth)
    {
        
        float reciWidth=1.0f/width;
        float reciHeight=1.0f/height;

        for(int j=0;j!=height;++j)
        {
            std::cerr<<"\rFinished "<<j<<" lines"<<std::flush;
#pragma omp parallel for
            for(int i=0;i!=width;++i)
            {
                MyGeo::Vec3f pixelColor{0,0,0};
                for(int k=0;k!=spp;k++)
                {
                    auto u=(i+getRand(0.f,1.f))*reciWidth;
                    auto v=(height-j-1+getRand(0.f,1.f))*reciHeight;
                    auto ray=camera->getRay(u,v);
                    pixelColor+=trace(ray,0.001f,std::numeric_limits<float>::max(),objList,0,maxdepth);
                }

                pixelColor/=(float)spp;
                setPixel(i,j,pixelColor);
            }
        }
        stbi_write_png("test.png",width,height,3,framebuffer.data(),3*width);
    }

    void renderSDL(ObjList objList,int spp,int maxdepth,WindowContext& sdlcontext)
    {
        
        float reciWidth=1.0f/width;
        float reciHeight=1.0f/height;

        for(int j=0;j!=height;++j)
        {
            // std::cerr<<"\rFinished "<<j<<" lines"<<std::flush;
#pragma omp parallel for
            for(int i=0;i!=width;++i)
            {
                MyGeo::Vec3f pixelColor{0,0,0};
                for(int k=0;k!=spp;k++)
                {
                    auto u=(i+getRand(0.f,1.f))*reciWidth;
                    auto v=(height-j-1+getRand(0.f,1.f))*reciHeight;
                    auto ray=camera->getRay(u,v);
                    pixelColor+=trace(ray,0.001f,std::numeric_limits<float>::max(),objList,0,maxdepth);
                }

                pixelColor/=(float)spp;
                setPixel(i,j,pixelColor);
            }
        }
        for(auto j=0;j!=height;++j)
        for(auto i=0;i!=width;++i)
        {
            auto ind=j*width+i;
            if(framebuffer[ind]!=MyGeo::Vec3c{0,0,0})
            {
                sdlcontext.drawPixel(i,j,framebuffer[j*width+i]);
            }
        }

    }

    MyGeo::Vec3f trace(const Ray& ray,float t0,float t1,ObjList objList,int depth,int maxdepth)
    {
        std::optional<HitRecord> rec;
        if(depth>maxdepth || !objList.hit(ray,t0,t1,rec)==true)
        {
            return backgroundColor;
        }
        auto emitColor=rec->material->emitColor(rec->u,rec->v,rec->position);

        MyGeo::Vec3f attenuation{0,0,0};
        Ray outray;
        if(rec->material->scatter(ray,rec.value(),attenuation,outray)==false)
        {
            return emitColor;
        }
        return emitColor+attenuation.cwiseprod(trace(outray,t0,t1,objList,depth+1,maxdepth));
    }

    
};


END_NAMESPACE
