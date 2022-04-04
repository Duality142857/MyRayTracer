#pragma once
#include<SDL2/SDL.h>
#include<iostream>
#include<array>
#include<mygeo/vec.h>

struct WindowContext
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    int width;
    int height;
    // SDL_Surface* surface;
    // SDL_Texture* framebuffer;    
    WindowContext(int width,int height,const std::string& name):width{width},height{height}
    {
        if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)<0)
        {
            std::cout<<"sdl init failed "<<SDL_GetError()<<std::endl;
            return ;
        }
        window=SDL_CreateWindow(name.c_str(),SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN );
        if(window==nullptr)
        {
            throw std::runtime_error("window create failed "+std::string(SDL_GetError()));
        }

        renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    }

    void test()
    {
        rendererClear();
        for(int i=0;i!=width;i++)
        for(int j=0;j!=height;j++)
            drawPixel(i,j,{0,255,255,255});
        renderPresent();
        sleep(2000);
    }
    void rendererClear()
    {
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);
    }

    void drawPixel(int x,int y, const MyGeo::Vec3c& color={255,255,255,255})
    {
        // std::cout<<"draw "<<x<<','<<y<<std::endl;
        SDL_SetRenderDrawColor(renderer,color.x,color.y,color.z,255);
        SDL_RenderDrawPoint(renderer,x,y);
    }
    
    void renderPresent()
    {
        SDL_RenderPresent(renderer);
    }
    void sleep(uint32_t miliseconds)
    {
        SDL_Delay(miliseconds);
    }


    ~WindowContext()
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

