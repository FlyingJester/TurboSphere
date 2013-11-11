#ifndef BMPFONT_HEAD
#define BMPFONT_HEAD

#include"../common/plugin.h"
#include "../../SDL2/SDL_opengl.h"

#include "../../configmanager/opengame.h"
#include "../common/graphic_common.h"
#include "../common/graphic_algorithm.h"
#include "../common/font.h"


#define BPP 4
#define DEPTH 32

    #define Frmask 0x000000ff
	#define Fgmask 0x0000ff00
	#define Fbmask 0x00ff0000
	#define Famask 0xff000000

#define CHANNEL_MASKS Frmask, Fgmask, Fbmask, Famask

#define MAX_CACHED_TEXTLINES 64
#define MAX_CACHED_WORDWRAPS 8

#ifdef _WIN32
    #ifdef BMPFONT_INTERNAL
	#define BMPFONT_EXPORT __declspec(dllexport)
	#else
	#define BMPFONT_EXPORT __declspec(dllimport)
	#endif

    #define CCALL __cdecl

#else
#define CCALL
#define BMPFONT_EXPORT extern "C"
#endif
class TS_BMPGlyph;

struct TS_BMPWordWrapResult{
    int numlines;
    const char *str;
    const char **lines;
};

class MINMEMALIGN TS_BMPFont{
public:
	TS_BMPFont();
	~TS_BMPFont();
	int Load(const char *);
		const char *fontname;
		TS_Color *mask;
		std::vector<TS_BMPGlyph*> glyphs;
	void drawText(int, int, const char*);
	void drawZoomedText(int, int, double, const char*);
	int getStringWidth(const char*) const;
	int getStringHeight(const char*);
	int getCharWidth(char) const;
	int getCharHeight(char);
	int getHeight(void);
	void setColorMask(TS_Color *c);
	TS_Color *getColorMask(void);
	const char **wordWrapString(const char * __restrict , int, int * __restrict);
private:
    unsigned short *widths;
    SDL_Surface *surfaceAtlas;
    TS_Texture textureAtlas;
    int inheight;
    inline const char ** addline(const char ** __restrict tl, int * __restrict nl, char ** __restrict lb) const;
};

class TS_BMPGlyph{
public:
    friend class TS_BMPFont;
    //TS_BMPGlyph(TS_Texture texture, int width, int height);
    TS_BMPGlyph(uint32_t *pixels, int width, int height);
    ~TS_BMPGlyph();
    TS_Texture texture;
    int width;
    int height;
    double dwidth;
    double dheight;
    int SetImage(TS_Texture texture, int width, int height);
    int SetImage(SDL_Surface*);
    SDL_Surface *surface;
private:
    void blit(int x, int y, TS_Color *mask);
    int  zoomBlit(int x, int y, double factor, TS_Color *mask);
};

#define TS_BMPERROR_NOERROR 0x0
#define TS_BMPERROR_HEADER  0x1
#define TS_BMPERROR_SIG     0x2
#define TS_BMPERROR_GLYPH   0x3
#define TS_BMPERROR_ENDDATA 0x3
#define TS_BMPERROR_BADFILE 0x4
#define TS_BMPERROR_UNKOWN  0xFF

    void BMPFontInit();
    void BMPFontClose();

    v8::Handle<v8::Value> TS_BMPdrawText(const v8::Arguments& args);
    v8Function TS_BMPdrawZoomedText(V8ARGS);
	v8::Handle<v8::Value> LoadSystemTTFFont(const v8::Arguments& args);
	v8::Handle<v8::Value> LoadBMPFont(const v8::Arguments& args);
	v8::Handle<v8::Value> LoadSystemBMPFont(const v8::Arguments& args);
	v8::Handle<v8::Value> TS_BMPgetStringWidth(const v8::Arguments& args);
    v8Function TS_BMPsetColorMask(V8ARGS);
    v8Function TS_BMPgetColorMask(V8ARGS);

#ifdef BMPFONT_INTERNAL
#ifdef _WIN32
	extern "C" {
#endif

BMPFONT_EXPORT void          CCALL GetPluginInfo(TS_PluginInfo *info);

BMPFONT_EXPORT void          CCALL Close(void);
BMPFONT_EXPORT initFunction  CCALL Init(void);
BMPFONT_EXPORT int           CCALL GetNumFunctions(void);
BMPFONT_EXPORT functionArray CCALL GetFunctions(void);
BMPFONT_EXPORT nameArray     CCALL GetFunctionNames(void);
BMPFONT_EXPORT int           CCALL GetNumVariables(void);
BMPFONT_EXPORT void **       CCALL GetVariables(void);
BMPFONT_EXPORT nameArray     CCALL GetVariableNames(void);
#ifdef _WIN32
	}
#endif
#endif
#endif
