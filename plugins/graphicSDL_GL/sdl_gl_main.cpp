#define GRAPHIC_INTERNAL

#include <assert.h>

#include "main.h"

#ifndef _WIN32
#include <GL/glext.h>
#include <SDL/SDL.h>
#endif
#include "image.h"
#include "surface.h"
#include "color.h"
#include "screen.h"
#include "primitives.h"
#include "../../t5.h"

#define CHECK_FOR_PROCESS(NAME){\
        if(SDL_GL_GetProcAddress(NAME)==NULL){\
        printf("[" PLUGINNAME "] Init Error: " NAME " is not present in OpenGL library.\n");\
        exit(1);\
    }\
}

#define GET_GL_FUNCTION(NAME, TYPING)\
CHECK_FOR_PROCESS( #NAME );\
NAME = TYPING SDL_GL_GetProcAddress( #NAME )

SDL_Rect cmpltscreen = {0, 0, (Uint16)GetScreenWidth(), (Uint16)GetScreenHeight()};

SDL_GLContext glcontext;

SDL_Event event;
SDL_Window *screen = NULL;

TS_GLVideoData *GetGLVideoData(){
    static TS_GLVideoData data;
    return &data;
}

void (APIENTRY * glGenBuffers)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glDeleteBuffers)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glGenVertexArrays)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glBindBuffer)(GLenum,  GLuint) = NULL;
void (APIENTRY * glBindVertexArray)(GLuint) = NULL;
void (APIENTRY * glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum) = NULL;
void (APIENTRY * glEnableVertexAttribArray)(GLint) = NULL;
void (APIENTRY * glDisableVertexAttribArray)(GLint) = NULL;
void (APIENTRY * glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*) = NULL;
GLuint (APIENTRY * glCreateShader)(GLenum) = NULL;
void (APIENTRY * glDeleteShader)(GLenum) = NULL;
void (APIENTRY * glShaderSource)(GLuint, GLint, const GLchar **, const GLint *) = NULL;
void (APIENTRY * glGetShaderiv)(GLuint, GLenum, GLint*) = NULL;
void (APIENTRY * glCompileShader)(GLuint) = NULL;
GLuint (APIENTRY * glCreateProgram)(void) = NULL;
void (APIENTRY * glAttachShader)(GLuint, GLuint) = NULL;
void (APIENTRY * glLinkProgram)(GLuint) = NULL;
void (APIENTRY * glGetProgramiv)(GLuint, GLenum, GLint*) = NULL;

void LoadGLFunctions(){

    GET_GL_FUNCTION(glGenBuffers,               (void (APIENTRY *)(GLsizei, GLuint*)));
    GET_GL_FUNCTION(glDeleteBuffers,            (void (APIENTRY *)(GLsizei, GLuint*)));
    GET_GL_FUNCTION(glGenVertexArrays,          (void (APIENTRY *)(GLsizei, GLuint*)));
    GET_GL_FUNCTION(glBindBuffer,               (void (APIENTRY *)(GLenum, GLuint)));
    GET_GL_FUNCTION(glBindVertexArray,          (void (APIENTRY *)(GLuint)));
    GET_GL_FUNCTION(glBufferData,               (void (APIENTRY *)(GLenum, GLsizeiptr, const GLvoid *, GLenum)));
    GET_GL_FUNCTION(glEnableVertexAttribArray,  (void (APIENTRY *)(GLint)));
    GET_GL_FUNCTION(glDisableVertexAttribArray, (void (APIENTRY *)(GLint)));
    GET_GL_FUNCTION(glVertexAttribPointer,      (void (APIENTRY *)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *)));
    GET_GL_FUNCTION(glCreateShader,           (GLuint (APIENTRY *)(GLenum)));
    GET_GL_FUNCTION(glDeleteShader,             (void (APIENTRY *)(GLenum)));
    GET_GL_FUNCTION(glShaderSource,             (void (APIENTRY *)(GLuint, GLsizei, const GLchar **, const GLint *)));
    GET_GL_FUNCTION(glGetShaderiv,              (void (APIENTRY *)(GLuint, GLenum, GLint *)));
    GET_GL_FUNCTION(glCompileShader,            (void (APIENTRY *)(GLuint)));
    GET_GL_FUNCTION(glCreateProgram,          (GLuint (APIENTRY *)(void)));
    GET_GL_FUNCTION(glAttachShader,             (void (APIENTRY *)(GLuint,  GLuint)));
    GET_GL_FUNCTION(glLinkProgram,              (void (APIENTRY *)(GLuint)));
    GET_GL_FUNCTION(glGetProgramiv,             (void (APIENTRY *)(GLuint, GLenum, GLint*)));

}

int numerate(bool reset){
    static int i = 0;
    if(reset) {
        i = 0;
        return 0;
    }
    i++;
    return i-1;
}

int BuildDefaultShaders(){
    TS_Directories *TS_Dirs = GetDirs();
    const char *VertexShaderSource      = T5_LoadFileAsText((string(TS_Dirs->system)+string("vertex.glsl")).c_str());
    const char *FragmentShaderSource    = T5_LoadFileAsText((string(TS_Dirs->system)+string("fragment.glsl")).c_str());

    T5_FreeFileText(VertexShaderSource);
    T5_FreeFileText(FragmentShaderSource);
    return 1;
}

int TS_Filter(void * _unused, SDL_Event *event);
void * FlipScreenPointer            = V8FUNCPOINTER(FlipScreen);
void * GetScreenWidthPointer        = V8FUNCPOINTER(V8GetScreenHeight);
void * GetScreenHeightPointer       = V8FUNCPOINTER(V8GetScreenWidth);
void * CreateColorPointer           = V8FUNCPOINTER(CreateColor);

void * RectanglePointer             = V8FUNCPOINTER(Rectangle);
void * LinePointer                  = V8FUNCPOINTER(Line);
void * PointPointer                 = V8FUNCPOINTER(Point);
void * TrianglePointer              = V8FUNCPOINTER(Triangle);
void * PolygonPointer               = V8FUNCPOINTER(Polygon);

void * GradientRectanglePointer     = V8FUNCPOINTER(GradientRectangle);
void * GradientLinePointer          = V8FUNCPOINTER(GradientLine);
void * GradientTrianglePointer      = V8FUNCPOINTER(GradientTriangle);

void * OutlinedRectanglePointer     = V8FUNCPOINTER(OutlinedRectangle);

void * OutlinedCirclePointer        = V8FUNCPOINTER(OutlinedCircle);
void * FilledCirclePointer          = V8FUNCPOINTER(FilledCircle);
void * GradientCirclePointer        = V8FUNCPOINTER(GradientCircle);

void * OutlinedPolygonPointer       = V8FUNCPOINTER(OutlinedPolygon);

void * LoadImagePointer             = V8FUNCPOINTER(LoadImage);

void * NewSurfacePointer            = V8FUNCPOINTER(NewSurface);
void * SurfaceGrabPointer           = V8FUNCPOINTER(SurfaceGrab);
void * ImageGrabPointer             = V8FUNCPOINTER(ImageGrab);

initFunction Init(void){

    TS_GLVideoData *data = GetGLVideoData();

    data->ARBCopyTexture    = false;
    data->ARBPointSprite    = false;
    data->EXTSecondaryColor = false;
    data->NVCopyTexture     = false;
    data->NVPathRendering   = false;



	TS_Config *TS_conf = GetConfig();
    int *num_shaders = (int *)malloc(4);
    *num_shaders = 1;
    TS_conf->reserved = num_shaders;

    float scaleSize = TS_conf->scale;

    if(scaleSize==0){
        //ideally just disable video altogether.
        scaleSize = 1;
    }

    if(SDL_WasInit(SDL_INIT_EVERYTHING)==0){
        SDL_Init(SDL_INIT_VIDEO);
        atexit(SDL_Quit);
    }
    else if(SDL_WasInit(SDL_INIT_VIDEO)==0){
        SDL_InitSubSystem(SDL_INIT_VIDEO);
    }
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //TODO: make this set in the config.


    //TODO: fix this for SDL2
    //SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0); //TODO: make this set in the config


/*
    //Old SDL 1.2 code
    if((screen = SDL_SetVideoMode(GetScreenWidth()*scaleSize, GetScreenHeight()*scaleSize, DEPTH, SDL_VIDEO_FLAGS|((TS_conf->fullscreen)?SDL_FULLSCREEN:0)))==NULL)
    {
        printf("[" PLUGINNAME "] Video mode setup failed. Graphics subsystem closing.\n%s\n", SDL_GetError());
    }
	else
	{
        printf("[" PLUGINNAME "] Video mode setup succeeded.\n");
	}

*/



    if((screen = SDL_CreateWindow("TurboSphere Game Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  GetScreenWidth()*scaleSize, GetScreenHeight()*scaleSize, SDL2_VIDEO_FLAGS|((TS_conf->fullscreen)?SDL_WINDOW_FULLSCREEN:0)))==NULL){
        printf("[" PLUGINNAME "] Video mode setup failed. Graphics subsystem closing.\n%s\n", SDL_GetError());
    }

    LoadGLFunctions();

    glcontext = SDL_GL_CreateContext(screen);

    int vsync = 0;
    int err = SDL_GL_SetSwapInterval(vsync);

    if(err<0){
        printf("[" PLUGINNAME "] Init Warning: preferred vsync mode (%s) is not supported.\n", (vsync==0)?((const char *)"vsync off"):((const char *)"vsync on"));
    }

    glClearColor(0, 0, 0, 255);
    glClearDepth(1.0f);
    glEnable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);
    //glEnable(GL_PROGRAM_POINT_SIZE);
    //glPointSize();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    glEnable(GL_SCISSOR_TEST);

    glViewport(0, 0, GetScreenWidth()*scaleSize, GetScreenHeight()*scaleSize);
    glScissor(0, 0, GetScreenWidth()*scaleSize, GetScreenHeight()*scaleSize);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();


    glOrtho(0, GetScreenWidth(), GetScreenHeight(), 0, 1, -1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

    if(scaleSize!=1){
        glPointSize(scaleSize);
        glLineWidth(scaleSize);
    }
    //glLineStipple(1, 0xAAAA);
    //glScalef(2.0f, 2.0f, 2.0f);

    //glEnable(GL_DITHER);
    //glEnable(GL_PROGRAM_POINT_SIZE);

    ColorInit();
    ImageInit();
    SurfaceInit();
    PrimitivesInit();

    //glLoadIdentity();
    printf("[" PLUGINNAME "] Info: Using OpenGL version %s\n", glGetString(GL_VERSION));
    if (IMG_Init(IMG_FLAGS) <=0) {
        printf("[graphicSDL] Unable to init SDL Image: %s\n", IMG_GetError());
    }
/*
    //Old SDL 1.2 code.
    if(SDL_GetVideoInfo()->wm_available){
        SDL_WM_SetCaption("TurboSphere Game Engine", "TurboSphere Game Engine"); //TODO: Make this set in the config file.
    }
*/
	SDL_ShowCursor(0);

    //set up the event filter
/*
    //Old SDL 1.2 code.
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
*/
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
    SDL_SetEventFilter(TS_Filter, NULL);

    return (const char *)"SDL_GL";
}


void Close(){
    ColorClose();
    ImageClose();
    SurfaceClose();

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(screen);

	SDL_VideoQuit();
}

int GetNumFunctions(){
    return NUMFUNCS;
}

functionArray GetFunctions(){
    numerate(true);
    functionArray funcs = (functionArray)calloc(NUMFUNCS, sizeof(void*));
    funcs[numerate(false)] = FlipScreenPointer;
    funcs[numerate(false)] = GetScreenWidthPointer;
    funcs[numerate(false)] = GetScreenHeightPointer;
    funcs[numerate(false)] = CreateColorPointer;

    funcs[numerate(false)] = RectanglePointer;
    funcs[numerate(false)] = LinePointer;
    funcs[numerate(false)] = PointPointer;
    funcs[numerate(false)] = TrianglePointer;
    funcs[numerate(false)] = PolygonPointer;

    funcs[numerate(false)] = GradientRectanglePointer;
    funcs[numerate(false)] = GradientLinePointer;
    funcs[numerate(false)] = GradientTrianglePointer;

    funcs[numerate(false)] = OutlinedRectanglePointer;

    funcs[numerate(false)] = OutlinedCirclePointer;
    funcs[numerate(false)] = FilledCirclePointer;
    funcs[numerate(false)] = GradientCirclePointer;

    funcs[numerate(false)] = LoadImagePointer;
    funcs[numerate(false)] = ImageGrabPointer;

    funcs[numerate(false)] = NewSurfacePointer;
    funcs[numerate(false)] = SurfaceGrabPointer;


    assert(numerate(false)==NUMFUNCS);

    return funcs;
}

nameArray GetFunctionNames(){
    numerate(true);
    nameArray names = (nameArray)calloc(NUMFUNCS, sizeof(functionName));
    names[numerate(false)] = (functionName)"FlipScreen";        //1
    names[numerate(false)] = (functionName)"GetScreenHeight";   //2
    names[numerate(false)] = (functionName)"GetScreenWidth";    //3
    names[numerate(false)] = (functionName)"Color";             //4

    names[numerate(false)] = (functionName)"Rectangle";         //5
    names[numerate(false)] = (functionName)"Line";              //6
    names[numerate(false)] = (functionName)"Point";             //7
    names[numerate(false)] = (functionName)"Triangle";          //8
    names[numerate(false)] = (functionName)"Polygon";           //9

    names[numerate(false)] = (functionName)"GradientRectangle"; //10
    names[numerate(false)] = (functionName)"GradientLine";      //11
    names[numerate(false)] = (functionName)"GradientTriangle";  //12

    names[numerate(false)] = (functionName)"OutlinedRectangle"; //13

    names[numerate(false)] = (functionName)"OutlinedCircle";    //14
    names[numerate(false)] = (functionName)"FilledCircle";      //15
    names[numerate(false)] = (functionName)"GradientCircle";    //16

    names[numerate(false)] = (functionName)"Image";             //17
    names[numerate(false)] = (functionName)"GrabImage";         //18

    names[numerate(false)] = (functionName)"Surface";           //19
    names[numerate(false)] = (functionName)"GrabSurface";       //20

    assert(numerate(false)==NUMFUNCS);

    return names;
}

int GetNumVariables(void){return 0;}

v8FunctionArray GetVariables(void){return NULL;}

nameArray GetVariableNames(void){return NULL;}

int TS_Filter(void * _unused, SDL_Event *event){
	if(event->type==SDL_QUIT){
        //QuitAll();
        exit(0);
    }
    return 1;
}
