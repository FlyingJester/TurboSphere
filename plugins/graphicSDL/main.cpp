#define GRAPHIC_INTERNAL
#include "main.h"
#include "surface.h"
#include "primitives.h"

SDL_Rect cmpltscreen = {0, 0, (Uint16)GetScreenWidth(), (Uint16)GetScreenHeight()};


SDL_Event event;
SDL_Surface *screen = NULL;


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
void * FlipScreenPointer          = V8FUNCPOINTER(FlipScreen);
void * SetClippingRectanglePointer= V8FUNCPOINTER(SetClippingRectangle);
void * GetClippingRectanglePointer= V8FUNCPOINTER(GetClippingRectangle);
void * SetTitlePointer            = V8FUNCPOINTER(SetTitle);
void * GetScreenWidthPointer      = V8FUNCPOINTER(V8GetScreenHeight);
void * GetScreenHeightPointer     = V8FUNCPOINTER(V8GetScreenWidth);
void * SetResolutionPointer       = V8FUNCPOINTER(SetResolution);
void * PointPointer               = V8FUNCPOINTER(Point);
void * TS_CreateSurfacePointer    = V8FUNCPOINTER(TS_CreateSurface);
void * TS_CreateImagePointer      = V8FUNCPOINTER(TS_CreateImage);
void * TS_GetSystemArrowPointer   = V8FUNCPOINTER(TS_GetSystemArrow);
void * LinePointer                = V8FUNCPOINTER(Line);
void * CreateColorPointer         = V8FUNCPOINTER(CreateColor);
void * RectanglePointer           = V8FUNCPOINTER(Rectangle);
void * GradientRectanglePointer   = V8FUNCPOINTER(GradientRectangle);
void * FilledCirclePointer        = V8FUNCPOINTER(FilledCircle);
void * OutlinedCirclePointer      = V8FUNCPOINTER(OutlinedCircle);
void * OutlinedPolygonPointer     = V8FUNCPOINTER(OutlinedPolygon);
void * OutlinedRectanglePointer   = V8FUNCPOINTER(OutlinedRectangle);
void * PointSeriesPointer         = V8FUNCPOINTER(PointSeries);
void * GradientLinePointer         = V8FUNCPOINTER(GradientLine);

initFunction Init(void){

	TS_Config *TS_conf = GetConfig();
	TS_Directories *TS_dirs = GetDirs();

    if(SDL_WasInit(SDL_INIT_EVERYTHING)==0){
        SDL_Init(SDL_INIT_VIDEO);
        atexit(SDL_Quit);
    }
    else if(SDL_WasInit(SDL_INIT_VIDEO)==0){
        SDL_InitSubSystem(SDL_INIT_VIDEO);
    }

    if((screen = SDL_SetVideoMode(GetScreenWidth(), GetScreenHeight(), DEPTH, SDL_VIDEO_FLAGS|((TS_conf->fullscreen)?SDL_FULLSCREEN:0)))==NULL)
    {
        printf("[graphicSDL] SDL Video mode setup failed. Graphics subsystem closing.\n%s\n", SDL_GetError());
    }
	else
	{
        printf("[graphicSDL] SDL Video mode setup succeeded.\n");
	}

    if (IMG_Init(IMG_FLAGS) <=0) {
        printf("[graphicSDL] Unable to init SDL Image: %s\n", IMG_GetError());
    }
    if(SDL_GetVideoInfo()->wm_available){
        SDL_WM_SetCaption("TurboSphere Game Engine", "TurboSphere Game Engine");
    }
	SDL_ShowCursor(SDL_DISABLE);

    //initialize JS objects.
    //will replace with object registration later.
    SurfaceInit();
    ImageInit();
    ColorInit();

    //set up the event filter
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
    SDL_SetEventFilter(TS_Filter);

	ColorInsttempl->SetAccessor(v8::String::New("red"), GetRed, SetRed);
	ColorInsttempl->SetAccessor(v8::String::New("green"), GetGreen, SetGreen);
	ColorInsttempl->SetAccessor(v8::String::New("blue"), GetBlue, SetBlue);
	ColorInsttempl->SetAccessor(v8::String::New("alpha"), GetAlpha, SetAlpha);

	SurfaceInsttempl->SetAccessor(v8::String::New("width"), GetWidth, SetWidth);
	SurfaceInsttempl->SetAccessor(v8::String::New("height"), GetHeight, SetHeight);
	SurfaceInsttempl->SetAccessor(v8::String::New("clippingRectangle"), GetHeight, SetHeight);
	ImageInsttempl->SetAccessor(v8::String::New("width"), GetWidth, SetWidth);
	ImageInsttempl->SetAccessor(v8::String::New("height"),  GetHeight, SetHeight);
    return (const char *)"graphicSDL";
}


void Close(){
    ColorClose();
    SurfaceClose();
    ImageClose();
	SDL_VideoQuit();
}

int GetNumFunctions(){
    return NUMFUNCS;
}

functionArray GetFunctions(){
    numerate(true);
    functionArray funcs = (functionArray)calloc(GetNumFunctions(), sizeof(void*));
    funcs[numerate(false)] = FlipScreenPointer;
    funcs[numerate(false)] = SetClippingRectanglePointer;
    funcs[numerate(false)] = GetClippingRectanglePointer;
    funcs[numerate(false)] = SetTitlePointer;
    funcs[numerate(false)] = GetScreenWidthPointer;
    funcs[numerate(false)] = GetScreenHeightPointer;
    funcs[numerate(false)] = SetResolutionPointer;
    funcs[numerate(false)] = LinePointer;
    funcs[numerate(false)] = TS_CreateImagePointer;
    funcs[numerate(false)] = TS_CreateSurfacePointer;
    funcs[numerate(false)] = TS_GetSystemArrowPointer;
    funcs[numerate(false)] = PointPointer;
    funcs[numerate(false)] = CreateColorPointer;
    funcs[numerate(false)] = RectanglePointer;
    funcs[numerate(false)] = GradientRectanglePointer;
    funcs[numerate(false)] = FilledCirclePointer;
    funcs[numerate(false)] = OutlinedCirclePointer;
    funcs[numerate(false)] = OutlinedPolygonPointer;
    funcs[numerate(false)] = OutlinedRectanglePointer;
    funcs[numerate(false)] = PointSeriesPointer;
    funcs[numerate(false)] = GradientLinePointer;
    return funcs;
}

nameArray GetFunctionNames(){
    numerate(true);
    nameArray names = (nameArray)calloc(GetNumFunctions(), sizeof(functionName));
    names[numerate(false)] = (functionName)"FlipScreen";
    names[numerate(false)] = (functionName)"SetClippingRectangle";
    names[numerate(false)] = (functionName)"GetClippingRectangle";
    names[numerate(false)] = (functionName)"SetTitle";
    names[numerate(false)] = (functionName)"GetScreenHeight";
    names[numerate(false)] = (functionName)"GetScreenWidth";
    names[numerate(false)] = (functionName)"SetResolution";
    names[numerate(false)] = (functionName)"Line";
    names[numerate(false)] = (functionName)"Image";
    names[numerate(false)] = (functionName)"Surface";
    names[numerate(false)] = (functionName)"GetSystemArrow";
    names[numerate(false)] = (functionName)"Point";
    names[numerate(false)] = (functionName)"Color";
    names[numerate(false)] = (functionName)"Rectangle";
    names[numerate(false)] = (functionName)"GradientRectangle";
    names[numerate(false)] = (functionName)"FilledCircle";
    names[numerate(false)] = (functionName)"OutlinedCircle";
    names[numerate(false)] = (functionName)"OutlinedPolygon";
    names[numerate(false)] = (functionName)"OutlinedRectangle";
    names[numerate(false)] = (functionName)"PointSeries";
    names[numerate(false)] = (functionName)"GradientLine";
    return names;
}

int GetNumVariables(void){return 0;}

void ** GetVariables(void){return NULL;}

nameArray GetVariableNames(void){return NULL;}

int TS_Filter(const SDL_Event *event){
	if(event->type==SDL_QUIT){
        //QuitAll();
        exit(0);
    }
    return 1;
}
