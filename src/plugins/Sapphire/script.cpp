#include "script.hpp"
#include "Sapphire.hpp"
#include "SapphireInit.hpp"
#include "api.hpp"

#include <jsapi.h>

#include "Galileo/Galileo.hpp"
#include "Image.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "GLStart.hpp"
#include "SaveImage.hpp" // RenderQueue()
#include "Surface/Primitives.hpp"

#include <algorithm>
#include <queue>
#include <memory>
#include <string>
#include <screen.h>
#include <openscript.h>
#include <opengame.h>
#include <t5.h>

namespace Sapphire {
namespace Script {

bool ShapeImageGetter(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    Galileo::Shape *mShape = shape_proto.getSelf(ctx, vp, &args);
    
    args.rval().set(OBJECT_TO_JSVAL(image_proto.wrap(ctx, new ScriptImage_t(mShape->GetImage()))));
    
    return true;
}

bool ShapeImageSetter(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckForSingleArg(ctx, args, Turbo::Object, __func__))
        return false;
    
    if(ScriptImage_t *image = image_proto.unwrap(ctx, args[0], &args)){
        shape_proto.getSelf(ctx, vp, &args)->ReplaceImage(*image);
    }
    else{
        
        SDL_Surface *surface = surface_proto.unwrap(ctx, args[0], &args);
        if(!surface){
            Turbo::SetError(ctx, std::string("[" PLUGINNAME "] ") + __func__ + " Error argument 0 is not a Surface or an Image");
            return false;
        }
        
        shape_proto.getSelf(ctx, vp, &args)->ReplaceImage(ScriptImage_t(new Sapphire::Image(surface)));
    }
    
    return true;
}

template<typename T>
unsigned NumberToColorChannel(T n){return std::max(std::min((int)n, 0xFF), 0x0);}

#define SAPPHIRE_SCRIPT_OBJECT_GETTER(FUNC_NAME, TYPE, PROTO, VALUATOR)\
bool FUNC_NAME(JSContext *ctx, unsigned argc, JS::Value *vp){\
    JS::CallArgs args = CallArgsFromVp(argc, vp);\
    const TYPE *that = PROTO.getSelf(ctx, vp, &args);\
    args.rval().set(VALUATOR);\
    return true;\
}

#define SAPPHIRE_SCRIPT_GROUP_GETTER(FUNC_NAME, VALUATOR) SAPPHIRE_SCRIPT_OBJECT_GETTER(FUNC_NAME, Galileo::Group, group_proto, VALUATOR)

#define SAPPHIRE_SCRIPT_OBJECT_SETTER(FUNC_NAME, TYPE, PROTO, TURBO_T, SETTER)\
bool FUNC_NAME(JSContext *ctx, unsigned argc, JS::Value *vp){\
    JS::CallArgs args = CallArgsFromVp(argc, vp);\
    const Turbo::JSType signature[] = {TURBO_T};\
    if(!Turbo::CheckSignature<1>(ctx, args, signature, __func__))\
        return false;\
    TYPE *that = PROTO.getSelf(ctx, vp, &args);\
    SETTER;\
    return true;\
}

#define SAPPHIRE_SCRIPT_OBJECT_SETTER_NUMBER(FUNC_NAME, TYPE, PROTO, SETTER) SAPPHIRE_SCRIPT_OBJECT_SETTER(FUNC_NAME, TYPE, PROTO, Turbo::Number, SETTER)
#define SAPPHIRE_SCRIPT_GROUP_SETTER_NUMBER(FUNC_NAME, SETTER) SAPPHIRE_SCRIPT_OBJECT_SETTER_NUMBER(FUNC_NAME, Galileo::Group, group_proto, SETTER)

SAPPHIRE_SCRIPT_OBJECT_GETTER(SurfaceWidthGetter, SDL_Surface, surface_proto, JS_NumberValue(that->w))
SAPPHIRE_SCRIPT_OBJECT_GETTER(SurfaceHeightGetter, SDL_Surface, surface_proto, JS_NumberValue(that->h))
SAPPHIRE_SCRIPT_OBJECT_GETTER(ImageWidthGetter, ScriptImage_t, image_proto, JS_NumberValue((*that)->Width()))
SAPPHIRE_SCRIPT_OBJECT_GETTER(ImageHeightGetter, ScriptImage_t, image_proto, JS_NumberValue((*that)->Height()))

SAPPHIRE_SCRIPT_OBJECT_GETTER(ColorRedGetter, TS_Color, color_proto, JS_NumberValue(that->red))
SAPPHIRE_SCRIPT_OBJECT_SETTER_NUMBER(ColorRedSetter, TS_Color, color_proto, that->red = NumberToColorChannel(args[0].toNumber()))
SAPPHIRE_SCRIPT_OBJECT_GETTER(ColorGreenGetter, TS_Color, color_proto, JS_NumberValue(that->green))
SAPPHIRE_SCRIPT_OBJECT_SETTER_NUMBER(ColorGreenSetter, TS_Color, color_proto, that->green = NumberToColorChannel(args[0].toNumber()))
SAPPHIRE_SCRIPT_OBJECT_GETTER(ColorBlueGetter, TS_Color, color_proto, JS_NumberValue(that->blue))
SAPPHIRE_SCRIPT_OBJECT_SETTER_NUMBER(ColorBlueSetter, TS_Color, color_proto, that->blue = NumberToColorChannel(args[0].toNumber()))
SAPPHIRE_SCRIPT_OBJECT_GETTER(ColorAlphaGetter, TS_Color, color_proto, JS_NumberValue(that->alpha))
SAPPHIRE_SCRIPT_OBJECT_SETTER_NUMBER(ColorAlphaSetter, TS_Color, color_proto, that->alpha = NumberToColorChannel(args[0].toNumber()))

SAPPHIRE_SCRIPT_GROUP_GETTER(GroupXGetter, JS_NumberValue(that->GetX()))
SAPPHIRE_SCRIPT_GROUP_SETTER_NUMBER(GroupXSetter, that->SetX(args[0].toNumber()))

SAPPHIRE_SCRIPT_GROUP_GETTER(GroupYGetter, JS_NumberValue(that->GetY()))
SAPPHIRE_SCRIPT_GROUP_SETTER_NUMBER(GroupYSetter, that->SetY(args[0].toNumber()))

SAPPHIRE_SCRIPT_GROUP_GETTER(GroupRotXGetter, JS_NumberValue(that->GetRotX()))
SAPPHIRE_SCRIPT_GROUP_SETTER_NUMBER(GroupRotXSetter, that->SetRotX(args[0].toNumber()))

SAPPHIRE_SCRIPT_GROUP_GETTER(GroupRotYGetter, JS_NumberValue(that->GetRotY()))
SAPPHIRE_SCRIPT_GROUP_SETTER_NUMBER(GroupRotYSetter, that->SetRotY(args[0].toNumber()))

SAPPHIRE_SCRIPT_GROUP_GETTER(GroupAngleGetter,JS_NumberValue(that->GetRotation<double>()))
SAPPHIRE_SCRIPT_GROUP_SETTER_NUMBER(GroupAngleSetter, that->SetRotation(args[0].toNumber()))

bool GroupSetter(JSContext *ctx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp, JS::ObjectOpResult &result){
    JS::RootedValue id_value(ctx);
    if(!JS_IdToValue(ctx, id, &id_value)){
        Turbo::SetError(ctx, std::string(BRACKNAME " GroupSetter Error can not decode property ID"), result);
        return false;
    }
    
    struct Turbo::JSStringHolder<> file(ctx, JS_EncodeString(ctx, id_value.toString()));
    
    if(strcmp(file.string, "shader_program")==0){
        if((!vp.isObject()) || vp.isNull() || vp.isUndefined()){
            Turbo::SetError(ctx, std::string(BRACKNAME " GroupSetter ") + file.string + " Error value is not a valid object", result);
            return false;
        }
        
        std::shared_ptr<Galileo::Shader> *shader_program = shader_program_proto.unwrap(ctx, vp.toObjectOrNull(), nullptr);
        if(!shader_program){
            Turbo::SetError(ctx, std::string(BRACKNAME " GroupSetter ") + file.string + " Error value is not a ShaderProgram", result);
            return false;
        }
        
        group_proto.unsafeUnwrap(obj)->SetShader(*shader_program);
        result.succeed();
        return true;
    }
    else if(strcmp(file.string, "shapes")==0){
        if(!(JS_IsArrayObject(ctx, vp))){
            Turbo::SetError(ctx, std::string(BRACKNAME " GroupSetter ") + file.string + " Error value is not an Array", result);
            return false;
        }
        JS::RootedObject shapes_array(ctx, vp.toObjectOrNull());
        if(!GroupSetNativeShapes(ctx, group_proto.unsafeUnwrap(obj), shapes_array)){
            result.fail(1);
            return false;
        }
        
        result.succeed();
        return true;
    }
    result.succeed();
    return true;
}

void ColorFinalizer(JSFreeOp *fop, JSObject *obj){
    delete color_proto.unsafeUnwrap(obj);
}

void SurfaceFinalizer(JSFreeOp *fop, JSObject *obj){
    SDL_Surface *surface = surface_proto.unsafeUnwrap(obj);
    SDL_FreeSurface(surface);
}

void ImageFinalizer(JSFreeOp *fop, JSObject *obj){
    delete image_proto.unsafeUnwrap(obj);
}

void ShapeFinalizer(JSFreeOp *fop, JSObject *obj){
    delete shape_proto.unsafeUnwrap(obj);
}

void GroupFinalizer(JSFreeOp *fop, JSObject *obj){
    delete group_proto.unsafeUnwrap(obj);
}

void ShaderProgramFinalizer(JSFreeOp *fop, JSObject *obj){
    delete shader_program_proto.unsafeUnwrap(obj);
}


const unsigned FIXEDUVCOORD_SIZE = 8u;

const float FixedUV1[2]  = {
    0.0f, 1.0f
  };

const float FixedUV2[4]  = {
    0.0f, 1.0f, 1.0f, 1.0f
  };

const float FixedUV3[6]  = {
    0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
  };

const float FixedUV4[8]  = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
  };

const float FixedUV5[10] = {
    0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f,
  };

const float FixedUV6[12] = {
    0.0f, 0.5f, 1.0f/3.0f, 1.0f, 2.0f/3.0f, 1.0f, 1.0f, 0.5f, 2.0f/3.0f, 0.0f, 1.0f/3.0f, 0.0f,
  };
const float FixedUV7[14] = {
    0.0f, 0.5f, 1.0f/3.0f, 1.0f, 2.0f/3.0f, 1.0f, 1.0f, 0.5f, 2.0f/3.0f, 0.0f, 1.0f/3.0f, 0.0f, 0.0f, 0.0f,
  };

const float FixedUV8[16] = {
    0.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
  };

const float *FixedUVCoord[FIXEDUVCOORD_SIZE] = { // Known to be disproportionate.
  FixedUV1,
  FixedUV2,
  FixedUV3,
  FixedUV4,
  FixedUV5,
  FixedUV6,
  FixedUV7,
  FixedUV8
};

Turbo::JSPrototype<TS_Color>         color_proto("Color", ColorCtor, 3, ColorFinalizer);
Turbo::JSPrototype<SDL_Surface>      surface_proto("Surface", SurfaceCtor, 1, SurfaceFinalizer);
Turbo::JSPrototype<ScriptImage_t>    image_proto("Image", ImageCtor, 1, ImageFinalizer);
Turbo::JSPrototype<Galileo::Shape>   shape_proto("Shape", ShapeCtor, 2, ShapeFinalizer);
Turbo::JSPrototype<Galileo::Group>   group_proto("Group", GroupCtor, 2, GroupFinalizer, GroupSetter);
Turbo::JSPrototype<std::shared_ptr<Galileo::Shader> > shader_program_proto("ShaderProgram", ShaderProgramCtor, 1, ShaderProgramFinalizer);

std::array<JSNative, NumFuncs> FunctionList = {{
    FlipScreen,
    ArrayBufferToSurface,
    ArrayBufferToImage,
    GetDefaultShaderProgram,
    GetScreenWidth,
    GetScreenHeight,

}};

std::array<const char * const, NumFuncs> FunctionNameList = {{
    "FlipScreen",
    "SurfaceFromArrayBuffer",
    "ImageFromArrayBuffer",
    "GetDefaultShaderProgram",
    "GetScreenWidth",
    "GetScreenHeight",
}};

static JSFunctionSpec group_methods[] = {
    JS_FN("draw", DrawGroup, 0, 0),
    JS_FN("setX", GroupXSetter, 1, 0),
    JS_FN("setY", GroupYSetter, 1, 0),
    JS_FN("getX", GroupXGetter, 0, 0),
    JS_FN("getY", GroupYGetter, 0, 0),
    JS_FS_END
};

static JSPropertySpec group_properties[] = {
    JS_PSGS("x", GroupXGetter, GroupXSetter, 0),
    JS_PSGS("y", GroupYGetter, GroupYSetter, 0),
    JS_PSGS("angle", GroupAngleGetter,  GroupAngleSetter, 0),
    JS_PSGS("rotX", GroupRotXGetter,  GroupRotXSetter, 0),
    JS_PSGS("rotY", GroupRotYGetter,  GroupRotYSetter, 0),
    JS_PS_END
};

static JSPropertySpec shape_properties[] = {
    JS_PSGS("image", ShapeImageGetter, ShapeImageSetter, 0),
    JS_PS_END
};

static JSFunctionSpec image_methods[] = {
    JS_FN("clone", CloneImage, 0, 0),
    JS_FN("createSurface", ImageCreateSurface, 0, 0),
    JS_FN("update", ImageUpdate, 1, 0),
    JS_FN("updateAt", ImageUpdateAt, 3, 0),
    JS_FS_END
};
    
static JSPropertySpec image_properties[] = {
    JS_PSG("width", ImageWidthGetter, 0),
    JS_PSG("height", ImageHeightGetter, 0),
    JS_PS_END
};

static JSFunctionSpec surface_methods[] = {
    JS_FN("clone", CloneSurface, 0, 0),
    JS_FN("save", SaveSurface, 0, 0),
    JS_FN("setPixel", SetPixelSurface, 3, 0),
    JS_FN("blitSurface", SurfaceBlitSurface, 3, 0),
    JS_FN("setClippingRectangle", SurfaceSetClippingRectangle, 4, 0),
    JS_FN("line", LineSurface, 3, 0),
    JS_FN("gradientLine", GradientLineSurface, 5, 0),
    JS_FN("triangle", TriangleSurface, 7, 0),
    JS_FN("gradientTriangle", GradientTriangleSurface, 9, 0),
    JS_FN("outlinedTriangle", OutlinedTriangleSurface, 7, 0),
    JS_FN("rectangle", RectangleSurface, 3, 0),
    JS_FN("gradientRectangle", GradientRectangleSurface, 8, 0),
    JS_FN("outlinedRectangle", OutlinedRectangleSurface, 3, 0),
    JS_FN("quad", QuadSurface, 9, 0),
    JS_FN("gradientQuad", GradientQuadSurface, 12, 0),
    JS_FN("outlinedQuad", OutlinedQuadSurface, 9, 0),
    JS_FN("circle", CircleSurface, 4, 0),
    JS_FN("filledCircle", CircleSurface, 4, 0),
    JS_FN("gradientCircle", GradientCircleSurface, 4, 0),
    JS_FN("outlinedCircle", OutlinedCircleSurface, 4, 0),
    JS_FS_END
};

static JSPropertySpec surface_properties[] = {
    JS_PSG("width", SurfaceWidthGetter, 0),
    JS_PSG("height", SurfaceHeightGetter, 0),
    JS_PS_END
};

static JSPropertySpec color_properties[] = {
    JS_PSGS("red", ColorRedGetter, ColorRedSetter, 0),
    JS_PSGS("green", ColorGreenGetter, ColorGreenSetter, 0),
    JS_PSGS("blue", ColorBlueGetter, ColorBlueSetter, 0),
    JS_PSGS("alpha", ColorAlphaGetter, ColorAlphaSetter, 0),
    JS_PS_END
};

void initScript(JSContext *ctx){
    
    group_proto.initForContext(ctx, group_properties, group_methods);
    shape_proto.initForContext(ctx, shape_properties);
    image_proto.initForContext(ctx, image_properties, image_methods);
    surface_proto.initForContext(ctx, surface_properties, surface_methods);
    color_proto.initForContext(ctx, color_properties);
    shader_program_proto.initForContext(ctx);
    
}

void closeScript(JSContext *ctx){
    
    group_proto.closeForContext(ctx);
    shape_proto.closeForContext(ctx);
    image_proto.closeForContext(ctx);
    surface_proto.closeForContext(ctx);
    color_proto.closeForContext(ctx);
    shader_program_proto.closeForContext(ctx);
    
}

bool FlipScreen(JSContext *ctx, unsigned argc, JS::Value *vp){

    GL::ThreadKit &lKit = *GL::GetSystemThreadkit();

    lKit.monitor.Lock();
    lKit.Queues[lKit.draw_to].push(new Sapphire::Galileo::FlipScreen());

    GL::AdvanceDrawQueue<GL::ThreadKit &>(lKit);

    while(!lKit.Queues[lKit.draw_to].empty()){

        if(!lKit.Queues[lKit.draw_to].front()->IsPersistent())
            delete lKit.Queues[lKit.draw_to].front();

        lKit.Queues[lKit.draw_to].pop();
    }

    lKit.monitor.Unlock();
    lKit.monitor.Notify();

    return true;

}

/////
// Old School
bool ColorCtor(JSContext *ctx, unsigned argc, JS::Value *vp){

    JS::CallArgs args = CallArgsFromVp(argc, vp);
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Number};
    if(!Turbo::CheckSignature<3>(ctx, args, signature, __func__))
        return false;
    
    unsigned alpha = 0xFF;
    
    if(args.length()>3){
        if(!Turbo::CheckArg(ctx, args[3], Turbo::Number)){
            Turbo::SetError(ctx, Turbo::BadArgTypeErrorMessage(3, Turbo::Number, __func__));
            return false;
        }
        alpha = NumberToColorChannel(args[3].toNumber());
    }
    
    TS_Color * const rColor = new TS_Color(NumberToColorChannel(args[0].toNumber()), 
                                           NumberToColorChannel(args[1].toNumber()),
                                           NumberToColorChannel(args[2].toNumber()),
                                           alpha);
    
    args.rval().set(OBJECT_TO_JSVAL(color_proto.wrap(ctx, rColor)));
    return true;
}

// Can construct an Image from an SDL_Surface, a path, or dimensions and a color.
bool ImageCtor(JSContext *ctx, unsigned argc, JS::Value *vp){
    
    const Turbo::JSType sig_surface[] = {Turbo::Object};
    const Turbo::JSType sig_parametric[] = {Turbo::Number, Turbo::Number, Turbo::Object};
    const Turbo::JSType sig_path[] = {Turbo::String};
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(Turbo::CheckSignature<1>(ctx, args, sig_surface, __func__, false) && surface_proto.instanceOf(ctx, args[0], &args)){
        const SDL_Surface *surface = surface_proto.unsafeUnwrap(args[0]); // We checked above if we have an instance of a Surface
        assert(surface);
        
        Image *image = new Image(surface);
        
        args.rval().set(OBJECT_TO_JSVAL(image_proto.wrap(ctx, new ScriptImage_t(image))));
        return true;
    }
    
    if(Turbo::CheckSignature<1>(ctx, args, sig_path, __func__, false)){
        struct Turbo::JSStringHolder<> file(ctx, JS_EncodeString(ctx, args[0].toString()));
        
        if(!file.string){
            Turbo::SetError(ctx, BRACKNAME " ImageCtor Error could not encode string for argument 0");
            return false;
        }
        
        const SDL_Surface *surface = LoadSurface(file.string, TS_GetContextEnvironment(ctx)->directories);
        
        if(!surface){
            Turbo::SetError(ctx, std::string(BRACKNAME " ImageCtor Error could not load Image ") + file.string);
            return false;
        }
        
        Image *image = new Image(surface);
        SDL_FreeSurface((SDL_Surface *)surface);
        
        args.rval().set(OBJECT_TO_JSVAL(image_proto.wrap(ctx, new ScriptImage_t(image))));
        return true;
        
    }
    
    if(!Turbo::CheckSignature<3>(ctx, args, sig_parametric, __func__))
        return false;

    TS_Color *color = color_proto.unwrap(ctx, args[2], &args);
    if(!color)
        return false;
    else{
        const unsigned w = std::max<int>(0, args[0].toNumber()), h = std::max<int>(0, args[1].toNumber()), pixel_count = w*h;

        uint32_t *pixels = new uint32_t[pixel_count];
        std::fill(pixels, &(pixels[pixel_count]), color->toInt());

        Image *image = new Image(pixels, w, h);
        
        delete[] pixels;
        
        args.rval().set(OBJECT_TO_JSVAL(image_proto.wrap(ctx, new ScriptImage_t(image))));
        return true;
    }

}

bool SurfaceCtor(JSContext *ctx, unsigned argc, JS::Value *vp){

    const Turbo::JSType sig_surface[] = {Turbo::Object};
    const Turbo::JSType sig_parametric[] = {Turbo::Number, Turbo::Number, Turbo::Object};
    const Turbo::JSType sig_path[] = {Turbo::String};

    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(Turbo::CheckSignature<1>(ctx, args, sig_surface, __func__, false) && image_proto.instanceOf(ctx, args[0], &args)){
        
        ScriptImage_t *image_holder = image_proto.unsafeUnwrap(args[0]); // We checked above if we have an instance of a Image
        assert(image_holder);
        Sapphire::Image *image = image_holder->get();
        assert(image);

        SDL_Surface *surface = GenerateSurface(image->Width(), image->Height());

        SDL_LockSurface(surface);
        image->CopyData(surface->pixels);

        SDL_UnlockSurface(surface);

        args.rval().set(OBJECT_TO_JSVAL(surface_proto.wrap(ctx, surface)));
        
        return true;
    }
    
    if(Turbo::CheckSignature<1>(ctx, args, sig_path, __func__, false)){
        struct Turbo::JSStringHolder<> file(ctx, JS_EncodeString(ctx, args[0].toString()));
        
        if(!file.string){
            Turbo::SetError(ctx, BRACKNAME " SurfaceCtor Error could not encode string for argument 0");
            return false;
        }
        
        SDL_Surface *surface = LoadSurface(file.string, TS_GetContextEnvironment(ctx)->directories);
        
        if(!surface){
            Turbo::SetError(ctx, std::string(BRACKNAME " SurfaceCtor Error could not load Image ") + file.string);
            return false;
        }
        
        args.rval().set(OBJECT_TO_JSVAL(surface_proto.wrap(ctx, surface)));
        
        return true;
        
    }
    
    if(!Turbo::CheckSignature<3>(ctx, args, sig_parametric, __func__))
        return false;

    TS_Color *color = color_proto.unwrap(ctx, args[2], &args);
    if(!color){
        Turbo::SetError(ctx, BRACKNAME " SurfaceCtor Error argument 2 is not a color.");
        return false;
    }
    else{
        const unsigned w = std::max<int>(0, args[0].toNumber()), h = std::max<int>(0, args[1].toNumber()), pixel_count = w*h;

        SDL_Surface *surface = GenerateSurface(w, h);
        assert(surface);
        
        if(w && h){
            SDL_LockSurface(surface);
            uint32_t *pixels = static_cast<uint32_t *>(surface->pixels);
            
            assert(pixels);
            assert(color);
            
            std::fill(pixels, &(pixels[pixel_count]), color->toInt());
            SDL_UnlockSurface(surface);
        }
        args.rval().set(OBJECT_TO_JSVAL(surface_proto.wrap(ctx, surface)));
        return true;
    }
}

struct VertexProperties{
    bool has_color, has_u, has_v;
    int x, y;
    float u, v;
    TS_Color *color;
};

// assert if in debug, still execute otherwise.
#ifndef NDEBUG
#define ASSERT_ALWAYS_EXECUTE(A) assert(A)
#else
#define ASSERT_ALWAYS_EXECUTE(A) A
#endif

// This will fill out the vertex's x and y from `that'. If x and y do not exist on `that', return false.
// If a u, v, and/or color properties exist on `that' and they are the correct type, they will be applied
// to the vertex.
// If the u, v, and/or color properties exist and are the wrong type, return false.
// Otherwise, return true.
static inline bool ValidateVertex(JSContext *ctx, JS::HandleObject that, Sapphire::Galileo::Vertex &vertex){

    bool has_x, has_y, has_color, has_u, has_v;
    
    #define TEST_PROP(P) ASSERT_ALWAYS_EXECUTE(JS_HasProperty(ctx, that, #P, &has_##P))
    TEST_PROP(x);
    TEST_PROP(y);
    TEST_PROP(u);
    TEST_PROP(v);
    TEST_PROP(color);
    #undef TEST_PROP
    
    if(!(has_x && has_y))
        return false;
    
    #define ENSURE_PROP(NAME, VALUATOR, WRAPPER)\
    if(has_##NAME){\
        JS::RootedValue out_value(ctx);\
        JS_GetProperty(ctx, that, #NAME, &out_value);\
        if(!(VALUATOR)){\
            return false;\
        }\
        WRAPPER\
    }
    
    ENSURE_PROP(x, true, vertex.x = out_value.toNumber();)
    ENSURE_PROP(y, true, vertex.y = out_value.toNumber();)
    ENSURE_PROP(u, out_value.isNumber(), vertex.u = out_value.toNumber();)
    ENSURE_PROP(v, out_value.isNumber(), vertex.v = out_value.toNumber();)
    ENSURE_PROP(color, out_value.isObject() && color_proto.instanceOf(ctx, out_value, nullptr),
        {TS_Color *color = color_proto.unsafeUnwrap(out_value.toObjectOrNull());
        vertex.color = TS_Color(color->red, color->green, color->blue, color->alpha);}
    )
    
    #undef ENSURE_PROP
    
    return true;
}

#undef ASSERT_ALWAYS_EXECUTE

/////
// New School!
bool ShapeCtor(JSContext *ctx, unsigned argc, JS::Value *vp){
    
    const Turbo::JSType signature[] = {Turbo::Array, Turbo::Object};
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<2>(ctx, args, signature, __func__))
        return false;
    
    ScriptImage_t *image_holder = image_proto.unwrap(ctx, args[1], &args);
    if(!image_holder)
        return false;
    
    // We checked if it was an array, toObjectOrNull() will always return non-NULL.
    JS::RootedObject vertex_array(ctx, args[0].toObjectOrNull());
    unsigned num_vertices;
    JS_GetArrayLength(ctx, vertex_array, &num_vertices);
    
    std::vector<Sapphire::Galileo::Vertex> vertices; 
    vertices.resize(num_vertices);
    
    // Fill with the default u, v, and color values. These will be overwritten if they exist on the JS object.
    // Currently we just zero-out the UV if it is not a recognized size. Ideally we woulduse UV values that
    // would correspond to a circular approximation.
    if((num_vertices>0) && (num_vertices<FIXEDUVCOORD_SIZE)){
        for(int i = 0; i<num_vertices; i++){
            vertices[i].u = FixedUVCoord[num_vertices-1][(i*2)  ];
            vertices[i].v = FixedUVCoord[num_vertices-1][(i*2)+1];
            vertices[i].color = TS_Color(0xFF, 0xFF, 0xFF, 0xFF);
        }
    }
    else{
        for(int i = 0; i<num_vertices; i++){
            vertices[i].u = 0.0f;
            vertices[i].v = 0.0f;
            vertices[i].color = TS_Color(0xFF, 0xFF, 0xFF, 0xFF);
        }
    }
    
    for(int i = 0; i<num_vertices; i++){
        
        // Ensure we actually get the element. Probably not necessary to verify.
        JS::RootedValue element(ctx), property(ctx);
        if(!JS_GetElement(ctx, vertex_array, i, &element)){
            Turbo::SetError(ctx, std::string(BRACKNAME " ShapeCtor Error could not access element ") + std::to_string(i) + " of argument 0");
            return false;
        }
        
        // Test that the element is an object, as opposed to a value.
        JSObject *obj = element.toObjectOrNull();
        if(!obj){
            Turbo::SetError(ctx, std::string(BRACKNAME " ShapeCtor Error element ") + std::to_string(i) + " of argument 0 is not an object");
            return false;
        }
        
        // Check that works as a Vertex, and fill in the x and y.
        // The UV and color will be set if they exist.
        JS::RootedObject element_obj(ctx, obj);
        if(!ValidateVertex(ctx, element_obj, vertices[i])){
            Turbo::SetError(ctx, std::string(BRACKNAME " ShapeCtor Error invalid vertex element ") + std::to_string(i) + " of argument 0");
            return false;
        }
    }
    
    // Build and initialize the shape
    Sapphire::Galileo::Shape *shape = new Galileo::Shape(vertices, *image_holder);
    shape->FillGL();
    
    // Wrap it.
    args.rval().set(OBJECT_TO_JSVAL(shape_proto.wrap(ctx, shape)));    
    return true;
}

// Sets a pending error if Shape doesn't exist and returns nullptr
inline Galileo::Shape *GetArrayShape(JSContext *ctx, unsigned i, JS::HandleObject shape_array){
    JS::RootedValue element(ctx);
    JS_GetElement(ctx, shape_array, i, &element);
    Galileo::Shape *shape = shape_proto.unwrap(ctx, element.toObjectOrNull(), nullptr);
    
    if(!shape)
        Turbo::SetError(ctx, std::string(BRACKNAME " GetArrayShape Error element ") + std::to_string(i) + " is not a Shape");
    
    return shape;
}

// Updates the native-side Shape array to match the script-side array
bool GroupSetNativeShapes(JSContext *ctx, Galileo::Group *group, JS::HandleObject shape_array){
    unsigned num_vertices;
    JS_GetArrayLength(ctx, shape_array, &num_vertices);
    struct Turbo::Monitor::Locker locker = group->createLocker();
    
    Galileo::Group::iterator iter = group->begin();
    int i = 0;
    
    while((i<num_vertices) && (iter!=group->end())){
        
        Galileo::Shape *shape = GetArrayShape(ctx, i, shape_array);
        if(!shape)
            return false;
        
        if(shape!=*iter){
            iter = group->erase(iter);
            group->insert(iter, shape);
        }
        
        i++; 
        iter++;
    }

    while(i<num_vertices){
        Galileo::Shape *shape = GetArrayShape(ctx, i, shape_array);
        if(!shape)
            return false;
        
        group->push(shape);
        i++;
        iter++;
    }
    
    return true;
    
}

bool GroupCtor(JSContext *ctx, unsigned argc, JS::Value *vp){
        
    const Turbo::JSType signature[] = {Turbo::Array, Turbo::Object};
    std::shared_ptr<Galileo::Shader> shader_program;
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<2>(ctx, args, signature, __func__))
        return false;
    
    Galileo::Group *group_native = new Galileo::Group();
    
    JSObject *group_object = group_proto.wrap(ctx, group_native);
    JS::RootedObject group_handle(ctx, group_object);
    
    if(!(JS_SetProperty(ctx, group_handle, "shader_program", args[1]) &&
        JS_SetProperty(ctx, group_handle, "shapes", args[0]))){
        return false;
    }
    
    args.rval().set(OBJECT_TO_JSVAL(group_handle));
    
    return true;
}

bool DrawGroup(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    Galileo::Group *mGroup = group_proto.getSelf(ctx, vp, &args);
    if(!mGroup)
        return false;
        
    Turbo::Monitor monitor(Sapphire::GL::GetSystemThreadkit()->monitor);
    
    monitor.Lock();
    mGroup->lock();
    
    mGroup->DrawAll(&(Sapphire::GL::GetSystemThreadkit()->Queues[Sapphire::GL::GetSystemThreadkit()->draw_to]));
    
    monitor.Unlock();
    mGroup->unlock();
    
    return true;
    
}

bool CloneSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
    
    const unsigned w = mSurface->w, h = mSurface->h;
    
    SDL_Surface *surface = GenerateSurface(w, h);

    SDL_LockSurface(surface);
    SDL_LockSurface(mSurface);
    
    memcpy(surface->pixels, mSurface->pixels, w*h*4);
    
    SDL_LockSurface(mSurface);
    SDL_LockSurface(surface);
    
    args.rval().set(OBJECT_TO_JSVAL(surface_proto.wrap(ctx, surface)));
    
    return true;
}

bool SaveSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::String};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<1>(ctx, args, signature, __func__))
        return false;
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
    
    struct Turbo::JSStringHolder<> file(ctx, JS_EncodeString(ctx, args[0].toString()));
    
    if(Save::Save(mSurface, file.string)!=Save::SaveStatus::ssSuccess){
        Turbo::SetError(ctx, std::string(BRACKNAME " SaveSurface Error could not save ") + file.string);
        return false;
    }
    
    return true;
}

bool SetPixelSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Object};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<3>(ctx, args, signature, __func__))
        return false;
    
    TS_Color *color = color_proto.unwrap(ctx, args[2], &args);
    if(!color){
        Turbo::SetError(ctx, std::string(BRACKNAME " SetPixelSurface Error argument 2 is not a Color"));
        return false;
    }
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
    const unsigned x = args[0].toNumber(), y = args[1].toNumber();
    
    SDL_LockSurface(mSurface);
    static_cast<uint32_t *>(mSurface->pixels)[x + (y*mSurface->w)] = color->toInt();
    SDL_UnlockSurface(mSurface);
    
    return true;
}

bool LineSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Object};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<5>(ctx, args, signature, __func__))
        return false;
    
    TS_Color *color = color_proto.unwrap(ctx, args[4], &args);
    if(!color){
        Turbo::SetError(ctx, std::string(BRACKNAME " LineSurface Error argument 4 is not a Color"));
        return false;
    }
    
    const struct Surface::vertex vertices[2] = {
        {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber())}, *color},
        {{static_cast<int>(args[2].toNumber()), static_cast<int>(args[3].toNumber())}, *color}
    };
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
    
    Surface::SurfaceOperator::Line(mSurface, vertices);
    
    return true;
}

bool GradientLineSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Object, Turbo::Object};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<6>(ctx, args, signature, __func__))
        return false;
    
    TS_Color *color1 = color_proto.unwrap(ctx, args[4], &args), *color2 = color_proto.unwrap(ctx, args[5], &args);
    if(!color1){
        Turbo::SetError(ctx, std::string(BRACKNAME " GradientLineSurface Error argument 4 is not a Color"));
        return false;
    }
    if(!color2){
        Turbo::SetError(ctx, std::string(BRACKNAME " GradientLineSurface Error argument 5 is not a Color"));
        return false;
    }
    
    const struct Surface::vertex vertices[2] = {
        {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber())}, *color1},
        {{static_cast<int>(args[2].toNumber()), static_cast<int>(args[3].toNumber())}, *color2}
    };
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
    
    Surface::SurfaceOperator::Line(mSurface, vertices);
    
    return true;
}

bool TriangleSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Object};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<7>(ctx, args, signature, __func__))
        return false;
    
    TS_Color *color = color_proto.unwrap(ctx, args[6], &args);
    if(!color){
        Turbo::SetError(ctx, std::string(BRACKNAME " TriangleSurface Error argument 6 is not a Color"));
        return false;
    }
    
    const struct Surface::vertex vertices[3] = {
        {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber())}, *color},
        {{static_cast<int>(args[2].toNumber()), static_cast<int>(args[3].toNumber())}, *color},
        {{static_cast<int>(args[4].toNumber()), static_cast<int>(args[5].toNumber())}, *color}
    };
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
    
    Surface::SurfaceOperator::Triangle(mSurface, vertices);
    
    return true;
}

bool GradientTriangleSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, 
                                       Turbo::Number, Turbo::Number,
                                       Turbo::Number, Turbo::Number, 
                                       Turbo::Object, Turbo::Object, Turbo::Object};
                                       
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<9>(ctx, args, signature, __func__))
        return false;
    
    TS_Color *colors[3] = {color_proto.unwrap(ctx, args[6], &args), 
                          color_proto.unwrap(ctx, args[7], &args), 
                          color_proto.unwrap(ctx, args[8], &args)};
    if(!colors[0]){
        Turbo::SetError(ctx, std::string(BRACKNAME " GradientTriangleSurface Error argument 6 is not a Color"));
        return false;
    }   
    if(!colors[1]){
        Turbo::SetError(ctx, std::string(BRACKNAME " GradientTriangleSurface Error argument 7 is not a Color"));
        return false;
    }
    if(!colors[2]){
        Turbo::SetError(ctx, std::string(BRACKNAME " GradientTriangleSurface Error argument 8 is not a Color"));
        return false;
    }
    
    const struct Surface::vertex vertices[3] = {
        {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber())}, *(colors[0])},
        {{static_cast<int>(args[2].toNumber()), static_cast<int>(args[3].toNumber())}, *(colors[1])},
        {{static_cast<int>(args[4].toNumber()), static_cast<int>(args[5].toNumber())}, *(colors[2])}
    };
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
    
    Surface::SurfaceOperator::Triangle(mSurface, vertices);
    
    return true;
}

bool OutlinedTriangleSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Object};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<7>(ctx, args, signature, __func__))
        return false;
    
    TS_Color *color = color_proto.unwrap(ctx, args[6], &args);
    if(!color){
        Turbo::SetError(ctx, std::string(BRACKNAME " OutlinedTriangleSurface Error argument 6 is not a Color"));
        return false;
    }
    
    const struct Surface::vertex vertices[4] = {
        {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber())}, *color}, 
        {{static_cast<int>(args[2].toNumber()), static_cast<int>(args[3].toNumber())}, *color},
        {{static_cast<int>(args[4].toNumber()), static_cast<int>(args[5].toNumber())}, *color}, 
        {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber())}, *color},
    };
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
    
    Surface::SurfaceOperator::Line(mSurface, &vertices[0]);
    Surface::SurfaceOperator::Line(mSurface, &vertices[1]);
    Surface::SurfaceOperator::Line(mSurface, &vertices[2]);
    
    return true;
}

bool RectangleSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Object};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<5>(ctx, args, signature, __func__))
        return false;
    
    TS_Color *color = color_proto.unwrap(ctx, args[4], &args);
    if(!color){
        Turbo::SetError(ctx, std::string(BRACKNAME " RectangleSurface Error argument 4 is not a Color"));
        return false;
    }
    
    const struct Surface::vertex vertices[4] = {
        {{static_cast<int>(args[0].toNumber()),                    static_cast<int>(args[1].toNumber())}, *color}, 
        {{static_cast<int>(args[0].toNumber()+args[2].toNumber()), static_cast<int>(args[1].toNumber())}, *color},
        {{static_cast<int>(args[0].toNumber()+args[2].toNumber()), static_cast<int>(args[1].toNumber()+args[3].toNumber())}, *color},
        {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber()+args[3].toNumber())}, *color}
    };
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
    
    Surface::SurfaceOperator::Quad(mSurface, vertices);
    
    return true;
}
bool GradientRectangleSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
        const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Object, Turbo::Object, Turbo::Object, Turbo::Object};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<8>(ctx, args, signature, __func__))
        return false;
    
    TS_Color *color1 = color_proto.unwrap(ctx, args[4], &args);
    TS_Color *color2 = color_proto.unwrap(ctx, args[5], &args);
    TS_Color *color3 = color_proto.unwrap(ctx, args[6], &args);
    TS_Color *color4 = color_proto.unwrap(ctx, args[7], &args);
    if(!color1){
        Turbo::SetError(ctx, std::string(BRACKNAME " GradientRectangleSurface Error argument 4 is not a Color"));
        return false;
    }
    if(!color2){
        Turbo::SetError(ctx, std::string(BRACKNAME " GradientRectangleSurface Error argument 5 is not a Color"));
        return false;
    }
    if(!color3){
        Turbo::SetError(ctx, std::string(BRACKNAME " GradientRectangleSurface Error argument 6 is not a Color"));
        return false;
    }
    if(!color4){
        Turbo::SetError(ctx, std::string(BRACKNAME " GradientRectangleSurface Error argument 7 is not a Color"));
        return false;
    }
    
    const struct Surface::vertex vertices[4] = {
        {{static_cast<int>(args[0].toNumber()),                    static_cast<int>(args[1].toNumber())}, *color1}, 
        {{static_cast<int>(args[0].toNumber()+args[2].toNumber()), static_cast<int>(args[1].toNumber())}, *color2},
        {{static_cast<int>(args[0].toNumber()+args[2].toNumber()), static_cast<int>(args[1].toNumber()+args[3].toNumber())}, *color3},
        {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber()+args[3].toNumber())}, *color4}
    };
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
    
    Surface::SurfaceOperator::Quad(mSurface, vertices);
    
    return true;
}

bool OutlinedRectangleSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Object};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<5>(ctx, args, signature, __func__))
        return false;
    
    TS_Color *color = color_proto.unwrap(ctx, args[4], &args);
    if(!color){
        Turbo::SetError(ctx, std::string(BRACKNAME " OutlinedRectangleSurface Error argument 4 is not a Color"));
        return false;
    }
    
    const struct Surface::vertex vertices[5] = {
        {{static_cast<int>(args[0].toNumber()),                    static_cast<int>(args[1].toNumber())}, *color}, 
        {{static_cast<int>(args[0].toNumber()+args[2].toNumber()), static_cast<int>(args[1].toNumber())}, *color},
        {{static_cast<int>(args[0].toNumber()+args[2].toNumber()), static_cast<int>(args[1].toNumber()+args[3].toNumber())}, *color},
        {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber()+args[3].toNumber())}, *color},
        {{static_cast<int>(args[0].toNumber()),                    static_cast<int>(args[1].toNumber())}, *color}, 
    };
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
    
    Surface::SurfaceOperator::Line(mSurface, &vertices[0]);
    Surface::SurfaceOperator::Line(mSurface, &vertices[1]);
    Surface::SurfaceOperator::Line(mSurface, &vertices[2]);
    Surface::SurfaceOperator::Line(mSurface, &vertices[3]);
    
    return true;
}

bool QuadSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, 
        Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Object};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<9>(ctx, args, signature, __func__))
        return false;
    
    TS_Color *color = color_proto.unwrap(ctx, args[8], &args);
    if(!color){
        Turbo::SetError(ctx, std::string(BRACKNAME " QuadSurface Error argument 8 is not a Color"));
        return false;
    }
    
    const struct Surface::vertex vertices[4] = {
        {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber())}, *color}, 
        {{static_cast<int>(args[2].toNumber()), static_cast<int>(args[3].toNumber())}, *color}, 
        {{static_cast<int>(args[4].toNumber()), static_cast<int>(args[5].toNumber())}, *color}, 
        {{static_cast<int>(args[6].toNumber()), static_cast<int>(args[7].toNumber())}, *color}, 
    };
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
    
    Surface::SurfaceOperator::Quad(mSurface, vertices);
    
    return true;
}

bool GradientQuadSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, 
        Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, 
        Turbo::Object, Turbo::Object, Turbo::Object, Turbo::Object};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<12>(ctx, args, signature, __func__))
        return false;
    
    TS_Color *color1 = color_proto.unwrap(ctx, args[8], &args);
    TS_Color *color2 = color_proto.unwrap(ctx, args[9], &args);
    TS_Color *color3 = color_proto.unwrap(ctx, args[10], &args);
    TS_Color *color4 = color_proto.unwrap(ctx, args[11], &args);
    if(!color1){
        Turbo::SetError(ctx, std::string(BRACKNAME " GradientQuadSurface Error argument 8 is not a Color"));
        return false;
    }
    if(!color2){
        Turbo::SetError(ctx, std::string(BRACKNAME " GradientQuadSurface Error argument 9 is not a Color"));
        return false;
    }
    if(!color3){
        Turbo::SetError(ctx, std::string(BRACKNAME " GradientQuadSurface Error argument 10 is not a Color"));
        return false;
    }
    if(!color4){
        Turbo::SetError(ctx, std::string(BRACKNAME " GradientQuadSurface Error argument 11 is not a Color"));
        return false;
    }
    
    const struct Surface::vertex vertices[4] = {
        {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber())}, *color1}, 
        {{static_cast<int>(args[2].toNumber()), static_cast<int>(args[3].toNumber())}, *color2}, 
        {{static_cast<int>(args[4].toNumber()), static_cast<int>(args[5].toNumber())}, *color3}, 
        {{static_cast<int>(args[6].toNumber()), static_cast<int>(args[7].toNumber())}, *color4}
    };
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
    
    Surface::SurfaceOperator::Quad(mSurface, vertices);
    
    return true;
}

bool OutlinedQuadSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, 
        Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Object};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<9>(ctx, args, signature, __func__))
        return false;
    
    TS_Color *color = color_proto.unwrap(ctx, args[8], &args);
    if(!color){
        Turbo::SetError(ctx, std::string(BRACKNAME " OutlinedRectangleSurface Error argument 8 is not a Color"));
        return false;
    }
    
    const struct Surface::vertex vertices[5] = {
        {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber())}, *color}, 
        {{static_cast<int>(args[2].toNumber()), static_cast<int>(args[3].toNumber())}, *color}, 
        {{static_cast<int>(args[4].toNumber()), static_cast<int>(args[5].toNumber())}, *color}, 
        {{static_cast<int>(args[6].toNumber()), static_cast<int>(args[7].toNumber())}, *color},
        {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber())}, *color}, 
    };
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
    
    Surface::SurfaceOperator::Line(mSurface, &vertices[0]);
    Surface::SurfaceOperator::Line(mSurface, &vertices[1]);
    Surface::SurfaceOperator::Line(mSurface, &vertices[2]);
    Surface::SurfaceOperator::Line(mSurface, &vertices[3]);
    
    return true;
}
            
bool CircleSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Object};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<4>(ctx, args, signature, __func__))
        return false;
    
    TS_Color *color = color_proto.unwrap(ctx, args[3], &args);
    if(!color){
        Turbo::SetError(ctx, std::string(BRACKNAME " CircleSurface Error argument 3 is not a Color"));
        return false;
    }
    const struct Surface::circle circle = {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber())}, static_cast<unsigned>(args[2].toNumber()), {*color, *color}};
        
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
        
    Surface::SurfaceOperator::Circle(mSurface, &circle);
    return true;
}
      
bool GradientCircleSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Object};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<5>(ctx, args, signature, __func__))
        return false;
    
    TS_Color *color1 = color_proto.unwrap(ctx, args[3], &args);
    TS_Color *color2 = color_proto.unwrap(ctx, args[4], &args);
    if(!color1){
        Turbo::SetError(ctx, std::string(BRACKNAME " GradientCircleSurface Error argument 3 is not a Color"));
        return false;
    }
    if(!color2){
        Turbo::SetError(ctx, std::string(BRACKNAME " GradientCircleSurface Error argument 4 is not a Color"));
        return false;
    }
    const struct Surface::circle circle = {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber())}, static_cast<unsigned>(args[2].toNumber()), {*color1, *color2}};
          
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
        
    Surface::SurfaceOperator::Circle(mSurface, &circle);
    return true;
}
          
bool OutlinedCircleSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Object};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<4>(ctx, args, signature, __func__))
        return false;
    
    TS_Color *color = color_proto.unwrap(ctx, args[3], &args);
    if(!color){
        Turbo::SetError(ctx, std::string(BRACKNAME " CircleSurface Error argument 3 is not a Color"));
        return false;
    }
    const struct Surface::circle circle = {{static_cast<int>(args[0].toNumber()), static_cast<int>(args[1].toNumber())}, static_cast<unsigned>(args[2].toNumber()), {*color, *color}};
          
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    if(!mSurface)
        return false;
        
    Surface::SurfaceOperator::OutlinedCircle(mSurface, &circle);
    return true;
}

bool ArrayBufferToImage(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::ArrayBuffer};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<3>(ctx, args, signature, __func__))
        return false;
    
    if(args[0].toNumber()<0){
        Turbo::SetError(ctx, std::string(BRACKNAME " ArrayBufferToImage Error argument 0 is not positive number"));
        return false;
    }
    if(args[1].toNumber()<0){
        Turbo::SetError(ctx, std::string(BRACKNAME " ArrayBufferToImage Error argument 1 is not positive number"));
        return false;
    }
    
    const unsigned w = args[0].toNumber(), h = args[1].toNumber();
    JS::RootedObject buffer_root(ctx, args[2].toObjectOrNull()); 
        
    uint32_t *data;
    uint32_t len;
    
    js::GetArrayBufferLengthAndData(buffer_root, &len, (unsigned char **)(&data));
    
    if(len!=(w*h*4)){
        Turbo::SetError(ctx, std::string(BRACKNAME " ArrayBufferToImage Error argument 2 is of length ") + std::to_string(len) + ", should be " + std::to_string(w*h*4));
        return false;
    }
    
    Image *image = new Image(data, w, h);
    
    args.rval().set(OBJECT_TO_JSVAL(image_proto.wrap(ctx, new ScriptImage_t(image))));
    return true;
}


bool ArrayBufferToSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
   const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::ArrayBuffer};
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<3>(ctx, args, signature, __func__))
        return false;
    
    if(args[0].toNumber()<0){
        Turbo::SetError(ctx, std::string(BRACKNAME " ArrayBufferToSurface Error argument 0 is not positive number"));
        return false;
    }
    if(args[1].toNumber()<0){
        Turbo::SetError(ctx, std::string(BRACKNAME " ArrayBufferToSurface Error argument 1 is not positive number"));
        return false;
    }
    
    const unsigned w = args[0].toNumber(), h = args[1].toNumber();
    JS::RootedObject buffer_root(ctx, args[2].toObjectOrNull()); 
        
    uint32_t *data;
    uint32_t len;
    
    js::GetArrayBufferLengthAndData(buffer_root, &len, (unsigned char **)(&data));
    
    if(len!=(w*h*4)){
        Turbo::SetError(ctx, std::string(BRACKNAME " ArrayBufferToSurface Error argument 2 is of length ") + std::to_string(len) + ", should be " + std::to_string(w*h*4));
        return false;
    }

    SDL_Surface *surface = GenerateSurface(w, h);

    SDL_LockSurface(surface);
    memcpy(surface->pixels, data, len);
    SDL_UnlockSurface(surface);

    args.rval().set(OBJECT_TO_JSVAL(surface_proto.wrap(ctx, surface)));

    return true;

}

bool SaveImage(JSContext *ctx, unsigned argc, JS::Value *vp);

bool ImageCreateSurface(JSContext *ctx, unsigned argc, JS::Value *vp){

    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    ScriptImage_t *mImage = image_proto.getSelf(ctx, vp, &args);
    assert(mImage);
    
    Image *image = mImage->get();
    
    SDL_Surface *surface = GenerateSurface(image->Width(), image->Height());
    assert(surface);

    SDL_LockSurface(surface);
    image->CopyData(surface->pixels);
    SDL_UnlockSurface(surface);

    args.rval().set(OBJECT_TO_JSVAL(surface_proto.wrap(ctx, surface)));
    return true;
}

bool CloneImage(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    args.rval().set(OBJECT_TO_JSVAL(image_proto.wrap(ctx, new ScriptImage_t(new Image(image_proto.getSelf(ctx, vp, &args)->get())))));
    return true;
}

bool ImageUpdate(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Object};
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<1>(ctx, args, signature, __func__))
        return false;

    SDL_Surface * const surface = surface_proto.unwrap(ctx, args[0], &args);
    if(!surface){
        Turbo::SetError(ctx, std::string(BRACKNAME " ") + __func__ + " Error argument 0 is not a Surface");
        return false;
    }
    
    ScriptImage_t * const mImage = image_proto.getSelf(ctx, vp, &args);
    assert(mImage);
    
    const unsigned w = surface->w, h = surface->h;
    
    Image * const image = mImage->get();
    image->Reserve(w, h);
    
    SDL_LockSurface(surface);
    {
        Image::PixelData * &data = image->Lock();
        uint32_t *const pixels = static_cast<uint32_t *>(surface->pixels);
        const Image::PixelDataOutput copier;
        std::transform<uint32_t *, Image::PixelData *, const Image::PixelDataOutput &>(pixels, &pixels[w*h], data, copier);
    }
    SDL_UnlockSurface(surface);

    image->Unlock(w, h);

    args.rval().set(OBJECT_TO_JSVAL(surface_proto.wrap(ctx, surface)));
    return true;
}

bool ImageUpdateAt(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Object, Turbo::Number, Turbo::Number};
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<3>(ctx, args, signature, __func__))
        return false;

    SDL_Surface * const surface = surface_proto.unwrap(ctx, args[0], &args);
    if(!surface){
        Turbo::SetError(ctx, std::string(BRACKNAME " ") + __func__ + " Error argument 0 is not a Surface");
        return false;
    }
    
    ScriptImage_t * const mImage = image_proto.getSelf(ctx, vp, &args);
    assert(mImage);
    
    const unsigned w = surface->w, h = surface->h, 
        x = static_cast<unsigned>(args[0].toNumber()), y = static_cast<unsigned>(args[1].toNumber());
    
    Image * const image = mImage->get();
    
    SDL_LockSurface(surface);
    {
        image->UpdateAt(x, y, w, h, surface->pixels);
    }
    SDL_UnlockSurface(surface);

    args.rval().set(OBJECT_TO_JSVAL(surface_proto.wrap(ctx, surface)));
    return true;
}

bool SurfaceBlitSurface(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Object, Turbo::Number, Turbo::Number};
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    assert(mSurface);

    if(!Turbo::CheckSignature<3>(ctx, args, signature, __func__))
        return false;

    SDL_Surface *surface = surface_proto.unwrap(ctx, args[0], &args);
    if(!surface){
        Turbo::SetError(ctx, std::string(BRACKNAME " ") + __func__ + " Error argument 0 is not a Surface");
        return false;
    }
    
    
    SDL_Rect to = {static_cast<int>(args[1].toNumber()),
                   static_cast<int>(args[2].toNumber()),
                   surface->w, surface->h};

    SDL_BlitSurface(surface, nullptr, mSurface, &to);

    return true;
}

bool SurfaceSetClippingRectangle(JSContext *ctx, unsigned argc, JS::Value *vp){
    const Turbo::JSType signature[] = {Turbo::Number, Turbo::Number, Turbo::Number, Turbo::Number};
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    SDL_Surface *mSurface = surface_proto.getSelf(ctx, vp, &args);
    assert(mSurface);

    if(!Turbo::CheckSignature<4>(ctx, args, signature, __func__))
        return false;
    
    SDL_Rect rect = {static_cast<int>(args[0].toNumber()), 
                     static_cast<int>(args[1].toNumber()),
                     static_cast<int>(args[2].toNumber()),
                     static_cast<int>(args[3].toNumber())
                     };
    
    SDL_SetClipRect(mSurface, &rect);
    
    t5::DataSource::StdOut()->WriteF(BRACKNAME " ", __func__, " Info setting clipping rectangle ", rect.x, ',', rect.y, ',', rect.w, ',', rect.h, '\n');
    
    return true;
}

/////
// Middle School
bool ShaderCtor(JSContext *ctx, unsigned argc, JS::Value *vp){

    return true;
}

bool ShaderProgramCtor(JSContext *ctx, unsigned argc, JS::Value *vp){

    return true;
}

bool GetDefaultShaderProgram(JSContext *ctx, unsigned argc, JS::Value *vp){

    JS::CallArgs args = CallArgsFromVp(argc, vp);
    Sapphire::Galileo::Shader *lShader = Sapphire::Galileo::Shader::GetDefaultShader(ctx);
    assert(lShader);

    args.rval().set(OBJECT_TO_JSVAL(shader_program_proto.wrap(ctx, new std::shared_ptr<Galileo::Shader> (lShader))));
    return true;

}

bool GetScreenWidth(JSContext *ctx, unsigned argc, JS::Value *vp){
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    args.rval().set(JS_NumberValue(TS_GetContextEnvironment(ctx)->config->screenwidth));
    return true;
    
}

bool GetScreenHeight(JSContext *ctx, unsigned argc, JS::Value *vp){
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    args.rval().set(JS_NumberValue(TS_GetContextEnvironment(ctx)->config->screenheight));
    return true;
    
}

}
}
