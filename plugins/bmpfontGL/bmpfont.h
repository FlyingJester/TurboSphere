#ifndef BMPFONT_HEAD
#define BMPFONT_HEAD

#define TS_TEST(Words){\
    printf("This is a test of some stuff. Module: %s, function: %s, and %s of course\n", MODULE, __FUNCTION__, (const char *)Words);\
}

#include"../common/plugin.h"
#include <SDL/SDL_opengl.h>
#include "../../configmanager/opengame.h"
#include "../common/graphic_common.h"
#include "../common/graphic_algorithm.h"
#include "../common/font.h"

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

class MINMEMALIGN TS_BMPFont{
public:
	TS_BMPFont (const char *);
	~TS_BMPFont();
		const char *fontname;
		TS_Color *mask;
		std::vector<TS_BMPGlyph*> glyphs;
	void drawText(int, int, const char*);
	void drawZoomedText(int, int, float, const char*);
	int getStringWidth(const char*) const;
	int getStringHeight(const char*);
	int getCharWidth(char) const;
	int getCharHeight(char);
	int getHeight(void);
	const char **wordWrapString(const char *, int, int *);
private:
    int inheight;
    inline const char ** addline(const char **tl, int *nl, char *lb) const;
};

class TS_BMPGlyph{
public:
    friend class TS_BMPFont;
    TS_BMPGlyph(TS_Texture texture, int width, int height);
    TS_BMPGlyph(uint32_t *pixels, int width, int height);
    ~TS_BMPGlyph();
    TS_Texture texture;
    int width;
    int height;
    double dwidth;
    double dheight;
    int SetImage(TS_Texture texture, int width, int height);
    int SetImage(SDL_Surface*);
private:
    void blit(int x, int y, TS_Color *mask);
    int  zoomBlit(int x, int y, double factor, TS_Color *mask);
};

    void BMPFontInit();
    void BMPFontClose();

    v8::Handle<v8::Value> TS_BMPdrawText(const v8::Arguments& args);
	v8::Handle<v8::Value> LoadSystemTTFFont(const v8::Arguments& args);
	v8::Handle<v8::Value> LoadBMPFont(const v8::Arguments& args);
	v8::Handle<v8::Value> LoadSystemBMPFont(const v8::Arguments& args);
	v8::Handle<v8::Value> TS_BMPgetStringWidth(const v8::Arguments& args);

#ifdef BMPFONT_INTERNAL
#ifdef _WIN32
	extern "C" {
#endif
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
