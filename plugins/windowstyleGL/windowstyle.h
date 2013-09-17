#ifndef WS_HEAD
#define WS_HEAD

#define NUMVARS 0
#define NUMFUNCS 2

#ifndef WINDOWSTYLE_HEAD
#define WINDOWSTYLE_HEAD

#ifdef _WIN32
#undef WS_TILED
	#define WS_EXPORT __declspec(dllexport)

    #define CCALL __cdecl

#endif
#ifndef _WIN32
#define CCALL
#define WS_EXPORT extern "C"
#endif

typedef GLuint TS_Texture;

enum WSbackgroundType {WS_TILED, WS_STRETCHED, WS_GRADIENT, WS_TILED_GRADIENT, WS_STRETCHED_GRADIENT};

#ifdef _WIN32
	extern "C" {
#endif
WS_EXPORT void            CCALL Close(void);
WS_EXPORT initFunction    CCALL Init(void);
WS_EXPORT int             CCALL GetNumFunctions(void);
WS_EXPORT functionArray   CCALL GetFunctions(void);
WS_EXPORT nameArray       CCALL GetFunctionNames(void);
WS_EXPORT int             CCALL GetNumVariables(void);
WS_EXPORT v8FunctionArray CCALL GetVariables(void);
WS_EXPORT nameArray       CCALL GetVariableNames(void);

#ifdef _WIN32
	}
#endif

#endif

class MINMEMALIGN TS_WSborder{
    public:
    TS_WSborder(TS_Texture texture, int w, int h);
    TS_WSborder();
    ~TS_WSborder();
    int width;
    int height;
    TS_Texture texture;
    TS_Color *mask;
};
class MINMEMALIGN TS_WScorner{
    public:
    TS_WScorner(TS_Texture tex, int width, int height, int xOff, int yOff);
    TS_WScorner();
    ~TS_WScorner();
    int width;
    int height;
    int xOffset;
    int yOffset;
    TS_Texture texture;
    TS_Color *mask;
};

class MINMEMALIGN TS_WSbackground{
    public:
    TS_WSbackground(TS_Texture texture, int w, int h, WSbackgroundType);
    TS_WSbackground();
    ~TS_WSbackground();
	int width;
	int height;
    TS_Texture texture;
    TS_Color *mask;
	WSbackgroundType type;
};

class MINMEMALIGN TS_WindowStyle{
    public:
    TS_WindowStyle();
    ~TS_WindowStyle();
    int Load(const char*);
    TS_Texture textures[9];
    TS_WScorner upperleft;  //0
    TS_WSborder top;        //1
    TS_WScorner upperright; //2
    TS_WSborder right;      //3
    TS_WScorner lowerright; //4
    TS_WSborder bottom;     //5
    TS_WScorner lowerleft;  //6
    TS_WSborder left;       //7
	TS_WSbackground background; //8
    TS_Color cornerColors[4];
    char edge_offsets[4];
    TS_Color *mask;
    TS_Texture getImage(int);
    int * getComponentDimensions(int);
    void drawWindow(int, int, int, int);
private:
	const char *name;
};

#define TS_WSERROR_NOERROR 	0x0
#define TS_WSERROR_HEADER  	0x1
#define TS_WSERROR_SIG     	0x2
#define TS_WSERROR_SECTION 	0x3
#define TS_WSERROR_ENDDATA 	0x3
#define TS_WSERROR_BADFILE 	0x4
#define TS_WSERROR_BADBGMODE	0xF0
#define TS_WSERROR_BADVERSION	0xF1
#define TS_WSERROR_UNKOWN  	0xFF


void WindowStyleInit();
void WindowStyleClose();

v8Function TS_WSdrawWindow(V8ARGS);

//Macro for creating WindowStyle components.

#define CREATE_WS_COMPONENT(sum, rwfile, component, i, w, h){ \
        component.width     = w;\
        component.height    = h;\
        uint32_t *pixels    = (uint32_t *)calloc(w*h, sizeof(uint32_t));\
        sum+=SDL_RWread(rwfile, pixels, sizeof(uint32_t), w*h);\
        glBindTexture(GL_TEXTURE_2D, textures[i]);\
        component.texture   = textures[i];\
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);\
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);\
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);\
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);\
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);\
        free(pixels);}

#endif
