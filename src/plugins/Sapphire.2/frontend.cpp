#include "frontend.h"
#include "api.h"
#include "blendmode.h"
#include "../common/plugin.h"
#include "../../configmanager/openscript.h"

#ifdef _WIN32
//...Not sure. Use resource DLLs, I guess.
#else
    #ifdef __linux__
        #include <sys/mman.h>
    #else
        #error FIXME: Add the header for mmap.
    #endif

    #include "resourcefile.sup"

#endif

v8::Isolate *iso = NULL;

const size_t numVariables = 10;
Turbo::JSValueArray ScriptVariableList = NULL;

Turbo::JSVariableName ScriptVariableNameList[] = {
    "BLEND",
    "REPLACE",
    "RGB_ONLY",
    "ALPHA_ONLY",
    "ADD",
    "SUBTRACT",
    "MULTIPLY",
    "AVERAGE",
    "INVERT",
    "SQUARE",
};

inline void InitVariables(void){

    if(!iso){
        iso = v8::Isolate::GetCurrent();
    }

    ScriptVariableList = new Turbo::JSValue[numVariables];

    for(uint32_t i = 0; i<10; i++) //BlendModes.
        ScriptVariableList[i] = v8::Integer::New(iso, i+1);
}

inline void CloseVariables(void){
    delete[] ScriptVariableList;
}

Turbo::JSCallback ScriptFunctionList[] = {
    Script::FlipScreen,
    Script::GetFrameRate,
    Script::SetFrameRate,
    Script::GetScreenWidth,
    Script::GetScreenHeight,
    Script::GetClippingRectangle,
    Script::SetClippingRectangle,
    Script::Point,
    Script::PointSeries,
    Script::StippleLine,
    Script::Line,
    Script::LineSeries,
    Script::GradientLine,
    Script::BezierCurve,
    Script::Polygon,
    Script::Triangle,
    Script::GradientTriangle,
    Script::OutlinedTriangle,
    Script::Rectangle,
    Script::GradientRectangle,
    Script::OutlinedRectangle,
    Script::OutlinedEllipse,
    Script::GradientEllipse,
    Script::FilledComplex,
    Script::OutlinedComplex,
    Script::GradientComplex,
    Script::CreateColor,
    Script::CreateColorMatrix,
    Script::LoadImage,
    Script::CreateImage,
    Script::GrabImage,
    Script::GetSystemArrow,
    Script::GetSystemUpArrow,
    Script::GetSystemDownArrow,
    Script::LoadSurface,
    Script::CreateSurface,
    Script::GrabSurface,
    Script::LoadAnimation
};

Turbo::JSFunctionName ScriptFunctionNameList[] = {
    "FlipScreen",
    "GetFrameRate",
    "SetFrameRate",
    "GetScreenWidth",
    "GetScreenHeight",
    "GetClippingRectangle",
    "SetClippingRectangle",
    "Point",
    "PointSeries",
    "StippleLine",
    "Line",
    "LineSeries",
    "GradientLine",
    "BezierCurve",
    "Polygon",
    "Triangle",
    "GradientTriangle",
    "OutlinedTriangle",
    "Rectangle",
    "GradientRectangle",
    "OutlinedRectangle",
    "OutlinedEllipse",
    "GradientEllipse",
    "FilledComplex",
    "OutlinedComplex",
    "GradientComplex",
    "CreateColor",
    "CreateColorMatrix",
    "LoadImage",
    "CreateImage",
    "GrabImage",
    "GetSystemArrow",
    "GetSystemUpArrow",
    "GetSystemDownArrow",
    "LoadSurface",
    "CreateSurface",
    "GrabSurface",
    "LoadAnimation"
};

const char * Init(void){
    InitVariables();



    iso = v8::Isolate::GetCurrent();
    //bool err = ExecuteString(v8::String::NewFromUtf8(),v8::Handle<v8::Value> name,bool print_result);




    return PLUGINNAME;
}

void Close(void){
    CloseVariables();
}

int GetNumFunctions(void){
    return sizeof(ScriptFunctionList)/sizeof(*ScriptFunctionList);
}

Turbo::JSFunctionArray GetFunctions(void){
    return ScriptFunctionList;
}

Turbo::JSNameArray GetFunctionNames(void){
    return ScriptFunctionNameList;
}

int GetNumVariables(void){
    return numVariables;
}

Turbo::JSValueArray GetVariables(void){
    return ScriptVariableList;
}

Turbo::JSNameArray GetVariableNames(void){
    return ScriptVariableNameList;
}
