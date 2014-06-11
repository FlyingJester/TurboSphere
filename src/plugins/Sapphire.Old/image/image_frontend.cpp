#include "image.h"
#include "../api.h"
#include "../../common/plugin.h"
#include "../../../configmanager/opengame.h"
#include "../../../graphiccommon/screen.h"
#include "../libapi.h"

#include "../color/color.h"

#include <t5.h>
#include <string>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#ifndef OPENGL_ES
#include <SDL2/SDL_opengl.h>
#include "../loadfunc/gl_extra.h"
#else
#include <SDL2/SDL_opengles.h>
#include <SDL2/SDL_opengles2.h>
#endif

static TS_Config *TS_Conf = NULL;
static TS_Directories *TS_Dirs = NULL;

static string ImageDirectory        = "";
static string SystemArrowPath       = "";
static string SystemUpArrowPath     = "";
static string SystemDownArrowPath   = "";

using std::string;

GLfloat stexcoords[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
GLfloat ztexcoords[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};


Turbo::JSObj<TS_Image> ImageObject;

static void TS_ImageFinalizer(const v8::WeakCallbackData<v8::Object, TS_Image> &args){
    DestroyImage(args.GetParameter());
    args.GetValue().Clear();
}

void InitImage(uint32_t ID){
    IMG_Init(IMG_FLAGS);
    TS_Conf = GetConfig();
    TS_Dirs = GetDirs();

    ImageDirectory = TS_Dirs->image;
    ImageDirectory+="/";

    SystemArrowPath     = TS_Dirs->system;
    SystemUpArrowPath   = TS_Dirs->system;
    SystemDownArrowPath = TS_Dirs->system;

    SystemArrowPath     += TS_Conf->systemarrow;
    SystemUpArrowPath   += TS_Conf->systemuparrow;
    SystemDownArrowPath += TS_Conf->systemdownarrow;

    ImageObject                 = Turbo::JSObj<TS_Image>();
    ImageObject.Finalize        = TS_ImageFinalizer;
    ImageObject.ID              = (ID<<16)|(0xFE23u);

    glGenBuffers(1, &SeriousCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, SeriousCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint)<<4, stexcoords, GL_STATIC_DRAW);

}

void CloseImage(void){
    IMG_Quit();

    /////
    // Make the state completely blank.
    //
    // This helps a lot with identifying the issues with the reentrance of TurboSphere.
    //

    SystemArrowPath     = "";
    SystemUpArrowPath   = "";
    SystemDownArrowPath = "";

    TS_Conf = NULL;
    TS_Dirs = NULL;

    ImageObject.Finalize        = NULL;
    ImageObject.ID              = 0;

    glDeleteBuffers(1, &SeriousCoordBuffer);

}

    /////
    // Images moved to image/image_frontend.cpp


    // Creation

static void InnerLoadImage(Turbo::JSArguments args, const char *loc){

    if(!T5_IsFile(loc)){
        Turbo::SetError(args, std::string(("[" PLUGINNAME "] InnerLoadImage Error: Could not find ")+(std::string(loc))).c_str());
        return;
    }

    SDL_Surface *surface = IMG_Load(loc);

    if(!surface) {
        Turbo::SetError(args, std::string(("[" PLUGINNAME "] InnerLoadImage Error: Could not open ")+(std::string(loc)+"\nError was ").append(IMG_GetError())).c_str());
        return;
    }

    TS_Image *image = ::CreateImage(surface->w, surface->h, (RGBA*)(surface->pixels));

    SDL_FreeSurface(surface);

    Turbo::WrapObject(args, ImageObject, image);
}

Turbo::JSFunction Script::LoadImage(Turbo::JSArguments args){

    int sig[] = {Turbo::String, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig))
        return;

    v8::String::Utf8Value str(args[0]);

    InnerLoadImage(args, *str);
/*
    if(!T5_IsFile(*str)){
        Turbo::SetError(args, ((string("[" PLUGINNAME "] ").append(__func__).append(" Error: File ").append(*str).append(" does not exist.")).c_str()));
        return;
    }
    SDL_Surface *surface = IMG_Load((ImageDirectory+string(*str)).c_str());

    TS_Image *image = ::CreateImage(surface->w, surface->h, (RGBA*)(surface->pixels));

    SDL_FreeSurface(surface);

    Turbo::WrapObject(args, ImageObject, image);
*/
}

Turbo::JSFunction Script::CreateImage(Turbo::JSArguments args){

    int sig[] = {Turbo::Uint, Turbo::Uint, Turbo::Object, 0};

    if(!Turbo::CheckArg::CheckSig(args, 3, sig))
        return;


    int w = args[0]->Int32Value();
    int h = args[1]->Int32Value();


    if(!ColorObject.IsA(args[2])){
        Turbo::SetError(args, ((std::string("[" PLUGINNAME "] CreateImage Error: Argument ")+std::to_string(2)+std::string(" is not a color.")).c_str()), v8::Exception::TypeError);
        return;
    }

    v8::Local<v8::Object> obj0 = v8::Local<v8::Object>::Cast(args[2]);
    TS_Color *colorobj = (TS_Color*)obj0->GetAlignedPointerFromInternalField(0);

    RGBA color = colorobj->toInt();

    RGBA *pixels = (RGBA*)malloc((w*h)<<2);

    for(int i = 0; i<w*h; i++){
        pixels[i] = color;
    }

    TS_Image *image = ::CreateImage(w, h, pixels);

    free(pixels);

    Turbo::WrapObject(args, ImageObject, image);
}


Turbo::JSFunction Script::GrabImage(Turbo::JSArguments args){
    int sig[] = {Turbo::Int, Turbo::Int, Turbo::Int, Turbo::Int, 0};

    int x, y, w, h;

    if(args.Length()==0){
        x = 0;
        y = 0;
        w = ::GetScreenWidth();
        h = ::GetScreenHeight();
    }
    else if(!Turbo::CheckArg::CheckSig(args, 4, sig))
        return;
    else{
        x = args[0]->Int32Value();
        y = args[1]->Int32Value();
        w = args[2]->Int32Value();
        h = args[3]->Int32Value();

        if(w<0)
            Turbo::SetError(args, std::string("[" PLUGINNAME "] GrabImage Error: Argument 2 is Negative").c_str(), v8::Exception::RangeError);
        else if(h<0)
            Turbo::SetError(args, std::string("[" PLUGINNAME "] GrabImage Error: Argument 3 is Negative").c_str(), v8::Exception::RangeError);

        if(!((w)&&(h)))
            return;
    }


    TS_Image *image = ::GrabImage(x, y, w, h);
    Turbo::WrapObject(args, ImageObject, image);
}

// System images



Turbo::JSFunction Script::GetSystemArrow(Turbo::JSArguments args){
    InnerLoadImage(args, SystemArrowPath.c_str());
}

Turbo::JSFunction Script::GetSystemUpArrow(Turbo::JSArguments args){
    InnerLoadImage(args, SystemUpArrowPath.c_str());
}

Turbo::JSFunction Script::GetSystemDownArrow(Turbo::JSArguments args){
    InnerLoadImage(args, SystemDownArrowPath.c_str());
}
