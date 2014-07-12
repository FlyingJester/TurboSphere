#pragma once

#include "Sapphire.hpp"

#include "Galileo/Galileo.hpp"
#include "Image.hpp"
#include <SDL2/SDL.h>
#include <array>

/////
// This file contains all JS-Wrapping functions
//

namespace Sapphire {
namespace Script {

void InitScript(int64_t ID);

static const size_t NumFuncs = 8;
static const size_t NumVars  = 0;

extern std::array<Turbo::JSCallback,    NumFuncs> FunctionList;
extern std::array<Turbo::JSName,        NumFuncs> FunctionNameList;
extern std::array<Turbo::JSValue,       NumVars>  VariableList;
extern std::array<Turbo::JSVariableName,NumVars>  VariableNameList;

extern Turbo::JSObj<TS_Color>        ColorJSObj;
extern Turbo::JSObj<SDL_Surface>     SurfaceJSObj;
extern Turbo::JSObj<Image>           ImageJSObj;
extern Turbo::JSObj<Galileo::Vertex> VertexJSObj;
extern Turbo::JSObj<Galileo::Shape>  ShapeJSObj;
extern Turbo::JSObj<Galileo::Group>  GroupJSObj;

/////
// Old School
Turbo::JSFunction ColorCtor(Turbo::JSArguments args);
Turbo::JSFunction ImageCtor(Turbo::JSArguments args);
Turbo::JSFunction SurfaceCtor(Turbo::JSArguments args);

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
