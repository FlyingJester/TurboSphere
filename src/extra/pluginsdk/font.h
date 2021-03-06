#ifndef FONT_HEAD
#define FONT_HEAD
#include <vector>

typedef GLuint TS_Texture;
typedef GLuint TS_TexVertexBuffer;
typedef GLuint TS_TextureBuffer;

class TS_TTFFont{
public:
	TS_TTFFont(const char *);
	TS_TTFFont(const char *, int);
	TTF_Font *font;
	const char *fontname;
	int size;
	SDL_Color mask;
	SDL_Surface *text_surface;
	void drawText(int, int, const char*);
	void drawZoomedText(int, int, float, const char*);
	int  getStringWidth(const char*);
    ~TS_TTFFont  ();
};

	v8::Handle<v8::Value> TS_TTFdrawText(const v8::Arguments& args);
	v8::Handle<v8::Value> TS_TTFdrawZoomedText(const v8::Arguments& args);
	v8::Handle<v8::Value> TS_TTFgetStringWidth(const v8::Arguments& args);
	v8::Handle<v8::Value> LoadTTFFont(const v8::Arguments& args);
	v8::Handle<v8::Value> GetFontname(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	v8::Handle<v8::Value> GetFontsize(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	v8::Handle<v8::Value> GetFontMask(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	void SetFontname(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
	void SetFontsize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
	void SetFontMask(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);

#endif
