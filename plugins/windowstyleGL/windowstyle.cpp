
//////////////////
/*
Let's try and keep this file a paragon of cleanliness.
The windowstyle plugin is a prime example of how every
plugin should be done. It also shows quite well how to
cleanly and concisely read from Sphere files.
*/
///////////////////
#define PLUGINNAME "windowstyleGL"
#include "../common/plugin.h"
#include <SDL/SDL_opengl.h>
#include "../../configmanager/opengame.h"
#include "../common/graphic_common.h"
#include "../common/graphic_algorithm.h"
#include "windowstyle.h"

#ifdef _MSC_VER
   typedef __int16 int16_t;
#else
   #include <stdint.h>
#endif

v8Function TS_WSdrawWindow(V8ARGS);

TS_Color *fullmask;

const int WSelementsSize = 9;
enum WSelements {WS_UPPERLEFT, WS_TOP, WS_UPPERRIGHT, WS_RIGHT, WS_LOWERRIGHT, WS_BOTTOM, WS_LOWERLEFT, WS_LEFT, WS_BACKGROUND};

void GLColor(TS_Color *color){
    if(color==NULL){
    glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);
    }
    else{
    glColor4ub(color->red, color->green, color->blue, color->alpha);
    }
}

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
    SET_CLASS_NAME(WindowStyle, "WindowStyle");
    ADD_TO_PROTO(WindowStyle, "drawWindow", TS_WSdrawWindow);

    fullmask = new TS_Color(255, 255, 255, 255);

    return (char *)PLUGINNAME;
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

TS_WSborder::TS_WSborder(TS_Texture tex, int w, int h){
    texture = tex;
    width   = w;
    height  = h;
    mask    = fullmask;
}

TS_WSborder::TS_WSborder(){
    width = 0;
    height= 0;
    mask  = fullmask;
}

TS_WScorner::TS_WScorner(TS_Texture tex, int w, int h, int xOff, int yOff){
    texture = tex;
    width   = w;
    height  = h;
    xOffset = xOff;
    yOffset = yOff;
}

TS_WScorner::TS_WScorner(){
    width = 0;
    height= 0;
    xOffset = 0;
    yOffset = 0;
    mask    = fullmask;
}

TS_WSbackground::TS_WSbackground(TS_Texture tex, int w, int h, WSbackgroundType _type){
    texture = tex;
    width   = w;
    height  = h;
	type    = _type;
    mask    = fullmask;
}

TS_WSbackground::TS_WSbackground(){
    width = 0;
    height= 0;
	type = WS_TILED;
    mask    = fullmask;
}

TS_WSbackground::~TS_WSbackground(){
    glDeleteTextures(1, &texture);
}

int * TS_WindowStyle::getComponentDimensions(int num){
    int *dims = (int*)calloc(2, sizeof(int));
    switch(num){
        case 0:
        //return {upperleft.width, upperleft.height};
        dims[0] = upperleft.width; dims[1] = upperleft.height;
		break;
        case 1:
        //return {top.width, top.height};
        dims[0] = top.width; dims[1] = top.height;
		break;
        case 2:
        //return {upperright.width, upperright.height};
        dims[0] = upperright.width; dims[1] = upperright.height;
		break;
        case 3:
        //return {right.width, rightheight};
        dims[0] = right.width; dims[1] = right.height;
		break;
        case 4:
        //return {lowerright.width, lowerright.height};
        dims[0] = lowerright.width; dims[1] = lowerright.height;
		break;
        case 5:
        //return {bottom.width, bottom.height};
        dims[0] = bottom.width; dims[1] = bottom.height;
		break;
        case 6:
        //return {lowerleft.width, lowerleft.height};
        dims[0] = lowerleft.width; dims[1] = lowerleft.height;
		break;
        case 7:
        //return {left.width, left.height};
        dims[0] = left.width; dims[1] = left.height;
		break;
        case 8:
        //return {background.width, background.height};
        dims[0] = background.width; dims[1] = background.height;
		break;
		default:  //at least don't explode.
		printf("[windowstyleGL] Error: Error reading WindowStyle Element in %s. Element was %i.\n", name, num);
		return NULL;//new SDL_Surface;
	}
	return dims;
}

TS_WindowStyle::TS_WindowStyle(const char *file){

	SDL_RWops *wsfile;
	wsfile = SDL_RWFromFile(file, "rb");
    if(!wsfile){
		printf("Could not open file %s\n", file);   //the file should be checked BEFORE passing it to the constructor!
	}


    glGenTextures(9, textures);

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
                CREATE_WS_COMPONENT(upperleft, i, dimensions[0], dimensions[1]);
                break;
            case WS_TOP:
                CREATE_WS_COMPONENT(top, i, dimensions[0], dimensions[1]);
                break;
            case WS_UPPERRIGHT:
                CREATE_WS_COMPONENT(upperright, i, dimensions[0], dimensions[1]);
                break;
            case WS_RIGHT:
                CREATE_WS_COMPONENT(right, i, dimensions[0], dimensions[1]);
                break;
            case WS_LOWERRIGHT:
                CREATE_WS_COMPONENT(lowerright, i, dimensions[0], dimensions[1]);
                break;
            case WS_BOTTOM:
                CREATE_WS_COMPONENT(bottom, i, dimensions[0], dimensions[1]);
                break;
            case WS_LOWERLEFT:
                CREATE_WS_COMPONENT(lowerleft, i, dimensions[0], dimensions[1]);
                break;
            case WS_LEFT:
                CREATE_WS_COMPONENT(left, i, dimensions[0], dimensions[1]);
                break;
			case WS_BACKGROUND:
				CREATE_WS_COMPONENT(background, i, dimensions[0], dimensions[1]);
				break;
            default:
				printf("[windowstyleGL] Error: Error reading WindowStyle Element in %s. Element was %i.\n", file, i);
                CREATE_WS_COMPONENT(upperleft, i, dimensions[0], dimensions[1]);
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
    glEnable(GL_TEXTURE_2D);
	int *dimensions = this->getComponentDimensions(i);
        int width  = dimensions[0];
        int height = dimensions[1];
	free(dimensions);
        int tx = 0;
        int ty = 0;
        TS_Texture texture = this->textures[i];
        glBindTexture(GL_TEXTURE_2D, texture);
        glBegin(GL_QUADS);
            GLColor(fullmask);

            if(i%2==0){//is a corner.
                if(i==0||i==6){
                    tx = x-width;
                }
                else{
                    tx = x+w;
                }
                if(i>3){
                    ty = y+h;
                }
                else{
                    ty = y-height;
                }
                glTexCoord2i(0, 0);
                glVertex2i(tx,       ty);
                glTexCoord2i(1, 0);
                glVertex2i(tx+width, ty);
                glTexCoord2i(1, 1);
                glVertex2i(tx+width, ty+height);
                glTexCoord2i(0, 1);
                glVertex2i(tx,       ty+height);
            }
            else{
                if(i==1||i==5){ //this runs horizontal.
                    float wtile = ((float)w)/((float)width);
                    tx = x;
                    if(i==1){
                        ty = y-height;
                    }
                    else{
                        ty = y+h;
                    }
                    glTexCoord2f(0.0f,  0.0f);
                    glVertex2i(x,       ty);
                    glTexCoord2f(wtile, 0.0f);
                    glVertex2i(x+w,     ty);
                    glTexCoord2f(wtile, 1.0f);
                    glVertex2i(x+w,     ty+height);
                    glTexCoord2f(0.0f,  1.0f);
                    glVertex2i(x,       ty+height);

                }
                else{   //this runs vertical.
                    float htile = ((float)h)/((float)height);
                    ty = y;
                    if(i==3){
                        tx = x+w;
                    }
                    else{
                        tx = x-width;
                    }
                    //printf("Width: %i ; Height %i ; variant: %f\n", width, height, htile);

                    glTexCoord2f(0.0f, 0.0f);
                    glVertex2i(tx, ty);
                    glTexCoord2f(1.0f, 0.0f);
                    glVertex2i(tx+width,  ty);
                    glTexCoord2f(1.0f, htile);
                    glVertex2i(tx+width,  ty+h);
                    glTexCoord2f(0.0f, htile);
                    glVertex2i(tx, ty+h);

                }
            }

        glEnd();
    glDisable(GL_TEXTURE_2D);
        }

	//draw background.
	//WS_TILED, WS_STRETCHED, WS_GRADIENT, WS_TILED_GRADIENT, WS_STRETCHED_GRADIENT;
	float fh;
	float fv;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, background.texture);
    glBegin(GL_QUADS);
    float wtile = 0.0f;
    float htile = 0.0f;
	switch(background.type){
		case WS_TILED:
		case WS_TILED_GRADIENT:
            wtile = ((float)w)/((float)background.width );
            htile = ((float)h)/((float)background.height);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2i(x,   y);
            glTexCoord2f(wtile, 0.0f);
            glVertex2i(x+w, y);
            glTexCoord2f(wtile, htile);
            glVertex2i(x+w, y+h);
            glTexCoord2f(0.0f,  htile);
            glVertex2i(x,   y+h);
			break;
		case WS_STRETCHED:
		case WS_STRETCHED_GRADIENT:
            glTexCoord2i(0, 0);
            glVertex2i(x,   y);
            glTexCoord2i(1, 0);
            glVertex2i(x+w, y);
            glTexCoord2i(1, 1);
            glVertex2i(x+w, y+h);
            glTexCoord2i(0,  1);
            glVertex2i(x,   y+h);

			break;
		default:
			break;
	}
	glEnd();
    glDisable(GL_TEXTURE_2D);
	if(background.type>=2){ //If the background is a gradient.
		glBegin(GL_QUADS);
            GLColor(&cornerColors[0]);
            glVertex2i(x,   y);
            GLColor(&cornerColors[1]);
            glVertex2i(x+w, y);
            GLColor(&cornerColors[2]);
            glVertex2i(x+w, y+h);
            GLColor(&cornerColors[3]);
            glVertex2i(x,   y+h);
        glEnd();
    }
}


TS_WSborder::~TS_WSborder(){
    glDeleteTextures(1, &texture);
}

TS_WScorner::~TS_WScorner(){
    glDeleteTextures(1, &texture);
}

v8Function LoadWindowStyle(V8ARGS) {
    if(args.Length()<1){
        THROWERROR("LoadWindowStyle Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);

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

    BEGIN_OBJECT_WRAP_CODE;

	TS_Directories *TS_dirs = GetDirs();
	TS_Config *TS_conf = GetConfig();

        const char *wsname = string(TS_dirs->system).append(TS_conf->systemwindowstyle).c_str();

        SDL_RWops *wstest = SDL_RWFromFile(wsname, "rb");
        if(!wstest){
            SDL_RWclose(wstest);
            return v8::ThrowException(v8::String::New(string("GetSystemWindowStyle Error: Could not load windowstyle ").append(wsname).c_str()));
        }
        SDL_RWclose(wstest);

       MINMEMALIGN TS_WindowStyle *ws = new TS_WindowStyle(string(TS_dirs->system).append(TS_conf->systemwindowstyle).c_str());

    END_OBJECT_WRAP_CODE(WindowStyle, ws);
}

v8Function TS_WSgetElementWidth(V8ARGS){

}

v8Function TS_WSgetElementHeight(V8ARGS){

}

v8Function TS_WSdrawWindow(V8ARGS) {

	if(args.Length()<4){
        return v8::ThrowException(v8::String::New("TS_WSdrawWindow Error: called with less than 3 parameters."));
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);

	int x = args[0]->Int32Value();
	int y = args[1]->Int32Value();
	int w = args[2]->Int32Value();
	int h = args[3]->Int32Value();

	GET_SELF(TS_WindowStyle*)->drawWindow(x,y,w,h);

	return v8::Undefined();
}
