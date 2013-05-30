#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include "../../configmanager/opengame.h"
#define TTFFONT_INTERNAL
#define PLUGINNAME "ttffontGL"
#define BRACKNAME "[" PLUGINNAME "]"
#include "ttffont.h"
#define NUMFUNCS 1
#define NUMVARS 0
#define NUMCATS 3

using namespace std;

v8Function LoadTTFFont(V8ARGS);
void * LoadTTFFontPointer          = V8FUNCPOINTER(LoadTTFFont);

DECLARE_OBJECT_TEMPLATES(TTFFont);

void GLColor(TS_Color *color){
    glColor4ub(color->red, color->green, color->blue, color->alpha);
}

TS_Color *fullmask = new TS_Color(0xFF, 0xFF, 0xFF, 0xFF);

initFunction Init(void){
	if(!TTF_WasInit()){
		if(TTF_Init()==-1){
			printf("TTFFont Init Error: %s\n", TTF_GetError());
		}
    }

    INIT_OBJECT_TEMPLATES(TTFFont);
    ADD_TO_PROTO(TTFFont, "drawText", TS_TTFdrawText);
    ADD_TO_PROTO(TTFFont, "drawZoomedText", TS_TTFdrawZoomedText);
    ADD_TO_PROTO(TTFFont, "getStringWidth", TS_TTFgetStringWidth);

	return (initFunction)PLUGINNAME;
}

void Close(){
    CLOSE_OBJECT_TEMPLATES(TTFFont);
	TTF_Quit();
}

int GetNumFunctions(){
	return NUMFUNCS;
}


functionArray GetFunctions(void){
	functionArray funcs = (functionArray)calloc(NUMFUNCS, sizeof(v8Function));
	*funcs = LoadTTFFontPointer;
	return funcs;
}
nameArray GetFunctionNames(void){
	nameArray vars = (nameArray)calloc(NUMFUNCS, sizeof(const char *));
	*vars = (const char *)"TTFFont";
	return vars;
}


int GetNumVariables(){
	return NUMVARS;
}


void ** GetVariables(void){
	return NULL;
}

nameArray GetVariableNames(void){
	return NULL;
}


TS_GlyphAttribs::TS_GlyphAttribs(unsigned long long a, unsigned char s, unsigned char r, unsigned char f, unsigned short w, unsigned short h){
    age      = a;
    style    = s;
    real     = r;
    fontsize = f;
    width    = w;
    height   = h;
}

TS_GlyphAttribs::TS_GlyphAttribs(void){
    age      = 0;
    style    = 0;
    real     = 0;
    fontsize = 0;
    width    = 0;
    height   = 0;
}

void TS_TTFFontFinalizer(v8::Persistent<v8::Value> object, void* parameter) {
    TS_TTFFont* font = (TS_TTFFont*)parameter;
    delete font;
    object.Dispose();
}

void TS_TTFFont::drawText(int x, int y, const char *text){

    bool needtobind = true;

    int oldest = 0;
    int age    = cacheattribs[0].age;

    for(int i = 0; i<MAX_CACHED_STRINGS; i++){
        cacheattribs[i].age++;
        if(cacheattribs[i].age>age){
            age = cacheattribs[i].age;
            oldest = i;
        }
        if((!needtobind)||(strings[i]==NULL)) continue;

        if(strcmp(text, strings[i])==0){
            if(cacheattribs[i].age>0) cacheattribs[i].age--;

            const GLint   vertexData[]   = {x, y, x+cacheattribs[i].width, y, x+cacheattribs[i].width, y+cacheattribs[i].height, x, y+cacheattribs[i].height};
            const GLint   texcoordData[] = {0, 0, 1, 0, 1, 1, 0, 1};
            const GLuint  colorData[]    = {
                tsmask->toInt(),
                tsmask->toInt(),
                tsmask->toInt(),
                tsmask->toInt()
            };

            glTexCoordPointer(2, GL_INT, 0, texcoordData);
            glVertexPointer(2, GL_INT, 0, vertexData);
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, cache[i]);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            glDrawArrays(GL_QUADS, 0, 4);
            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisable(GL_TEXTURE_2D);
            needtobind = false;
        }
    }
    if(!needtobind){
        return;
    }
    printf(BRACKNAME " DrawText Info: Binding string %s to cache slot %i.\n", text, oldest);
    if(cacheattribs[oldest].real){
        glDeleteTextures(1, &(cache[oldest]));
        free((void *)strings[oldest]);
    }
    strings[oldest] = strdup(text);
    cacheattribs[oldest] = TS_GlyphAttribs(0, 0, 1, 0, 0, 0);
    glGenTextures(1, &(cache[oldest]));
    SDL_Color color={0xFF,0xFF,0xFF};
    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, color);
    cacheattribs[oldest].width  = surface->w;
    cacheattribs[oldest].height = surface->h;
    glBindTexture(GL_TEXTURE_2D, cache[oldest]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_FreeSurface(surface);

    const GLint   vertexData[]   = {x, y, x+cacheattribs[oldest].width, y, x+cacheattribs[oldest].width, y+cacheattribs[oldest].height, x, y+cacheattribs[oldest].height};
    const GLint   texcoordData[] = {0, 0, 1, 0, 1, 1, 0, 1};
    const GLuint  colorData[]    = {
        tsmask->toInt(),
        tsmask->toInt(),
        tsmask->toInt(),
        tsmask->toInt()
    };

    glTexCoordPointer(2, GL_INT, 0, texcoordData);
    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cache[oldest]);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);

}

void TS_TTFFont::drawZoomedText(int x, int y, float s, const char *text){
	TS_Directories *TS_dirs = GetDirs();
	//TS_ShowSurface_Clear(TTF_RenderText_Solid(TTF_OpenFont(string(TS_dirs->font).append(fontname).c_str(), static_cast<int>(s*size)),text,mask), x,y);
}

int TS_TTFFont::getStringWidth(const char *text){
 return 0;
}

TS_TTFFont::TS_TTFFont(const char *f, int s){
	printf("%s\n", f);
	//const char *f2 = "DejaVuSans.ttf";
	//f = f2;
	fontname = f;
    SDL_RWops *fontfile = SDL_RWFromFile(f, "r");
    if(!fontfile){
        printf("\nerror opening file %s\n", f);
        printf("%s\n", SDL_GetError());
    }
    font=TTF_OpenFont(f, 10);
    if(!font) {
        printf("TTF_OpenFontRW: %s\n", TTF_GetError());
    }
    size = s;
    tsmask = new TS_Color(0xFF, 0xFF, 0xFF, 0xFF);

    cache        = (GLuint*)calloc(MAX_CACHED_STRINGS, sizeof(GLuint));
    cacheattribs = (TS_GlyphAttribs*)calloc(MAX_CACHED_STRINGS, sizeof(TS_GlyphAttribs));
    strings      = (const char **)calloc(MAX_CACHED_STRINGS, sizeof(const char*));
}

TS_TTFFont::TS_TTFFont(const char *f){
	TS_Directories *TS_dirs = GetDirs();
    printf(f);
    fontname = f;
    font = TTF_OpenFont(string(TS_dirs->font).append(f).c_str(), 10);
    size = 10;

    tsmask = new TS_Color(0xFF, 0xFF, 0xFF, 0xFF);

    cache        = (GLuint*)calloc(MAX_CACHED_STRINGS, sizeof(GLuint));
    cacheattribs = (TS_GlyphAttribs*)calloc(MAX_CACHED_STRINGS, sizeof(TS_GlyphAttribs));
    strings      = (const char **)calloc(MAX_CACHED_STRINGS, sizeof(const char*));
}

v8Function LoadTTFFont(V8ARGS){
    if(args.Length()<1){
        THROWERROR(BRACKNAME " LoadTTFFont Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);

    int size = 10;

    if(args.Length()>1){
		CHECK_ARG_INT(1);
        size = args[1]->Int32Value();
    }
	TS_Directories *TS_dirs = GetDirs();
    BEGIN_OBJECT_WRAP_CODE;
    TS_TTFFont *font = NULL;
    v8::String::AsciiValue str(args[0]);
    const char *fontname = *str;
    font = new TS_TTFFont(string(TS_dirs->font).append(fontname).c_str(), size);

    if(!font){
        THROWERROR(string(BRACKNAME "LoadTTFFont Error: Could not open font ").append(fontname).c_str());
    }

    END_OBJECT_WRAP_CODE(TTFFont, font);

}



TS_TTFFont::~TS_TTFFont(){
    TTF_CloseFont(font);
	free(cacheattribs);
	for(int i = 0; i<MAX_CACHED_STRINGS; i++){
        if(cacheattribs[i].real){
            glDeleteTextures(1, &(cache[i]));
        }
        if(strings[i]!=NULL){
            free((void*)strings[i]);
        }
	}
	free(cache);
	free(cacheattribs);
	free(strings);
}


v8::Handle<v8::Value> GetFontname(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    const char *value = static_cast<TS_TTFFont*>(ptr)->fontname;
    return v8::String::New(value);
}

void SetFontname(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
}

v8::Handle<v8::Value> GetFontMask(v8::Local<v8::String> property, const v8::AccessorInfo &info){
return v8::Undefined();
}

void SetFontMask(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
   // void* ptr = wrap->Value();
    self.Clear();
    wrap.Clear();
    //TS_Color* c = (TS_Color*)value->ToObject()->GetPointerFromInternalField(0);
    //SDL_Color color = {c->red, c->green,c->blue,c->alpha};
    //static_cast<TS_TTFFont*>(ptr)->mask = color;
}
/*
    const v8::AccessorInfo& info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<TS_Color*>(ptr)->blue = value->Int32Value();
*/
v8::Handle<v8::Value> GetFontsize(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    int value = static_cast<TS_TTFFont*>(ptr)->size;
    self.Clear();
    wrap.Clear();
    return v8::Integer::New(value);
}

void SetFontsize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    if(value->IsNumber()){
        self.Clear();
        wrap.Clear();
        static_cast<TS_TTFFont*>(ptr)->size = value->Int32Value();
    }
}

v8Function TS_TTFdrawText(const v8::Arguments& args) {
	if(args.Length()<3){
        THROWERROR("TS_drawText Error: called with less than 3 parameters!");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);

	int x = args[0]->v8::Value::IntegerValue();
	int y = args[1]->v8::Value::IntegerValue();
	v8::String::Utf8Value str(args[2]);

	GET_SELF(TS_TTFFont*)->drawText(x,y,(const char*)(*str));

	return v8::Undefined();
}

v8::Handle<v8::Value> TS_TTFgetStringWidth(const v8::Arguments& args)
{
	if(args.Length()<1){
		THROWERROR("TS_TTFgetStringWidth Error: Less than 1 argument.");
	}
	CHECK_ARG_STR(0);
	v8::String::Utf8Value str(args[0]);
	return v8::Number::New(GET_SELF(TS_TTFFont*)->getStringWidth((const char*)(*str)));

}


v8::Handle<v8::Value> TS_TTFdrawZoomedText(const v8::Arguments& args)
{
	if(args.Length()<4)
	{
		THROWERROR("TS_TTFdrawZoomedText Error: Less than 4 arguments.");
	}

	CHECK_ARG_INT(0);
	CHECK_ARG_INT(1);
	CHECK_ARG_INT(2);
	CHECK_ARG_STR(3);

	int x = (int)args[0]->v8::Value::IntegerValue();
	int y = (int)args[1]->v8::Value::IntegerValue();
	float s = (float)args[2]->v8::Value::NumberValue();
	v8::String::Utf8Value str(args[3]);
	GET_SELF(TS_TTFFont*)->drawZoomedText(x,y,s,(const char*)(*str));

	return v8::Undefined();
}
