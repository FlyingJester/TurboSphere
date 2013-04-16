#define GRAPHIC_INTERNAL
#include "main.h"
#include "surface.h"

v8Function TS_surfaceBlit(V8ARGS);
v8Function TS_surfaceZoomBlit(V8ARGS);
v8Function TS_surfaceStretchBlit(V8ARGS);
v8Function TS_surfaceBlitSurface(V8ARGS);
v8Function TS_surfaceSetPixel(V8ARGS);
v8Function TS_surfaceGetPixel(V8ARGS);

	v8Constructor Colortempl;
	v8InstanceTemplate ColorInsttempl;
	v8PrototypeTemplate Colorproto;

void ColorInit(){
    Colortempl = *v8::FunctionTemplate::New();
	ColorInsttempl = *Colortempl->InstanceTemplate();
    Colorproto     = *Colortempl->PrototypeTemplate();
    Colortempl->SetClassName(v8::String::New("Color"));
	//Colorproto->Set("toString", v8::FunctionTemplate::New(TS_surfaceBlit));

}

void ColorClose(){
    Colortempl.Clear();
	ColorInsttempl.Clear();
    Colortempl.Dispose();
	ColorInsttempl.Dispose();

}

v8Function TS_colorToString(V8ARGS){
        return v8::String::New("[object Color name=[Color]]");
}

//window handling
v8::Handle<v8::Value> FlipScreen(const v8::Arguments& args)
{

    SDL_PumpEvents();
    SDL_Flip(SDL_GetVideoSurface());
	SDL_FillRect(screen, NULL, 0x000000);

	return v8::Undefined();
}

v8::Handle<v8::Value> SetTitle(const v8::Arguments& args)
{

    if(!SDL_GetVideoInfo()->wm_available) return v8::Undefined();

    if(args.Length()<1){
        THROWERROR("SetTitle Error: No arguments.");
    }
    CHECK_ARG_STR(0, "SetTitle Error: argument 0 is not a string.");
    v8::String::Utf8Value str(args[0]);
    SDL_WM_SetCaption(*str, *str);
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

	TS_Config *TS_conf = GetConfig();

	if(args.Length()<2){
        return v8::ThrowException(v8::String::New("SetResolution Error: Called with fewer than 2 arguments."));
	}

    CHECK_ARG_INT(0, "SetResolution Error: Argument 0 is not a number.");
    CHECK_ARG_INT(1, "SetResolution Error: Argument 1 is not a number.");

    unsigned int w = args[0]->Uint32Value();
    unsigned int h = args[1]->Uint32Value();

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

void TS_FlipScreen()
{
	//TendEvents();
    SDL_Flip(SDL_GetVideoSurface());
}

SDL_Rect *TS_StoreInternalClippingRectangle(){//Maintain a clipping rectangle.
	static SDL_Rect rect = {0, 0, (unsigned short)GetScreenWidth(), (unsigned short)GetScreenHeight()};
	return &rect;
}

void TS_SetClippingRectangle(int x, int y, int w, int h){
	//New clipping rectangle.
	SDL_Rect rect = {(short)x, (short)y, (unsigned short)w, (unsigned short)h};
	//store the original value.
	SDL_Rect *trect = TS_StoreInternalClippingRectangle();
	trect = &rect;
	//Set the clipping rectangle.
	SDL_SetClipRect(screen, &rect);
}

void TS_ResetClippingRectangle(){
	SDL_SetClipRect(screen, TS_StoreInternalClippingRectangle());
}

void TS_ShowSurface(SDL_Surface *image, int x, int y)
{
	SDL_Rect dest = {(short int)x, (short int)y, (short unsigned int)(image->w), (short unsigned int)(image->h)};
	SDL_BlitSurface(image, NULL, screen, &dest);
}

void TS_SurfaceShowSurface(SDL_Surface *image, SDL_Surface *dest, int x, int y){
	SDL_Rect destrect = {(short int)x, (short int)y, (short unsigned int)(image->w), (short unsigned int)(image->h)};
	SDL_BlitSurface(image, NULL, dest, &destrect);
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
        }
    }
    TS_ShowSurface_Clear(surface, _x, _y);
}

v8::Handle<v8::Value> TS_GetSystemArrow(const v8::Arguments& args)
{
	TS_Config *TS_conf = GetConfig();
	TS_Directories *TS_dirs = GetDirs();
    v8::HandleScope loadsurfacescope;
    v8::Handle<v8::Value> external;
	SDL_Surface *surface MINMEMALIGN = IMG_Load(string(TS_dirs->system).append(TS_conf->systemarrow).c_str());

    external = v8::External::New(surface);

  	SurfaceInsttempl->SetInternalFieldCount(1);
	v8::Handle<v8::Function> Surfacector = Surfacetempl->GetFunction();

	v8::Local<v8::Object> Surfaceobj = Surfacector->NewInstance();

    v8::Persistent<v8::Object> Psurfaceobj = v8::Persistent<v8::Object>::New(Surfaceobj);

    Psurfaceobj.MakeWeak(surface, TS_SurfaceFinalizer);
	Psurfaceobj->SetInternalField(0, external);

    return loadsurfacescope.Close(Psurfaceobj);
}


void TS_ColorFinalizer(v8::Persistent<v8::Value> object, void* parameter) {
    TS_Color* color = (TS_Color*)parameter;
    delete color;
    object.Dispose();
    object.Clear();
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

  MINMEMALIGN TS_Color *color = new TS_Color(255, 255, 255, 255);


    if(args.Length()<3){
        return v8::ThrowException(v8::String::New("[graphicSDL] CreateColor Error: Called with fewer than 3 arguments."));
    }

    CHECK_ARG_INT(0, "[graphicSDL] CreateColor Error: Could not parse arg 0 to number.");
    CHECK_ARG_INT(1, "[graphicSDL] CreateColor Error: Could not parse arg 1 to number.");
    CHECK_ARG_INT(2, "[graphicSDL] CreateColor Error: Could not parse arg 2 to number.");

    int r = args[0]->Int32Value();
    int g = args[1]->Int32Value();
    int b = args[2]->Int32Value();
    int a = 255;
    if(args.Length()>3){
        if(args[3]->IsNumber()){
            a = args[3]->Int32Value();
        }
    }

    color->red      = r;
    color->green    = g;
    color->blue     = b;
    color->alpha    = a;


  	ColorInsttempl->SetInternalFieldCount(1);
	v8::Handle<v8::Function> colorctor = Colortempl->GetFunction();
	v8::Local<v8::Object> colorobj = colorctor->NewInstance();

    //colorobj->SetInternalField(0, external);
	v8::Persistent<v8::Object> Pcolorobj = v8::Persistent<v8::Object>::New(colorobj);//colorctor->NewInstance();

    colorobj.Clear();
    colorctor.Clear();

    Pcolorobj.MakeWeak(color, TS_ColorFinalizer);
    Pcolorobj->SetAlignedPointerInInternalField(0, color);

    Pcolorobj->GetConstructorName()=v8::String::New("Color");

	return loadcolorscope.Close(Pcolorobj);
}

TS_Color *TS_BlendColors(TS_Color *c1, TS_Color *c2){
    unsigned int r = c1->red    +c2->red;
    unsigned int g = c1->green  +c2->green;
    unsigned int b = c1->blue   +c2->blue;
    unsigned int a = c1->alpha  +c2->alpha;
    uint8_t r8 = (r>>1);
    uint8_t g8 = (g>>1);
    uint8_t b8 = (b>>1);
    uint8_t a8 = (a>>1);

    return new TS_Color(r8, g8, b8, a8);

}

TS_Color *TS_BlendColorsWeighted(TS_Color *c1, TS_Color *c2, int w1, int w2){

    if(w2<=0){
        return new TS_Color(c1->red, c1->green, c1->blue, c1->alpha);
    }

    if(w1<=0){
        return new TS_Color(c2->red, c2->green, c2->blue, c2->alpha);
    }

    unsigned int r = (c1->red*w1)+(c2->red*w2);
    unsigned int g = (c1->green*w1)+(c2->green*w2);
    unsigned int b = (c1->blue*w1)+(c2->blue*w2);
    unsigned int a = (c1->alpha*w1)+(c2->alpha*w2);

    r/=w1+w2;
    g/=w1+w2;
    b/=w1+w2;
    a/=w1+w2;

    uint8_t r8 = r;
    uint8_t g8 = g;
    uint8_t b8 = b;
    uint8_t a8 = a;

    return new TS_Color(r8, g8, b8, a8);

}

TS_Color *TS_BlendColorsWeighted(TS_Color *c1, TS_Color *c2, double w1, double w2){

    if(w2<=0){
        return new TS_Color(c1->red, c1->green, c1->blue, c1->alpha);
    }

    if(w1<=0){
        return new TS_Color(c2->red, c2->green, c2->blue, c2->alpha);
    }

    double r = (c1->red*w1)+(c2->red*w2);
    double g = (c1->green*w1)+(c2->green*w2);
    double b = (c1->blue*w1)+(c2->blue*w2);
    double a = (c1->alpha*w1)+(c2->alpha*w2);

    r/=w1+w2;
    g/=w1+w2;
    b/=w1+w2;
    a/=w1+w2;

    uint8_t r8 = (uint8_t) r;
    uint8_t g8 = (uint8_t) g;
    uint8_t b8 = (uint8_t) b;
    uint8_t a8 = (uint8_t) a;

    return new TS_Color(r8, g8, b8, a8);

}

v8Function SetClippingRectangle(V8ARGS){
    if(args.Length()<4){
        return v8::ThrowException(v8::String::New("SetClippingRectangle Error: Less than four arguments passed.\n"));
    }
    CHECK_ARG_INT(0, "SetClippingRectangle Error: Argument 0 is not a number.\n");
    CHECK_ARG_INT(1, "SetClippingRectangle Error: Argument 1 is not a number.\n");
    CHECK_ARG_INT(2, "SetClippingRectangle Error: Argument 2 is not a number.\n");
    CHECK_ARG_INT(3, "SetClippingRectangle Error: Argument 3 is not a number.\n");

    int x = args[0]->Int32Value();
    int y = args[1]->Int32Value();
    int w = args[2]->Int32Value();
    int h = args[3]->Int32Value();


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
