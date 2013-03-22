#ifndef WS_HEAD
#define WS_HEAD

#define NUMVARS 0
#define NUMFUNCS 2

#ifndef WINDOWSTYLE_HEAD
#define WINDOWSTYLE_HEAD
 
#ifdef _WIN32
	#define WS_EXPORT __declspec(dllexport)
 
    #define CCALL __cdecl
 
#endif
#ifndef _WIN32
#define CCALL 
#define WS_EXPORT extern "C"
#endif
 
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

class TS_WSborder{
    public:
    TS_WSborder(SDL_Surface *);
    TS_WSborder();
    ~TS_WSborder();
    int width;
    int height;
    SDL_Surface *image;
    TS_Color mask;
};
class TS_WScorner{
    public:
    TS_WScorner(SDL_Surface *, int, int);
    TS_WScorner();
    ~TS_WScorner();
    int width;
    int height;
    int xOffset;
    int yOffset;
    SDL_Surface *image;
    TS_Color mask;
};

class TS_WindowStyle{
    public:
    TS_WindowStyle(const char*);
    ~TS_WindowStyle();
    TS_WSborder top;
    TS_WSborder right;
    TS_WSborder bottom;
    TS_WSborder left;
    TS_WScorner upperleft;
    TS_WScorner upperright;
    TS_WScorner lowerright;
    TS_WScorner lowerleft;
    TS_Color cornerColors[4];
    char edge_offsets[4];
    TS_Color mask;
    SDL_Surface *getImage(int);
    void drawWindow(int, int, int, int);
};

    v8::Handle<v8::Value> LoadWindowStyle(const v8::Arguments& args);
    v8::Handle<v8::Value> GetSystemWindowStyle(const v8::Arguments& args);
    v8::Handle<v8::Value> TS_WSdrawWindow(const v8::Arguments& args);

void WindowStyleInit();
void WindowStyleClose();

//Macro for creating WindowStyle components.

#define CREATE_WS_COMPONENT(component, w, h) \
        component = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA, w, h, DEPTH, CHANNEL_MASKS);\
        SDL_RWread(wsfile, component->pixels, BPP, w*h);
#endif
