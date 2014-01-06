#define GRAPHIC_INTERNAL

#include <assert.h>
#include "main.h"
#include "screen.h"
#include "shader.h"

#ifndef _WIN32
//#include <GL/glext.h>
#include "../../SDL2/SDL_opengl.h"
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

int PluginID;

unsigned int ShaderID = 0;
unsigned int ShaderProgramID = 0;

TS_Shader TS_DefaultShader = 0;
TS_Shader TS_CurrentShader = 0;
TS_Shader TS_DefaultCompositeShader = 0;
TS_Shader TS_CurrentCompositeShader = 0;

DECLARE_OBJECT_TEMPLATES(ScriptShader);
DECLARE_OBJECT_TEMPLATES(ScriptShaderProgram);

GLuint TS_SDL_GL_GetCurrentShader(void){
    return TS_CurrentShader;
}

GLuint TS_SDL_GL_GetDefaultShader(void){
    return TS_DefaultShader;
}

void GetPluginInfo(TS_PluginInfo *info){

    info->name      = "SDL_GL_threaded";
    info->version   = "v0.3";
    info->author    = "Martin McDonough";
    info->date      = __DATE__;
    info->description = "GL based graphics plugin. SDL2 for window management. Threaded surface drawing and blitting. Exports TS_SDL_GL_MakeV8SurfaceHandleFromPixels and TS_SDL_GL_MakeV8ImageHandleFromGLTexture.";
}

TS_Texture TS_EmptyTexture = 0;

SDL_GLContext glcontext;

SDL_Event event;
SDL_Window *screen = NULL;

TS_GLVideoData *GetGLVideoData(){
    static TS_GLVideoData data;
    return &data;
}

void (APIENTRY * glBufferSubData)(GLenum, GLintptr, GLsizeiptr, const GLvoid *) = NULL;
void (APIENTRY * glGenBuffers)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glDeleteBuffers)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glGenVertexArrays)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glDeleteVertexArrays)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glBindBuffer)(GLenum,  GLuint) = NULL;
void (APIENTRY * glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum) = NULL;
GLenum (APIENTRY * glCreateShader)(GLenum) = NULL;
void (APIENTRY * glDeleteShader)(GLenum) = NULL;
void (APIENTRY * glShaderSource)(GLenum, GLint, const GLchar **, const GLint *) = NULL;
void (APIENTRY * glGetShaderiv)(GLuint, GLenum, GLint*) = NULL;
void (APIENTRY * glCompileShader)(GLenum) = NULL;
GLenum (APIENTRY * glCreateProgram)(void) = NULL;
void (APIENTRY * glUseProgram)(GLenum) = NULL;
void (APIENTRY * glAttachShader)(GLenum, GLenum) = NULL;
void (APIENTRY * glLinkProgram)(GLenum) = NULL;
void (APIENTRY * glGetProgramiv)(GLuint, GLenum, GLint*) = NULL;
GLboolean (APIENTRY * glIsShader)(GLuint) = NULL;
void (APIENTRY * glGetShaderInfoLog)(GLuint,  GLsizei,  GLsizei *,  GLchar *) = NULL;
void (APIENTRY * glGetProgramInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*) = NULL;
void (APIENTRY * glDeleteProgram)(GLuint) = NULL;
void (APIENTRY * glProgramUniform4fv)(GLuint,  GLint,  GLsizei,  const GLfloat *) = NULL;
void (APIENTRY * glProgramUniform2fv)(GLuint,  GLint,  GLsizei,  const GLfloat *) = NULL;
void (APIENTRY * glProgramUniform2iv)(GLuint,  GLint,  GLsizei,  const GLint *) = NULL;
void (APIENTRY * glProgramUniform1f)(GLuint program, GLint location, GLfloat v0) = NULL;
void (APIENTRY * glEnableVertexAttribArray)(GLuint) = NULL;
void (APIENTRY * glDisableVertexAttribArray)(GLuint) = NULL;
void (APIENTRY * glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*) = NULL;
void (APIENTRY * glVertexAttribIPointer)(GLuint, GLint, GLenum, GLsizei, const GLvoid*) = NULL;
void (APIENTRY * glGenFramebuffers)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glDeleteFramebuffers)(GLsizei, GLuint*) = NULL;
void (APIENTRY * glBindFramebuffer)(GLenum, GLuint) = NULL;
void (APIENTRY * glFramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint) = NULL;
void (APIENTRY * glBindVertexArray)(GLuint) = NULL;
void (APIENTRY * glCopyImageSubData)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei) = NULL;
void (APIENTRY * glBindAttribLocation)(GLuint program, GLuint index, const GLchar *name) = NULL;
GLint(APIENTRY * glGetAttribLocation )(GLuint program, const GLchar *name) = NULL;
GLboolean (APIENTRY * glIsProgram)(GLuint program) = NULL;
GLint(APIENTRY * glGetUniformLocation)(GLuint program, const GLchar *name) = NULL;

GLuint softTexCopy = 0;

void TS_CopyImageSubData(GLuint srcName, GLenum srcTarget, GLint srcLevel,
	    GLint srcX, GLint srcY, GLint srcZ,
	    GLuint dstName, GLenum dstTarget, GLint dstLevel,
	    GLint dstX, GLint dstY, GLint dstZ,
	    GLsizei width, GLsizei height, GLsizei depth){

    int srcwidth, srcheight, dstwidth, dstheight;
    glBindTexture(GL_TEXTURE_2D, dstName);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &dstwidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &dstheight);
    glBindTexture(GL_TEXTURE_2D, srcName);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &srcwidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &srcheight);

    if(!((srcwidth==width)&&(srcheight==height)&&(srcheight==dstheight)&&(srcwidth==srcheight))){

        //Make the surfaces to hold the source and destinations.
        SDL_Surface *srcsurface = SDL_CreateRGBSurface(0, srcwidth, srcheight, DEPTH, CHANNEL_MASKS);

        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, srcsurface->pixels);

        //Fill the dst surface with the current pixels of the destination texture.
        glBindTexture(GL_TEXTURE_2D, dstName);

        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &dstwidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &dstheight);

        SDL_Surface *dstsurface = SDL_CreateRGBSurface(0, dstwidth, dstheight, DEPTH, CHANNEL_MASKS);

        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, dstsurface->pixels);

        //Copy the pixels needed for the new sub data.

        SDL_Rect srcrect = {srcX, srcY, width, height};
        SDL_Rect dstrect = {dstX, dstY, width, height};

        SDL_BlitSurface(srcsurface, &srcrect, dstsurface, &dstrect);


        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dstwidth, dstheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, dstsurface->pixels);

        SDL_FreeSurface(dstsurface);
        SDL_FreeSurface(srcsurface);
    }
    else{
        void * pixels = malloc(width*height);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glBindTexture(GL_TEXTURE_2D, dstName);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        free(pixels);
    }
}

void LoadGLFunctions(){

    GET_GL_FUNCTION(glGenBuffers,               (void (APIENTRY *)(GLsizei, GLuint*)));
    GET_GL_FUNCTION(glDeleteBuffers,            (void (APIENTRY *)(GLsizei, GLuint*)));
    GET_GL_FUNCTION(glGenVertexArrays,          (void (APIENTRY *)(GLsizei, GLuint*)));
    GET_GL_FUNCTION(glDeleteVertexArrays,       (void (APIENTRY *)(GLsizei, GLuint*)));
    GET_GL_FUNCTION(glBindBuffer,               (void (APIENTRY *)(GLenum, GLuint)));
    GET_GL_FUNCTION(glBufferData,               (void (APIENTRY *)(GLenum, GLsizeiptr, const GLvoid *, GLenum)));
    GET_GL_FUNCTION(glCreateShader,           (GLenum (APIENTRY *)(GLenum)));
    GET_GL_FUNCTION(glDeleteShader,             (void (APIENTRY *)(GLenum)));
    GET_GL_FUNCTION(glShaderSource,             (void (APIENTRY *)(GLuint, GLsizei, const GLchar **, const GLint *)));
    GET_GL_FUNCTION(glGetShaderiv,              (void (APIENTRY *)(GLuint, GLenum, GLint *)));
    GET_GL_FUNCTION(glCompileShader,            (void (APIENTRY *)(GLenum)));
    GET_GL_FUNCTION(glCreateProgram,          (GLenum (APIENTRY *)(void)));
    GET_GL_FUNCTION(glUseProgram,               (void (APIENTRY *)(GLenum)));
    GET_GL_FUNCTION(glAttachShader,             (void (APIENTRY *)(GLenum,  GLenum)));
    GET_GL_FUNCTION(glLinkProgram,              (void (APIENTRY *)(GLenum)));
    GET_GL_FUNCTION(glGetProgramiv,             (void (APIENTRY *)(GLuint, GLenum, GLint*)));
    GET_GL_FUNCTION(glIsShader,            (GLboolean (APIENTRY *)(GLuint)));
    GET_GL_FUNCTION(glGetShaderInfoLog,         (void (APIENTRY *)(GLuint,  GLsizei,  GLsizei *,  GLchar *)));
    GET_GL_FUNCTION(glGetProgramInfoLog,        (void (APIENTRY *)(GLuint, GLsizei, GLsizei*, GLchar*)));
    GET_GL_FUNCTION(glDeleteProgram,            (void (APIENTRY *)(GLuint)));
    GET_GL_FUNCTION(glGetUniformLocation,      (GLint (APIENTRY *)(GLuint, const GLchar *)));
    GET_GL_FUNCTION(glProgramUniform4fv,        (void (APIENTRY *)(GLuint, GLint, GLsizei, const GLfloat *)));
    GET_GL_FUNCTION(glProgramUniform2fv,        (void (APIENTRY *)(GLuint, GLint, GLsizei, const GLfloat *)));
    GET_GL_FUNCTION(glProgramUniform2iv,        (void (APIENTRY *)(GLuint, GLint, GLsizei, const GLint *)));
    GET_GL_FUNCTION(glProgramUniform1f,         (void (APIENTRY *)(GLuint program, GLint location, GLfloat v0)));
    GET_GL_FUNCTION(glEnableVertexAttribArray,  (void (APIENTRY *)(GLuint)));
    GET_GL_FUNCTION(glDisableVertexAttribArray, (void (APIENTRY *)(GLuint)));
    GET_GL_FUNCTION(glVertexAttribPointer,      (void (APIENTRY *)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*)));
    GET_GL_FUNCTION(glVertexAttribIPointer,     (void (APIENTRY *)(GLuint, GLint, GLenum, GLsizei, const GLvoid*)));
    GET_GL_FUNCTION(glBufferSubData,            (void (APIENTRY *)(GLenum, GLintptr, GLsizeiptr, const GLvoid *)));
    GET_GL_FUNCTION(glGenFramebuffers,          (void (APIENTRY *)(GLsizei, GLuint*)));
    GET_GL_FUNCTION(glDeleteFramebuffers,       (void (APIENTRY *)(GLsizei, GLuint*)));
    GET_GL_FUNCTION(glBindFramebuffer,          (void (APIENTRY *)(GLenum, GLuint)));
    GET_GL_FUNCTION(glFramebufferTexture2D,     (void (APIENTRY *)(GLenum, GLenum, GLenum, GLuint, GLint)));
    GET_GL_FUNCTION(glBindVertexArray,          (void (APIENTRY *)(GLuint)));
    GET_GL_FUNCTION(glBindAttribLocation,       (void (APIENTRY *)(GLuint program, GLuint index, const GLchar *name)));
    GET_GL_FUNCTION(glGetAttribLocation,       (GLint (APIENTRY *)(GLuint program, const GLchar *name)));
    GET_GL_FUNCTION(glIsProgram,           (GLboolean (APIENTRY *)(GLuint program)));
    GET_GL_FUNCTION(glGetUniformLocation,      (GLint (APIENTRY *)(GLuint program, const GLchar *name)));


    if(SDL_GL_GetProcAddress("glCopyImageSubData")!=NULL){
        glCopyImageSubData = (void(APIENTRY *)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)) SDL_GL_GetProcAddress("glCopyImageSubData");
        printf("[" PLUGINNAME "] Init Info: CopyImageSubData is present in OpenGL library.\n");
    }
    else
    if(SDL_GL_GetProcAddress("glCopyImageSubDataNV")!=NULL){
        glCopyImageSubData = (void(APIENTRY *)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)) SDL_GL_GetProcAddress("glCopyImageSubDataNV");
        printf("[" PLUGINNAME "] Init Info: CopyImageSubData is not present in OpenGL library. Using alternative.\n");
    }
    else{
        glCopyImageSubData = TS_CopyImageSubData;
        printf("[" PLUGINNAME "] Init Warning: Neither glCopyImageSubData or glCopyImageSubDataNV is not present in OpenGL library. Reverting to software image copying.\n");
    }

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

int TS_Filter(void * _unused, SDL_Event *event);
void * FlipScreenPointer            = V8FUNCPOINTER(V8FlipScreen);
void * GetScreenWidthPointer        = V8FUNCPOINTER(V8GetScreenHeight);
void * GetScreenHeightPointer       = V8FUNCPOINTER(V8GetScreenWidth);
void * GetClippingRectanglePointer  = V8FUNCPOINTER(GetClippingRectangle);
void * SetClippingRectanglePointer  = V8FUNCPOINTER(SetClippingRectangle);
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


void * CreateFragShaderPointer      = V8FUNCPOINTER(CreateFragShader);
void * CreateVertShaderPointer      = V8FUNCPOINTER(CreateVertShader);
void * CreateProgramPointer         = V8FUNCPOINTER(CreateProgram);
void * LoadProgramPointer           = V8FUNCPOINTER(LoadProgram);

void * UseProgramPointer            = V8FUNCPOINTER(UseProgram);
void * UseCompositeProgramPointer   = V8FUNCPOINTER(UseCompositeProgram);

void * GetSystemProgramPointer      = V8FUNCPOINTER(GetSystemProgram);
void * GetSystemCompositePointer    = V8FUNCPOINTER(GetSystemCompositeProgram);


void ResetOrtho(void){

    glClearColor(0, 0, 0, 255);
	TS_Config *TS_conf = GetConfig();
    float scaleSize = TS_conf->scale;

    if(scaleSize==0){
        //ideally just disable video altogether.
        scaleSize = 1;
    }
    glViewport(0, 0, GetScreenWidth()*scaleSize, GetScreenHeight()*scaleSize);
    //glScissor(0, 0, GetScreenWidth()*scaleSize, GetScreenHeight()*scaleSize);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();


    glOrtho(0, GetScreenWidth(), GetScreenHeight(), 0, 1, -1);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

initFunction Init(int ID){


    PluginID = ID;

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

    if(SDL_WasInit(0)==0){
        SDL_Init(SDL_INIT_VIDEO);
        //atexit(SDL_Quit);
    }
    else if(SDL_WasInit(SDL_INIT_VIDEO)==0){
        SDL_InitSubSystem(SDL_INIT_VIDEO);
    }

    //Try and get the version of GL that we wrote this for.
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //TODO: make this set in the config.

    if((screen = SDL_CreateWindow("TurboSphere Game Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  GetScreenWidth()*scaleSize, GetScreenHeight()*scaleSize, SDL2_VIDEO_FLAGS|((TS_conf->fullscreen)?SDL_WINDOW_FULLSCREEN:0)))==NULL){
        printf("[" PLUGINNAME "] Video mode setup failed. Graphics subsystem closing.\n%s\n", SDL_GetError());
    }

    glcontext = SDL_GL_CreateContext(screen);

    int vsync = 0;
    int err = SDL_GL_SetSwapInterval(vsync);

    if(err<0){
        printf("[" PLUGINNAME "] Init Warning: preferred vsync mode (%s) is not supported.\n", (vsync==0)?((const char *)"vsync off"):((const char *)"vsync on"));
    }

    LoadGLFunctions();

    glClearColor(0, 0, 0, 255);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);


    printf("[" PLUGINNAME "] Info: Using OpenGL version %s\n", glGetString(GL_VERSION));
    if (IMG_Init(IMG_FLAGS) <=0) {
        printf("[" PLUGINNAME "] Unable to init SDL Image: %s\n", IMG_GetError());
    }

	SDL_ShowCursor(0);

    //set up the event filter

    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
    SDL_SetEventFilter(TS_Filter, NULL);


    INIT_OBJECT_TEMPLATES(ScriptShader);
    INIT_OBJECT_TEMPLATES(ScriptShaderProgram);

    ShaderID = SHADER_ID;
    ShaderProgramID = SHADERPROGRAM_ID;

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


    ScreenInit();
    ColorInit();
    ImageInit();
    SurfaceInit();
    PrimitivesInit();

    printf(BRACKNAME " Info: Compiled default shader as %i. Locations are \t%i,\t%i,\t%i\n", TS_DefaultShader, CurrentColorAttrib, CurrentVertexAttrib , CurrentTexcoordAttrib);

    glUseProgram(TS_DefaultShader);
    uint32_t white = 0xFFFFFFFF;
    glGenTextures(1, &TS_EmptyTexture);
    glBindTexture(GL_TEXTURE_2D, TS_EmptyTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &white);

    glEnable(GL_TEXTURE_2D);

    printf(BRACKNAME " Info: EmptyTexture is number %i.\n", TS_EmptyTexture);

    return (const char *)"SDL_GL_threaded";
}


void Close(){
    ColorClose();
    ImageClose();
    SurfaceClose();
    ScreenClose();

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(screen);

	SDL_VideoQuit();
}

int GetNumFunctions(){
    return NUMFUNCS;
}

functionArray GetFunctions(){
    numerate(true);
    assert(FlipScreen!=NULL);
    functionArray funcs = (functionArray)calloc(NUMFUNCS, sizeof(void*));
    funcs[numerate(false)] = FlipScreenPointer;
    funcs[numerate(false)] = GetScreenWidthPointer;
    funcs[numerate(false)] = GetScreenHeightPointer;
    funcs[numerate(false)] = GetClippingRectanglePointer;
    funcs[numerate(false)] = SetClippingRectanglePointer;
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


    funcs[numerate(false)] =  CreateFragShaderPointer;
    funcs[numerate(false)] = CreateVertShaderPointer;
    funcs[numerate(false)] = CreateProgramPointer;
    funcs[numerate(false)] = LoadProgramPointer;

    funcs[numerate(false)] = UseProgramPointer;
    funcs[numerate(false)] = UseCompositeProgramPointer;

    assert(numerate(false)==NUMFUNCS);

    return funcs;
}

nameArray GetFunctionNames(){
    numerate(true);
    nameArray names = (nameArray)calloc(NUMFUNCS, sizeof(functionName));
    names[numerate(false)] = (functionName)"FlipScreen";        //1
    names[numerate(false)] = (functionName)"GetScreenHeight";   //2
    names[numerate(false)] = (functionName)"GetScreenWidth";    //3
    names[numerate(false)] = (functionName)"GetClippingRectangle";//4
    names[numerate(false)] = (functionName)"SetClippingRectangle";//5
    names[numerate(false)] = (functionName)"Color";             //6

    names[numerate(false)] = (functionName)"Rectangle";         //7
    names[numerate(false)] = (functionName)"Line";              //8
    names[numerate(false)] = (functionName)"Point";             //9
    names[numerate(false)] = (functionName)"Triangle";          //10
    names[numerate(false)] = (functionName)"Polygon";           //11

    names[numerate(false)] = (functionName)"GradientRectangle"; //12
    names[numerate(false)] = (functionName)"GradientLine";      //13
    names[numerate(false)] = (functionName)"GradientTriangle";  //14

    names[numerate(false)] = (functionName)"OutlinedRectangle"; //15
    names[numerate(false)] = (functionName)"OutlinedCircle";    //16
    names[numerate(false)] = (functionName)"FilledCircle";      //17
    names[numerate(false)] = (functionName)"GradientCircle";    //18

    names[numerate(false)] = (functionName)"Image";             //19
    names[numerate(false)] = (functionName)"GrabImage";         //20

    names[numerate(false)] = (functionName)"Surface";           //21
    names[numerate(false)] = (functionName)"GrabSurface";       //22
    names[numerate(false)] = (functionName)"FragmentShader";    //23
    names[numerate(false)] = (functionName)"VertexShader";      //24
    names[numerate(false)] = (functionName)"ShaderProgram";     //25
    names[numerate(false)] = (functionName)"LoadShaderProgram"; //26
    names[numerate(false)] = (functionName)"UseShaderProgram";  //27
    names[numerate(false)] = (functionName)"UseCompositeShaderProgram"; //28

    assert(numerate(false)==NUMFUNCS);

    return names;
}

int GetNumVariables(void){return 0;}

v8FunctionArray GetVariables(void){return NULL;}

nameArray GetVariableNames(void){return NULL;}

int TS_Filter(void * _unused, SDL_Event *event){
	if(event->type==SDL_QUIT){
        exit(0);
    }

    if(event->type==SDL_KEYDOWN){
        //printf("[SDL_GL] Info: Keydown Event.\n");
        int key = event->key.keysym.sym;
        if(!key) {
            return 1;
        }
        if(key==0x40000105){
            printf("[SDL_GL] Info: Flagging for screenshot.\n");
            FlagForScreenshot();
            return 0;
        }
    }
    return 1;
}


extern "C" v8Function CCALL TS_SDL_GL_MakeV8SurfaceHandleFromPixels(int w, int h, void *pixels){

    SDL_Surface * surface = SDL_CreateRGBSurfaceFrom(pixels, w, h, DEPTH, w, CHANNEL_MASKS);

    BEGIN_OBJECT_WRAP_CODE;
    END_OBJECT_WRAP_CODE(Surface, surface);

}

