#define MAPENGINE_INTERNAL
#include "mapengine.h"
#include "map.h"

#define CHECK_FOR_PROCESS(NAME){\
        if(SDL_GL_GetProcAddress(NAME)==NULL){\
        printf("[" PLUGINNAME "] Init Error: " NAME " is not present in OpenGL library.\n");\
        exit(1);\
    }\
}

GLuint defaultShaderProgram = 0;

DECLARE_OBJECT_TEMPLATES(Map);

void (APIENTRY * glGenBuffers)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glDeleteBuffers)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glGenVertexArrays)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glBindBuffer)(GLenum,  GLuint) = NULL;
void (APIENTRY * glBindVertexArray)(GLuint) = NULL;
void (APIENTRY * glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum) = NULL;
GLuint (APIENTRY * glCreateShader)(GLenum) = NULL;
void (APIENTRY * glDeleteShader)(GLenum) = NULL;
void (APIENTRY * glShaderSource)(GLuint, GLint, const GLchar **, const GLvoid *) = NULL;
void (APIENTRY * glGetShaderiv)(GLuint, GLenum, GLint*) = NULL;
void (APIENTRY * glCompileShader)(GLuint) = NULL;
GLuint (APIENTRY * glCreateProgram)(void) = NULL;
void (APIENTRY * glAttachShader)(GLuint, GLuint) = NULL;
void (APIENTRY * glLinkProgram)(GLuint) = NULL;
void (APIENTRY * glGetProgramiv)(GLuint, GLenum, GLint*) = NULL;


void LoadGLFunctions(){

    //char *extensions = (char *)glGetString(GL_EXTENSIONS);


    CHECK_FOR_PROCESS("glGenBuffers");
    glGenBuffers                = (void (APIENTRY *)(GLsizei, GLuint*))   SDL_GL_GetProcAddress("glGenBuffers");

    CHECK_FOR_PROCESS("glDeleteBuffers");
    glDeleteBuffers             = (void (APIENTRY *)(GLsizei, GLuint*))   SDL_GL_GetProcAddress("glDeleteBuffers");

    CHECK_FOR_PROCESS("glGenVertexArrays");
    glGenVertexArrays           = (void (APIENTRY *)(GLsizei, GLuint*))   SDL_GL_GetProcAddress("glGenVertexArrays");

    CHECK_FOR_PROCESS("glBindBuffer");
    glBindBuffer                = (void (APIENTRY *)(GLenum, GLuint))     SDL_GL_GetProcAddress("glBindBuffer");

    CHECK_FOR_PROCESS("glBindVertexArray");
    glBindVertexArray           = (void (APIENTRY *)(GLuint))             SDL_GL_GetProcAddress("glBindVertexArray");

    CHECK_FOR_PROCESS("glBufferData");
    glBufferData                = (void (APIENTRY *)(GLenum, GLsizeiptr, const GLvoid *, GLenum))   SDL_GL_GetProcAddress("glBufferData");

    CHECK_FOR_PROCESS("glCreateShader");
    glCreateShader  = (GLuint (APIENTRY *)(GLenum))   SDL_GL_GetProcAddress("glCreateShader");

    CHECK_FOR_PROCESS("glDeleteShader");
    glDeleteShader  = (void (APIENTRY *)(GLenum))   SDL_GL_GetProcAddress("glDeleteShader");

    CHECK_FOR_PROCESS("glShaderSource");
    glShaderSource  = (void (APIENTRY *)(GLuint, GLint, const GLchar **, const GLvoid *))   SDL_GL_GetProcAddress("glShaderSource");

    CHECK_FOR_PROCESS("glGetShaderiv");
    glGetShaderiv  = (void (APIENTRY *)(GLuint, GLenum, GLint*))   SDL_GL_GetProcAddress("glGetShaderiv");

    CHECK_FOR_PROCESS("glCompileShader");
    glCompileShader  = (void (APIENTRY *)(GLuint))   SDL_GL_GetProcAddress("glCompileShader");

    CHECK_FOR_PROCESS("glCreateProgram");
    glCreateProgram  = (GLuint (APIENTRY *)(void))   SDL_GL_GetProcAddress("glCreateProgram");

    CHECK_FOR_PROCESS("glAttachShader");
    glAttachShader  = (void (APIENTRY *)(GLuint,  GLuint))   SDL_GL_GetProcAddress("glAttachShader");

    CHECK_FOR_PROCESS("glLinkProgram");
    glLinkProgram  = (void (APIENTRY *)(GLuint))   SDL_GL_GetProcAddress("glLinkProgram");

    CHECK_FOR_PROCESS("glGetProgramiv");
    glGetProgramiv  = (void (APIENTRY *)(GLuint, GLenum, GLint*))   SDL_GL_GetProcAddress("glGetProgramiv");
}

v8Function LoadMap(V8ARGS);
v8Function MapEngine(V8ARGS);

void* LoadMapPointer            = V8FUNCPOINTER(LoadMap);
void* IsMapEngineRunningPointer = V8FUNCPOINTER(IsMapEngineRunning);
void* SetRenderScriptPointer    = V8FUNCPOINTER(SetRenderScript);
void* SetUpdateScriptPointer    = V8FUNCPOINTER(SetUpdateScript);
void* ChangeMapPointer          = V8FUNCPOINTER(ChangeMap);
void* ExitMapEnginePointer      = V8FUNCPOINTER(ExitMapEngine);
void* MapEnginePointer          = V8FUNCPOINTER(MapEngine);
void* LoadSpriteSetPointer      = V8FUNCPOINTER(LoadSpriteSet);
void* CreatePersonPointer      = V8FUNCPOINTER(CreatePerson);

initFunction Init(void){

    LoadGLFunctions();
    //BuildShaders();
    if(sizeof(float)!=4){
        printf("[" PLUGINNAME "] Warning: Internal floating point size mismatch. Undefined behaviour may occur.\n");
    }
    ResetMapEngine();
    INIT_OBJECT_TEMPLATES(Map);
    SET_CLASS_NAME(Map, "Map");

    InitSpriteSet();
    InitMapMain();

    return (const char *)"mapengineGL";
}

void Close(void){
    CLOSE_OBJECT_TEMPLATES(Map);
    CloseSpriteSet();
    CloseMapMain();
}

int GetNumFunctions(void){
    return NUMFUNCS;
}

functionArray GetFunctions(void){
    functionArray funcs = (functionArray)calloc(NUMFUNCS, sizeof(void*));
    funcs[0] = LoadMapPointer;
    funcs[1] = IsMapEngineRunningPointer;
    funcs[2] = SetRenderScriptPointer;
    funcs[3] = SetUpdateScriptPointer;
    funcs[4] = ChangeMapPointer;
    funcs[5] = ExitMapEnginePointer;
    funcs[6] = MapEnginePointer;
    funcs[7] = LoadSpriteSetPointer;
    funcs[8] = CreatePersonPointer;
    return funcs;
}

nameArray GetFunctionNames(void){
    nameArray funcnames = (nameArray)calloc(NUMFUNCS, sizeof(const char *));
    funcnames[0] = (const char *)"Map";
    funcnames[1] = (const char *)"IsMapEngineRunning";
    funcnames[2] = (const char *)"SetRenderScript";
    funcnames[3] = (const char *)"SetUpdateScript";
    funcnames[4] = (const char *)"ChangeMap";
    funcnames[5] = (const char *)"ExitMapEngine";
    funcnames[6] = (const char *)"MapEngine";
    funcnames[7] = (const char *)"SpriteSet";
    funcnames[8] = (const char *)"CreatePerson";
    return funcnames;
}

int GetNumVariables(void){
    return 0;
}

void ** GetVariables(void){
    return NULL;
}

nameArray GetVariableNames(void){
    return NULL;
}

v8Function LoadMap(V8ARGS){
    if(args.Length()<1){
        THROWERROR("[" PLUGINNAME "] LoadMap Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);

    BEGIN_OBJECT_WRAP_CODE;

    TS_Directories *TS_dirs = GetDirs();

    v8::String::Utf8Value str(args[0]);

    TS_Map *map = new TS_Map((std::string(TS_dirs->map)+std::string(*str)).c_str());

    if(map==NULL){
        THROWERROR(string("[" PLUGINNAME "] LoadMap Error: Could not open map ").append(*str).c_str());
    }

    END_OBJECT_WRAP_CODE(Map, map);

}

v8Function MapEngine(V8ARGS){
    if(args.Length()<1){
        THROWERROR("[" PLUGINNAME "] MapEngine Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);

    TS_Directories *TS_dirs = GetDirs();

    v8::String::Utf8Value str(args[0]);

    TS_Map *map = new TS_Map((std::string(TS_dirs->map)+std::string(*str)).c_str());

    if(map==NULL){
        THROWERROR(string("[" PLUGINNAME "] MapEngine Error: Could not open map ").append(*str).c_str());
    }
    TS_MapEngine(map);
    return v8::Undefined();
}

