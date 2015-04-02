#pragma once

#include "Sapphire.hpp"

#include "Galileo/Galileo.hpp"
#include "Image.hpp"
#include <SDL2/SDL.h>
#include <array>
#include <vector>

#include <memory>

/////
// This file contains all JS-Wrapping functions
//

namespace Sapphire {
namespace Script {

void initScript(JSContext *ctx);
void closeScript(JSContext *ctx);

static const size_t NumFuncs = 6;

typedef std::vector<JSFunctionSpec>::iterator memberiter_t;
typedef std::shared_ptr<Image> ScriptImage_t;

extern std::array<JSNative, NumFuncs> FunctionList;
extern std::array<const char * const, NumFuncs> FunctionNameList;
extern std::vector<JSFunctionSpec>     CrossPluginSurfaceMembers;

extern Turbo::JSPrototype<TS_Color>         color_proto;
extern Turbo::JSPrototype<SDL_Surface>      surface_proto;
extern Turbo::JSPrototype<ScriptImage_t>    image_proto;
extern Turbo::JSPrototype<Galileo::Shape>   shape_proto;
extern Turbo::JSPrototype<Galileo::Group>   group_proto;
extern Turbo::JSPrototype<std::shared_ptr<Galileo::Shader> > shader_program_proto;

void ColorFinalizer(JSFreeOp *fop, JSObject *obj);
void SurfaceFinalizer(JSFreeOp *fop, JSObject *obj);
void ImageFinalizer(JSFreeOp *fop, JSObject *obj);
void ShapeFinalizer(JSFreeOp *fop, JSObject *obj);
void GroupFinalizer(JSFreeOp *fop, JSObject *obj);
void ShaderProgramFinalizer(JSFreeOp *fop, JSObject *obj);

/////
// Old School
bool ColorCtor(JSContext *ctx, unsigned argc, JS::Value *vp);
bool ImageCtor(JSContext *ctx, unsigned argc, JS::Value *vp);
bool SurfaceCtor(JSContext *ctx, unsigned argc, JS::Value *vp);
bool ArrayBufferToSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool ArrayBufferToImage(JSContext *ctx, unsigned argc, JS::Value *vp);

bool SaveSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool CloneSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool SetPixelSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool SurfaceBlitSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool SurfaceSetClippingRectangle(JSContext *ctx, unsigned argc, JS::Value *vp);

bool LineSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool GradientLineSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool TriangleSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool GradientTriangleSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool OutlinedTriangleSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool RectangleSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool GradientRectangleSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool OutlinedRectangleSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool QuadSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool GradientQuadSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool OutlinedQuadSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool CircleSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool GradientCircleSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool OutlinedCircleSurface(JSContext *ctx, unsigned argc, JS::Value *vp);

bool SaveImage(JSContext *ctx, unsigned argc, JS::Value *vp);
bool ImageCreateSurface(JSContext *ctx, unsigned argc, JS::Value *vp);
bool CloneImage(JSContext *ctx, unsigned argc, JS::Value *vp);
bool ImageUpdate(JSContext *ctx, unsigned argc, JS::Value *vp);
bool ImageUpdateAt(JSContext *ctx, unsigned argc, JS::Value *vp);
/////
// New School!
bool VertexCtor(JSContext *ctx, unsigned argc, JS::Value *vp);
bool ShapeCtor(JSContext *ctx, unsigned argc, JS::Value *vp);
bool GroupCtor(JSContext *ctx, unsigned argc, JS::Value *vp);

bool DrawGroup(JSContext *ctx, unsigned argc, JS::Value *vp);
bool GroupSetPosition(JSContext *ctx, unsigned argc, JS::Value *vp);
bool GroupSetRotation(JSContext *ctx, unsigned argc, JS::Value *vp);

bool GroupSetNativeShapes(JSContext *ctx, Galileo::Group *group, JS::HandleObject shape_array);

/////
// Middle School
bool ShaderCtor(JSContext *ctx, unsigned argc, JS::Value *vp);
bool ShaderProgramCtor(JSContext *ctx, unsigned argc, JS::Value *vp);
bool GetDefaultShaderProgram(JSContext *ctx, unsigned argc, JS::Value *vp);


bool FlipScreen(JSContext *ctx, unsigned argc, JS::Value *vp);
bool GetScreenWidth(JSContext *ctx, unsigned argc, JS::Value *vp);
bool GetScreenHeight(JSContext *ctx, unsigned argc, JS::Value *vp);
//bool (JSContext *ctx, unsigned argc, JS::Value *vp);

}
}
