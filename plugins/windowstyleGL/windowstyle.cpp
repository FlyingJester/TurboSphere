#define PLUGINNAME "windowstyleGL"
#define BRACKNAME "[" PLUGINNAME "]"
#include "../common/plugin.h"
#ifdef _WIN32
#include "../../SDL/SDL_opengl.h"
#else
#include <SDL/SDL_opengl.h>
#endif
#include "../../configmanager/opengame.h"
#include "../common/graphic_common.h"
#include "../common/graphic_algorithm.h"
#include "windowstyle.h"

#ifdef _MSC_VER
   typedef __int16 int16_t;
   #define STRDUP _strdup
#else
   #include <stdint.h>
   #define STRDUP strdup
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

void TS_WindowStyleFinalizer(V8FINALIZERARGS) {
    //basic finalizer. Thanks, Kyuu!
    TS_WindowStyle* ws = (TS_WindowStyle*)parameter;
    delete ws;
    object->Dispose();
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

#define TS_WSERROR_NOERROR 0x0
#define TS_WSERROR_HEADER  0x1
#define TS_WSERROR_SIG     0x2
#define TS_WSERROR_SECTION 0x3
#define TS_WSERROR_ENDDATA 0x3
#define TS_WSERROR_BADFILE 0x4
#define TS_WSERROR_UNKOWN  0xFF

#define TS_WS_THROWERROR(rwfile, errname, errcode)\
    printf(BRACKNAME " Error: " errname, file);\
    SDL_RWclose(rwfile);\
    return errcode;\


#define TS_WS_CHECKLENGTH(rwfile, lsize, errname, errcode)\
    if(rwfile->size(rwfile)<(long long)lsize){\
    TS_WS_THROWERROR(rwfile, errname, errcode);}

TS_WindowStyle::TS_WindowStyle(){
    glGenTextures(9, textures);
    name = NULL;
}

int TS_WindowStyle::Load(const char *file){

	SDL_RWops *wsfile;
	wsfile = SDL_RWFromFile(file, "rb");
    if(!wsfile){
	TS_WS_THROWERROR(wsfile, "Could not open file %s\n", TS_WSERROR_BADFILE);
    }

    name = STRDUP(file);
    uint8_t   sig[5]    = "    ";
    uint16_t  version[1];
    uint8_t   edge_width[1];
    uint8_t   background_mode[1];
    uint8_t   corner_color_comp[16];
    uint8_t   edge_offsets[4];


    TS_WS_CHECKLENGTH(wsfile, 64, "File %s ends before header\n", TS_WSERROR_HEADER);

	//sig
    SDL_RWread(wsfile, sig, 1,4);

    //version
    SDL_RWread(wsfile, version, 2,1);

    //edge width
    SDL_RWread(wsfile, edge_width, 1,1);

    //background mode
    SDL_RWread(wsfile, background_mode, 1,1);

    if((*background_mode)>4){
	TS_WS_THROWERROR(wsfile, "File %s has an invalid background mode\n", TS_WSERROR_BADBGMODE);
    }

    //corner colors
    SDL_RWread(wsfile, corner_color_comp, 1,16);

    //edge offsets
    SDL_RWread(wsfile, edge_offsets, 1,4);

	SDL_RWseek(wsfile, 64, SEEK_SET);

	for(int c = 0; c<4; c++){
	int e = c*4;
		cornerColors[c]=TS_Color(corner_color_comp[e], corner_color_comp[e+1], corner_color_comp[e+2], corner_color_comp[e+3]);
	}
	background.type = (WSbackgroundType)(*background_mode);
    if(version[0]==1){
      TS_WS_THROWERROR(wsfile, "Version 1 windowstyles are not supported. Please upgrade %s using a newer editor.\n", TS_WSERROR_BADVERSION);
    }
    if(version[0]==2){
	int blocks_read = 64;
        uint16_t dimensions[2];
        for(int i = 0; i<WSelementsSize; i++){
	    TS_WS_CHECKLENGTH(wsfile, (blocks_read+4), "Unexpected end of data in file %s\n", TS_WSERROR_ENDDATA);
            blocks_read+=SDL_RWread(wsfile, dimensions, 2, 2);
	    TS_WS_CHECKLENGTH(wsfile, (blocks_read+(dimensions[0]*dimensions[1])), "Unexpected end of data in file %s\n", TS_WSERROR_ENDDATA);
            switch(i){
            case WS_UPPERLEFT:
                CREATE_WS_COMPONENT(blocks_read, wsfile, upperleft, i, dimensions[0], dimensions[1]);
                break;
            case WS_TOP:
                CREATE_WS_COMPONENT(blocks_read, wsfile, top, i, dimensions[0], dimensions[1]);
                break;
            case WS_UPPERRIGHT:
                CREATE_WS_COMPONENT(blocks_read, wsfile, upperright, i, dimensions[0], dimensions[1]);
                break;
            case WS_RIGHT:
                CREATE_WS_COMPONENT(blocks_read, wsfile, right, i, dimensions[0], dimensions[1]);
                break;
            case WS_LOWERRIGHT:
                CREATE_WS_COMPONENT(blocks_read, wsfile, lowerright, i, dimensions[0], dimensions[1]);
                break;
            case WS_BOTTOM:
                CREATE_WS_COMPONENT(blocks_read, wsfile, bottom, i, dimensions[0], dimensions[1]);
                break;
            case WS_LOWERLEFT:
                CREATE_WS_COMPONENT(blocks_read, wsfile, lowerleft, i, dimensions[0], dimensions[1]);
                break;
            case WS_LEFT:
                CREATE_WS_COMPONENT(blocks_read, wsfile, left, i, dimensions[0], dimensions[1]);
                break;
			case WS_BACKGROUND:
				CREATE_WS_COMPONENT(blocks_read, wsfile, background, i, dimensions[0], dimensions[1]);
				break;
            default:
		printf("[windowstyleGL] Warning: Error reading WindowStyle Element in %s. Element was %i.\n", file, i);
                CREATE_WS_COMPONENT(blocks_read, wsfile, upperleft, i, dimensions[0], dimensions[1]);
                break;
            }
        }
    }
	SDL_RWclose(wsfile);
	return 0;
}

TS_WindowStyle::~TS_WindowStyle(){
    glDeleteTextures(9, textures);
    if(name!=NULL)
	free((void *)name);
}

void TS_WindowStyle::drawWindow(int x, int y, int w, int h){
    for(int i = 0; i<8; i++){
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
        int *dim = this->getComponentDimensions(i);
        int width  = dim[0];
        int height = dim[1];
        free(dim);
        int tx = 0;
        int ty = 0;
        TS_Texture texture = this->textures[i];
        glBindTexture(GL_TEXTURE_2D, texture);
        const GLuint  colorData[]    = {
            fullmask->toInt(),
            fullmask->toInt(),
            fullmask->toInt(),
            fullmask->toInt()
        };

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
            const GLint   vertexData[]   = {tx, ty, tx+width, ty, tx+width, ty+height, tx, ty+height};
            const GLint   texcoordData[] = {0, 0, 1, 0, 1, 1, 0, 1};

            glTexCoordPointer(2, GL_INT, 0, texcoordData);
            glVertexPointer(2, GL_INT, 0, vertexData);
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texture);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

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
                const GLint     vertexData[]   = {tx, ty, tx+w, ty, tx+w, ty+height, tx, ty+height};
                const GLfloat   texcoordData[] = {0, 0, wtile, 0, wtile, 1, 0, 1};

                glTexCoordPointer(2, GL_FLOAT, 0, texcoordData);
                glVertexPointer(2, GL_INT, 0, vertexData);
                glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, texture);
                glDrawArrays(GL_QUADS, 0, 4);
                /*
                glTexCoord2f(0.0f,  0.0f);
                glVertex2i(x,       ty);
                glTexCoord2f(wtile, 0.0f);
                glVertex2i(x+w,     ty);
                glTexCoord2f(wtile, 1.0f);
                glVertex2i(x+w,     ty+height);
                glTexCoord2f(0.0f,  1.0f);
                glVertex2i(x,       ty+height);
                */
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

                const GLint     vertexData[]   = {tx, ty, tx+width, ty, tx+width, ty+h, tx, ty+h};
                const GLfloat   texcoordData[] = {0, 0, 1, 0, 1, htile, 0, htile};

                glTexCoordPointer(2, GL_FLOAT, 0, texcoordData);
                glVertexPointer(2, GL_INT, 0, vertexData);
                glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, texture);
                glDrawArrays(GL_QUADS, 0, 4);
            }
        }
        }

	//draw background.
	//WS_TILED, WS_STRETCHED, WS_GRADIENT, WS_TILED_GRADIENT, WS_STRETCHED_GRADIENT;
	//float fh;
	//float fv;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, background.texture);
    float wtile = 0.0f;
    float htile = 0.0f;

    GLuint  colorData[]    = {
            fullmask->toInt(),
            fullmask->toInt(),
            fullmask->toInt(),
            fullmask->toInt()
    };



    GLint     vertexData1[]   = {x, y, x+w, y, x+w, y+h, x, y+h};
    GLfloat   texcoordData1[] = {0, 0, 1, 0, 1, 1, 0, 1};

	switch(background.type){
		case WS_TILED:
		case WS_TILED_GRADIENT:
            wtile = ((float)w)/((float)background.width );
            htile = ((float)h)/((float)background.height);

            texcoordData1[2] = wtile;
            texcoordData1[4] = wtile;
            texcoordData1[5] = htile;
            texcoordData1[7] = htile;

            glTexCoordPointer(2, GL_FLOAT, 0, texcoordData1);
            glVertexPointer(2, GL_INT, 0, vertexData1);
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
            glDrawArrays(GL_QUADS, 0, 4);

			break;
		case WS_STRETCHED:
		case WS_STRETCHED_GRADIENT:
            glTexCoordPointer(2, GL_FLOAT, 0, texcoordData1);
            glVertexPointer(2, GL_INT, 0, vertexData1);
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
            glDrawArrays(GL_QUADS, 0, 4);
			break;
        case WS_GRADIENT:
            break;
	}

	glDisable(GL_TEXTURE_2D);

    if(background.type>=2){ //If the background is a gradient.
        colorData[0] = cornerColors[0].toInt();
        colorData[1] = cornerColors[1].toInt();
        colorData[2] = cornerColors[2].toInt();
        colorData[3] = cornerColors[3].toInt();
        glDrawArrays(GL_QUADS, 0, 4);
    }

	glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);


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

        ws = new TS_WindowStyle();
	int error = ws->Load(string(TS_dirs->windowstyle).append(wsname).c_str());

	if(error!=TS_WSERROR_NOERROR){
	    THROWERROR(" Error: Could not load windowstyle.");
	}

    END_OBJECT_WRAP_CODE(WindowStyle, ws);

}

v8Function GetSystemWindowStyle(V8ARGS) {

    BEGIN_OBJECT_WRAP_CODE;

	TS_Directories *TS_dirs = GetDirs();
	TS_Config *TS_conf = GetConfig();

        const char *wsname = STRDUP(string(TS_dirs->system).append(TS_conf->systemwindowstyle).c_str());

        SDL_RWops *wstest = SDL_RWFromFile(wsname, "rb");
        if(!wstest){
            SDL_RWclose(wstest);
            string errorname = string("GetSystemWindowStyle Error: Could not load windowstyle ").append(wsname).c_str();
            free((void *)wsname);
            return v8::ThrowException(v8::String::New(errorname.c_str()));
        }
        SDL_RWclose(wstest);

        TS_WindowStyle *ws = new TS_WindowStyle();
	int error = ws->Load(wsname);
	if(error!=TS_WSERROR_NOERROR){
	    free((void *)wsname);
	    THROWERROR(" Error: Could not load system windowstyle.");
	}

    free((void *)wsname);

    END_OBJECT_WRAP_CODE(WindowStyle, ws);
}

v8Function TS_WSgetElementWidth(V8ARGS){

    return v8::Undefined();
}

v8Function TS_WSgetElementHeight(V8ARGS){

    return v8::Undefined();
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
