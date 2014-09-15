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

void InitScript(int64_t ID);

static const size_t NumFuncs = 12;
static const size_t NumVars  = 0;

typedef std::pair<const char *, Turbo::JSCallback> CallbackWithName;
typedef std::vector<CallbackWithName>::iterator memberiter_t;

extern std::array<Turbo::JSCallback,    NumFuncs> FunctionList;
extern std::array<Turbo::JSName,        NumFuncs> FunctionNameList;
extern std::array<Turbo::JSValue,       NumVars>  VariableList;
extern std::array<Turbo::JSVariableName,NumVars>  VariableNameList;
extern std::vector<CallbackWithName>     CrossPluginSurfaceMembers;

extern Turbo::JSObj<TS_Color>        ColorJSObj;
extern Turbo::JSObj<SDL_Surface>     SurfaceJSObj;
extern Turbo::JSObj<std::shared_ptr<Image> >           ImageJSObj;
extern Turbo::JSObj<Galileo::Vertex> VertexJSObj;
extern Turbo::JSObj<Galileo::Shape>  ShapeJSObj;
extern Turbo::JSObj<Galileo::Group>  GroupJSObj;
extern Turbo::JSObj<std::shared_ptr<Galileo::Shader> > ShaderProgramJSObj;

/////
// Old School
Turbo::JSFunction ColorCtor(Turbo::JSArguments args);
Turbo::JSFunction ImageCtor(Turbo::JSArguments args);
Turbo::JSFunction SurfaceCtor(Turbo::JSArguments args);

Turbo::JSFunction SaveSurface(Turbo::JSArguments args);
Turbo::JSFunction SaveImage(Turbo::JSArguments args);
Turbo::JSFunction ImageCreateSurface(Turbo::JSArguments args);
/////
// New School!
Turbo::JSFunction VertexCtor(Turbo::JSArguments args);
Turbo::JSFunction ShapeCtor(Turbo::JSArguments args);
Turbo::JSFunction GroupCtor(Turbo::JSArguments args);

Turbo::JSFunction DrawGroup(Turbo::JSArguments args);
Turbo::JSFunction GroupSetPosition(Turbo::JSArguments args);
Turbo::JSFunction GroupSetRotation(Turbo::JSArguments args);

/////
// Middle School
Turbo::JSFunction ShaderCtor(Turbo::JSArguments args);
Turbo::JSFunction ShaderProgramCtor(Turbo::JSArguments args);
Turbo::JSFunction GetDefaultShaderProgram(Turbo::JSArguments args);


Turbo::JSFunction FlipScreen(Turbo::JSArguments args);
Turbo::JSFunction GetScreenWidth(Turbo::JSArguments args);
Turbo::JSFunction GetScreenHeight(Turbo::JSArguments args);
//Turbo::JSFunction (Turbo::JSArguments args);

}
}
