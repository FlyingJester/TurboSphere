#include "surface.h"
#include "colamd.h"
#include "color.hpp"
#include <SDL2/SDL_image.h>
#include <t5.h>

#include <string>
#include <utility>
#include <memory>

#include "../../configmanager/opengame.h"
#include "../../configmanager/openscript.h"

static const char *ImageDir = NULL;
Turbo::JSObj<SDL_Surface> Sapphire::SurfaceObject;


int TS_IsSurfacePathValid(const char *path){
    return 0;
}

void TS_SurfaceFinalizer(const v8::WeakCallbackData<v8::Object, SDL_Surface> &args){

    TS_ClearSurface(args.GetParameter());
    SDL_FreeSurface(args.GetParameter());
    args.GetValue().Clear();

}

void SurfaceObjInit(uint32_t ID){

    Sapphire::SurfaceObject = Turbo::JSObj<SDL_Surface> ();


    Sapphire::SurfaceObject.Finalize        = TS_SurfaceFinalizer;
    Sapphire::SurfaceObject.SetTypeName("Surface");
    Sapphire::SurfaceObject.ID              = (ID<<16)|(0x88EAu);

    ImageDir = GetDirs()->image;
}

Turbo::JSFunction Sapphire::Script::CreateSurface(Turbo::JSArguments args){

    const int sig1[] = {Turbo::String, 0};
    const int sig2[] = {Turbo::Int, Turbo::Int, Turbo::Object, 0};

    if(Turbo::CheckArg::CheckSig(args, 1, sig1, false)){ //Load from file

        v8::String::Utf8Value str(args[0]);

        std::string path = std::string(ImageDir) + std::string(*str);

        if(!T5_IsFile(path.c_str())){
            Turbo::SetError(args, "[" PLUGINNAME "] CreateSurface Error: Argument 0 is not a valid path.", v8::Exception::TypeError);
            return;
        }

        SDL_Surface *surf = IMG_Load(path.c_str());

        Turbo::WrapObject(args, Sapphire::SurfaceObject, surf);

    }
    else if(Turbo::CheckArg::CheckSig(args, 3, sig2)){ //Create a single color

        const int w = args[0]->Int32Value();
        const int h = args[1]->Int32Value();

        if(!ColorObject.IsA(args[2])){
            Turbo::SetError(args, "[" PLUGINNAME "] CreateSurface Error: Argument 2 is not a Color.", v8::Exception::TypeError);
            return;
        }

        const uint32_t colint = ((TS_Color*)(v8::Local<v8::Object>::Cast(args[2]))->GetAlignedPointerFromInternalField(0))->toInt();

        SDL_Surface *surf = SDL_CreateRGBSurface(0, w, h, 32, CHANNEL_MASKS);

        SDL_FillRect(surf, NULL, colint);
        TS_PrimeSurface(surf);

        Turbo::WrapObject(args, Sapphire::SurfaceObject, surf);

    }

}
