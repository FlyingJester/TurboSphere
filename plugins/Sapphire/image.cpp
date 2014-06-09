#include "image.hpp"
#include "surface.h"
#include <SDL2/SDL_image.h>
#include <string>

#include "../../configmanager/opengame.h"
#include "../../configmanager/openscript.h"

using namespace Sapphire;

Turbo::JSObj<Sapphire::Image> Sapphire::ImageObject;

static const char *ImageDir = NULL;

void TS_ImageFinalizer(const v8::WeakCallbackData<v8::Object, Sapphire::Image> &args){
    DecImageRef(args.GetParameter());
    args.GetValue().Clear();
}

void Sapphire::InitImage(uint32_t ID, v8::Isolate *isol){

    IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG|IMG_INIT_TIF|IMG_INIT_WEBP);

    Sapphire::ImageObject = Turbo::JSObj<Sapphire::Image> ();



    printf("[" PLUGINNAME "] Info: Image ID is %x\n", (ID<<16)|(0x88EBu));

    Sapphire::ImageObject.Finalize        = TS_ImageFinalizer;
    Sapphire::ImageObject.SetTypeName("Image");
    Sapphire::ImageObject.ID              = (ID<<16)|(0x88EBu);
    //Sapphire::ImageObject.AddAccessor("red",   ColorRedGetter,    ColorRedSetter);
    //Sapphire::ImageObject.AddAccessor("green", ColorGreenGetter,  ColorGreenSetter);
    //Sapphire::ImageObject.AddToProto("RedMember", RedMember);

    ImageDir = GetDirs()->image;

}

Sapphire::Image::Image(RGBA *pix, unsigned int _w, unsigned int _h, bool owns_pixels){

    texture = 0;

    if(!(_w&&_h)){
        width = 0;
        height = 0;
        return;
    }

    assert(pix);
    assert(_w);
    assert(_h);

    width = _w;
    height = _h;

    glGenTextures(1, &texture);

    assert(texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _w, _h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pix);

    persistent = 0;
    SDL_AtomicSet(&refs, 0);

    if(owns_pixels)
        pixels = pix;
    else{
        pixels = (RGBA*)malloc(sizeof(RGBA)*_w*_h);
        memcpy(pixels, pix, sizeof(RGBA)*_w*_h);
    }

}

Image::~Image(){
    free(pixels);
    glDeleteTextures(1, &texture);
}

Turbo::JSFunction Sapphire::Script::CreateImage(Turbo::JSArguments args){

    if(args.Length() < 1){
        args.GetReturnValue().Set( v8::Exception::TypeError(v8::String::NewFromUtf8(iso, "[" PLUGINNAME "] CreateImage Error: Called with fewer than 1 argument.")));
        return;
    }

    int sig1[] = {Turbo::Int, Turbo::Int, Turbo::Object, 0};
    int sig2[] = {Turbo::Object, 0};
    int sig3[] = {Turbo::String, 0};

    Sapphire::Image *im = NULL;

    if(Turbo::CheckArg::CheckSig(args, 3, sig1, false)){
        RGBA *color = (RGBA*)malloc(args[0]->Int32Value()*args[1]->Int32Value()*sizeof(RGBA));

        im = new Sapphire::Image(color, args[0]->Int32Value(), args[1]->Int32Value(), true);

        Turbo::WrapObject(args, Sapphire::ImageObject, im);

        return;

    }


    if((Turbo::CheckArg::CheckSig(args, 1, sig2, false)&&(Sapphire::SurfaceObject.IsA(args[0])))){

        //printf("Creating an image from a Surface.\n");

        const SDL_Surface *surf = (SDL_Surface*)(v8::Local<v8::Object>::Cast(args[0]))->GetAlignedPointerFromInternalField(0);

        im = new Sapphire::Image((uint32_t *)(surf->pixels), surf->w, surf->h, false);

        Turbo::WrapObject(args, Sapphire::ImageObject, im);

        return;
    }
    else if(!Turbo::CheckArg::CheckSig(args, 1, sig3, true))
        return;
        //The following block is intentionally NOT directly connected to the prior conditional.
    {

        v8::String::Utf8Value str(args[0]);
        //printf("Creating an image from a file: %s.\n", *str);

        std::string path = std::string(ImageDir) + std::string(*str);

        if(!T5_IsFile(path.c_str())){
            Turbo::SetError(args, (std::string("[" PLUGINNAME "] CreateImage Error: No such image file \"")+path+std::string("\".")).c_str(), v8::Exception::Error);
            return;
        }

        SDL_Surface *surf = IMG_Load(path.c_str());

        if(!surf){
            Turbo::SetError(args, (std::string("[" PLUGINNAME "] CreateImage Error: Cannot open image file \"")+path+std::string("\".")).c_str(), v8::Exception::Error);
            return;
        }

        im = new Sapphire::Image((uint32_t *)(surf->pixels), surf->w, surf->h, false);

        SDL_FreeSurface(surf);

        Turbo::WrapObject(args, Sapphire::ImageObject, im);
    }

}
