#define NO_TTF_FONT_DECL

#include <iostream>
#include <fstream>
#include <string>
#define BMPFONT_INTERNAL
#include "bmpfont.h"
#include "../../configmanager/opengame.h"
#include "../graphicSDL/graphic.h"
#include "../common/graphic_common.h"
#include "../common/graphic_algorithm.h"
#include "../common/font.h"

#define NUMFUNCS 2
#define NUMVARS 0

using namespace std;

	v8Constructor       BMPFonttempl;
	v8InstanceTemplate  BMPFontInsttempl;
    v8PrototypeTemplate BMPFontproto;

v8Function LoadBMPFont(V8ARGS);
v8Function LoadSystemBMPFont(V8ARGS);

void * LoadBMPFontPointer          = V8FUNCPOINTER(LoadBMPFont);
void * LoadSystemBMPFontPointer    = V8FUNCPOINTER(LoadSystemBMPFont);

int numerate(bool reset){
    static int i = 0;
    if(reset) {
        i = 0;
        return 0;
    }
    i++;
    return i-1;
}

initFunction Init(void){
    BMPFonttempl    = *v8::FunctionTemplate::New();
	BMPFontInsttempl= *BMPFonttempl->InstanceTemplate();
    BMPFontproto    = *BMPFonttempl->PrototypeTemplate();
    BMPFontproto->Set("drawText", v8::FunctionTemplate::New(TS_BMPdrawText));
    BMPFontproto->Set("getStringWidth", v8::FunctionTemplate::New(TS_BMPgetStringWidth));

	return (char *)"bmpfontSDL";
}

void Close(){
    BMPFontClose();
}

int GetNumFunctions(){
    return NUMFUNCS;
}

functionArray GetFunctions(){
    numerate(true);
    functionArray funcs = (functionArray)calloc(GetNumFunctions(), sizeof(void*));
    funcs[numerate(false)] = LoadBMPFontPointer;
    funcs[numerate(false)] = LoadSystemBMPFontPointer;
    return funcs;
}

nameArray GetFunctionNames(){
    numerate(true);
    nameArray names = (nameArray)calloc(GetNumFunctions(), sizeof(functionName));
    names[numerate(false)] = (functionName)"Font";
    names[numerate(false)] = (functionName)"GetSystemFont";
    return names;
}

int GetNumVariables(){
    return 0;
}

void ** GetVariables(void){
    return NULL;
}

nameArray GetVariableNames(void){
    return NULL;
}

void BMPFontClose(){
    BMPFonttempl.Clear();
	BMPFontInsttempl.Clear();
    BMPFontproto.Clear();
    BMPFonttempl.Dispose();
	BMPFontInsttempl.Dispose();
    BMPFontproto.Dispose();
}


void TS_BMPFontFinalizer(v8::Persistent<v8::Value> object, void* parameter) {
    TS_BMPFont* font = (TS_BMPFont*)parameter;
    delete font;
    object.Dispose();
}


TS_BMPFont::TS_BMPFont(const char* file){
    //mask = {255,255,255,255};
	SDL_RWops *fontfile;
	fontfile = SDL_RWFromFile(file, "rb");
    if(!fontfile){
		printf("Could not open file %s\n", file);   //the file should be checked BEFORE passing it to the constructor!
	}
    unsigned short version[1];
	unsigned short numchars[1];
	unsigned char sig[5]    = "    ";
    unsigned char compressed[1];// = {CTRLCHAR};

	//sig
    SDL_RWread(fontfile, sig, 1,4);

    //version
    SDL_RWread(fontfile, version, 2,1);

    //num_chars
    SDL_RWread(fontfile, numchars, 2,1);

    //ideally...
    SDL_RWread(fontfile, compressed, 1,1);

    printf("Font was compressed: %i\n", *compressed);

	SDL_RWseek(fontfile, 256, SEEK_SET);
    glyphs.reserve(numchars[0]);
    //printf("Opening rfn font  %s\n", file);
    if(version[0]==1){

    }
    if(version[0]==2){
        if(*compressed==0){
            for(unsigned short i = 0; i<numchars[0]; i++){
                unsigned short   width[1];
                unsigned short  height[1];
                SDL_RWread(fontfile,  width, 2, 1);
                SDL_RWread(fontfile, height, 2, 1);
                SDL_RWseek(fontfile, 28, SEEK_CUR);

                glyphs.push_back(SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, *width, *height, DEPTH, CHANNEL_MASKS));//SDL_ConvertSurface(tempglyph, tempglyph->format, tempglyph->flags));

                SDL_RWread(fontfile, glyphs.at(i)->pixels, BPP,(*width)*(*height));
            }
        }
        else if(*compressed==1){
            /*
            for(unsigned short i = 0; i<numchars[0]; i++){
                unsigned short size[1];

                SDL_RWread(fontfile, size, 2, 1);

                unsigned short   width[1];
                unsigned short  height[1];
                SDL_RWread(fontfile,  width, 2, 1);
                SDL_RWread(fontfile, height, 2, 1);

                const unsigned short compsize = *size;

                const unsigned short decompsize = (*width)*(*height)*BPP;

                char data[compsize];

                SDL_RWread(fontfile, data, compsize, 1);

                glyphs.push_back(SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, *width, *height, DEPTH, CHANNEL_MASKS));//SDL_ConvertSurface(tempglyph, tempglyph->format, tempglyph->flags));

                z_stream_s zstr;

                zstr.next_in = (Bytef*)data;
                zstr.avail_in = compsize;
                zstr.next_out= (Bytef*)glyphs.at(i)->pixels;
                zstr.avail_out= decompsize;

                inflate(&zstr, Z_FINISH);
            }
*/
        }
    SDL_RWclose(fontfile);
    }

}


TS_BMPFont::~TS_BMPFont(){
    for(unsigned int i = 0; i<glyphs.size(); i++){
        SDL_FreeSurface(glyphs.at(i));
    }
}

v8Function LoadBMPFont(const v8::Arguments& args) {

	TS_Directories *TS_dirs = GetDirs();

    if(args.Length()<1){
        return v8::ThrowException(v8::String::New("TS_LoadBMPFont Error: Called with no arguments."));
    }
    CHECK_ARG_STR(0, "TS_LoadBMPFont Error: Arg 0 is not a string.");
    v8::HandleScope loadBMPfontscope;
	v8::Handle<v8::Value> external;
    TS_BMPFont *font = NULL;
    if (args[0]->IsExternal()) {
        external = v8::Local<v8::External>::Cast(args[0]);
    }
    else{
        v8::String::Utf8Value str(args[0]);
        const char *fontname = *str;

        SDL_RWops *fonttest = SDL_RWFromFile(string(TS_dirs->font).append(fontname).c_str(), "rb");
        if(!fonttest){
            SDL_RWclose(fonttest);
            printf("TS: Could not open rfn file %s\n", fontname);
            return v8::ThrowException(v8::String::New(string("TS_LoadFont Error: Could not load font ").append(fontname).c_str()));
        }
        SDL_RWclose(fonttest);

        font = new TS_BMPFont(string(TS_dirs->font).append(fontname).c_str());

        external = v8::External::New(font);
    }

  	BMPFontInsttempl->SetInternalFieldCount(1);
	v8::Local<v8::Function> BMPFontctor = BMPFonttempl->GetFunction();
	v8::Local<v8::Object> BMPFontobj = BMPFontctor->NewInstance();
    v8::Persistent<v8::Object> BMPPfontobj = v8::Persistent<v8::Object>::New(BMPFontobj);
    BMPPfontobj.MakeWeak(font, TS_BMPFontFinalizer);
	BMPPfontobj->SetInternalField(0, external);
    return loadBMPfontscope.Close(BMPPfontobj);
}


v8Function LoadSystemBMPFont(V8ARGS) {

	TS_Config *TS_conf = GetConfig();	
	TS_Directories *TS_dirs = GetDirs();

    v8::HandleScope loadBMPfontscope;
	v8::Handle<v8::Value> external;
    SDL_RWops *fonttest = SDL_RWFromFile(string(TS_dirs->system).append(TS_conf->systemfont).c_str(), "rb");
    if(!fonttest){
        SDL_RWclose(fonttest);
        return v8::ThrowException(v8::String::New(string("TS_LoadSystemBMPFont Error: Could not open rfn file ").append(string(TS_dirs->system).append(TS_conf->systemfont)).c_str()));
    }
    TS_BMPFont *font = new TS_BMPFont(string(TS_dirs->system).append(TS_conf->systemfont).c_str());
    external = v8::External::New(font);

  	BMPFontInsttempl->SetInternalFieldCount(1);
	v8::Local<v8::Function> BMPFontctor = BMPFonttempl->GetFunction();
	v8::Local<v8::Object> BMPFontobj = BMPFontctor->NewInstance();
    v8::Persistent<v8::Object> BMPPfontobj = v8::Persistent<v8::Object>::New(BMPFontobj);
    BMPPfontobj.MakeWeak(font, TS_BMPFontFinalizer);
	BMPPfontobj->SetInternalField(0, external);
    return loadBMPfontscope.Close(BMPPfontobj);
}

void TS_BMPFont::drawText(int x, int y, const char *t){
    int curWidth = 0;
    int glyphnum = 0;
    std::string text = t;
    for (size_t i=0; i < text.length(); i++){
        glyphnum = int(text.at(i));
        TS_ShowSurface(glyphs.at(glyphnum), x+curWidth, y);
        curWidth+=glyphs.at(glyphnum)->w;
        //
    }

}

void TS_BMPFont::drawZoomedText(int x, int y, float f, const char* t){
    float curWidth = 0;
    int glyphnum = 0;
    std::string text = t;
    for (size_t i=0; i < text.length(); i++){
        glyphnum = int(text.at(i));
        TS_StretchShowSurface(glyphs.at(glyphnum), x+curWidth, y, f, f);
        curWidth+=(glyphs.at(glyphnum)->w)*f;
        //
    }

}

int TS_BMPFont::getStringWidth(const char *t){
    int curWidth = 0;
    int glyphnum;
    std::string text = t;
    for (size_t i=0; i < text.length(); i++){
        glyphnum = int(text.at(i));
        curWidth+=glyphs.at(glyphnum)->w;
    }
    return curWidth;

}

v8Function TS_BMPgetStringWidth(V8ARGS)
{
	v8::String::Utf8Value str(args[2]);
	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	return v8::Number::New(static_cast<TS_BMPFont*>(ptr)->getStringWidth((const char*)(*str)));

}


v8Function TS_BMPdrawText(V8ARGS) {


	if(args.Length()<3){
        return v8::ThrowException(v8::String::New("TS_BMPdrawText Error: called with less than 3 parameters!"));
    }
    CHECK_ARG_INT(0, "TS_BMPdrawText Error: could not parse arg 0 to number.")
    CHECK_ARG_INT(1, "TS_BMPdrawText Error: could not parse arg 1 to number.")

	int x = static_cast<int>(args[0]->NumberValue());
	int y = static_cast<int>(args[1]->NumberValue());
	v8::String::Utf8Value str(args[2]);
	//const char * z = args[2]->AsciiValue();
	v8::Local<v8::Object> self = args.Holder();
	//v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));

	static_cast<TS_BMPFont*>(v8::Local<v8::External>::Cast(self->GetInternalField(0))->Value())->drawText(x,y,(const char*)(*str));
    //delete font;
    self.Clear();
    //wrap.Clear();

	return v8::Undefined();
}
