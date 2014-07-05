#include "script.hpp"
#include "Sapphire.hpp"

#include "Galileo/Galileo.hpp"
#include "Image.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <algorithm>
#include <string>

namespace Sapphire {
namespace Script {

Turbo::JSObj<TS_Color>        ColorJSObj;
Turbo::JSObj<SDL_Surface>     SurfaceJSObj;
Turbo::JSObj<Image>           ImageJSObj;
Turbo::JSObj<Galileo::Vertex> VertexJSObj;
Turbo::JSObj<Galileo::Shape>  ShapeJSObj;
Turbo::JSObj<Galileo::Group>  GroupJSObj;

namespace Finalizer {

template <class T>
void Generic(const v8::WeakCallbackData<v8::Object, T>       &args) {
    delete args.GetParameter();
    args.GetValue().Clear();
}

template <class T>
void CGeneric(const v8::WeakCallbackData<v8::Object, T>       &args) {
    free(args.GetParameter());
    args.GetValue().Clear();
}

void NoFree(const v8::WeakCallbackData<v8::Object, void>       &args){
    args.GetValue().Clear();
}

void Surface(const v8::WeakCallbackData<v8::Object, SDL_Surface>    &args) {

    SDL_FreeSurface(args.GetParameter());
    args.GetValue().Clear();
}

void Vertex (const v8::WeakCallbackData<v8::Object, Galileo::Vertex>&args){
    args.GetValue().Clear();
}

void Shape  (const v8::WeakCallbackData<v8::Object, Galileo::Shape> &args){
    args.GetValue().Clear();
}

void Group  (const v8::WeakCallbackData<v8::Object, Galileo::Group> &args){
    args.GetValue().Clear();
}


}

void InitScript(int64_t ID){

    printf(BRACKNAME " Info: ID is %llx\n", ID);


    ColorJSObj   = Turbo::JSObj<TS_Color>       ();
    SurfaceJSObj = Turbo::JSObj<SDL_Surface>    ();
    ImageJSObj   = Turbo::JSObj<Image>          ();
    VertexJSObj  = Turbo::JSObj<Galileo::Vertex>();
    ShapeJSObj   = Turbo::JSObj<Galileo::Shape> ();
    GroupJSObj   = Turbo::JSObj<Galileo::Group> ();

    ColorJSObj.Finalize        = Finalizer::Generic<TS_Color>;
    ImageJSObj.SetTypeName("Color");

    ColorJSObj.ID              = (ID<<16)|(0x0312u);
    SurfaceJSObj.ID            = (ID<<16)|(0x0302u);
    ImageJSObj.ID              = (ID<<16)|(0x0292u);
    VertexJSObj.ID             = (ID<<16)|(0x0289u);
    ShapeJSObj.ID              = (ID<<16)|(0x0260u);
    GroupJSObj.ID              = (ID<<16)|(0x0221u);
}

/////
// Old School
Turbo::JSFunction ColorCtor(Turbo::JSArguments args){

    const int sig[] = {Turbo::Int, Turbo::Int, Turbo::Int, Turbo::Int, 0};

    TS_Color *rColor = new TS_Color(0, 0, 0, 255);

    if(!Turbo::CheckArg::CheckSig(args, 3, sig, true))
      return;

    if(Turbo::CheckArg::CheckSig(args, 4, sig, false))
      rColor->alpha = std::min(0xFF, args[3]->Int32Value());

    rColor->red   = std::min(0xFF, args[0]->Int32Value());
    rColor->green = std::min(0xFF, args[1]->Int32Value());
    rColor->blue  = std::min(0xFF, args[2]->Int32Value());

    Turbo::WrapObject(args, ColorJSObj, rColor);
}

// Can construct an Image from an SDL_Surface, a path, or dimensions and a color.
Turbo::JSFunction ImageCtor(Turbo::JSArguments args){

    const int sig[] = {Turbo::Int, Turbo::Int, Turbo::Object, 0};

    if(args.Length()==1){
        if(SurfaceJSObj.IsA(args[0])){//From Surface
            const SDL_Surface *lSurf =
              (SDL_Surface*)(v8::Local<v8::Object>::Cast(args[0]))->GetAlignedPointerFromInternalField(0);
            Image *rImage = new Image(lSurf);
            Turbo::WrapObject(args, ImageJSObj, rImage);
            return;
        }

        if(Turbo::CheckArg::String(args, 0, __func__)){ // From Path
            v8::String::Utf8Value lStr(args[0]);
            SDL_Surface *lSurf = IMG_Load(*lStr);
            if(!lSurf){
                Turbo::SetError(args, (std::string(BRACKNAME " ImageCtor Error: Could not open path '") +
                                       std::string(*lStr) + std::string("'.") ).c_str());
            }
            else{
              Image *rImage = new Sapphire::Image(lSurf);
              Turbo::WrapObject(args, ImageJSObj, rImage);
            }

            SDL_FreeSurface(lSurf);

            return;
        }
        else { //Not a surface, not a string.
            Turbo::SetError(args, BRACKNAME " ImageCtor Error: Called with 1 argument,"
                                            " argument 1 is not a Surface or a string.");
            return;
        }
    }

    if(Turbo::CheckArg::CheckSig(args, 3, sig)){ // From dim's and color.

        if(!ColorJSObj.IsA(args[2])){
            Turbo::SetError(args, BRACKNAME " ImageCtor Error: Argument 2 is not a Color.");
            return;
        }

        const size_t lPixelCount = args[0]->Int32Value()*args[1]->Int32Value();

        uint32_t *pixels = new uint32_t[lPixelCount];

        const TS_Color *lColor =
          (TS_Color*)(v8::Local<v8::Object>::Cast(args[2]))->GetAlignedPointerFromInternalField(0);

        std::fill(
                    static_cast<uint32_t *>(pixels),
                    static_cast<uint32_t *>(&(pixels[lPixelCount-1])),
                    lColor->toInt()
                  );

        SDL_Surface *lSurf = SDL_CreateRGBSurfaceFrom(
                              pixels,
                              args[0]->Int32Value(),
                              args[1]->Int32Value(),
                              32,
                              args[1]->Int32Value()*4,
                              CHANNEL_MASKS);
        Image *rImage = new Image(lSurf);
        SDL_FreeSurface(lSurf);
        delete[] pixels;

        Turbo::WrapObject(args, ImageJSObj, rImage);

    }

}

Turbo::JSFunction SurfaceCtor(Turbo::JSArguments args){

    const int sig[] = {Turbo::Int, Turbo::Int, Turbo::Object, 0};

    if(args.Length()==1){

        if(Turbo::CheckArg::String(args, 0, __func__)){ // From Path
            v8::String::Utf8Value lStr(args[0]);
            SDL_Surface *rSurf = IMG_Load(*lStr);
            if(!rSurf){
                Turbo::SetError(args, (std::string(BRACKNAME " SurfaceCtor Error: Could not open path '") +
                                        std::string(*lStr) + std::string("'.") ).c_str());
            }
            else{
              Turbo::WrapObject(args, SurfaceJSObj, rSurf);
            }
            return;
        }
        else { //Not a surface, not a string.
            Turbo::SetError(args, BRACKNAME " SurfaceCtor Error: Called with 1 argument, argument 1 is not a string.");
            return;
        }


    }

    if(Turbo::CheckArg::CheckSig(args, 3, sig)){ // From dim's and color.

        if(!ColorJSObj.IsA(args[2])){
            Turbo::SetError(args, BRACKNAME " SurfaceCtor Error: Argument 2 is not a Color.");
            return;
        }

        const TS_Color *lColor = (TS_Color*)(v8::Local<v8::Object>::Cast(args[2]))->GetAlignedPointerFromInternalField(0);

        const size_t lPixelCount = args[0]->Int32Value()*args[1]->Int32Value();

        SDL_Surface *rSurf = SDL_CreateRGBSurface(0, args[0]->Int32Value(), args[1]->Int32Value(),
                                                  32, CHANNEL_MASKS);

        uint32_t *lPix = static_cast<uint32_t *>(rSurf->pixels);

        std::fill(
                    lPix,
                    &(lPix[lPixelCount-1]),
                    lColor->toInt()
                  );

        Turbo::WrapObject(args, SurfaceJSObj, rSurf);
    }

}

/////
// New School!
Turbo::JSFunction VertexCtor(Turbo::JSArguments args);
Turbo::JSFunction ShapeCtor(Turbo::JSArguments args);
Turbo::JSFunction GroupCtor(Turbo::JSArguments args);

/////
// Middle School
Turbo::JSFunction ShaderCtor(Turbo::JSArguments args);
Turbo::JSFunction ShaderProgramCtor(Turbo::JSArguments args);

//Turbo::JSFunction (Turbo::JSArguments args);

}
}
