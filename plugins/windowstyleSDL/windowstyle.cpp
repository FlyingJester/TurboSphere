
//////////////////
/*
Let's try and keep this file a paragon of cleanliness.
The windowstyle plugin is a prime example of how every
plugin should be done. It also shows quite well how to
cleanly and concisely read from Sphere files.
*/
///////////////////

#include "../common/plugin.h"
#include "../../configmanager/opengame.h"
#include "../common/graphic_common.h"
#include "../common/graphic_algorithm.h"
#include "../graphicSDL/graphic.h"
#include "windowstyle.h"

#ifdef _MSC_VER
   typedef __int16 int16_t;
#else
   #include <stdint.h>
#endif

const int WSelementsSize = 9;
enum WSelements {WS_UPPERLEFT, WS_TOP, WS_UPPERRIGHT, WS_RIGHT, WS_LOWERRIGHT, WS_BOTTOM, WS_LOWERLEFT, WS_LEFT, WS_BACKGROUND};

//forward declare C++ functions to be bound to JS
v8Function LoadWindowStyle(V8ARGS);
v8Function GetSystemWindowStyle(V8ARGS);

//declare pointers to functions that will be passed to the engine to be bound
void* LoadWindowStylePointer      = V8FUNCPOINTER(LoadWindowStyle);
void* GetSystemWindowStylePointer = V8FUNCPOINTER(GetSystemWindowStyle);

//create function template, instance template, and prototype template for JS-side objects
DECLARE_OBJECT_TEMPLATES(WindowStyle);

//to simplify numbering functions and names.
int numerate(bool reset){
    static int i = 0;
    if(reset) {
        i = 0;
        return 0;
    }
    i++;
    return i-1;
}

initFunction Init(){
    //Initialize the JS-side object templates
    INIT_OBJECT_TEMPLATES(WindowStyle);
    ADD_TO_PROTO(WindowStyle, "drawWindow", TS_WSdrawWindow);

//    WindowStyleproto->Set("drawWindow", v8::FunctionTemplate::New(TS_WSdrawWindow));

    //return plugin's name to the engine.
    return (char *)"windowstyleSDL";
}

void Close(){
    CLOSE_OBJECT_TEMPLATES(WindowStyle);
}

int GetNumFunctions(){
    return NUMFUNCS;
}

functionArray GetFunctions(){
    //reset numeration function to 0.
    numerate(true);

    functionArray funcs = (functionArray)calloc(NUMFUNCS, sizeof(void*));

    funcs[numerate(false)] = LoadWindowStylePointer;
    funcs[numerate(false)] = GetSystemWindowStylePointer;
    //return array of function pointers to to the engine.
    return funcs;
}

nameArray GetFunctionNames(){
    //reset numeration function to 0.
    numerate(true);

    nameArray funcnames = (nameArray)calloc(NUMFUNCS, sizeof(functionName));

    funcnames[numerate(false)] = (functionName)"WindowStyle";
    funcnames[numerate(false)] = (functionName)"GetSystemWindowStyle";
    //return array of c-string function names to the engine.
    return funcnames;

}

int GetNumVariables(){
    return 0;
}

v8FunctionArray GetVariables(){
    //no variables from this plugin.
    return NULL;
}

nameArray GetVariableNames(){
    //no variables from this plugin.
    return NULL;
}

void TS_WindowStyleFinalizer(v8::Persistent<v8::Value> object, void* parameter) {
    //basic finalizer. Thanks, Kyuu!
    TS_WindowStyle* ws = (TS_WindowStyle*)parameter;
    delete ws;
    object.Dispose();
}

TS_WSborder::TS_WSborder(SDL_Surface *im){
    image = SDL_ConvertSurface(im, im->format, im->flags);
    width = image->w;
    height = image->h;
    mask = TS_Color(0,0,0,0);
}

TS_WSborder::TS_WSborder(){
    image = new SDL_Surface;
    width = 0;
    height= 0;
    mask = TS_Color(0,0,0,0);
}

TS_WScorner::TS_WScorner(SDL_Surface *im, int xOff, int yOff){
    image = SDL_ConvertSurface(im, im->format, im->flags);
    width = image->w;
    height = image->h;
    xOffset = xOff;
    yOffset = yOff;
}

TS_WScorner::TS_WScorner(){
    image = new SDL_Surface;
    width = 0;
    height= 0;
    xOffset = 0;
    yOffset = 0;
    mask = TS_Color(0,0,0,0);
}

TS_WSbackground::TS_WSbackground(SDL_Surface *im, WSbackgroundType _type){
    image = SDL_ConvertSurface(im, im->format, im->flags);
    width = image->h;
    height= image->w;
	type = _type;
    mask = TS_Color(0,0,0,0);
}

TS_WSbackground::TS_WSbackground(){
    image = new SDL_Surface;
    width = 0;
    height= 0;
	type = WS_TILED;
    mask = TS_Color(0,0,0,0);
}

TS_WSbackground::~TS_WSbackground(){
    SDL_FreeSurface(image);
}

SDL_Surface *TS_WindowStyle::getImage(int num){
    switch(num){
        case 0:
        return upperleft.image;
        case 1:
        return top.image;
        case 2:
        return upperright.image;
        case 3:
        return right.image;
        case 4:
        return lowerright.image;
        case 5:
        return bottom.image;
        case 6:
        return lowerleft.image;
        case 7:
        return left.image;
        case 8:
        return background.image;
		default:  //at least don't explode.
		printf("[windowstyleSDL] Error: Error reading WindowStyle Element in %s. Element was %i.\n", name, num);
		return new SDL_Surface;
	}
}

TS_WindowStyle::TS_WindowStyle(const char *file){

	SDL_RWops *wsfile;
	wsfile = SDL_RWFromFile(file, "rb");
    if(!wsfile){
		printf("Could not open file %s\n", file);   //the file should be checked BEFORE passing it to the constructor!
	}
	name = file;
    uint8_t   sig[5]    = "    ";
	uint16_t  version[1];
	uint8_t   edge_width[1];
	uint8_t   background_mode[1];
	uint8_t   corner_color_comp[16];
    uint8_t   edge_offsets[4];

	//sig
    SDL_RWread(wsfile, sig, 1,4);

    //version
    SDL_RWread(wsfile, version, 2,1);

    //edge width
    SDL_RWread(wsfile, edge_width, 1,1);

    //background mode
    SDL_RWread(wsfile, background_mode, 1,1);

    //corner colors
    SDL_RWread(wsfile, corner_color_comp, 1,16);

    //edge offsets
    SDL_RWread(wsfile, edge_offsets, 1,4);

	SDL_RWseek(wsfile, 64, SEEK_SET);

	for(int c = 0; c<4; c++){
	int e = c*4;
		cornerColors[c]=TS_Color(corner_color_comp[e], corner_color_comp[e+1], corner_color_comp[e+2], corner_color_comp[e+3]);
	}
	background.type = (WSbackgroundType)background_mode[0];
    if(version[0]==1){

    }
    if(version[0]==2){
        uint16_t dimensions[2];
        for(int i = 0; i<WSelementsSize; i++){
            SDL_RWread(wsfile, dimensions, 2, 2);
            switch(i){
            case WS_UPPERLEFT:
                CREATE_WS_COMPONENT(upperleft.image, dimensions[0], dimensions[1]);
                break;
            case WS_TOP:
                CREATE_WS_COMPONENT(top.image, dimensions[0], dimensions[1]);
                break;
            case WS_UPPERRIGHT:
                CREATE_WS_COMPONENT(upperright.image, dimensions[0], dimensions[1]);
                break;
            case WS_RIGHT:
                CREATE_WS_COMPONENT(right.image, dimensions[0], dimensions[1]);
                break;
            case WS_LOWERRIGHT:
                CREATE_WS_COMPONENT(lowerright.image, dimensions[0], dimensions[1]);
                break;
            case WS_BOTTOM:
                CREATE_WS_COMPONENT(bottom.image, dimensions[0], dimensions[1]);
                break;
            case WS_LOWERLEFT:
                CREATE_WS_COMPONENT(lowerleft.image, dimensions[0], dimensions[1]);
                break;
            case WS_LEFT:
                CREATE_WS_COMPONENT(left.image, dimensions[0], dimensions[1]);
                break;
			case WS_BACKGROUND:
				CREATE_WS_COMPONENT(background.image, dimensions[0], dimensions[1]);
				break;
            default:
				printf("[windowstyleSDL] Error: Error reading WindowStyle Element in %s. Element was %i.\n", file, i);
                CREATE_WS_COMPONENT(upperleft.image, dimensions[0], dimensions[1]);
                break;
            }
        }
    }
	SDL_RWclose(wsfile);
}

TS_WindowStyle::~TS_WindowStyle(){

}

void TS_WindowStyle::drawWindow(int x, int y, int w, int h){
    for(int i = 0; i<8; i++){
        SDL_Surface *imageptr = this->getImage(i);
		if(i%2==0){
            SDL_Rect rect = {(short int)((i%3>0)?w+x:x-imageptr->w), (short int)((i>3)?h+y:y-imageptr->h), (short unsigned int)(imageptr->w), (short unsigned int)(imageptr->h)};

            TS_ShowSurface(imageptr, rect.x, rect.y);
            //SDL_BlitSurface(imageptr, NULL, screen, &rect);
        }
        else if(i==1||i==5){
            int curX = 0;
            while(curX<w){
                SDL_Rect src = {0, 0, (short unsigned int)(((w-curX)>(imageptr->w))?imageptr->w:w-curX), (short unsigned int)(imageptr->h)};
                SDL_Rect dest;
                if(i==1){
                    dest.x  = x+curX;
                    dest.y  = y-imageptr->h;
                    dest.w  = imageptr->w;
                    dest.h  = imageptr->h;
                }
                else{
                    dest.x  = x+curX;
                    dest.y  = h+y;
                    dest.w  = imageptr->w;
                    dest.h  = imageptr->h;
                }
                TS_ShowSurfaceClipped(imageptr, dest, src);
                //SDL_BlitSurface(imageptr, &src, screen, &dest);
                curX+=imageptr->w;
            }
        }
        else{
            int curY = 0;
            while(curY<h){
                SDL_Rect src    = {0, 0, (short unsigned int)(imageptr->w), (short unsigned int)(((h-curY)>(imageptr->h))?imageptr->h:h-curY)};
                SDL_Rect dest;
                if(i==3){
                    dest.x  = x+w;
                    dest.y  = y+curY;
                    dest.w  = imageptr->w;
                    dest.h  = imageptr->h;

                }
                else{

                    dest.x  = x-imageptr->w;
                    dest.y  = y+curY;
                    dest.w  = imageptr->w;
                    dest.h  = imageptr->h;
                }
                TS_ShowSurfaceClipped(imageptr, dest, src);
                //SDL_BlitSurface(imageptr, &src, screen, &dest);
                curY+=imageptr->h;
            }
        }

    }
	//draw background.
	//WS_TILED, WS_STRETCHED, WS_GRADIENT, WS_TILED_GRADIENT, WS_STRETCHED_GRADIENT;
	float fh;
	float fv;
	switch(background.type){
		case WS_TILED:
		case WS_TILED_GRADIENT:
			TS_SetClippingRectangle(x, y, w, h);
			if((background.image->w==0)||(background.image->h==0)){
				//break;
			}
			for(int tx = 0; tx<w; tx+=background.image->w){
				for(int ty = 0; ty<h; ty+=background.image->h){
					TS_ShowSurface(background.image, tx+x, ty+y);
				}
			}
			TS_ResetClippingRectangle();
			break;
		case WS_STRETCHED:
		case WS_STRETCHED_GRADIENT:
			if((background.image->w==0)||(background.image->h==0)){
				break;
			}
			fh = float(w)/float(background.image->w);
			fv = float(h)/float(background.image->h);
			TS_StretchShowSurface(background.image, x, y, fh, fv);
			break;
		default:
			break;
	}
	if(background.type>=2){ //If the background is a gradient.
		TS_GradientRectangle(x, y, w, h, &cornerColors[0], &cornerColors[1], &cornerColors[2], &cornerColors[3]);
	}
}


TS_WSborder::~TS_WSborder(){
SDL_FreeSurface(image);
}

TS_WScorner::~TS_WScorner(){
SDL_FreeSurface(image);
}

v8Function LoadWindowStyle(V8ARGS) {
    if(args.Length()<1){
        THROWERROR("LoadWindowStyle Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0, "LoadWindowStyle Error: Arg 0 is not a string.");

    BEGIN_OBJECT_WRAP_CODE

    TS_WindowStyle *ws = NULL;
	TS_Directories *TS_dirs = GetDirs();
        v8::String::Utf8Value str(args[0]);
        const char *wsname = *str;

        SDL_RWops *wstest = SDL_RWFromFile(string(TS_dirs->windowstyle).append(wsname).c_str(), "rb");
        if(!wstest){
            SDL_RWclose(wstest);
            printf("TS: Could not open rws file %s\n", wsname);
            THROWERROR(string("LoadWindowStyle Error: Could not load windowstyle ").append(wsname).c_str());
        }
        SDL_RWclose(wstest);

        ws = new TS_WindowStyle(string(TS_dirs->windowstyle).append(wsname).c_str());

    END_OBJECT_WRAP_CODE(WindowStyle, ws);

}

v8Function GetSystemWindowStyle(V8ARGS) {

    v8::HandleScope loadWindowStylescope;
	v8External external;

	TS_Directories *TS_dirs = GetDirs();
	TS_Config *TS_conf = GetConfig();

        const char *wsname = string(TS_dirs->system).append(TS_conf->systemwindowstyle).c_str();

        SDL_RWops *wstest = SDL_RWFromFile(wsname, "rb");
        if(!wstest){
            SDL_RWclose(wstest);
            return v8::ThrowException(v8::String::New(string("GetSystemWindowStyle Error: Could not load windowstyle ").append(wsname).c_str()));
        }
        SDL_RWclose(wstest);

       TS_WindowStyle *ws = new TS_WindowStyle(string(TS_dirs->system).append(TS_conf->systemwindowstyle).c_str());

        external = v8::External::New(ws);


  	WindowStyleInsttempl->SetInternalFieldCount(1);
	v8::Local<v8::Function> WindowStylector = WindowStyletempl->GetFunction();
	v8::Local<v8::Object> WindowStyleobj = WindowStylector->NewInstance();
    v8::Persistent<v8::Object> PWindowStyleobj = v8::Persistent<v8::Object>::New(WindowStyleobj);
    PWindowStyleobj.MakeWeak(ws, TS_WindowStyleFinalizer);
	PWindowStyleobj->SetInternalField(0, external);
    return loadWindowStylescope.Close(PWindowStyleobj);
}

v8Function TS_WSgetElementWidth(V8ARGS){

}

v8Function TS_WSgetElementHeight(V8ARGS){

}

v8Function TS_WSdrawWindow(V8ARGS) {

	if(args.Length()<4){
        return v8::ThrowException(v8::String::New("TS_WSdrawWindow Error: called with less than 3 parameters."));
    }
    CHECK_ARG_INT(0, "TS_WSdrawWindow Error: could not parse arg 0 to number.")
    CHECK_ARG_INT(1, "TS_WSdrawWindow Error: could not parse arg 1 to number.")
    CHECK_ARG_INT(2, "TS_WSdrawWindow Error: could not parse arg 2 to number.")
    CHECK_ARG_INT(3, "TS_WSdrawWindow Error: could not parse arg 3 to number.")

	int x = static_cast<int>(args[0]->NumberValue());
	int y = static_cast<int>(args[1]->NumberValue());
	int w = static_cast<int>(args[2]->NumberValue());
	int h = static_cast<int>(args[3]->NumberValue());
	//const char * z = args[2]->AsciiValue();
	v8::Local<v8::Object> self = args.Holder();
	//v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));

	static_cast<TS_WindowStyle*>(v8::Local<v8::External>::Cast(self->GetInternalField(0))->Value())->drawWindow(x,y,w,h);
    //delete font;
    self.Clear();
    //wrap.Clear();

	return v8::Undefined();
}
