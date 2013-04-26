#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include "../graphicSDL/graphic.h"
#include "../../configmanager/opengame.h"
#define TTFFONT_INTERNAL
#include "ttffont.h"
#define NO_BMP_FONT_DECL
#include "../common/font.h"
#undef  NO_BMP_FONT_DECL

#define NUMFUNCS 1
#define NUMVARS 0
#define NUMCATS 3

using namespace std;

v8Function LoadTTFFont(V8ARGS);
void * LoadTTFFontPointer          = V8FUNCPOINTER(LoadTTFFont);

	v8::Persistent<v8::FunctionTemplate> Fonttempl;
	v8::Persistent<v8::ObjectTemplate> FontInsttempl;
    v8::Persistent<v8::ObjectTemplate> Fontproto;//    = *Fonttempl->PrototypeTemplate();

initFunction Init(void){
	if(!TTF_WasInit()){
		if(TTF_Init()==-1){
			printf("TTFFont Init Error: %s\n", TTF_GetError());
		}
    }
	
	Fonttempl       = *v8::FunctionTemplate::New();
	FontInsttempl   = *Fonttempl->InstanceTemplate();
    Fontproto       = *Fonttempl->PrototypeTemplate();
    Fontproto->Set("drawText", v8::FunctionTemplate::New(TS_TTFdrawText));
	Fontproto->Set("drawZoomedText", v8::FunctionTemplate::New(TS_TTFdrawZoomedText));
	Fontproto->Set("getStringWidth", v8::FunctionTemplate::New(TS_TTFgetStringWidth));

	return (initFunction)"ttffontSDL";
}

int GetNumCategories(){
	return NUMCATS;
}

nameArray GetCategories(){
	nameArray cats = (nameArray)calloc((NUMCATS*2), sizeof(TS_name));
	cats[0] = CATEGORY_REQUIRES;
	cats[1] = CATEGORY_SDL_12;

	cats[2] = CATEGORY_REQUIRES;
	cats[3] = CATEGORY_GRAPHIC;
	
	cats[4] = CATEGORY_IS;
	cats[5] = CATEGORY_FONT;
	return cats;
}

void Close(){
    Fonttempl.Dispose();
	FontInsttempl.Dispose();
    Fontproto.Dispose();
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

void TS_TTFFontFinalizer(v8::Persistent<v8::Value> object, void* parameter) {
    TS_TTFFont* font = (TS_TTFFont*)parameter;
    delete font;
    object.Dispose();
}

void TS_TTFFont::drawText(int x, int y, const char *text){
	SDL_Surface *surface = TTF_RenderText_Solid(font,text,mask);
	TS_ShowSurface_Clear(surface, x,y);
}

void TS_TTFFont::drawZoomedText(int x, int y, float s, const char *text){
	TS_Directories *TS_dirs = GetDirs();
	TS_ShowSurface_Clear(TTF_RenderText_Solid(TTF_OpenFont(string(TS_dirs->font).append(fontname).c_str(), static_cast<int>(s*size)),text,mask), x,y);
}

int TS_TTFFont::getStringWidth(const char *text){
		text_surface=SDL_DisplayFormat(TTF_RenderText_Solid(font,text,mask));
		int s = text_surface->w;
		SDL_FreeSurface(text_surface);
		return s;
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
    SDL_Color color = {255,255,255,128};
    mask = color;
}

TS_TTFFont::TS_TTFFont(const char *f){
	TS_Directories *TS_dirs = GetDirs();
    printf(f);
    fontname = f;
    font = TTF_OpenFont(string(TS_dirs->font).append(f).c_str(), 10);
    size = 10;
    SDL_Color color = {255,255,255,128};
    mask = color;
}

v8::Handle<v8::Value> LoadTTFFont(const v8::Arguments& args) {
    if(args.Length()<1){
        return v8::ThrowException(v8::String::New("TS_LoadTTFFont Error: Called with no arguments."));
    }
    CHECK_ARG_STR(0, "TS_LoadTTFFont Error: Arg 0 is not a string.");
    if(args.Length()>1){
		CHECK_ARG_INT(1, "TS_LoadTTFFont Error: Arg 1 is not a number.");
    }
	TS_Directories *TS_dirs = GetDirs();
    v8::HandleScope loadfontscope;
	v8::Handle<v8::Value> external;
    TS_TTFFont *font = NULL;
    if (args[0]->IsExternal()) {
        external = v8::Local<v8::External>::Cast(args[0]);
    }
    else {
        int size = 10;
        v8::String::AsciiValue str(args[0]);
        const char *fontname = *str;
        if(!args[1]->IsUndefined()){
            size = args[1]->Int32Value();
        }
        font = new TS_TTFFont(string(TS_dirs->font).append(fontname).c_str(), size);
    	
		if(!font){
			THROWERROR(string("LoadTTFFont Error: Could not open font ").append(fontname).c_str());
		}

		external = v8::External::New(font);
    }



  	FontInsttempl->SetInternalFieldCount(1);
	v8::Handle<v8::Function> Fontctor = Fonttempl->GetFunction();
		//get class instance
	v8::Local<v8::Object> Fontobj = Fontctor->NewInstance();
		//Bridge the JS class seeds by setting the fields

    v8::Persistent<v8::Object> Pfontobj = v8::Persistent<v8::Object>::New(Fontobj);//colorctor->NewInstance();
    Fontobj.Clear();
    Pfontobj.MakeWeak(font, TS_TTFFontFinalizer);
	Pfontobj->SetInternalField(0, external);
    external.Clear();
    return loadfontscope.Close(Pfontobj);
}



TS_TTFFont::~TS_TTFFont(){
    TTF_CloseFont(font);
	SDL_FreeSurface(text_surface);
}


v8::Handle<v8::Value> GetFontname(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    const char *value = static_cast<TS_TTFFont*>(ptr)->fontname;
    return v8::String::New(value);
}

void SetFontname(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
				//READ ONLY!
				//Function required by V8 for both setters and getters.
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

v8::Handle<v8::Value> TS_TTFdrawText(const v8::Arguments& args) {
	if(args.Length()<3){
        THROWERROR("TS_drawText Error: called with less than 3 parameters!");
    }
    CHECK_ARG_INT(0, "TS_drawText Error: could not parse arg 0 to number.")
    CHECK_ARG_INT(1, "TS_drawText Error: could not parse arg 1 to number.")

	int x = args[0]->v8::Value::IntegerValue();
	int y = args[1]->v8::Value::IntegerValue();
	v8::String::Utf8Value str(args[2]);
	v8::Local<v8::Object> self = args.Holder();

	static_cast<TS_TTFFont*>(v8::Local<v8::External>::Cast(self->GetInternalField(0))->Value())->drawText(x,y,(const char*)(*str));
    //delete font;
    self.Clear();
    //wrap.Clear();

	return v8::Undefined();
}

v8::Handle<v8::Value> TS_TTFgetStringWidth(const v8::Arguments& args)
{
	if(args.Length()<1){
		THROWERROR("TS_TTFgetStringWidth Error: Less than 1 argument.");
	}
	CHECK_ARG_STR(0, "TS_TTFgetStringWidth Error: Argument 0 is not a string.");
	v8::String::Utf8Value str(args[0]);
	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	return v8::Number::New(static_cast<TS_TTFFont*>(ptr)->getStringWidth((const char*)(*str)));

}


v8::Handle<v8::Value> TS_TTFdrawZoomedText(const v8::Arguments& args)
{
	if(args.Length()<4)
	{
		THROWERROR("TS_TTFdrawZoomedText Error: Less than 4 arguments.");
	}
	
	CHECK_ARG_INT(0, "TS_TTFdrawZoomedText Error: Argument 0 is not a number.");
	CHECK_ARG_INT(1, "TS_TTFdrawZoomedText Error: Argument 1 is not a number.");
	CHECK_ARG_INT(2, "TS_TTFdrawZoomedText Error: Argument 2 is not a number.");
	CHECK_ARG_STR(3, "TS_TTFdrawZoomedText Error: Argument 3 is not a string.");

	int x = (int)args[0]->v8::Value::IntegerValue();
	int y = (int)args[1]->v8::Value::IntegerValue();
	float s = (float)args[2]->v8::Value::NumberValue();
	v8::String::Utf8Value str(args[3]);
	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<TS_TTFFont*>(ptr)->drawZoomedText(x,y,s,(const char*)(*str));

	return v8::Undefined();
}
