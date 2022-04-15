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
    MyGeo::Vec3f Right,Up;

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
        std::cout<<"w "<<w<<std::endl;
        std::cout<<"h "<<h<<std::endl;


        Right=focuslen*w*xaxis;
        Up=focuslen*h*yaxis;
        TopLeft=position-Right*0.5+Up*0.5-zaxis*focuslen;
        printvec(Right,"right");
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
        // return {position+offset,(TopLeft+u*Right-v*Up-position-offset).normalize()};
        return {position,(TopLeft+u*Right-v*Up-position).normalize()};

    }
};



struct Canvas
{
    int width,height;
    float lightPDF;
    float lightArea;
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

void getLightPDF(ObjList objList)
{
    float sumArea=0.f;
    for(auto obj:objList.objects)
    {
        if(obj->ifemit())
        {
            sumArea+=obj->getArea().value();
        }
    }
    lightPDF=1.f/sumArea;
    lightArea=sumArea;
    // std::cout<<"lightArea "<<lightArea<<std::endl;
}
    void sampleLight(ObjList objList, HitRecord& sampleRec)
    {
        //
        float threshold=getRand(0.f,1.f)*lightArea;
        // std::cout<<"threashold "<<threshold<<std::endl;
        float accum=0.f;
        for(auto obj:objList.objects)
        {
            if(obj->ifemit())
            {
                accum+=obj->getArea().value();
                // std::cout<<"accum "<<accum<<std::endl;
                if (accum>threshold)
                {
                    obj->sample(sampleRec);
                    return;
                }
            }
        }
    }
    void render(ObjList objList,int spp,int maxdepth)
    {
        float reciWidth=1.0f/width;
        float reciHeight=1.0f/height;
        getLightPDF(objList);
        for(int j=0;j!=height;++j)
        {
            std::cerr<<"\rFinished "<<j<<" lines"<<std::flush;
#pragma omp parallel for
            for(int i=0;i!=width;++i)
            {
                MyGeo::Vec3f pixelColor{0,0,0};
                for(int k=0;k!=spp;k++)
                {
                    // auto u=(i+getRand(0.f,1.f))*reciWidth;
                    // auto v=1.0f-(height-j-1+getRand(0.f,1.f))*reciHeight;
                    auto u=i*reciWidth;
                    auto v=1.0f-(height-j-1)*reciHeight;
                    auto ray=camera->getRay(u,v);
                    // pixelColor+=trace(ray,0.001f,std::numeric_limits<float>::max(),objList,0,maxdepth);
                    pixelColor+=pathTrace(ray,0.001f,std::numeric_limits<float>::max(),objList,0);

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
                    auto v=1.0f-(height-j-1+getRand(0.f,1.f))*reciHeight;
                    // auto u=i*reciWidth;
                    // auto v=1.0f-(height-j-1)*reciHeight;
                    auto ray=camera->getRay(u,v);
                    // pixelColor+=trace(ray,0.001f,std::numeric_limits<float>::max(),objList,0,maxdepth);
                    pixelColor+=pathTrace(ray,0.001f,std::numeric_limits<float>::max(),objList,0);

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

    MyGeo::Vec3f pathTrace(const Ray& ray,float t0,float t1,ObjList objList,int depth)
    {
        std::optional<HitRecord> rec;
        if(!objList.hit(ray,t0,t1,rec)==true)
        {
            return backgroundColor;
        }

        //视线打到发光体上
        if(rec->material->ifemit()==true)
        {
            return rec->material->emitColor(rec->u,rec->v,rec->position);
        }
        
        //光源采样，从所有光源的总面积中随机选取一个点，获取位置，法线，材质，uv等
        HitRecord lightRec;
        sampleLight(objList,lightRec);
        
        //
        auto emit=lightRec.material->emitColor(lightRec.u,lightRec.v,lightRec.position);
        auto fr=rec->material->getKd()*RECI_PI;

        auto d=lightRec.position-rec->position;
        auto dist2=d.norm2();
        d.normalize();

        auto cos0=std::fabs(rec->normal.dot(d));
        auto cos1=std::fabs(lightRec.normal.dot(d));

        Ray rin{rec->position,d};
        std::optional<HitRecord> testRec;
        MyGeo::Vec3f directLight{0,0,0};

        objList.hit(rin,t0,t1,testRec);
        auto td2=(testRec->position-rec->position).norm2();
        if(td2-dist2>-1.f)
        {//lightArea
            directLight=cos0*cos1/dist2*lightArea*emit.cwiseprod(fr);
        }

static float prr=0.8;
static float reciprr=1.f/prr;
        
        auto tprr=getRand(0.f,1.f);
        //0.2概率返回直接光照结果(一次反射)
        if(tprr>prr) return directLight;
        
        auto wi=randomInHemisphere(rec->normal);
        // auto pdf=0.5f*RECI_PI;
        // if(wi.dot(rec->normal)<0.00001f) pdf=0.001f;
        
        std::optional<HitRecord> secondrec;
        Ray secondRay{rec->position,wi};
        // std::cout<<"wi "<<wi<<std::endl;
        if(!objList.hit(secondRay,t0,t1,secondrec)==true || secondrec->material->ifemit()==true)
        {
            return directLight;
        }
        
        fr=secondrec->material->getKd()*RECI_PI;
        cos0=std::fabs(wi.dot(rec->normal));

        auto indirectLight=pathTrace(Ray{rec->position,wi},t0,t1,objList,depth+1).cwiseprod(fr)*cos0*2*PI*reciprr;
        // auto indirectLight=pathTrace(Ray{rec->position,wi},t0,t1,objList,depth+1).cwiseprod(fr)*cos0*reciprr*(1.f/pdf);


        return indirectLight+directLight;
    }

    
};


END_NAMESPACE
