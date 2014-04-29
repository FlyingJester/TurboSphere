#include "frontend.h"
#include "api.h"
#include "iff.h"
#include "blendmode.h"
#include "backend.h"
#include "color/color.h"
#include "shader/shader.h"
#include "../common/plugin.h"
#include "../../configmanager/openscript.h"
#include "../../configmanager/opengame.h"
#include "../../graphiccommon/screen.h"

#include "config.h"

#include "init/sapphire_init.h"

#include <string>
#include <cassert>
#include <t5.h>

using std::string;

#ifdef _WIN32
//...Not sure. Use resource DLLs, I guess.
#else
    #ifdef __linux__
        #include <sys/mman.h>
    #else
//        #error FIXME: Add the header for mmap.
    #endif

//    #include "resourcefile.sup"

#endif

v8::Isolate *iso = NULL;

const size_t numVariables = 10;
Turbo::JSValueArray SapphireScriptVariableList = NULL;

Turbo::JSVariableName SapphireScriptVariableNameList[] = {
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

    SapphireScriptVariableList = new Turbo::JSValue[numVariables];

    for(uint32_t i = 0; i<10; i++) //BlendModes.
        SapphireScriptVariableList[i] = v8::Integer::New(iso, i+1);
}

inline void CloseVariables(void){
    delete[] SapphireScriptVariableList;
}

Turbo::JSCallback SapphireScriptFunctionList[] = {
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

Turbo::JSFunctionName SapphireScriptFunctionNameList[] = {
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
    "Color",
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

static void CompileIffScripts(void * isol, const iff::IffData &value){

printf("[" PLUGINNAME "] Info: Compiling Script %s of size %i.\n", value.name.c_str(), value.content.first);

    v8::Script::Compile(v8::String::NewFromUtf8(iso, value.content.second, v8::String::NewStringType::kNormalString, value.content.first),v8::String::NewFromUtf8(iso, value.name.c_str()));

}

const char * Init(int ID){
    InitVariables();

    /////
    // Initialize and populate configuration

    TS_Config *TS_Conf = GetConfig();

    InitColor(ID);

    configl.niceImages = 0;
    configl.niceCircles = 1;
    configl.fullscreen = TS_Conf->fullscreen;
    configl.composite = TS_Conf->compositing;
    configl.surfaceThread = 1;
    configl.glThread = 1;

    configl.vsync = (TS_Conf->fullscreen&&TS_Conf->compositing);
    configl.scale = TS_Conf->scale;

    SetScreenWidth(  TS_Conf->screenwidth  );
    SetScreenHeight( TS_Conf->screenheight );

    InitVideo(TS_Conf->screenwidth, TS_Conf->screenheight, &configl);

    iso = v8::Isolate::GetCurrent();
    //bool err = ExecuteString(v8::String::NewFromUtf8(),v8::Handle<v8::Value> name,bool print_result);



    TS_CurrentShader = TS_LoadSystemShader("system.shade");
    TS_DefaultShader = TS_CurrentShader;
    TS_DefaultCompositeShader = TS_CurrentShader;
    TS_CurrentCompositeShader = TS_CurrentShader;

    CurrentColorAttrib      = glGetAttribLocation(TS_CurrentShader, "Color");
    CurrentVertexAttrib     = glGetAttribLocation(TS_CurrentShader, "Vertex");
    CurrentTexcoordAttrib   = glGetAttribLocation(TS_CurrentShader, "Texcoord");

    glEnableVertexAttribArray(CurrentColorAttrib);
    glEnableVertexAttribArray(CurrentVertexAttrib);
    glEnableVertexAttribArray(CurrentTexcoordAttrib);

    assert(T5_IsFile("extra/" PLUGINNAME "/scripts.tsiff"));

    printf("[" PLUGINNAME "] Info: Reading from file %s.\n", "extra/" PLUGINNAME "/scripts.tsiff");

    iff::IffFile file = iff::IffFile();

    file.Load("extra/" PLUGINNAME "/scripts.tsiff");

    file.foreach(CompileIffScripts, (void *)iso);

    BackendInit(false, false);

    return PLUGINNAME;
}

void Close(void){
    CloseVariables();
    BackendClose();
}

int GetNumFunctions(void){
    return sizeof(SapphireScriptFunctionList)/sizeof(*SapphireScriptFunctionList);
}

Turbo::JSFunctionArray GetFunctions(void){
    return SapphireScriptFunctionList;
}

Turbo::JSNameArray GetFunctionNames(void){
    return SapphireScriptFunctionNameList;
}

int GetNumVariables(void){
    return numVariables;
}

Turbo::JSValueArray GetVariables(void){
    return SapphireScriptVariableList;
}

Turbo::JSNameArray GetVariableNames(void){
    return SapphireScriptVariableNameList;
}
