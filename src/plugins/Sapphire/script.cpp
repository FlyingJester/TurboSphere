#include "script.hpp"
#include "Sapphire.hpp"
#include "SapphireInit.hpp"
#include "api.hpp"

#include <v8.h>

#include "Galileo/Galileo.hpp"
#include "Image.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "GLStart.hpp"
#include "SaveImage.hpp" // RenderQueue()

#include <algorithm>
#include <queue>
#include <memory>
#include <string>
#include <screen.h>
#include <openscript.h>
#include <opengame.h>

using std::string;

namespace Sapphire {
namespace Script {

class ScriptGroup : public Galileo::Group {
public:
    virtual ~ScriptGroup() {}
    v8::Persistent<v8::Array> mScriptShapes;
};

struct JSProps {
    static v8::Handle<v8::Value> X;
    static v8::Handle<v8::Value> Y;
    static v8::Handle<v8::Value> U;
    static v8::Handle<v8::Value> V;
    static v8::Handle<v8::Value> Color;
};

template <class T, class A>
class AccessorSelf {
  public:
  T *operator ()(v8::PropertyCallbackInfo<A> info){
    return Turbo::GetAccessorSelf<T, A>(info);
  }
};

template <class T>
class MemberSelf {
  public:
  T *operator ()(Turbo::JSArguments args){
    return Turbo::GetMemberSelf<T>(args);
  }
};

template <class T>
Turbo::JSFunction GroupSetX(Turbo::JSArguments args){
    const int sig[] = {Turbo::Number, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig))
      return;

    T self;
    Galileo::Group* mGroup = self(args);
    assert(mGroup!=nullptr);

    mGroup->SetX(args[0]->NumberValue());
}

template <class T>
Turbo::JSFunction GroupSetY(Turbo::JSArguments args){
    const int sig[] = {Turbo::Number, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig))
      return;

    T self;
    Galileo::Group* mGroup = self(args);
    assert(mGroup!=nullptr);

    mGroup->SetY(args[0]->NumberValue());
}

void ShapeImageGetter(Turbo::JSAccessorProperty aProp, Turbo::JSAccessorInfo aInfo){
    const Galileo::Shape *mShape = Turbo::GetAccessorSelf<Galileo::Shape>(aInfo);
    assert(mShape);

    Turbo::WrapObject(aInfo, ImageJSObj, new std::shared_ptr<Image>(mShape->GetImage()));
}

void ShapeImageSetter(Turbo::JSAccessorProperty aProp, Turbo::JSValue aVal, Turbo::JSAccessorSetterInfo aInfo){

    // Hold a shared pointer of the image just in case.
    // We should technically be fine, since we are inside an accessor.
    std::shared_ptr<Image> image(nullptr);

    if(ImageJSObj.IsA(aVal)){
        image = *ImageJSObj.Unwrap(aVal);
    }
    else if(SurfaceJSObj.IsA(aVal)){
        image.reset(new Image(SurfaceJSObj.Unwrap(aVal)));
    }
    else {
        Turbo::SetError(aInfo, (string(BRACKNAME " ") + string(__func__) + string(" Error: Value is not a Surface or Image.")).c_str(), v8::Exception::TypeError);
        return;
    }

    assert(image);

    Turbo::GetAccessorSelf<Galileo::Shape>(aInfo)->ReplaceImage(image.get());

}

void GroupXGetter(Turbo::JSAccessorProperty aProp, Turbo::JSAccessorGetterInfo aInfo){
    const Galileo::Group *mGroup = Turbo::GetAccessorSelf<Galileo::Group>(aInfo);
    assert(mGroup);
    aInfo.GetReturnValue().Set(mGroup->GetX());
}

void GroupXSetter(Turbo::JSAccessorProperty aProp, Turbo::JSValue aVal, Turbo::JSAccessorSetterInfo aInfo){
    if(!aVal->IsNumber()){
        Turbo::SetError(aInfo, (string(BRACKNAME " ") + string(__func__) + string(" Error: Value is not a number.")).c_str());
        return;
    }
    Galileo::Group *mGroup = Turbo::GetAccessorSelf<Galileo::Group>(aInfo);
    assert(mGroup);
    mGroup->SetX(aVal->NumberValue());
}

void GroupYGetter(Turbo::JSAccessorProperty aProp, Turbo::JSAccessorGetterInfo aInfo){
    const Galileo::Group *mGroup = Turbo::GetAccessorSelf<Galileo::Group>(aInfo);
    assert(mGroup);
    aInfo.GetReturnValue().Set(mGroup->GetY());
}

void GroupYSetter(Turbo::JSAccessorProperty aProp, Turbo::JSValue aVal, Turbo::JSAccessorSetterInfo aInfo){
    if(!aVal->IsNumber()){
        Turbo::SetError(aInfo, (string(BRACKNAME " ") + string(__func__) + string(" Error: Value is not a number.")).c_str());
        return;
    }
    Galileo::Group *mGroup = Turbo::GetAccessorSelf<Galileo::Group>(aInfo);
    assert(mGroup);
    mGroup->SetY(aVal->NumberValue());
}

void GroupRotYGetter(Turbo::JSAccessorProperty aProp, Turbo::JSAccessorGetterInfo aInfo){
    const Galileo::Group *mGroup = Turbo::GetAccessorSelf<Galileo::Group>(aInfo);
    assert(mGroup);
    aInfo.GetReturnValue().Set(mGroup->GetRotY());
}

void GroupRotYSetter(Turbo::JSAccessorProperty aProp, Turbo::JSValue aVal, Turbo::JSAccessorSetterInfo aInfo){
    if(!aVal->IsNumber()){
        Turbo::SetError(aInfo, (string(BRACKNAME " ") + string(__func__) + string(" Error: Value is not a number.")).c_str());
        return;
    }
    Galileo::Group *mGroup = Turbo::GetAccessorSelf<Galileo::Group>(aInfo);
    assert(mGroup);
    mGroup->SetRotY(aVal->NumberValue());
}

void GroupRotXGetter(Turbo::JSAccessorProperty aProp, Turbo::JSAccessorGetterInfo aInfo){
    const Galileo::Group *mGroup = Turbo::GetAccessorSelf<Galileo::Group>(aInfo);
    assert(mGroup);
    aInfo.GetReturnValue().Set(mGroup->GetRotX());
}

void GroupRotXSetter(Turbo::JSAccessorProperty aProp, Turbo::JSValue aVal, Turbo::JSAccessorSetterInfo aInfo){
    if(!aVal->IsNumber()){
        Turbo::SetError(aInfo, (string(BRACKNAME " ") + string(__func__) + string(" Error: Value is not a number.")).c_str());
        return;
    }
    Galileo::Group *mGroup = Turbo::GetAccessorSelf<Galileo::Group>(aInfo);
    assert(mGroup);
    mGroup->SetRotX(aVal->NumberValue());
}

void GroupAngleGetter(Turbo::JSAccessorProperty aProp, Turbo::JSAccessorGetterInfo aInfo){
    const Galileo::Group *mGroup = Turbo::GetAccessorSelf<Galileo::Group>(aInfo);
    assert(mGroup);
    aInfo.GetReturnValue().Set(mGroup->GetRotation<double>());
}

void GroupAngleSetter(Turbo::JSAccessorProperty aProp, Turbo::JSValue aVal, Turbo::JSAccessorSetterInfo aInfo){
    if(!aVal->IsNumber()){
        Turbo::SetError(aInfo, (string(BRACKNAME " ") + string(__func__) + string(" Error: Value is not a number.")).c_str());
        return;
    }
    Galileo::Group *mGroup = Turbo::GetAccessorSelf<Galileo::Group>(aInfo);
    assert(mGroup);
    mGroup->SetRotation(aVal->NumberValue());
}

class GroupExtraWrapCall {
public:
    static v8::Handle<v8::Object> Wrap (Galileo::Group::const_iterator &lIter, v8::Isolate *aIso){
        return Turbo::CreateObject<Galileo::Shape>(ShapeJSObj, static_cast<Galileo::Shape*>(*lIter), aIso);
    }

    constexpr static const char * const wrapname = "shapes";
};
class ShapeExtraWrapCall {
public:
    static v8::Handle<v8::Object> Wrap (Galileo::Shape::const_iterator &lIter, v8::Isolate *aIso){
        v8::Handle<v8::Object> lObj = v8::Object::New(aIso);
        lObj->Set(JSProps::X, v8::Integer::New(aIso, lIter->x));
        lObj->Set(JSProps::Y, v8::Integer::New(aIso, lIter->y));
        lObj->Set(JSProps::U, v8::Integer::New(aIso, lIter->u));
        lObj->Set(JSProps::V, v8::Integer::New(aIso, lIter->v));
        return lObj;
    }

    constexpr static const char * const wrapname = "vertices";
};

template <class T, class W>
void ExtraWrapperArrayPopulator(v8::Handle<v8::Object> &aObj, const T *a, v8::Isolate *aIso){
    assert(a);
    auto lArray = v8::Array::New(aIso, a->size());

    unsigned i = 0;
    for(typename T::const_iterator lIter = a->begin(); lIter!=a->end(); lIter++){
        lArray->Set(i++, W::Wrap(lIter, aIso));
    }

    aObj->Set(v8::String::NewFromUtf8(aIso, W::wrapname), lArray);

}

std::vector<CallbackWithName> CrossPluginSurfaceMembers = std::vector<CallbackWithName>();

const char *JSVertexCode = "\
function Vertex(_x, _y){\
this.x = _x;\
this.y = _y;\
}\
";

#define FIXEDUVCOORD_SIZE 8

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

Turbo::JSObj<TS_Color>        ColorJSObj;
Turbo::JSObj<SDL_Surface>     SurfaceJSObj;
Turbo::JSObj<std::shared_ptr<Image> > ImageJSObj;
Turbo::JSObj<Galileo::Vertex> VertexJSObj;
Turbo::JSObj<Galileo::Shape>  ShapeJSObj;
Turbo::JSObj<Galileo::Group>  GroupJSObj;

Turbo::JSObj<std::shared_ptr<Galileo::Shader> >  ShaderProgramJSObj;
//Turbo::JSObj<Galileo::Group>  GroupJSObj;

namespace Finalizer {

template <class T>
void CGeneric(const v8::WeakCallbackData<v8::Object, T> &args) {
    free(args.GetValue()->GetAlignedPointerFromInternalField(0));
    args.GetValue().Clear();
}

template <class T = void>
void NoFree(const v8::WeakCallbackData<v8::Object, T> &args){
    args.GetValue().Clear();
}

void Surface(const v8::WeakCallbackData<v8::Object, SDL_Surface> &args) {
    SDL_FreeSurface((SDL_Surface *)(args.GetValue()->GetAlignedPointerFromInternalField(0)));
    args.GetValue().Clear();
}

void Vertex (const v8::WeakCallbackData<v8::Object, Galileo::Vertex>&args){

}

void Shape  (const v8::WeakCallbackData<v8::Object, Galileo::Shape> &args){

}

void Group  (const v8::WeakCallbackData<v8::Object, Galileo::Group> &args){

}


}

std::array<Turbo::JSCallback, NumFuncs> FunctionList = {
    FlipScreen,
    ColorCtor,
    SurfaceCtor,
    ArrayBufferToSurface,
    ArrayBufferToImage,
    ImageCtor,
    VertexCtor,
    ShapeCtor,
    GroupCtor,
    ShaderCtor,
    ShaderProgramCtor,
    GetDefaultShaderProgram,
    GetScreenWidth,
    GetScreenHeight,

};

std::array<Turbo::JSName, NumFuncs> FunctionNameList = {
    "FlipScreen",
    "Color",
    "Surface",
    "SurfaceFromArrayBuffer",
    "ImageFromArrayBuffer",
    "Image",
    "VertexNative",
    "Shape",
    "Group",
    "Shader",
    "ShaderProgram",
    "GetDefaultShaderProgram",
    "GetScreenWidth",
    "GetScreenHeight",
};

std::array<Turbo::JSValue,       NumVars>  VariableList = {};
std::array<Turbo::JSVariableName,NumVars>  VariableNameList = {};
v8::Handle<v8::Value> JSProps::X;
v8::Handle<v8::Value> JSProps::Y;
v8::Handle<v8::Value> JSProps::U;
v8::Handle<v8::Value> JSProps::V;
v8::Handle<v8::Value> JSProps::Color;

void InitScript(int64_t ID){


    JSProps::X = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "x");
    JSProps::Y = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "y");
    JSProps::U = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "u");
    JSProps::V = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "v");
    JSProps::Color = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "color");

    printf(BRACKNAME " Info: ID is %llx!\n", ID);

    ColorJSObj   = Turbo::JSObj<TS_Color>       ();
    SurfaceJSObj = Turbo::JSObj<SDL_Surface>    ();
    ImageJSObj   = Turbo::JSObj<std::shared_ptr<Image> > ();
    VertexJSObj  = Turbo::JSObj<Galileo::Vertex>();
    ShapeJSObj   = Turbo::JSObj<Galileo::Shape> ();
    GroupJSObj   = Turbo::JSObj<Galileo::Group> ();
    ShaderProgramJSObj   = Turbo::JSObj<std::shared_ptr<Galileo::Shader> > ();

    ColorJSObj.Finalize             = Turbo::Finalizer<TS_Color>;
    ShaderProgramJSObj.Finalize     = Turbo::Finalizer<std::shared_ptr<Galileo::Shader> >;
    ImageJSObj.Finalize     = Turbo::Finalizer<std::shared_ptr<Image> >;
    SurfaceJSObj.Finalize   = Finalizer::NoFree;
    VertexJSObj.Finalize    = Finalizer::NoFree;
    ShapeJSObj.Finalize     = Finalizer::NoFree;
    ImageJSObj.SetTypeName("Image");

    SurfaceJSObj.AddToProto("save", SaveSurface);
    SurfaceJSObj.AddToProto("setPixel", SetPixelSurface);
    SurfaceJSObj.AddToProto("blitSurface", SurfaceBlitSurface);

    memberiter_t lIter = CrossPluginSurfaceMembers.begin();

    while(lIter!=CrossPluginSurfaceMembers.end()){

      if(!(*lIter).first){
          fprintf(stderr, BRACKNAME " Error: Bad cross-plugin Surface member name.\n");
          lIter++;
          continue;
      }
      if(!(*lIter).second){
          fprintf(stderr, BRACKNAME " Error: Bad cross-plugin Surface member function for %s.\n", (*lIter).first);
          lIter++;
          continue;
      }

      SurfaceJSObj.AddToProto((*lIter).first, (*lIter).second);
      printf(BRACKNAME " Info: Registered cross-plugin Surface member function %s.\n", (*lIter).first);
      lIter++;
    }

    ColorJSObj.ID              = (ID<<16)|(0x0312u);
    SurfaceJSObj.ID            = (ID<<16)|(0x0302u);
    ImageJSObj.ID              = (ID<<16)|(0x0292u);
    VertexJSObj.ID             = (ID<<16)|(0x0289u);
    ShapeJSObj.ID              = (ID<<16)|(0x0260u);
    GroupJSObj.ID              = (ID<<16)|(0x0221u);
    ShaderProgramJSObj.ID      = (ID<<16)|(0x0144u);

    printf(BRACKNAME " Info: Group is ID %llu\n", GroupJSObj.ID);

    GroupJSObj.Finalize     = Turbo::Finalizer<Galileo::Group>;
    GroupJSObj.SetTypeName("Group");
    GroupJSObj.AddToProto("Draw", DrawGroup);
    GroupJSObj.AddToProto("draw", DrawGroup);
    GroupJSObj.AddToProto("setPosition", GroupSetPosition);
    GroupJSObj.AddToProto("setRotation", GroupSetRotation);
    GroupJSObj.AddAccessor("x",   GroupXGetter,    GroupXSetter);
    GroupJSObj.AddAccessor("y",   GroupYGetter,    GroupYSetter);
    GroupJSObj.AddAccessor("angle", GroupAngleGetter,  GroupAngleSetter);
    GroupJSObj.AddAccessor("rotX", GroupRotXGetter,  GroupRotXSetter);
    GroupJSObj.AddAccessor("rotY", GroupRotYGetter,  GroupRotYSetter);
    GroupJSObj.AddToProto("setX", GroupSetX< MemberSelf<Galileo::Group> >);
    GroupJSObj.AddToProto("setY", GroupSetY< MemberSelf<Galileo::Group> >);
    GroupJSObj.AddWrappingFunc(ExtraWrapperArrayPopulator<Galileo::Group, GroupExtraWrapCall>);
    ShapeJSObj.AddWrappingFunc(ExtraWrapperArrayPopulator<Galileo::Shape, ShapeExtraWrapCall>);

    ImageJSObj.AddToProto("createSurface", ImageCreateSurface);
    ImageJSObj.AddAccessor("image", ShapeImageGetter, ShapeImageSetter);

    SurfaceJSObj.AddAccessor("width",  Turbo::GenericPropertyGetter<int, SDL_Surface, &SDL_Surface::w>, nullptr);
    SurfaceJSObj.AddAccessor("height", Turbo::GenericPropertyGetter<int, SDL_Surface, &SDL_Surface::h>, nullptr);
    auto lIso = v8::Isolate::GetCurrent();

    ExecuteString(v8::String::NewFromUtf8(lIso, JSVertexCode), v8::String::NewFromUtf8(lIso, "Sapphire_Internal"), lIso, true);

}

Turbo::JSFunction FlipScreen(Turbo::JSArguments args){
    Sapphire::GL::RenderQueue()->push(new Sapphire::Galileo::FlipScreen());
    Sapphire::GL::SwapQueues();
    Sapphire::GL::RenderQueue()->clear();
//    AtomicInc(Sapphire::GL::GetEngineFrame());
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
            Turbo::WrapObject(args, ImageJSObj, new std::shared_ptr<Image> (rImage) );
            return;
        }else if(Turbo::CheckArg::String(args, 0, __func__)){ // From Path
            v8::String::Utf8Value lStr(args[0]);
            SDL_Surface *lSurf = LoadSurface(*lStr);
            if(!lSurf){
                Turbo::SetError(args, (std::string(BRACKNAME " ImageCtor Error: Could not open path '") +
                                       std::string(*lStr) + std::string("'.") ).c_str());
            }
            else{
              Image *rImage = new Sapphire::Image(lSurf);
              Turbo::WrapObject(args, ImageJSObj, new std::shared_ptr<Image> (rImage)  );
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

        Turbo::WrapObject(args, ImageJSObj, new std::shared_ptr<Image> (rImage) );

    }

}

Turbo::JSFunction SurfaceCtor(Turbo::JSArguments args){

    const int sig[] = {Turbo::Int, Turbo::Int, Turbo::Object, 0};

    if(args.Length()==1){

        if(Turbo::CheckArg::String(args, 0, __func__)){ // From Path
            v8::String::Utf8Value lStr(args[0]);
            SDL_Surface *rSurf = LoadSurface(std::string(GetDirs()->image).append(*lStr).c_str());
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

        printf(BRACKNAME " CreateSurface Info: Creating a surface of color r%xg%xb%xa%x (%x)\n", lColor->red, lColor->green, lColor->blue, lColor->alpha, lColor->toInt());

        const size_t lPixelCount = args[0]->Int32Value()*args[1]->Int32Value();

        SDL_Surface *rSurf = SDL_CreateRGBSurface(0, args[0]->Int32Value(), args[1]->Int32Value(),
                                                  32, CHANNEL_MASKS);

        SDL_LockSurface(rSurf);

        uint32_t *lPix = static_cast<uint32_t *>(rSurf->pixels);

        std::fill(
                    lPix,
                    &(lPix[lPixelCount]),
                    lColor->toInt()
                  );

        if(rSurf->w*rSurf->h < 16*16){
            printf(BRACKNAME " CreateSurface Info: Dumping color:\n");
            for(int i = 0; i<rSurf->w*rSurf->h; i++){
                printf(BRACKNAME "\t" "%x", lPix[i]);
            }

            printf("\n" BRACKNAME "\n");
        }
        SDL_UnlockSurface(rSurf);
        Turbo::WrapObject(args, SurfaceJSObj, rSurf);
    }

}

/////
// New School!
Turbo::JSFunction VertexCtor(Turbo::JSArguments args){

}

inline bool ValidateVertex(const v8::Local<v8::Object> &aToCheck){
    return ((aToCheck->Has(JSProps::X)) && (aToCheck->Has(JSProps::Y)) && (aToCheck->Get(JSProps::X)->IsNumber()) && (aToCheck->Get(JSProps::Y)->IsNumber()));
}

Turbo::JSFunction ShapeCtor(Turbo::JSArguments args){
    const int sig[] = {Turbo::Array, Turbo::Object, 0};

    if(Turbo::CheckArg::CheckSig(args, 2, sig, true)){

        const v8::Local<v8::Array> vertexarray = v8::Local<v8::Array>::Cast(args[0]);

        const int num_vertices = vertexarray->Length();

        std::vector<Sapphire::Galileo::Vertex> Vertices; Vertices.resize(num_vertices);

        if(!ImageJSObj.IsA(args[1])){
            Turbo::SetError(args, BRACKNAME " ShapeCtor Error: Argument 1 is not an Image.", v8::Exception::TypeError);
            return;
        }
        // Validate and encode all the vertices to x and y point arrays.
        for(int i = 0; i<num_vertices; i++){
            const v8::Local<v8::Object> vertex = (vertexarray->Get(i))->ToObject();
            if(!ValidateVertex(vertex)){
                Turbo::SetError(args, (std::string(BRACKNAME " ShapeCtor Error: Argument 0, element ")+std::to_string(i)+" does not have a 'x' and a 'y' property.").c_str(), v8::Exception::TypeError);
                return;
            }

            Vertices[i].x = vertex->Get(JSProps::X)->NumberValue();// - (::GetScreenWidth());
            Vertices[i].y = vertex->Get(JSProps::Y)->NumberValue();// + (::GetScreenHeight()/2);

            TS_Color lColor(0xFF, 0xFF, 0xFF, 0xFF);

            if(vertex->Has(JSProps::Color)){
                if(!ColorJSObj.IsA(vertex->Get(JSProps::Color))){
                    Turbo::SetError(args, (std::string(BRACKNAME " ShapeCtor Error: Property 'color' of argument 0, element ")+std::to_string(i)+" is not a Color.").c_str(), v8::Exception::TypeError);
                    return;
                }

                lColor = *ColorJSObj.Unwrap(vertex->Get(JSProps::Color));
            }


            Vertices[i].color = lColor;

            if(vertex->Has(JSProps::U) && vertex->Has(JSProps::V)){
                  Vertices[i].u = vertex->Get(JSProps::U)->NumberValue();
                  Vertices[i].v = vertex->Get(JSProps::V)->NumberValue();
            }
            else if(num_vertices<FIXEDUVCOORD_SIZE){
                switch(num_vertices){
                case 4:
                  Vertices[i].u = FixedUV4[(i*2)  ];
                  Vertices[i].v = FixedUV4[(i*2)+1];
                break;
            }

            }
        }

        Image *lImage = static_cast< std::shared_ptr<Image> *>(v8::Local<v8::Object>::Cast(args[1])->GetAlignedPointerFromInternalField(0))->get();
        assert(lImage);

        Sapphire::Galileo::Shape *rShape = new Galileo::Shape(Vertices,lImage);

        rShape->FillGL();

        Turbo::WrapObject(args, ShapeJSObj, rShape);
    }
    else{
            Turbo::SetError(args, BRACKNAME " ShapeCtor Error: Called with fewer than 2 arguments.", v8::Exception::SyntaxError);
    }
}

Turbo::JSFunction GroupCtor(Turbo::JSArguments args){

    if(args.Length()<2){
        Turbo::SetError(args, BRACKNAME " GroupCtor Error: Called with fewer than 2 arguments.", v8::Exception::SyntaxError);
        return;
    }
    if((!args[0]->IsArray())&&(!ShapeJSObj.IsA(args[0]))){
        Turbo::SetError(args, BRACKNAME " GroupCtor Error: Argument 0 must either be a Shape or an array of Shapes.", v8::Exception::TypeError);
        return;
    }

    if(!ShaderProgramJSObj.IsA(args[1])){
        Turbo::SetError(args, BRACKNAME " GroupCtor Error: Argument 1 is not a ShaderProgram.", v8::Exception::TypeError);
        return;
    }

    if(ShapeJSObj.IsA(args[0])){

        ScriptGroup *lGroup = new ScriptGroup();
        lGroup->SetShader(ShaderProgramJSObj.Unwrap(args[1])->get());

        lGroup->push(ShapeJSObj.Unwrap(args[0]));
//                     static_cast<Galileo::Shape*>(v8::Local<v8::Object>::Cast(args[0])->GetAlignedPointerFromInternalField(0)));

        v8::Local<v8::Array> shapearray = v8::Array::New(args.GetIsolate(), 1);
        shapearray->Set(0, args[0]);

        lGroup->mScriptShapes.Reset(args.GetIsolate(), shapearray);
        Turbo::WrapObject(args, GroupJSObj, static_cast<Galileo::Group *>(lGroup));
        return;

    }
    else if(args[0]->IsArray()){

        const v8::Local<v8::Array> shapearray = v8::Local<v8::Array>::Cast(args[0]);
        for(int i = 0; i<shapearray->Length(); i++){
            if(!ShapeJSObj.IsA(shapearray->Get(i)->ToObject())){
                Turbo::SetError(args, (std::string(BRACKNAME " GroupCtor Error: Argument 0, element ")+std::to_string(i)+std::string(" is not a Shape.")).c_str(), v8::Exception::TypeError);
                return;
            }
        }

        ScriptGroup *lGroup = new ScriptGroup();
        lGroup->SetShader(ShaderProgramJSObj.Unwrap(args[1])->get());

        for(int i = 0; i<shapearray->Length(); i++){
            lGroup->push(ShapeJSObj.Unwrap(shapearray->Get(i)->ToObject()));
        }

        lGroup->mScriptShapes.Reset(args.GetIsolate(), shapearray);

        Turbo::WrapObject(args, GroupJSObj, static_cast<Galileo::Group *>(lGroup));
        return;

    }
    // Else is handled above with (!args[0]->IsArray())&&(!ShapeJSObj.IsA(args[0]))

}

Turbo::JSFunction DrawGroup(Turbo::JSArguments args){
    Galileo::Group* mGroup = Turbo::GetMemberSelf <Galileo::Group> (args);
    assert(mGroup!=nullptr);

    mGroup->DrawAll(Sapphire::GL::RenderQueue());

}

Turbo::JSFunction SaveSurface(Turbo::JSArguments args){
    SDL_Surface* mSurf = Turbo::GetMemberSelf <SDL_Surface> (args);
    assert(mSurf);

    std::string lSaveName;

    if(args.Length()<1){
      const char *lTmp = std::tmpnam(nullptr);
      SDL_SaveBMP(mSurf, lTmp);
      fprintf(stderr, BRACKNAME " %s Warning: no file name supplied. Using %s.", __func__, lTmp);
    }
    else if(Turbo::CheckArg::String(args, 0, __func__)){
      v8::String::Utf8Value lStr(args[0]);
      lSaveName = *lStr;
      std::string::reverse_iterator lStrExtBeg = lSaveName.rbegin();

      while(*lStrExtBeg!='.' && (lStrExtBeg.base()!=lSaveName.begin()))
        lStrExtBeg++;

      std::string lExt = std::string(lStrExtBeg.base(), lSaveName.end());
      printf(BRACKNAME " %s Info: Saving a '%s'.\n", __func__, lExt.c_str());
      std::map<std::string, Sapphire::Save::SaveFunction>::const_iterator lSaveFunc = Sapphire::Save::SaveWithExtension.find(lExt);


      if(lSaveFunc!=Sapphire::Save::SaveWithExtension.end()){
          printf(BRACKNAME " %s Info: Saving a with save func %p.\n", __func__, (*lSaveFunc).second);

          int err = (*lSaveFunc).second(mSurf, std::string(GetDirs()->image).append(lSaveName).c_str());

          if(err!=Save::SaveStatus::ssSuccess){

              fprintf(stderr, BRACKNAME " %s Error: Could not save surface.\n", __func__);

              args.GetIsolate()->ThrowException(
                v8::Exception::Error(v8::String::NewFromUtf8(args.GetIsolate(),
                (
                  std::string(BRACKNAME " SaveSurface Error: Could not save surface")
                + std::string(lSaveName)
                + std::string(".")
                ).c_str()
              )));
              return;
          }

      }
      else
        SDL_SaveBMP(mSurf, lSaveName.c_str());


    }
    else{
      args.GetIsolate()->ThrowException( v8::Exception::Error(v8::String::NewFromUtf8(args.GetIsolate(),
                                 BRACKNAME " SaveSurface Error: Argument 0 is not a string.")));
      return;
    }

}

Turbo::JSFunction SetPixelSurface(Turbo::JSArguments args){
    SDL_Surface* mSurf = Turbo::GetMemberSelf <SDL_Surface> (args);
    assert(mSurf);

    int sig[4] = {Turbo::Number, Turbo::Number, Turbo::Object, 0};

    if(!Turbo::CheckArg::CheckSig(args, 3, sig))
      return;

    if(!ColorJSObj.IsA(args[2])){
        Turbo::SetError(args, " Argument 2 is not a Color.");
        return;
    }

    const TS_Color *lColor =
      (TS_Color*)(v8::Local<v8::Object>::Cast(args[2]))->GetAlignedPointerFromInternalField(0);

    SDL_LockSurface(mSurf);
    static_cast<uint32_t *>(mSurf->pixels)[args[0]->Int32Value() + (args[1]->Int32Value()*mSurf->w)] = lColor->toInt();
    SDL_UnlockSurface(mSurf);

}

Turbo::JSFunction ArrayBufferToImage(Turbo::JSArguments args){
    int sig[4] = {Turbo::Number, Turbo::Number, Turbo::Object, 0};

    if(!Turbo::CheckArg::CheckSig(args, 2, sig))
      return;

    int w = args[0]->Uint32Value(), h = args[1]->Uint32Value();
    const size_t length = w*h*4;

    v8::Local<v8::ArrayBuffer> buffer;
    if(args[2]->IsArrayBuffer())
      buffer = v8::Local<v8::ArrayBuffer>::Cast(args[2]);
    else if(args[2]->IsTypedArray()){
        buffer = v8::Local<v8::TypedArray>::Cast(args[2])->Buffer();
    }
    else if(args[2]->IsArrayBufferView()){
        buffer = v8::Local<v8::ArrayBufferView>::Cast(args[2])->Buffer();
    }
    else{
        Turbo::SetError(args, BRACKNAME " ArrayBufferToImage Error: Argument 2 was not an ArrayBuffer, ArrayBufferView, or a TypedArray.");
        return;
    }

    v8::ArrayBuffer::Contents contents = buffer->Externalize();

    if(contents.ByteLength()<length){
        Turbo::SetError(args, "ArrayBuffer too small", v8::Exception::RangeError);
        return;
    }

    SDL_Surface *lSurf = SDL_CreateRGBSurfaceFrom(contents.Data(), w, w, IMAGE_DEPTH, w*((IMAGE_DEPTH)>>3), CHANNEL_MASKS);

    Image *rImage = new Image(lSurf);
    SDL_FreeSurface(lSurf);
    free(contents.Data());

    Turbo::WrapObject(args, ImageJSObj, new std::shared_ptr<Image> (rImage) );

}

Turbo::JSFunction ArrayBufferToSurface(Turbo::JSArguments args){
    int sig[4] = {Turbo::Number, Turbo::Number, Turbo::Object, 0};

    if(!Turbo::CheckArg::CheckSig(args, 2, sig))
      return;

    int w = args[0]->Uint32Value(), h = args[1]->Uint32Value();
    const size_t length = w*h*4;

    v8::Local<v8::ArrayBuffer> buffer;
    if(args[2]->IsArrayBuffer())
      buffer = v8::Local<v8::ArrayBuffer>::Cast(args[2]);
    else if(args[2]->IsTypedArray()){
        buffer = v8::Local<v8::TypedArray>::Cast(args[2])->Buffer();
    }
    else if(args[2]->IsArrayBufferView()){
        buffer = v8::Local<v8::ArrayBufferView>::Cast(args[2])->Buffer();
    }
    else{
        Turbo::SetError(args, BRACKNAME " ArrayBufferToSurface Error: Argument 2 was not an ArrayBuffer, ArrayBufferView, or a TypedArray.");
        return;
    }

    v8::ArrayBuffer::Contents contents = buffer->Externalize();

    if(contents.ByteLength()<length){
        Turbo::SetError(args, "ArrayBuffer too small", v8::Exception::RangeError);
        return;
    }

    SDL_Surface *rSurf = GenerateSurface(w, h);

    SDL_LockSurface(rSurf);

    memcpy(rSurf->pixels, contents.Data(), length);

    SDL_UnlockSurface(rSurf);

    Turbo::WrapObject(args, SurfaceJSObj, rSurf);

    free(contents.Data());
    buffer->Neuter();

}

Turbo::JSFunction SaveImage(Turbo::JSArguments args);

Turbo::JSFunction ImageCreateSurface(Turbo::JSArguments args){
    std::shared_ptr<Image> *mImage = Turbo::GetMemberSelf <std::shared_ptr<Image> > (args);
    assert(mImage);
    assert(mImage->get());

    printf(BRACKNAME " Info: Creating a surface of size %i, %i\n", (*mImage)->Width(), (*mImage)->Height());

    SDL_Surface *rSurf = GenerateSurface((*mImage)->Width(), (*mImage)->Height());
    assert(rSurf);

    SDL_LockSurface(rSurf);

    assert(rSurf->pixels);
    (*mImage)->CopyData(rSurf->pixels);

    SDL_UnlockSurface(rSurf);

    Turbo::WrapObject(args, SurfaceJSObj, rSurf);
}

Turbo::JSFunction GroupSetPosition(Turbo::JSArguments args){

    const int sig[] = {Turbo::Number, Turbo::Number, 0};

    if(!Turbo::CheckArg::CheckSig(args, 2, sig))
      return;

    Galileo::Group* mGroup = Turbo::GetMemberSelf <Galileo::Group> (args);
    assert(mGroup!=nullptr);

    mGroup->SetOffset(args[0]->NumberValue(), args[1]->NumberValue());
}

Turbo::JSFunction GroupSetRotation(Turbo::JSArguments args){

    const int sig[] = {Turbo::Number, Turbo::Number, Turbo::Number, 0};

    if(!Turbo::CheckArg::CheckSig(args, 3, sig))
      return;

    Galileo::Group* mGroup = Turbo::GetMemberSelf <Galileo::Group> (args);
    assert(mGroup!=nullptr);

    mGroup->SetRotationAttr(args[0]->NumberValue(), args[1]->NumberValue(), args[2]->NumberValue());
}

Turbo::JSFunction SurfaceBlitSurface(Turbo::JSArguments args){

    const int sig[] = {Turbo::Object, Turbo::Number, Turbo::Number, 0};

    if(!Turbo::CheckArg::CheckSig(args, 3, sig))
      return;

    if(!SurfaceJSObj.IsA(args[0])){
        Turbo::SetError(args, " Argument 0 is not a Surface.", v8::Exception::TypeError);
        return;
    }

    SDL_Surface *that = Turbo::GetMemberSelf<SDL_Surface>(args);
    SDL_Surface *other= SurfaceJSObj.Unwrap(args[0]);
    SDL_Rect to = {static_cast<int>(args[1]->IntegerValue()),
                   static_cast<int>(args[2]->IntegerValue()),
                     other->w, other->h};

    SDL_BlitSurface(other, nullptr, that, &to);

}

/////
// Middle School
Turbo::JSFunction ShaderCtor(Turbo::JSArguments args){

}

Turbo::JSFunction ShaderProgramCtor(Turbo::JSArguments args){

}

Turbo::JSFunction GetDefaultShaderProgram(Turbo::JSArguments args){

    Sapphire::Galileo::Shader *lShader = Sapphire::Galileo::Shader::GetDefaultShader();
    assert(lShader);

    Turbo::WrapObject(args, ShaderProgramJSObj, new std::shared_ptr<Galileo::Shader> (lShader));

}

Turbo::JSFunction GetScreenWidth(Turbo::JSArguments args){
    args.GetReturnValue().Set(::GetScreenWidth());
}

Turbo::JSFunction GetScreenHeight(Turbo::JSArguments args){
    args.GetReturnValue().Set(::GetScreenHeight());
}

}
}
