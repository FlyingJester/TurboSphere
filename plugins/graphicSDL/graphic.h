#ifndef GRAPHIC_HEAD
#define GRAPHIC_HEAD
//#include "SDL_V8.h"
#include"../common/plugin.h"
#include"../common/graphic_common.h"
#include"../common/graphic_algorithm.h"
#include <vector>

#ifdef _WIN32
    #ifdef GRAPHIC_INTERNAL
	#define GRAPHICSDL_EXPORT __declspec(dllexport)
	#else
	#define GRAPHICSDL_EXPORT __declspec(dllimport)
	#endif

    #define CCALL __cdecl

#endif
#ifndef _WIN32
#define CCALL
#define GRAPHICSDL_EXPORT extern "C"
#endif

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
//this is not possible on proper platforms
//Except MIPS?
//You got this running on MIPS, I wanna know.
#else

    #define Frmask 0x000000ff
	#define Fgmask 0x0000ff00
	#define Fbmask 0x00ff0000
	#define Famask 0xff000000
#endif

#define IMG_FLAGS (IMG_INIT_JPG|IMG_INIT_PNG|IMG_INIT_TIF)
#define SDL_VIDEO_FLAGS SDL_HWSURFACE|SDL_ASYNCBLIT|SDL_DOUBLEBUF|SDL_HWACCEL

#define BPP 4
#define DEPTH 32

#define CHANNEL_MASKS Frmask, Fgmask, Fbmask, Famask

extern v8::Persistent<v8::FunctionTemplate> Surfacetempl;
extern v8::Persistent<v8::ObjectTemplate> SurfaceInsttempl;

extern v8::Persistent<v8::FunctionTemplate> Imagetempl;
extern v8::Persistent<v8::ObjectTemplate> ImageInsttempl;

extern v8::Persistent<v8::FunctionTemplate> Colortempl;
extern v8::Persistent<v8::ObjectTemplate> ColorInsttempl;

//extern "C" int concatRGBA(int r, int g, int b, int a);

//void TS_FlipScreen();


void TS_ColorFinalizer(v8::Persistent<v8::Value> object, void* parameter);

#ifdef GRAPHIC_INTERNAL

#ifdef _WIN32
	extern "C" {
#endif
GRAPHICSDL_EXPORT void          CCALL Close(void);
GRAPHICSDL_EXPORT initFunction  CCALL Init(void);
GRAPHICSDL_EXPORT int           CCALL GetNumFunctions(void);
GRAPHICSDL_EXPORT functionArray CCALL GetFunctions(void);
GRAPHICSDL_EXPORT nameArray     CCALL GetFunctionNames(void);
GRAPHICSDL_EXPORT int           CCALL GetNumVariables(void);
GRAPHICSDL_EXPORT void **       CCALL GetVariables(void);
GRAPHICSDL_EXPORT nameArray     CCALL GetVariableNames(void);
#ifdef _WIN32
	}
#endif

void TS_GradientRectangle(int x, int y, int w, int h, TS_Color *c1, TS_Color *c2, TS_Color *c3, TS_Color *c4);
void TS_Rectangle(int x, int y, int w, int h, unsigned int color);
void TS_drawAlphaRect(int x, int y, int w, int h, int c, int a, SDL_Rect rect);
void TS_Rectangle(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void TS_FilledCircle(int x, int y, int rad, uint32_t c, SDL_Surface* destination);
void TS_OutlinedCircle(int x, int y, int rad, uint32_t c, SDL_Surface* destination);
void TS_Line(int x1, int y1, int x2, int y2, TS_Color *);


v8::Handle<v8::Value> FlipScreen(const v8::Arguments& args);
v8::Handle<v8::Value> SetClippingRectangle(const v8::Arguments& args);
v8::Handle<v8::Value> GetClippingRectangle(const v8::Arguments& args);
v8::Handle<v8::Value> SetTitle(const v8::Arguments& args);
v8::Handle<v8::Value> V8GetScreenHeight(const v8::Arguments& args);
v8::Handle<v8::Value> V8GetScreenWidth(const v8::Arguments& args);
v8::Handle<v8::Value> SetResolution(const v8::Arguments& args);
v8::Handle<v8::Value> Point(const v8::Arguments& args);
v8::Handle<v8::Value> Rectangle(const v8::Arguments& args);
v8::Handle<v8::Value> GradientRectangle(const v8::Arguments& args);
v8::Handle<v8::Value> FilledCircle(const v8::Arguments& args);
v8::Handle<v8::Value> OutlinedCircle(const v8::Arguments& args);
v8::Handle<v8::Value> TS_surfaceBlit(const v8::Arguments& args);
v8::Handle<v8::Value> TS_surfaceBlitSurface(const v8::Arguments& args);
v8::Handle<v8::Value> TS_surfaceSetPixel(const v8::Arguments& args);
v8::Handle<v8::Value> TS_CreateSurface(const v8::Arguments& args);
v8::Handle<v8::Value> TS_CreateImage(const v8::Arguments& args);
v8::Handle<v8::Value> CreateColor(const v8::Arguments& args);
v8::Handle<v8::Value> TS_surfaceGetPixel(const v8::Arguments& args);
v8::Handle<v8::Value> Line(const v8::Arguments& args);
v8::Handle<v8::Value> TS_GetSystemArrow(const v8::Arguments& args);

void ColorClose();
void SurfaceClose();
void ImageClose();

void ImageInit();
void SurfaceInit();
void ColorInit();



#endif

GRAPHICSDL_EXPORT void TS_ShowSurface(SDL_Surface *image, int x, int y);
GRAPHICSDL_EXPORT void TS_ShowSurfaceClipped(SDL_Surface *image, SDL_Rect dest, SDL_Rect src);
GRAPHICSDL_EXPORT void TS_StretchShowSurface(SDL_Surface *, int x, int y, float fh, float fv);


//surfaces
v8::Handle<v8::Value> GetWidth(v8::Local<v8::String> property, const v8::AccessorInfo &info);
void SetWidth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
v8::Handle<v8::Value> GetHeight(v8::Local<v8::String> property, const v8::AccessorInfo &info);
void SetHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);

//colors
v8::Handle<v8::Value> GetRed(v8::Local<v8::String> property, const v8::AccessorInfo &info);
void SetRed(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
v8::Handle<v8::Value> GetGreen(v8::Local<v8::String> property, const v8::AccessorInfo &info);
void SetGreen(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
v8::Handle<v8::Value> GetBlue(v8::Local<v8::String> property, const v8::AccessorInfo &info);
void SetBlue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
v8::Handle<v8::Value> GetAlpha(v8::Local<v8::String> property, const v8::AccessorInfo &info);
void SetAlpha(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);

#define TS_StretchShowSurface_Clear(image, _x, _y, fh, fv){ \
TS_StretchShowSurface(image, _x, _y, fh, fv);\
SDL_FreeSurface(image);\
}

#define TS_ShowSurface_Clear(image, _x, _y)\
TS_ShowSurface(image, _x, _y);\
SDL_FreeSurface(image);

#define TS_ShowSurfaceClipped_Clear(image, _r) \
TS_ShowSurfaceClipped(image, _r);\
SDL_FreeSurface(image);


#endif
