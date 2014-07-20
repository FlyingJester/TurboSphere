#include "script.hpp"
#include "Sapphire.hpp"

#include <v8.h>

#include "Galileo/Galileo.hpp"
#include "Image.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "GLStart.hpp" // RenderQueue()

#include <algorithm>
#include <queue>
#include <memory>
#include <string>
#include <screen.h>
#include <openscript.h>


namespace Sapphire {
namespace Script {

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
    0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f
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

v8::Handle<v8::Value> XProp, YProp;

Turbo::JSObj<TS_Color>        ColorJSObj;
Turbo::JSObj<SDL_Surface>     SurfaceJSObj;
Turbo::JSObj<Image>           ImageJSObj;
Turbo::JSObj<Galileo::Vertex> VertexJSObj;
Turbo::JSObj<Galileo::Shape>  ShapeJSObj;
Turbo::JSObj<Galileo::Group>  GroupJSObj;

Turbo::JSObj<Galileo::Shader>  ShaderProgramJSObj;
//Turbo::JSObj<Galileo::Group>  GroupJSObj;

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

std::array<Turbo::JSCallback, NumFuncs> FunctionList = {
    ColorCtor,
    SurfaceCtor,
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
    "Color",
    "Surface",
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

void InitScript(int64_t ID){

    printf(BRACKNAME " Info: ID is %llx!\n", ID);

    ColorJSObj   = Turbo::JSObj<TS_Color>       ();
    SurfaceJSObj = Turbo::JSObj<SDL_Surface>    ();
    ImageJSObj   = Turbo::JSObj<Image>          ();
    VertexJSObj  = Turbo::JSObj<Galileo::Vertex>();
    ShapeJSObj   = Turbo::JSObj<Galileo::Shape> ();
    GroupJSObj   = Turbo::JSObj<Galileo::Group> ();
    ShaderProgramJSObj   = Turbo::JSObj<Galileo::Shader> ();

    ColorJSObj.Finalize             = Finalizer::Generic<TS_Color>;
    ShaderProgramJSObj.Finalize     = Finalizer::Generic<Galileo::Shader>;
    ImageJSObj.SetTypeName("Image");

    GroupJSObj.Finalize     = Finalizer::Generic<Galileo::Group>;
    GroupJSObj.SetTypeName("Group");
    GroupJSObj.AddToProto("Draw", DrawGroup);

    ColorJSObj.ID              = (ID<<16)|(0x0312u);
    SurfaceJSObj.ID            = (ID<<16)|(0x0302u);
    ImageJSObj.ID              = (ID<<16)|(0x0292u);
    VertexJSObj.ID             = (ID<<16)|(0x0289u);
    ShapeJSObj.ID              = (ID<<16)|(0x0260u);
    GroupJSObj.ID              = (ID<<16)|(0x0221u);
    ShaderProgramJSObj.ID      = (ID<<16)|(0x0144u);


    auto lIso = v8::Isolate::GetCurrent();

    ExecuteString(v8::String::NewFromUtf8(lIso, JSVertexCode), v8::String::NewFromUtf8(lIso, "Sapphire_Internal"), lIso, true);

    XProp = v8::String::NewFromUtf8(lIso, "x");
    YProp = v8::String::NewFromUtf8(lIso, "y");

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
Turbo::JSFunction VertexCtor(Turbo::JSArguments args){

}

inline bool ValidateVertex(const v8::Local<v8::Object> &aToCheck){
    return ((aToCheck->Has(XProp)) && (aToCheck->Has(YProp)));
}

Turbo::JSFunction ShapeCtor(Turbo::JSArguments args){
    const int sig[] = {Turbo::Array, Turbo::Object, 0};

    if(Turbo::CheckArg::CheckSig(args, 2, sig, true)){

        const v8::Local<v8::Array> vertexarray = v8::Local<v8::Array>::Cast(args[0]);

        const int num_vertices = vertexarray->Length();

        std::vector<Sapphire::Galileo::Vertex> Vertices; Vertices.resize(num_vertices);

        if(!ImageJSObj.IsA(args[1])){
            Turbo::SetError(args, BRACKNAME " ShapeCtor Error: Argument 1 is not an Image.");
            return;
        }
        // Validate and encode all the vertices to x and y point arrays.
        for(int i = 0; i<num_vertices; i++){
            const v8::Local<v8::Object> vertex = (vertexarray->Get(i))->ToObject();
            if(!ValidateVertex(vertex)){
                Turbo::SetError(args, (std::string(BRACKNAME " ShapeCtor Error: Argument 0, element ")+std::to_string(i)+std::string(" does not have a 'x' and a 'y' property.")).c_str());
                return;
            }

            Vertices[i].x = vertex->Get(XProp)->NumberValue();
            Vertices[i].y = vertex->Get(YProp)->NumberValue();
            Vertices[i].color = TS_Color(0xFF, 0xFF, 0xFF, 0xFF);
            if(num_vertices<FIXEDUVCOORD_SIZE){
                Vertices[i].u = FixedUVCoord[i][(i*2)  ];
                Vertices[i].v = FixedUVCoord[i][(i*2)+1];
            }
        }

        Image *lImage = static_cast<Image*>(v8::Local<v8::Object>::Cast(args[1])->GetAlignedPointerFromInternalField(0));
        assert(lImage);

        Sapphire::Galileo::Shape *rShape = new Galileo::Shape(Vertices,lImage);

        rShape->FillGL();

        Turbo::WrapObject(args, ShapeJSObj, rShape);
    }
    else{
            Turbo::SetError(args, BRACKNAME " ShapeCtor Error: Called with fewer than 2 arguments.");
    }
}

Turbo::JSFunction GroupCtor(Turbo::JSArguments args){

    printf(BRACKNAME " Info: Creating a group.\n");

    if(args.Length()<2){
        Turbo::SetError(args, BRACKNAME " GroupCtor Error: Called with fewer than 2 arguments.");
        return;
    }
    if((!args[0]->IsArray())&&(!ShapeJSObj.IsA(args[0]))){
        Turbo::SetError(args, BRACKNAME " GroupCtor Error: Argument 0 must either be a Shape or an array of Shapes.");
        return;
    }

    if(!ShaderProgramJSObj.IsA(args[1])){
        Turbo::SetError(args, BRACKNAME " GroupCtor Error: Argument 1 must be a ShaderProgram.");
        return;
    }

    if(ShapeJSObj.IsA(args[0])){

        Sapphire::Galileo::Group *lGroup = new Galileo::Group();
        lGroup->SetShader(
          static_cast<Galileo::Shader*>(v8::Local<v8::Object>::Cast(args[1])->GetAlignedPointerFromInternalField(0)));

        lGroup->push(static_cast<Galileo::Shape*>(v8::Local<v8::Object>::Cast(args[0])->GetAlignedPointerFromInternalField(0)));

        Turbo::WrapObject(args, GroupJSObj, lGroup);
        return;

    }
    else if(args[0]->IsArray()){

        const v8::Local<v8::Array> shapearray = v8::Local<v8::Array>::Cast(args[0]);
        for(int i = 0; i<shapearray->Length(); i++){
            if(!ShapeJSObj.IsA(shapearray->Get(i)->ToObject())){
                Turbo::SetError(args, (std::string(BRACKNAME " GroupCtor Error: Argument 0, element ")+std::to_string(i)+std::string(" is not a Shape.")).c_str());
                return;
            }
        }

        Sapphire::Galileo::Group *lGroup = new Galileo::Group();
        lGroup->SetShader(
          static_cast<Galileo::Shader*>(v8::Local<v8::Object>::Cast(args[1])->GetAlignedPointerFromInternalField(0)));

        for(int i = 0; i<shapearray->Length(); i++){
            lGroup->push(static_cast<Galileo::Shader*>(v8::Local<v8::Object>::Cast(shapearray->Get(i)->ToObject())->GetAlignedPointerFromInternalField(0)));
        }

        Turbo::WrapObject(args, GroupJSObj, lGroup);
        return;

    }



}

Turbo::JSFunction DrawGroup(Turbo::JSArguments args){
    Galileo::Group* mGroup = Turbo::GetMemberSelf <Galileo::Group> (args);

    mGroup->DrawAll(Sapphire::GL::RenderQueue());

}

/////
// Middle School
Turbo::JSFunction ShaderCtor(Turbo::JSArguments args){

}

Turbo::JSFunction ShaderProgramCtor(Turbo::JSArguments args){

}

Turbo::JSFunction GetDefaultShaderProgram(Turbo::JSArguments args){

    Sapphire::Galileo::Shader *lShader = Sapphire::Galileo::Shader::GetDefaultShader();
    Turbo::WrapObject(args, ShaderProgramJSObj, lShader);

}

Turbo::JSFunction GetScreenWidth(Turbo::JSArguments args){
    args.GetReturnValue().Set(v8::Integer::New(v8::Isolate::GetCurrent(), ::GetScreenWidth()));
}

Turbo::JSFunction GetScreenHeight(Turbo::JSArguments args){
    args.GetReturnValue().Set(v8::Integer::New(v8::Isolate::GetCurrent(), ::GetScreenHeight()));
}

//Turbo::JSFunction (Turbo::JSArguments args);

}
}
