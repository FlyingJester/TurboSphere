#define GRAPHIC_INTERNAL

#include <assert.h>

#include "main.h"

#include <GL/glext.h>
#include <SDL/SDL.h>
#include "image.h"
#include "surface.h"
#include "color.h"
#include "screen.h"
#include "primitives.h"

SDL_Rect cmpltscreen = {0, 0, (Uint16)GetScreenWidth(), (Uint16)GetScreenHeight()};

SDL_Event event;
SDL_Surface *screen = NULL;

void LoadGLFunctions(){


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

int TS_Filter(const SDL_Event *event);
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
void * OutlinedPolygonPointer       = V8FUNCPOINTER(OutlinedPolygon);

void * LoadImagePointer             = V8FUNCPOINTER(LoadImage);

void * NewSurfacePointer            = V8FUNCPOINTER(NewSurface);
void * SurfaceGrabPointer           = V8FUNCPOINTER(SurfaceGrab);
void * ImageGrabPointer             = V8FUNCPOINTER(ImageGrab);


initFunction Init(void){

	TS_Config *TS_conf = GetConfig();


    if(SDL_WasInit(SDL_INIT_EVERYTHING)==0){
        SDL_Init(SDL_INIT_VIDEO);
        atexit(SDL_Quit);
    }
    else if(SDL_WasInit(SDL_INIT_VIDEO)==0){
        SDL_InitSubSystem(SDL_INIT_VIDEO);
    }
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //TODO: make this set in the config.
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0); //TODO: make this set in the config.

    if((screen = SDL_SetVideoMode(GetScreenWidth(), GetScreenHeight(), DEPTH, SDL_VIDEO_FLAGS|((TS_conf->fullscreen)?SDL_FULLSCREEN:0)))==NULL)
    {
        printf("[" PLUGINNAME "] Video mode setup failed. Graphics subsystem closing.\n%s\n", SDL_GetError());
    }
	else
	{
        printf("[" PLUGINNAME "] Video mode setup succeeded.\n");
	}

    LoadGLFunctions();

    glClearColor(0, 0, 0, 255);
    glClearDepth(1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, GetScreenWidth(), GetScreenHeight());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, GetScreenWidth(), GetScreenHeight(), 0, 1, -1);

    glMatrixMode(GL_MODELVIEW);

    ColorInit();
    ImageInit();
    SurfaceInit();
    PrimitivesInit();

    glLoadIdentity();
    printf("[" PLUGINNAME "] Info: Using OpenGL version %s\n", glGetString(GL_VERSION));
    if (IMG_Init(IMG_FLAGS) <=0) {
        printf("[graphicSDL] Unable to init SDL Image: %s\n", IMG_GetError());
    }
    if(SDL_GetVideoInfo()->wm_available){
        SDL_WM_SetCaption("TurboSphere Game Engine", "TurboSphere Game Engine");
    }
	SDL_ShowCursor(SDL_DISABLE);

    //set up the event filter
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
    SDL_SetEventFilter(TS_Filter);

    return (const char *)"SDL_GL";
}


void Close(){
    ColorClose();
    ImageClose();
    SurfaceClose();
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

    names[numerate(false)] = (functionName)"Image";             //14
    names[numerate(false)] = (functionName)"GrabImage";         //15

    names[numerate(false)] = (functionName)"Surface";           //16
    names[numerate(false)] = (functionName)"GrabSurface";       //17

    assert(numerate(false)==NUMFUNCS);

    return names;
}

int GetNumVariables(void){return 0;}

v8FunctionArray GetVariables(void){return NULL;}

nameArray GetVariableNames(void){return NULL;}

int TS_Filter(const SDL_Event *event){
	if(event->type==SDL_QUIT){
        //QuitAll();
        exit(0);
    }
    return 1;
}
