#define GRAPHIC_INTERNAL
#include "main.h"
#include "surface.h"


	v8Constructor Surfacetempl;
	v8InstanceTemplate SurfaceInsttempl;
	v8PrototypeTemplate Surfaceproto;

	v8Constructor Imagetempl;
	v8InstanceTemplate ImageInsttempl;

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
	Surfaceproto->Set("Rectangle", v8::FunctionTemplate::New(TS_surfaceRectangle));
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

//JS-Bound. Finalizer for JS Surface and Image objects.

void TS_SurfaceFinalizer(v8::Persistent<v8::Value> object, void* parameter) {
    SDL_Surface* surface = (SDL_Surface*)parameter;
    SDL_FreeSurface(surface);
    object.Dispose();
}

//JS-Bound. Accessor to get the width of a JS Surface.

v8::Handle<v8::Value> GetWidth(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    int value = static_cast<SDL_Surface*>(ptr)->w;
    return v8::Integer::New(value);
  }

//JS-Bound. Accessor to set the width of a JS Surface.

  void SetWidth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<SDL_Surface*>(ptr)->w = value->Uint32Value();
  }

//JS-Bound. Accessor to get the height of a JS Surface.

  v8::Handle<v8::Value> GetHeight(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    int value = static_cast<SDL_Surface*>(ptr)->h;
    return v8::Integer::New(value);
  }

//JS-Bound. Accessor to set the height of a JS Surface.

  void SetHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
	static_cast<SDL_Surface*>(ptr)->h = value->Uint32Value();
  }

//Create a surface given a width, height, and fill color.

SDL_Surface *TS_SurfaceFromSpec(int w, int h, int color, bool hwsurface){

    SDL_Surface *surface = SDL_CreateRGBSurface((hwsurface?SDL_HWSURFACE:SDL_SWSURFACE)|SDL_SRCALPHA, w, h, DEPTH, CHANNEL_MASKS);
	SDL_Rect rect = {0, 0, (short unsigned int)w, (short unsigned int)h};
	SDL_FillRect(surface, &rect, color);
    return surface;
}

//JS-Bound. Create an explicitly software JS Surface.

v8::Handle<v8::Value> TS_CreateSurface(const v8::Arguments& args)
{

	TS_Directories *TS_dirs = GetDirs();

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
    v8::Local<v8::Object> cliprect = v8::Object::New();
    return loadsurfacescope.Close(Psurfaceobj);
}

//JS-Bound. Create an explicitly (but not guaranteed) hardware JS Surface.

v8::Handle<v8::Value> TS_CreateImage(const v8::Arguments& args)
{

	TS_Directories *TS_dirs = GetDirs();

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

//JS-Bound. Blit a JS Surface to the screen with a scaling factor.

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

//JS-Bound. Blit a JS Surface to the screen with set horizontal and vertical scaling factors.

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

//JS-Bound. Blit a JS Surface to the screen.

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

//JS-Bound. Blit a JS Surface to another JS Surface with a set scaling factor.

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

//JS-Bound. Blit a JS Surface to another JS Surface at a specified X and Y.

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

//JS-Bound. Set the color of a pixel on a JS Surface. Identical to Surface.Point except it does not take into account blend mode.

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

//JS-Bound. Return the color of a JS Surface at a specified pixel as a JS TS_Color object.

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

v8::Handle<v8::Value> TS_surfaceRectangle(const v8::Arguments& args){
	if(args.Length()<2){
		printf("TS_surfaceRectangle Error: Called with fewer than 5 arguments.");
		return v8::False();
		}

    CHECK_ARG_INT(0, "TS_surfaceRectangle Error: Argument 0 is not a number.");
    CHECK_ARG_INT(1, "TS_surfaceRectangle Error: Argument 1 is not a number.");
    CHECK_ARG_INT(2, "TS_surfaceRectangle Error: Argument 2 is not a number.");
    CHECK_ARG_INT(3, "TS_surfaceRectangle Error: Argument 3 is not a number.");
    CHECK_ARG_OBJ(4, "TS_surfaceRectangle Error: Argument 4 is not an object.");

	int x = args[0]->v8::Value::IntegerValue();
	int y = args[1]->v8::Value::IntegerValue();
	int w = args[2]->v8::Value::IntegerValue();
	int h = args[3]->v8::Value::IntegerValue();

	v8::Handle<v8::Object> color = v8::Handle<v8::Object>::Cast(args[4]);

    TS_Color* c = (TS_Color*)color->GetPointerFromInternalField(0);
	unsigned int concatColor = c->toInt();

	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void *ptr = wrap->Value();

	SDL_Rect dest = {(short)x, (short)y, (unsigned short)w, (unsigned short)h};
    SDL_FillRect(static_cast<SDL_Surface*>(ptr), &dest, concatColor);
    return v8::Undefined();
}

v8::Handle<v8::Value> TS_surfaceLine(const v8::Arguments& args){
	if(args.Length()<2){
		printf("TS_surfaceRectangle Error: Called with fewer than 5 arguments.");
		return v8::False();
		}

    CHECK_ARG_INT(0, "TS_surfaceRectangle Error: Argument 0 is not a number.");
    CHECK_ARG_INT(1, "TS_surfaceRectangle Error: Argument 1 is not a number.");
    CHECK_ARG_INT(2, "TS_surfaceRectangle Error: Argument 2 is not a number.");
    CHECK_ARG_INT(3, "TS_surfaceRectangle Error: Argument 3 is not a number.");
    CHECK_ARG_OBJ(4, "TS_surfaceRectangle Error: Argument 4 is not an object.");

	int x1 = args[0]->v8::Value::IntegerValue();
	int y1 = args[1]->v8::Value::IntegerValue();
	int x2 = args[2]->v8::Value::IntegerValue();
	int y2 = args[3]->v8::Value::IntegerValue();

	v8::Handle<v8::Object> color = v8::Handle<v8::Object>::Cast(args[4]);

    TS_Color* c = (TS_Color*)color->GetPointerFromInternalField(0);

	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void *ptr = wrap->Value();

    TS_Line(static_cast<SDL_Surface*>(ptr), x1, y1, x2, y2, c);
    return v8::Undefined();
}


