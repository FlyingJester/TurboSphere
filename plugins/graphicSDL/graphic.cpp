#define GRAPHIC_INTERNAL
#include "graphic.h"
#include "../../configmanager/opengame.h"
#include "../common/plugin.h"
#include "../common/graphic_algorithm.h"
#include <cmath>

//POS standard libraries!
#ifndef UINT16_MAX
    #define UINT16_MAX 0xffff
#endif

#ifndef INT16_MAX
    #define INT16_MAX 0x7fff
#endif

#ifndef INT16_MIN
    #define INT16_MIN -0x7ffe
#endif

#define NUMFUNCS 17
#define NUMVARS  0

#ifndef _WIN32
    //this is a stop-gap until V8 has the full API for aligned pointers to internal fields. Probably will not be necessary in V8 v3.21
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif


TS_Config *TS_conf = NULL;
TS_Directories *TS_dirs = NULL;

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

initFunction Init(void){

	TS_conf = GetConfig();
	TS_dirs = GetDirs();

    if(SDL_WasInit(SDL_INIT_EVERYTHING)==0){
        SDL_Init(SDL_INIT_VIDEO);
        atexit(SDL_Quit);
    }
    else if(SDL_WasInit(SDL_INIT_VIDEO)==0){
        SDL_InitSubSystem(SDL_INIT_VIDEO);
    }

    if((screen = SDL_SetVideoMode(GetScreenWidth(), GetScreenHeight(), DEPTH, SDL_VIDEO_FLAGS|((TS_conf->fullscreen)?SDL_FULLSCREEN:0)))==NULL)
    {
        printf("SDL Video mode setup failed. Graphics subsystem closing.\n%s\n", SDL_GetError());
    }
	else
	{
        printf("SDL Video mode setup succeeded.\n");
	}

    if (IMG_Init(IMG_FLAGS) <=0) {
        printf("Unable to init SDL Image: %s\n", IMG_GetError());
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

v8Function TS_surfaceBlit(V8ARGS);
v8Function TS_surfaceZoomBlit(V8ARGS);
v8Function TS_surfaceStretchBlit(V8ARGS);
v8Function TS_surfaceBlitSurface(V8ARGS);
v8Function TS_surfaceSetPixel(V8ARGS);
v8Function TS_surfaceGetPixel(V8ARGS);

	v8Constructor Colortempl;
	v8InstanceTemplate ColorInsttempl;

void ColorInit(){
    Colortempl = *v8::FunctionTemplate::New();
	ColorInsttempl = *Colortempl->InstanceTemplate();

}

void ColorClose(){
    Colortempl.Clear();
	ColorInsttempl.Clear();
    Colortempl.Dispose();
	ColorInsttempl.Dispose();

}

	v8Constructor Surfacetempl;//   = *v8::FunctionTemplate::New();
	v8InstanceTemplate SurfaceInsttempl;// = *Surfacetempl->InstanceTemplate();
	v8PrototypeTemplate Surfaceproto;//    = *Surfacetempl->PrototypeTemplate();

	v8Constructor Imagetempl;//     = *v8::FunctionTemplate::New();
	v8InstanceTemplate ImageInsttempl;//   = *Imagetempl->InstanceTemplate();
//I'm going to try binding the SDL surface object directly to V8. Wish me luck!!

void SurfaceInit(){
    Surfacetempl     = *v8::FunctionTemplate::New();
    SurfaceInsttempl = *Surfacetempl->InstanceTemplate();
    Surfaceproto     = *Surfacetempl->PrototypeTemplate();
	Surfaceproto->Set("blit", v8::FunctionTemplate::New(TS_surfaceBlit));
	Surfaceproto->Set("transformBlit", v8::FunctionTemplate::New(TS_surfaceBlit));
	Surfaceproto->Set("zoomBlit", v8::FunctionTemplate::New(TS_surfaceZoomBlit));
	Surfaceproto->Set("stretchBlit", v8::FunctionTemplate::New(TS_surfaceStretchBlit));
	Surfaceproto->Set("setPixel", v8::FunctionTemplate::New(TS_surfaceSetPixel));
	Surfaceproto->Set("getPixel", v8::FunctionTemplate::New(TS_surfaceGetPixel));
	Surfaceproto->Set("blitSurface", v8::FunctionTemplate::New(TS_surfaceBlitSurface));
	Surfaceproto->Set("createImage", v8::FunctionTemplate::New());
	Surfaceproto->Set("createSurface", v8::FunctionTemplate::New());
}

void SurfaceClose(){
    Surfacetempl.Clear();
    SurfaceInsttempl.Clear();
    Surfaceproto.Clear();
    Surfacetempl.Dispose();
    SurfaceInsttempl.Dispose();
    Surfaceproto.Dispose();
}

void ImageInit(){
	Imagetempl     = *v8::FunctionTemplate::New();
	ImageInsttempl   = *Imagetempl->InstanceTemplate();
}

void ImageClose(){
    Imagetempl.Clear();
    ImageInsttempl.Clear();
    Imagetempl.Dispose();
    ImageInsttempl.Dispose();
}
void TS_drawAlphaRect(int x, int y, int w, int h, int c, int a, SDL_Rect rect)
{
	SDL_Surface *rectsurface = SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, screen->format->BitsPerPixel, 0, 0, 0, 0);
	SDL_FillRect(rectsurface, &rect,  (c<<2)|a);
	SDL_BlitSurface(rectsurface, NULL, screen, &rect);
	SDL_FreeSurface(rectsurface);
}

//window handling
v8::Handle<v8::Value> FlipScreen(const v8::Arguments& args)
{
    //TendEvents();

    SDL_Flip(SDL_GetVideoSurface());

	SDL_FillRect(screen, NULL, 0x000000);

	return v8::Undefined();
}

v8::Handle<v8::Value> SetTitle(const v8::Arguments& args)
{

    if(!SDL_GetVideoInfo()->wm_available) return v8::Undefined();

    CHECK_ARG_STR(0, "SetTitle Error: argument 0 is not a string.");
    v8::String::Utf8Value str(args[0]);
    const char *title = *str;
    SDL_WM_SetCaption(title, NULL );
	return v8::Undefined();
}

v8::Handle<v8::Value> V8GetScreenWidth(const v8::Arguments& args)
{
	return  v8::Number::New(GetScreenWidth());
}

v8::Handle<v8::Value> V8GetScreenHeight(const v8::Arguments& args)
{
	return  v8::Number::New(GetScreenHeight());
}

v8::Handle<v8::Value> SetResolution(const v8::Arguments& args)
{
	if(args.Length()<2){
        return v8::ThrowException(v8::String::New("SetResolution Error: Called with fewer than 2 arguments."));
	}

    CHECK_ARG_INT(0, "SetResolution Error: Argument 0 is not a number.");
    CHECK_ARG_INT(1, "SetResolution Error: Argument 1 is not a number.");

    int w = args[0]->IntegerValue();
    int h = args[1]->IntegerValue();

    if(w<0||h<0){
        THROWERROR_RANGE("SetResolution Error: Resolution given negative.");
    }

    unsigned int uw = args[0]->Uint32Value();
    unsigned int uh = args[1]->Uint32Value();

    if(w<GetMinScreenWidth()||h<GetMinScreenHeight()){
        THROWERROR_RANGE("SetResolution Error: Resolution given too small.");
    }

    if(SDL_VideoModeOK(w, h, DEPTH, SDL_VIDEO_FLAGS)!=DEPTH){
        THROWERROR_RANGE("SetResolution Error: Invalid Resolution.");
    }

	if(!SDL_SetVideoMode(w, h, DEPTH, SDL_VIDEO_FLAGS|(TS_conf->fullscreen?SDL_FULLSCREEN:0))){
        return v8::ThrowException(v8::String::New("SetResolution Error: SetVideoMode failed."));
	}

	SetScreenWidth(w);
	SetScreenHeight(h);
	cmpltscreen.w   = w;
	cmpltscreen.h   = h;
	return v8::True();
}

//Primitive Drawing:
//Contains all Sphere 1.x primitives
v8::Handle<v8::Value> Point(const v8::Arguments& args)
{
	if(args.Length()<3){
        return v8::ThrowException(v8::String::New("TS_Point Error: Called with fewer than 3 arguments!"));
	}

    CHECK_ARG_INT(0, "TS_Point Error: Argument 0 is not a number.");
    CHECK_ARG_INT(1, "TS_Point Error: Argument 1 is not a number.");
    CHECK_ARG_OBJ(2, "TS_Point Error: Argument 2 is not an object.");
	int x = args[0]->v8::Value::NumberValue();
	int y = args[1]->v8::Value::NumberValue();

	v8::Handle<v8::Object> color = v8::Handle<v8::Object>::Cast(args[2]);
    TS_Color* c = (TS_Color*)color->GetPointerFromInternalField(0);

    SDL_Rect rect = {(short int)x, (short int)y, 1, 1};
    SDL_FillRect(screen, &rect, c->toInt());

	return v8::Undefined();
}

v8::Handle<v8::Value> FilledCircle(const v8::Arguments& args)
{
	if(args.Length()<4){
        return v8::ThrowException(v8::String::New("TS_FilledCircle Error: Called with fewer than 4 arguments!"));
	}

    CHECK_ARG_INT(0, "TS_FilledCircle Error: argument 0 is not a valid number.");
    CHECK_ARG_INT(1, "TS_FilledCircle Error: argument 1 is not a valid number.");
    CHECK_ARG_INT(2, "TS_FilledCircle Error: argument 2 is not a valid number.");
    CHECK_ARG_OBJ(3, "TS_FilledCircle Error: argument 3 is not an object.");

	int x = args[0]->v8::Value::IntegerValue();
	int y = args[1]->v8::Value::IntegerValue();
	int rad = args[2]->v8::Value::IntegerValue();
	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[3]);

    TS_Color* c = (TS_Color*)color->GetPointerFromInternalField(0);

	TS_FilledCircle(x, y, rad, c->toInt(), screen);
	//TS_FilledCircle(x, y, rad, r, g, b, a);

	return v8::Undefined();
}


v8::Handle<v8::Value> OutlinedCircle(const v8::Arguments& args)
{

	int x = args[0]->v8::Value::IntegerValue();
	int y = args[1]->v8::Value::IntegerValue();
	int rad = args[2]->v8::Value::IntegerValue();
	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[3]);

    TS_Color* c = (TS_Color*)color->GetPointerFromInternalField(0);

	TS_OutlinedCircle(x, y, rad, c->toInt(), screen);

	return v8::Undefined();
}

SDL_Surface *TS_SurfaceFromSpec(int w, int h, int color, bool hwsurface){

    SDL_Surface *surface = SDL_CreateRGBSurface((hwsurface?SDL_HWSURFACE:SDL_SWSURFACE)|SDL_SRCALPHA, w, h, DEPTH, CHANNEL_MASKS);
	SDL_Rect rect = {0, 0, (short unsigned int)w, (short unsigned int)h};
	SDL_FillRect(surface, &rect, color);
    return surface;
}


v8::Handle<v8::Value> Rectangle(const v8::Arguments& args)
{
    if(args.Length()<5){
        return v8::ThrowException(v8::String::New("TS_Rectangle Error: Called with fewer than 5 arguments!"));
	}
    CHECK_ARG_INT(0, "TS_Rectangle Error: Arg 0 is not a number.");
    CHECK_ARG_INT(1, "TS_Rectangle Error: Arg 1 is not a number.");
    CHECK_ARG_INT(2, "TS_Rectangle Error: Arg 2 is not a number.");
    CHECK_ARG_INT(3, "TS_Rectangle Error: Arg 3 is not a number.");
    CHECK_ARG_OBJ(4, "TS_Rectangle Error: Arg 4 is not an object.");
	int x = args[0]->v8::Value::IntegerValue();
	int y = args[1]->v8::Value::IntegerValue();
	int w = args[2]->v8::Value::IntegerValue();
	int h = args[3]->v8::Value::IntegerValue();

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[4]);

    TS_Color* c = (TS_Color*)color->GetPointerFromInternalField(0);

	TS_Rectangle(x, y, w, h, c->toInt());

	return v8::Undefined();
}

void TS_Rectangle(int x, int y, int w, int h, unsigned int color) {
SDL_Rect rect = {(short int)x, (short int)y, (short unsigned int)w, (short unsigned int)h};
SDL_Rect locrect = {0, 0, (short unsigned int)w, (short unsigned int)h};

	SDL_Surface* surface = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA, w, h, DEPTH, CHANNEL_MASKS);
	SDL_FillRect(surface, &locrect, color);
	SDL_BlitSurface(surface, NULL, screen, &rect);
    SDL_FreeSurface(surface);
}

v8::Handle<v8::Value> Line(const v8::Arguments& args)
{
    if(args.Length()<5){
        return v8::ThrowException(v8::String::New("TS_Rectangle Error: Called with fewer than 5 arguments!"));
	}
    CHECK_ARG_INT(0, "TS_Rectangle Error: Arg 0 is not a number.");
    CHECK_ARG_INT(1, "TS_Rectangle Error: Arg 1 is not a number.");
    CHECK_ARG_INT(2, "TS_Rectangle Error: Arg 2 is not a number.");
    CHECK_ARG_INT(3, "TS_Rectangle Error: Arg 3 is not a number.");
    CHECK_ARG_OBJ(4, "TS_Rectangle Error: Arg 4 is not an object.");
	int x1 = args[0]->v8::Value::IntegerValue();
	int y1 = args[1]->v8::Value::IntegerValue();
	int x2 = args[2]->v8::Value::IntegerValue();
	int y2 = args[3]->v8::Value::IntegerValue();

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[4]);

    TS_Color* c = (TS_Color*)color->GetPointerFromInternalField(0);

	TS_Line(x1, y1, x2, y2, c);

	return v8::Undefined();
}

void TS_Line(int ax1, int ay1, int ax2, int ay2, TS_Color *c){

    int color = c->toInt();

    int w = abs(ax1-ax2);
    int h = abs(ay1-ay2);

    SDL_Rect rect = {(short int)min(ax1, ax2), (short int)min(ay1, ay2), (short unsigned int)w, (short unsigned int)h};

    if(ax1-ax2<=1&&ax1-ax2>=-1){
    SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, 1, h, DEPTH, CHANNEL_MASKS);
    SDL_Rect allsurf = {0, 0, 1, (short unsigned int)h};
    SDL_FillRect(surface, &allsurf, color);

	SDL_BlitSurface(surface, NULL, screen, &rect);
    SDL_FreeSurface(surface);

    return;
    }
    else if(ay1-ay2<=1&&ay1-ay2>=-1){
    SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w, 1, DEPTH, CHANNEL_MASKS);
    SDL_Rect allsurf = {0, 0, (short unsigned int)w, 1};
    SDL_FillRect(surface, &allsurf, color);

	SDL_BlitSurface(surface, NULL, screen, &rect);
    SDL_FreeSurface(surface);

    return;
    }

    int x1 = ax1-min(ax1, ax2);
    int y1 = ay1-min(ay1, ay2);
    int x2 = ax2-min(ax1, ax2);
    int y2 = ay2-min(ay1, ay2);

    SDL_Rect temprect = {(short int)x1, (short int)y1, 1, 1};

	SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w+1, h+1, DEPTH, CHANNEL_MASKS);

    int sx;
    int sy;
    if (x1<x2){
        sx  = 1;
    }
    else {
        sx  = -1;
    }
    if (y1<y2){
        sy  = 1;
    }
    else{
        sy  = -1;
    }
    float err = (float)w-h;

    float e2;
    while(true){
        temprect.x = x1;
        temprect.y = y1;

        SDL_FillRect(surface, &temprect, color);

        if ((x1==x2)&&(y1==y2)){break;}
        e2 = 2*err;

        if (e2>-h){

            err-= h;
            x1 += sx;

        }
        if (e2<w){

            err += w;
            y1  += sy;

        }
    }

	SDL_BlitSurface(surface, NULL, screen, &rect);
    SDL_FreeSurface(surface);
}

std::vector<TS_Point*> TS_GetLinePointsPoint(TS_Point *pa1, TS_Point *pa2){

    std::vector<TS_Point*> points;

    int w = abs(pa1->x-pa2->x);
    int h = abs(pa1->y-pa2->y);


    TS_Point *p1 = new TS_Point(pa1->x-min(pa1->x, pa2->x), pa1->y-min(pa1->y, pa2->y));
    TS_Point *p2 = new TS_Point(pa2->x-min(pa1->x, pa2->x), pa2->y-min(pa1->y, pa2->y));

    int sx;
    int sy;
    if (p1->x<p2->x){
        sx  = 1;
    }
    else {
        sx  = -1;
    }
    if (p1->y<p1->y){
        sy  = 1;
    }
    else{
        sy  = -1;
    }
    float err = (float)w-h;

    float e2;

    TS_Point temppoint(p1->x, p1->y);

    while(true){
        temppoint.x = pa1->x;
        temppoint.y = pa1->y;

        points.push_back(new TS_Point(temppoint));
        if ((p1->x==p1->x)&&(p1->y==p1->y)){break;}
        e2 = 2*err;
        if (e2>-h){
            err-= h;
            p1->x += sx;
        }
        if (e2<w){
            err += w;
            p1->y  += sy;
        }
    }

    return points;
}

std::vector<TS_Point*> TS_GetLinePoints(TS_Point *pa1, TS_Point *pa2){

    std::vector<TS_Point*> points;

    int ax1 = pa1->x;
    int ay1 = pa1->y;
    int ax2 = pa2->x;
    int ay2 = pa2->y;

    int w = abs(pa1->x-pa2->x);
    int h = abs(pa1->y-pa2->y);

    int x1 = ax1;
    int y1 = ay1;
    int x2 = ax2;
    int y2 = ay2;

    int sx;
    int sy;
    if (x1<x2){
        sx  = 1;
    }
    else {
        sx  = -1;
    }
    if (y1<y2){
        sy  = 1;
    }
    else{
        sy  = -1;
    }
    float err = (float)(w-h);

    float e2;

    while(true){
        TS_Point *point = new TS_Point(x1, y1);
        points.push_back(point);
        if ((x1==x2)&&(y1==y2)){break;}
        e2 = 2*err;
        if (e2>-h){
            err-= h;
            x1 += sx;
        }
        if (e2<w){
            err += w;
            y1  += sy;
        }
    }

    return points;
}


void TS_OutlinedPolygon(std::vector<int> points, TS_Color *color){

    //verify we have enough points and an even number.
    if(points.size()%2!=0){
        return;
    }

    if(points.size()<4){
        return;
    }

    int size = (int)points.size();

    //the first line uses points that need wrap around.
    TS_Line(points[size-2], points[size-1], points[0], points[1], color);

    //cycle through the rest of the points.
    for(int i = 2; i<size-2; i+=2){
        TS_Line(points[i], points[i+1], points[i+3], points[i+4], color);
    }
    //
}


void TS_FilledPolygon(SDL_Surface *surface, std::vector<TS_Point*> points, TS_Color *c){

    TS_Point *minPoint = new TS_Point(points[0]->x, points[0]->y);
    TS_Point *maxPoint = new TS_Point(points[0]->x, points[0]->y);

    for(size_t i = 0; i<points.size(); i++){
        if(minPoint->x>points[i]->x){
            minPoint->x=points[i]->x;
        }
        if(minPoint->y>points[i]->y){
            minPoint->y=points[i]->y;
        }
        if(maxPoint->x<points[i]->x){
            maxPoint->x=points[i]->x;
        }
        if(maxPoint->y<points[i]->y){
            maxPoint->y=points[i]->y;
        }
    }

    int w = maxPoint->x-minPoint->x;
    int h = maxPoint->y-minPoint->y;

    std::vector<std::vector<TS_Point*> > pixels;

    std::vector<bool> crossedLinesMaster;

    for(size_t i = 0; i<=points.size(); i++){
        pixels.push_back(TS_GetLinePoints(points[i%(int)points.size()], points[(i+1)%(int)points.size()]));
        crossedLinesMaster.push_back(false);
    }

    int pxsize = pixels.size();

    SDL_Rect temprect = {0, 0, 1, 1};

    unsigned int color = c->toInt();

    for(int i = 0; i<pxsize; i++){
        for(size_t e = 0; e<pixels[i].size(); e++){
                temprect.x=pixels[i][e]->x;
                temprect.y=pixels[i][e]->y;
        }
    }

    int parity;

    std::vector<bool> crossedLines;

    for(int y = minPoint->y-1; y<h+minPoint->y; y++){
        //crossedLines.assign(crossedLines.size(), false);
        crossedLines = crossedLinesMaster;
        parity = 0;
        for(int x = minPoint->x-1; x<w+minPoint->x; x++){
            temprect.x = x;
            temprect.y = y;
            for(size_t i = 0; i<pixels.size(); i++){
                if(crossedLines[i]) {
                    continue;
                }

                //Skip the first pixel. It will always overlap another line's last pixel, and cause the parity to be double switched.
                for(size_t e = 1; e<pixels[i].size(); e++){
                    if(pixels[i][e]->x==x){
                        if(pixels[i][e]->y==y){
                            parity++;
                            crossedLines[i] = true;
                            break;
                        }
                    }
                }

            }
            //if(!parity) continue;

            if(!(parity%2)){
                SDL_FillRect(surface, &temprect, color);
            }

        }
    }

    for(size_t i = 0; i<pixels.size(); i++){
        for(size_t e = 0; e<pixels[i].size(); e++){
            delete pixels[i][e];
        }
        pixels[i].resize(0);
    }

    pixels.resize(0);
    crossedLinesMaster.resize(0);
    crossedLines.resize(0);

	delete minPoint;
	delete maxPoint;
}

v8::Handle<v8::Value> TS_Test(const v8::Arguments& args)
{
    std::vector<TS_Point*> points;

    points.push_back(new TS_Point(100, 100));
    points.push_back(new TS_Point(200, 200));
    points.push_back(new TS_Point(230, 250));
    points.push_back(new TS_Point(100, 200));

    TS_Color c(255, 255, 255, 255);

    TS_FilledPolygon(screen, points,  &c);

    return v8::Undefined();
}


v8::Handle<v8::Value> GradientRectangle(const v8::Arguments& args)
{

    if(args.Length()<8){
        return v8::ThrowException(v8::String::New("TS_GradientRectangle Error: Called with fewer than 8 arguments!"));
	}
    CHECK_ARG_INT(0, "TS_GradientRectangle Error: Arg 0 is not a number.");
    CHECK_ARG_INT(1, "TS_GradientRectangle Error: Arg 1 is not a number.");
    CHECK_ARG_INT(2, "TS_GradientRectangle Error: Arg 2 is not a number.");
    CHECK_ARG_INT(3, "TS_GradientRectangle Error: Arg 3 is not a number.");
    CHECK_ARG_OBJ(4, "TS_GradientRectangle Error: Arg 4 is not an object.");
    CHECK_ARG_OBJ(5, "TS_GradientRectangle Error: Arg 5 is not an object.");
    CHECK_ARG_OBJ(6, "TS_GradientRectangle Error: Arg 6 is not an object.");
    CHECK_ARG_OBJ(7, "TS_GradientRectangle Error: Arg 7 is not an object.");


    int x = args[0]->v8::Value::IntegerValue();
	int y = args[1]->v8::Value::IntegerValue();
	int w = args[2]->v8::Value::IntegerValue();
	int h = args[3]->v8::Value::IntegerValue();

	v8::Local<v8::Object> color1 = v8::Local<v8::Object>::Cast(args[4]);
	v8::Local<v8::Object> color2 = v8::Local<v8::Object>::Cast(args[5]);
	v8::Local<v8::Object> color3 = v8::Local<v8::Object>::Cast(args[6]);
	v8::Local<v8::Object> color4 = v8::Local<v8::Object>::Cast(args[7]);

    TS_Color* c1 = (TS_Color*)color1->GetPointerFromInternalField(0);
    TS_Color* c2 = (TS_Color*)color2->GetPointerFromInternalField(0);
    TS_Color* c3 = (TS_Color*)color3->GetPointerFromInternalField(0);
    TS_Color* c4 = (TS_Color*)color4->GetPointerFromInternalField(0);

	TS_GradientRectangle(x, y, w, h, c1, c2, c3, c4);
	return v8::Undefined();
}

void TS_GradientRectangle(int x, int y, int w, int h, TS_Color *c1, TS_Color *c2, TS_Color *c3, TS_Color *c4){
    SDL_Rect rect       = {(short int)x, (short int)y, (short unsigned int)w, (short unsigned int)h};
    SDL_Rect temprect   = {0, 0, 1, 1};
    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w, h, DEPTH, CHANNEL_MASKS);

    TS_Color color(0, 0, 0, 0);

    int rDeltaTop = c2->red   - c1->red;
    int gDeltaTop = c2->green - c1->green;
    int bDeltaTop = c2->blue  - c1->blue;
    int aDeltaTop = c2->alpha - c1->alpha;

    int rDeltaLow = c3->red   - c4->red;
    int gDeltaLow = c3->green - c4->green;
    int bDeltaLow = c3->blue  - c4->blue;
    int aDeltaLow = c3->alpha - c4->alpha;

    color.red   = c1->red;
    color.green = c1->green;
    color.blue  = c1->blue;
    color.alpha = c1->alpha;

    int rLineDelta;
    int gLineDelta;
    int bLineDelta;
    int aLineDelta;

    TS_Color topColor = *c1;
    TS_Color lowColor = *c4;

    for(int tx = 0; tx<w; tx++){
        rLineDelta = lowColor.red   -topColor.red;
        gLineDelta = lowColor.green -topColor.green;
        bLineDelta = lowColor.blue  -topColor.blue;
        aLineDelta = lowColor.alpha -topColor.alpha;
        for(int ty = 0; ty<h; ty++){

            SDL_FillRect(surface, &temprect, color.toInt());
            color.red  =topColor.red  + (rLineDelta*ty/h);
            color.green=topColor.green+ (gLineDelta*ty/h);
            color.blue =topColor.blue + (bLineDelta*ty/h);
            color.alpha=topColor.alpha+ (aLineDelta*ty/h);
            temprect.y=ty;
        }

        topColor.red  =c1->red  + (rDeltaTop*tx/w);
        topColor.green=c1->green+ (gDeltaTop*tx/w);
        topColor.blue =c1->blue + (bDeltaTop*tx/w);
        topColor.alpha=c1->alpha+ (aDeltaTop*tx/w);

        lowColor.red  =c4->red  + (rDeltaLow*tx/w);
        lowColor.green=c4->green+ (gDeltaLow*tx/w);
        lowColor.blue =c4->blue + (bDeltaLow*tx/w);
        lowColor.alpha=c4->alpha+ (aDeltaLow*tx/w);

        temprect.x=tx;
    }

	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);
}

void TS_FilledCircleOld(int x, int y, int rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
SDL_Rect rect = {(short int)(x-rad),(short int)(y-rad),(short unsigned int)(rad*2),(short unsigned int)(rad*2)};
unsigned int color = concatRGBA(b, g, r, a);
	SDL_Surface* surface;
	surface = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA, rad*2, rad*2, DEPTH, CHANNEL_MASKS);
	for(int e = 0; e<rad; e++){
		for(int i = 0; i<rad; i++){
			if(sqrt(float(((rad-i)*(rad-i))+((rad-e)*(rad-e))))<rad){
			SDL_Rect locrect1 = {(short int)i,(short int)((2*rad)-e-1),(short unsigned int)((rad-i)*2),1};
			SDL_Rect locrect2 = {(short int)i,(short int)e,(short unsigned int)((rad-i)*2),1};
			SDL_FillRect(surface, &locrect1, color);
			SDL_FillRect(surface, &locrect2, color);
			i=rad;
			}

		}
	}
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);
}

void TS_FilledCircle(int x, int y, int rad, uint32_t color, SDL_Surface *destination){

    //Repair poor parameters.
    if(rad<0){
        rad*=-1;
    }
    else if(rad>0){

    }
    else{
        return;
    }
    //

    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA, rad*2, rad*2, DEPTH, CHANNEL_MASKS);

    int w = rad*2;
    double dis= 0;
    int xd2 = 0;
    int yd2 = 0;
    double r  = rad;

    int inf = (rad>4)?((rad>>3)+rad-floor((double)(((double)rad)/sqrt(2.0)))):(rad);

    SDL_Rect dest = {1, 1, 1, 1};

    for(int xt = 0; xt<=rad; xt++){
        xd2 = rad-xt;
        xd2*= xd2;
        for(int yt = 0; yt<inf; yt++){
            yd2 = rad-yt;
            yd2*= yd2;
            dis = floor(sqrt((float)(yd2+xd2)));
            if(dis<r){
                dest.x = yt;
                dest.y = xt;
                dest.w = (rad-yt)*2;
                SDL_FillRect(surface, &dest, color);
                //SDL_UpdateRects()
                dest.y=((rad*2)-dest.y);
                SDL_FillRect(surface, &dest, color);

                dest.x = xt;
                dest.y = yt;
                dest.w = (rad-xt)*2;
                SDL_FillRect(surface, &dest, color);
                //SDL_UpdateRects()
                dest.y=((rad*2)-dest.y);
                SDL_FillRect(surface, &dest, color);
//                field[((w-yt)*surface->w)+(w-xt)] = color;
  //              field[(xt*surface->w)+yt] = color;
    //            field[((w-xt)*surface->w)+(w-yt)] = color;

      //          field[((w-yt)*surface->w)+xt] = color;
        //        field[((w-xt)*surface->w)+yt] = color;
          //      field[(yt*surface->w)+(w-xt)] = color;
            //    field[(xt*surface->w)+(w-yt)] = color;
                break;
            }
        }
    }

    SDL_Rect rect = {(short int)(x-rad), (short int)(y-rad), (short unsigned int)(w), (short unsigned int)(w)};
    SDL_BlitSurface(surface, NULL, destination, &rect);

}


void TS_OutlinedCircle(int x, int y, int rad, uint32_t color, SDL_Surface* destination){

    //Repair poor parameters.
    if(rad<0){
        rad*=-1;
    }
    else if(rad>0){

    }
    else{
        return;
    }
    //

    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA, rad*2, rad*2, DEPTH, CHANNEL_MASKS);

    Uint32 *field = (Uint32 *)surface->pixels;
    int w = rad*2;
    double dis= 0;
    int xd2 = 0;
    int yd2 = 0;
    double r  = rad;
    int inf = (rad>4)?((rad>>2)+rad-floor((double)(((double)rad)/sqrt(2.0)))):(rad);

    for(int xt = 0; xt<=rad; xt++){
        xd2 = rad-xt;
        xd2*= xd2;
        for(int yt = 0; yt<inf; yt++){
            yd2 = rad-yt;
            yd2*= yd2;
            dis = floor(sqrt(double(yd2+xd2)));
            if(dis<r){
                field[(yt*surface->w)+xt] = color;
                field[((w-yt)*surface->w)+(w-xt)] = color;
                field[(xt*surface->w)+yt] = color;
                field[((w-xt)*surface->w)+(w-yt)] = color;

                field[((w-yt)*surface->w)+xt] = color;
                field[((w-xt)*surface->w)+yt] = color;
                field[(yt*surface->w)+(w-xt)] = color;
                field[(xt*surface->w)+(w-yt)] = color;
                break;
            }
        }
    }

    SDL_Rect rect = {(short int)(x-rad), (short int)(y-rad), (short unsigned int)(w), (short unsigned int)(w)};
    SDL_BlitSurface(surface, NULL, destination, &rect);

}

void TS_OutlinedCircleOld(int x, int y, int rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
SDL_Rect rect = {(short int)(x-rad), (short int)(y-rad), (short unsigned int)(rad*2), (short unsigned int)(rad*2)};
unsigned int color = concatRGBA(b, g, r, a);
	SDL_Surface* surface;
	surface = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA, rad*2, rad*2, DEPTH, CHANNEL_MASKS);
	for(int e = 0; e<rad; e++){
		for(int i = 0; i<=rad; i++){
			if(sqrt(double(((rad-i)*(rad-i))+((rad-e)*(rad-e))))<rad){

				SDL_Rect r1 = {(short int)i, (short int)e, 1, 1};
				SDL_Rect r2 = {(short int)i,(short int)((2*rad)-e-1), 1, 1};
				SDL_Rect r3 = {(short int)(i+(rad-i)*2), (short int)e, 1, 1};
				SDL_Rect r4 = {(short int)(i+(rad-i)*2), (short int)((2*rad)-e-1), 1, 1};
				SDL_FillRect(surface, &r1, color);
				SDL_FillRect(surface, &r2, color);
				SDL_FillRect(surface, &r3, color);
				SDL_FillRect(surface, &r4, color);
			i=rad;
			}

		}
	}
		for(int i = 0; i<=rad; i++){
			for(int e = 0; e<rad; e++){
			if(sqrt(double(((rad-i)*(rad-i))+((rad-e)*(rad-e))))<rad){

				SDL_Rect r1 = {(short int)i, (short int)e, 1, 1};
				SDL_Rect r2 = {(short int)i, (short int)((2*rad)-e-1), 1, 1};
				SDL_Rect r3 = {(short int)(i+(rad-i)*2), (short int)e, 1, 1};
				SDL_Rect r4 = {(short int)(i+(rad-i)*2), (short int)((2*rad)-e-1), 1, 1};
				SDL_FillRect(surface, &r1, color);
				SDL_FillRect(surface, &r2, color);
				SDL_FillRect(surface, &r3, color);
				SDL_FillRect(surface, &r4, color);
			e=rad;
			}

		}
	}
	SDL_BlitSurface(surface, NULL, screen, &rect);
	SDL_FreeSurface(surface);
}

void TS_FlipScreen()
{
	//TendEvents();
    SDL_Flip(SDL_GetVideoSurface());
}




void TS_ShowSurface(SDL_Surface *image, int x, int y)
{
	SDL_Rect dest = {(short int)x, (short int)y, (short unsigned int)(image->w), (short unsigned int)(image->h)};
	SDL_BlitSurface(image, NULL, screen, &dest);
}

void TS_ShowSurfaceClipped(SDL_Surface *image, SDL_Rect dest, SDL_Rect src)
{
	SDL_BlitSurface(image, &src, screen, &dest);
}

void TS_StretchShowSurface(SDL_Surface *image, int _x, int _y, float fh, float fv)
{
    float   w = image->w*fh;
    float   h = image->h*fv;
    int     wi = (int)w;
    int     hi = (int)h;

	SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, wi, hi, DEPTH, CHANNEL_MASKS);
    SDL_Rect rect = {0, 0, (unsigned short int)(ceil(fh)), (unsigned short int)(ceil(fv))};

    uint32_t color = 0;
    for(int x = 0; x < image->w; x++){
        rect.x = (Sint16)(x*fh);

        for(int y = 0; y < image->h; y++){
            rect.y = (Sint16)(y*fv);
            color = ((uint32_t *)image->pixels)[(image->w*y)+x];
            SDL_FillRect(surface, &rect, color);
            //
        }
    }
    /*
    float fx = 0;
    for(int x = 0; x<wi; x++, fx++){
        float fy = 0;
        for(int y = 0; y<hi; y++, fy++){
            ((uint32_t*)surface->pixels)[(wi*y)+x]=((uint32_t*)image->pixels)[(int)((float(wi*y))+(fx))];
        }
    }
    */
    TS_ShowSurface_Clear(surface, _x, _y);
}

v8::Handle<v8::Value> TS_surfaceZoomBlit(const v8::Arguments& args)
{
    if(args.Length()<3){
        return v8::ThrowException(v8::String::New("TS_surfaceZoomBlit Error: Called with fewer than 3 arguments.\n"));
    }
    CHECK_ARG_INT(0, "TS_surfaceZoomBlit Error: Argument 0 is not a number.");
    CHECK_ARG_INT(1, "TS_surfaceZoomBlit Error: Argument 1 is not a number.");
    CHECK_ARG_INT(2, "TS_surfaceZoomBlit Error: Argument 2 is not a number.");

	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	float factor = (float)args[2]->v8::Value::NumberValue();
	if(factor<=0){
        THROWERROR_RANGE("TS_surfaceZoomBlit Error: Argument 2 is negative.\n");
	}
	SDL_Surface* surface = static_cast<SDL_Surface*>(ptr);
	//SDL_Rect dest = {(short int)(args[0]->v8::Value::NumberValue()), (short int)(args[1]->v8::Value::NumberValue()), (short unsigned int)((float)surface->w*factor), (short unsigned int)((float)surface->h*factor)};
	TS_StretchShowSurface(surface, args[0]->v8::Value::IntegerValue(), args[1]->v8::Value::IntegerValue(), factor, factor);
	//SDL_SoftStretch(surface, NULL, screen, &dest);
	return v8::Number::New(((float)surface->w)*factor);
}

v8::Handle<v8::Value> TS_surfaceStretchBlit(const v8::Arguments& args)
{
    if(args.Length()<4){
        return v8::ThrowException(v8::String::New("TS_surfaceZoomBlit Error: Called with fewer than 3 arguments!\n"));
    return v8::False();
    }
    CHECK_ARG_INT(0, "TS_surfaceStretchBlit Error: Argument 0 is not a number.");
    CHECK_ARG_INT(1, "TS_surfaceStretchBlit Error: Argument 1 is not a number.");
    CHECK_ARG_INT(2, "TS_surfaceStretchBlit Error: Argument 2 is not a number.");
    CHECK_ARG_INT(3, "TS_surfaceStretchBlit Error: Argument 3 is not a number.");

	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	double xfactor = args[2]->v8::Value::NumberValue();
	double yfactor = args[3]->v8::Value::NumberValue();

	if(xfactor<=0){
        THROWERROR_RANGE("TS_surfaceZoomBlit Error: Argument 2 is negative.\n");
	}
	if(yfactor<=0){
        THROWERROR_RANGE("TS_surfaceZoomBlit Error: Argument 3 is negative.\n");
	}

	SDL_Surface* surface = static_cast<SDL_Surface*>(ptr);
	//SDL_Rect dest = {args[0]->v8::Value::NumberValue(), args[1]->v8::Value::NumberValue(), xfactor, yfactor};
	//SDL_SoftStretch(surface, NULL, screen, &dest);
    TS_StretchShowSurface(surface, args[0]->v8::Value::IntegerValue(), args[1]->v8::Value::IntegerValue(), (float)xfactor, (float)yfactor);
	return v8::Undefined();
}

v8::Handle<v8::Value> TS_surfaceZoomBlitSurface(const v8::Arguments& args)
{
    if(args.Length()<3){
        return v8::ThrowException(v8::String::New("TS_surfaceZoomBlitSurface Error: Called with fewer than 3 arguments!\n"));
    return v8::Undefined();
    }
    CHECK_ARG_INT(0, "TS_surfaceZoomBlitSurface Error: Argument 0 is not a number.");
    CHECK_ARG_INT(1, "TS_surfaceZoomBlitSurface Error: Argument 1 is not a number.");
    CHECK_ARG_OBJ(2, "TS_surfaceZoomBlitSurface Error: Argument 2 is not an object.");

	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	double factor = args[2]->v8::Value::NumberValue();
	SDL_Surface* surface = static_cast<SDL_Surface*>(ptr);
	//SDL_Rect dest = {args[0]->v8::Value::NumberValue(), args[1]->v8::Value::NumberValue(), surface->w*factor, surface->h*factor};
	//SDL_SoftStretch(surface, NULL, screen, &dest);
	TS_StretchShowSurface(surface, args[0]->v8::Value::IntegerValue(), args[1]->v8::Value::IntegerValue(), surface->w*factor, surface->h*factor);
	return v8::Undefined();
}

//A secret SDL function:
//SDL_SoftStretch(image, &src, screen, &dest);

v8::Handle<v8::Value> TS_surfaceBlit(const v8::Arguments& args)
{
    if(args.Length()<2){
	return v8::ThrowException(v8::String::New("TS_surface Error: Called with fewer than 2 arguments!\n"));
    }
    CHECK_ARG_INT(0, "TS_surfaceBlit Error: Argument 0 is not a number.");
    CHECK_ARG_INT(1, "TS_surfaceBlit Error: Argument 1 is not a number.");
	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	TS_ShowSurface(static_cast<SDL_Surface*>(ptr), args[0]->v8::Value::IntegerValue(), args[1]->v8::Value::IntegerValue());
	return v8::Undefined();
}

v8::Handle<v8::Value> TS_surfaceBlitSurface(const v8::Arguments& args)
{
	if(args.Length()<3){
	return v8::ThrowException(v8::String::New("TS_surfaceBlitSurface Error: Called with fewer than 3 arguments!\n"));
	}

    CHECK_ARG_INT(0, "TS_surfaceBlit Error: Argument 0 is not a number.");
    CHECK_ARG_INT(1, "TS_surfaceBlit Error: Argument 1 is not a number.");

    v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int x = args[1]->v8::Value::IntegerValue();
	int y = args[2]->v8::Value::IntegerValue();

	v8::Handle<v8::Object> surface = v8::Handle<v8::Object>::Cast(args[0]);
	v8::Local<v8::External> wraparg0 = v8::Local<v8::External>::Cast(surface->GetInternalField(0));
	void *ptrarg = wraparg0->Value();
	int w = surface->Get(v8::String::New("width"))->v8::Value::IntegerValue();
	int h = surface->Get(v8::String::New("height"))->v8::Value::IntegerValue();
	SDL_Rect srcrect = {0, 0, (short unsigned int)w, (short unsigned int)h};
	SDL_Rect dstrect = {(short int)x, (short int)y, (short unsigned int)w, (short unsigned int)h};
	SDL_BlitSurface( static_cast<SDL_Surface*>(ptrarg),&srcrect,static_cast<SDL_Surface*>(ptr),  &dstrect);
	return v8::Undefined();
}

v8::Handle<v8::Value> TS_surfaceSetPixel(const v8::Arguments& args)
{
	if(args.Length()<3){
	return v8::ThrowException(v8::String::New("TS_surfaceSetPixel Error: Called with fewer than 3 arguments!\n"));
	}

    CHECK_ARG_INT(0, "TS_surfaceSetPixel Error: Argument 0 is not a number.");
    CHECK_ARG_INT(1, "TS_surfaceSetPixel Error: Argument 1 is not a number.");
    CHECK_ARG_OBJ(2, "TS_surfaceSetPixel Error: Argument 2 is not an object.");

	int x = args[0]->v8::Value::IntegerValue();
	int y = args[1]->v8::Value::IntegerValue();

	v8::Handle<v8::Object> color = v8::Handle<v8::Object>::Cast(args[2]);

    TS_Color* c = (TS_Color*)color->GetPointerFromInternalField(0);
	unsigned int concatColor = c->toInt();

	SDL_Rect rect = {(short int)x, (short int)y, 1, 1};

	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	SDL_FillRect(static_cast<SDL_Surface*>(wrap->Value()), &rect, concatColor);
	return v8::Undefined();
}

v8::Handle<v8::Value> TS_surfaceGetPixel(const v8::Arguments& args);

v8::Handle<v8::Value> TS_surfaceStretchBlit(const v8::Arguments& args);

void TS_SurfaceFinalizer(v8::Persistent<v8::Value> object, void* parameter);

v8::Handle<v8::Value> TS_CreateSurface(const v8::Arguments& args)
{
    v8::HandleScope loadsurfacescope;
    v8::Handle<v8::Value> external;
    SDL_Surface *surface = NULL;
    if (args[0]->IsExternal()) {
        external = v8::Local<v8::External>::Cast(args[0]);
    }
    else {
        if(args[0]->IsString()){//Load Image from file
            v8::String::AsciiValue str(args[0]);
            const char *file = *str;

            surface = IMG_Load(string(TS_dirs->image).append(file).c_str());
            if(!surface) {
                printf("TS_CreateSurface: %s\n", IMG_GetError());
                return v8::ThrowException(v8::String::New(string("Could not open surface %s").append(file).c_str()));
            }
        }
        else if(args[0]->IsNumber()&&args[1]->IsNumber()){

            int w = args[0]->IntegerValue();
            int h = args[1]->IntegerValue();

            v8::Handle<v8::Object> color = v8::Handle<v8::Object>::Cast(args[2]);
            TS_Color* c = (TS_Color*)color->GetPointerFromInternalField(0);
            surface = TS_SurfaceFromSpec(w, h, c->toInt(), false);

        }
        else{
            return v8::ThrowException(v8::String::New("TS_CreateSurface error: Bad args. Must be string (filename) or number, number, Color_object"));
        }
        external = v8::External::New(surface);

    }

  	SurfaceInsttempl->SetInternalFieldCount(1);
	v8::Handle<v8::Function> Surfacector = Surfacetempl->GetFunction();

	v8::Local<v8::Object> Surfaceobj = Surfacector->NewInstance();

    v8::Persistent<v8::Object> Psurfaceobj = v8::Persistent<v8::Object>::New(Surfaceobj);
    Surfaceobj.Clear();
    Surfacector.Clear();

    Psurfaceobj.MakeWeak(surface, TS_SurfaceFinalizer);
	Psurfaceobj->SetInternalField(0, external);
    external.Clear();

    return loadsurfacescope.Close(Psurfaceobj);
}


v8::Handle<v8::Value> TS_CreateImage(const v8::Arguments& args)
{
    v8::HandleScope loadsurfacescope;
    v8::Handle<v8::Value> external;
	SDL_Surface *surface = NULL;
  //v8::Handle<v8::Object> wrapper;

    if(args.Length()<1){
        return v8::ThrowException(v8::String::New("TS_CreateImage Error: Called with no arguments."));
    }

    if (args[0]->IsExternal()) {
        external = v8::Local<v8::External>::Cast(args[0]);
    } else {
        if(args[0]->IsString()){//Load Image from file
            v8::String::AsciiValue str(args[0]);
            const char *file = *str;

            surface = IMG_Load(string(TS_dirs->image).append(file).c_str());
            if(!surface) {
                printf("TS_CreateImage: %s\n", IMG_GetError());
                return v8::ThrowException(v8::String::New(string("Could not open surface %s").append(file).c_str()));
            }
            //
        }
        else if(args[0]->IsNumber()&&args[1]->IsNumber()){

            int w = args[0]->NumberValue();
            int h = args[1]->NumberValue();

            v8::Handle<v8::Object> color = v8::Handle<v8::Object>::Cast(args[2]);
            TS_Color* c = (TS_Color*)color->GetPointerFromInternalField(0);
            surface = TS_SurfaceFromSpec(w, h, c->toInt(), true);

        }
        else{
            return v8::ThrowException(v8::String::New("TS_CreateImage error: Bad args. Must be string (filename) or number, number, Color_object"));
        }
        external = v8::External::New(surface);

    }

  	SurfaceInsttempl->SetInternalFieldCount(1);
	v8::Handle<v8::Function> Surfacector = Surfacetempl->GetFunction();

	v8::Local<v8::Object> Surfaceobj = Surfacector->NewInstance();

    v8::Persistent<v8::Object> Psurfaceobj = v8::Persistent<v8::Object>::New(Surfaceobj);
    Psurfaceobj.MakeWeak(surface, TS_SurfaceFinalizer);
	Psurfaceobj->SetInternalField(0, external);

    return loadsurfacescope.Close(Psurfaceobj);
}


v8::Handle<v8::Value> TS_GetSystemArrow(const v8::Arguments& args)
{
	TS_Config *TS_conf = GetConfig();
	TS_Directories *TS_dirs = GetDirs();
    v8::HandleScope loadsurfacescope;
    v8::Handle<v8::Value> external;
	SDL_Surface *surface = IMG_Load(string(TS_dirs->system).append(TS_conf->systemarrow).c_str());

    external = v8::External::New(surface);

  	SurfaceInsttempl->SetInternalFieldCount(1);
	v8::Handle<v8::Function> Surfacector = Surfacetempl->GetFunction();

	v8::Local<v8::Object> Surfaceobj = Surfacector->NewInstance();

    v8::Persistent<v8::Object> Psurfaceobj = v8::Persistent<v8::Object>::New(Surfaceobj);

    Psurfaceobj.MakeWeak(surface, TS_SurfaceFinalizer);
	Psurfaceobj->SetInternalField(0, external);

    return loadsurfacescope.Close(Psurfaceobj);
}


void TS_SurfaceFinalizer(v8::Persistent<v8::Value> object, void* parameter) {
    SDL_Surface* surface = (SDL_Surface*)parameter;
    SDL_FreeSurface(surface);
    object.Dispose();
}


void TS_ColorFinalizer(v8::Persistent<v8::Value> object, void* parameter) {
    TS_Color* color = (TS_Color*)parameter;
    delete color;
    object.Dispose();
    object.Clear();
}

v8::Handle<v8::Value> GetWidth(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    int value = static_cast<SDL_Surface*>(ptr)->w;
    return v8::Integer::New(value);
  }

  void SetWidth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<SDL_Surface*>(ptr)->w = value->Uint32Value();
  }

  v8::Handle<v8::Value> GetHeight(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    int value = static_cast<SDL_Surface*>(ptr)->h;
    return v8::Integer::New(value);
  }

  void SetHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
	static_cast<SDL_Surface*>(ptr)->h = value->Uint32Value();
  }

v8::Handle<v8::Value> GetRed(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    int value = static_cast<TS_Color*>(ptr)->red;
    return v8::Integer::New(value);
  }

  void SetRed(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<TS_Color*>(ptr)->red = value->Int32Value();
  }

v8::Handle<v8::Value> GetGreen(v8::Local<v8::String> property,
                          const v8::AccessorInfo &info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    int value = static_cast<TS_Color*>(ptr)->green;
    return v8::Integer::New(value);
  }

  void SetGreen(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                 const v8::AccessorInfo& info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<TS_Color*>(ptr)->green = value->Int32Value();
  }


v8::Handle<v8::Value> GetBlue(v8::Local<v8::String> property,
                          const v8::AccessorInfo &info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    int value = static_cast<TS_Color*>(ptr)->blue;
    return v8::Integer::New(value);
  }

  void SetBlue(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                 const v8::AccessorInfo& info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<TS_Color*>(ptr)->blue = value->Int32Value();
  }

v8::Handle<v8::Value> GetAlpha(v8::Local<v8::String> property,
                          const v8::AccessorInfo &info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    int value = static_cast<TS_Color*>(ptr)->alpha;
    return v8::Integer::New(value);
  }

  void SetAlpha(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                 const v8::AccessorInfo& info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<TS_Color*>(ptr)->alpha = value->Int32Value();
  }




v8::Handle<v8::Value> CreateColor(const v8::Arguments& args) {
  v8::HandleScope loadcolorscope;
   v8::Handle<v8::Value> external;

  TS_Color *color = new TS_Color(255, 255, 255, 255);
    if(args.Length()<3){
        return v8::ThrowException(v8::String::New("CreateColor Error: Called with fewer than 3 arguments!"));
    }
    if (args[0]->IsExternal()) {
        external = v8::Local<v8::External>::Cast(args[0]);
    } else {


    CHECK_ARG_INT(0, "CreateColor Error: Could not parse arg 0 to number!");
    CHECK_ARG_INT(1, "CreateColor Error: Could not parse arg 1 to number!");
    CHECK_ARG_INT(2, "CreateColor Error: Could not parse arg 2 to number!");

    int r = args[0]->Int32Value();
    int g = args[1]->Int32Value();
    int b = args[2]->Int32Value();
    int a = 255;
    if(args.Length()>3){
        CHECK_ARG_INT(3, "CreateColor Error: Could not parse arg 3 to number!");
        a = args[3]->Int32Value();
    }

    color->red      = r;
    color->green    = g;
    color->blue     = b;
    color->alpha    = a;
    external = v8::External::New(color);
  }

  	ColorInsttempl->SetInternalFieldCount(1);
	v8::Handle<v8::Function> colorctor = Colortempl->GetFunction();
	v8::Local<v8::Object> colorobj = colorctor->NewInstance();

    //colorobj->SetInternalField(0, external);
	v8::Persistent<v8::Object> Pcolorobj = v8::Persistent<v8::Object>::New(colorobj);//colorctor->NewInstance();

    colorobj.Clear();
    colorctor.Clear();

    Pcolorobj.MakeWeak(color, TS_ColorFinalizer);
    Pcolorobj->SetInternalField(0, external);

    external.Clear();
	return loadcolorscope.Close(Pcolorobj);
}



v8::Handle<v8::Value> TS_surfaceGetPixel(const v8::Arguments& args){
	if(args.Length()<2){
		printf("TS_surfaceGetPixel Error: Called with fewer than 2 arguments!");
		return v8::False();
		}

	v8::HandleScope loadcolorscope;
	v8::Handle<v8::Value> external;
	int x = args[0]->v8::Value::IntegerValue();
	int y = args[1]->v8::Value::IntegerValue();

	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void *ptr = wrap->Value();
	//SDL_Surface *surf = static_cast<SDL_Surface*>(ptr);
	//unsigned int color = ((unsigned int*)surf->pixels)[y*(surf->pitch/sizeof(unsigned int)) + x];
	unsigned int color = ((unsigned int*)static_cast<SDL_Surface*>(ptr)->pixels)[y*(static_cast<SDL_Surface*>(ptr)->pitch/sizeof(unsigned int)) + x];

	Uint8 b = (color<<8)>>24;
	Uint8 g = (color<<16)>>24;
	Uint8 r = (color<<24)>>24;
	Uint8 a = (color<<0)>>24;
    TS_Color *retcolor = new TS_Color(r, g, b, a);
    external = v8::External::New(retcolor);

  	ColorInsttempl->SetInternalFieldCount(1);
	v8::Handle<v8::Function> colorctor = Colortempl->GetFunction();
		//get class instance
	v8::Local<v8::Object> colorobj = colorctor->NewInstance();
		//Bridge the JS class seeds by setting the fields
	colorobj->SetInternalField(0, external);

	return loadcolorscope.Close(colorobj);

}


v8Function SetClippingRectangle(V8ARGS){
    if(args.Length()<4){
        return v8::ThrowException(v8::String::New("SetClippingRectangle Error: Less than four arguments passed.\n"));
    }
    CHECK_ARG_INT(0, "SetClippingRectangle Error: Argument 0 is not a number.\n");
    CHECK_ARG_INT(1, "SetClippingRectangle Error: Argument 1 is not a number.\n");
    CHECK_ARG_INT(2, "SetClippingRectangle Error: Argument 2 is not a number.\n");
    CHECK_ARG_INT(3, "SetClippingRectangle Error: Argument 3 is not a number.\n");

    int x = args[0]->IntegerValue();
    int y = args[1]->IntegerValue();
    int w = args[2]->IntegerValue();
    int h = args[3]->IntegerValue();


    //sanitize arguments
    if(w<0){
        return v8::ThrowException(v8::String::New("SetClippingRectangle Error: Width is negative.\n"));
    }

    if(h<0){
        return v8::ThrowException(v8::String::New("SetClippingRectangle Error: Height is negative.\n"));
    }

    if(x>0){
        if(x>INT16_MAX){
            x = INT16_MAX;
        }
    }
    else if (x<0){
        if(x<INT16_MIN){
            x = INT16_MIN;
        }
    }

    if(y>0){
        if(y>INT16_MAX){
            y = INT16_MAX;
        }
    }
    else if (y<0){
        if(y<INT16_MIN){
            y = INT16_MIN;
        }
    }

        if(w>UINT16_MAX){
            w = UINT16_MAX;
        }

        if(h>UINT16_MAX){
            h = UINT16_MAX;
        }

    short int sx = x;
    short int sy = y;
    short unsigned int sw = w;
    short unsigned int sh = h;

    SDL_Rect rect = {sx, sy, sw, sh};

    SDL_SetClipRect(screen, &rect);

    return v8::Undefined();
}

void TS_ClipRectFinalizer(v8::Persistent<v8::Value> object, void* parameter) {
    object.Dispose();
}

v8Function GetClippingRectangle(V8ARGS){

    v8::HandleScope GetClipRectscope;
    v8::Local<v8::Object> cliprect = v8::Object::New();

    SDL_Rect rect = {0, 0, 0, 0};
    SDL_GetClipRect(screen, &rect);

    cliprect->Set(v8::String::New("x"), v8::Integer::New(rect.x));
    cliprect->Set(v8::String::New("y"), v8::Integer::New(rect.y));
    cliprect->Set(v8::String::New("w"), v8::Integer::New(rect.w));
    cliprect->Set(v8::String::New("h"), v8::Integer::New(rect.h));

	return GetClipRectscope.Close(cliprect);

}

/*
void TS_TexturedQuad(SDL_Surface *dest, SDL_Surface *texture, TS_Point Pul, TS_Point Pur, TS_Point Pbl, TS_Point Pbr){

    TS_Point UL = Pul;
    TS_Point UR = Pur;
    TS_Point BL = Pbl;
    TS_Point BR = Pbr;

    hdir = 1;
    vdir = 1;

    if (sqrtf(abs(Pul.x - Pbl.x)+abs(Pul.y - Pbl.y))>sqrtf(abs(Pur.x - Pbr.x)+abs(Pur.y - Pbr.y))){
        UL = Pur;
        UR = Pul;
        BL = Pbr;
        BR = Pbl;
        hdir = -1;
    }

    if (sqrtf(abs(Pur.x - Pul.x)+abs(Pur.y - Pul.y))>sqrtf(abs(Pbl.x - Pbr.x)+abs(Pbl.y - Pbr.y))){
        TS_Point temp;

        UL =
        UR =
        BL =
        BR =
        hdir = -1;
    }

    //With the

}
*/
