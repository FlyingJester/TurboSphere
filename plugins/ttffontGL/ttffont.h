#ifndef TTFFONT_HEAD
#define TTFFONT_HEAD

#include"../common/plugin.h"
#include <SDL/SDL_opengl.h>
#include "../common/graphic_common.h"

#ifdef _WIN32
    #ifdef TTFFONT_INTERNAL
	#define TTFFONT_EXPORT __declspec(dllexport)
	#else
	#define TTFFONT_EXPORT __declspec(dllimport)
	#endif

    #define CCALL __cdecl

#else
#define CCALL
#define TTFFONT_EXPORT extern "C"
#endif

#define MAX_CACHED_STRINGS 0x0040

struct TS_GlyphAttribs;

class TS_TTFFont{
public:
	TS_TTFFont(const char *);
	TS_TTFFont(const char *, int);
	TTF_Font *font;
	const char *fontname;
	int size;
	SDL_Color mask;
	TS_Color *tsmask;
	TS_Color *text_surface;
	void drawText(int, int, const char*);
	void drawZoomedText(int, int, float, const char*);
	int  getStringWidth(const char*);
    ~TS_TTFFont();
private:
    GLuint *         cache;
    const char **    strings;
    TS_GlyphAttribs *cacheattribs;
};

struct TS_GlyphAttribs{
    TS_GlyphAttribs(unsigned long long, unsigned char, unsigned char, unsigned char, unsigned short, unsigned short);
    TS_GlyphAttribs(void);
    unsigned long long age;
    unsigned char style;
    unsigned char real;
    unsigned char fontsize;
    unsigned short width;
    unsigned short height;
};

v8::Handle<v8::Value> TS_TTFdrawText(const v8::Arguments& args);
v8::Handle<v8::Value> TS_TTFdrawZoomedText(const v8::Arguments& args);
v8::Handle<v8::Value> TS_TTFgetStringWidth(const v8::Arguments& args);
v8::Handle<v8::Value> LoadTTFFont(const v8::Arguments& args);
v8::Handle<v8::Value> GetSystemTTFFont(const v8::Arguments& args);

#ifdef TTFFONT_INTERNAL
#ifdef _WIN32
	extern "C" {
#endif
TTFFONT_EXPORT void          CCALL Close(void);
TTFFONT_EXPORT initFunction  CCALL Init(void);
TTFFONT_EXPORT int			 CCALL GetNumCategories(void);
TTFFONT_EXPORT nameArray	 CCALL GetCategories(void);
TTFFONT_EXPORT int           CCALL GetNumFunctions(void);
TTFFONT_EXPORT functionArray CCALL GetFunctions(void);
TTFFONT_EXPORT nameArray     CCALL GetFunctionNames(void);
TTFFONT_EXPORT int           CCALL GetNumVariables(void);
TTFFONT_EXPORT void **       CCALL GetVariables(void);
TTFFONT_EXPORT nameArray     CCALL GetVariableNames(void);
#ifdef _WIN32
	}
#endif
#endif
#endif
