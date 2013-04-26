#define NO_TTF_FONT_DECL

#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <cmath>

#define BMPFONT_INTERNAL
#define PLUGINNAME "bmpfontSDL"
#include "bmpfont.h"
#include "../../configmanager/opengame.h"
#include "../graphicSDL/graphic.h"
#include "../graphicSDL/surface.h"
#include "../common/graphic_common.h"
#include "../common/graphic_algorithm.h"
#include "../common/font.h"

#define NUMFUNCS 2
#define NUMVARS 0

#ifdef _WIN32

#define STRDUP _strdup
#else

#include <cstring>
#define STRDUP strdup
#endif

using namespace std;

	v8Constructor       BMPFonttempl;
	v8InstanceTemplate  BMPFontInsttempl;
    v8PrototypeTemplate BMPFontproto;
v8Function TS_BMPdrawTextBox(V8ARGS);
v8Function TS_BMPwordWrapString(V8ARGS);
v8Function TS_BMPgetStringWidth(V8ARGS);
v8Function TS_BMPgetHeight(V8ARGS);
v8Function TS_BMPgetStringHeight(V8ARGS);
v8Function TS_BMPdrawText(V8ARGS);
v8Function LoadBMPFont(V8ARGS);
v8Function LoadSystemBMPFont(V8ARGS);

void * LoadBMPFontPointer          = V8FUNCPOINTER(LoadBMPFont);
void * LoadSystemBMPFontPointer    = V8FUNCPOINTER(LoadSystemBMPFont);

int numerate(bool reset) {
    static int i = 0;
    if(reset) {
        i = 0;
        return 0;
    }
    i++;
    return i-1;
}

initFunction Init(void) {
    //TS_TEST("\"Words!\"");
    BMPFonttempl    = *v8::FunctionTemplate::New();
	BMPFontInsttempl= *BMPFonttempl->InstanceTemplate();
    BMPFontproto    = *BMPFonttempl->PrototypeTemplate();
    BMPFontproto->Set("drawText",        v8::FunctionTemplate::New(TS_BMPdrawText));
    BMPFontproto->Set("getStringWidth",  v8::FunctionTemplate::New(TS_BMPgetStringWidth));
    BMPFontproto->Set("getStringHeight", v8::FunctionTemplate::New(TS_BMPgetStringHeight));
    BMPFontproto->Set("getHeight",       v8::FunctionTemplate::New(TS_BMPgetHeight));
    BMPFontproto->Set("drawTextBox",     v8::FunctionTemplate::New(TS_BMPdrawTextBox));
    BMPFontproto->Set("wordWrapString",  v8::FunctionTemplate::New(TS_BMPwordWrapString));
//	TS_addToSurfaceProto("drawText",  v8::FunctionTemplate::New(TS_BMPdrawTextSurface));
	return (char *)"bmpfontSDL";
}

void Close() {
    BMPFontClose();
}

int GetNumFunctions() {
    return NUMFUNCS;
}

functionArray GetFunctions() {
    numerate(true);
    functionArray funcs = (functionArray)calloc(GetNumFunctions(), sizeof(void*));
    funcs[numerate(false)] = LoadBMPFontPointer;
    funcs[numerate(false)] = LoadSystemBMPFontPointer;
    return funcs;
}

nameArray GetFunctionNames() {
    numerate(true);
    nameArray names = (nameArray)calloc(GetNumFunctions(), sizeof(functionName));
    names[numerate(false)] = (functionName)"Font";
    names[numerate(false)] = (functionName)"GetSystemFont";
    return names;
}

int GetNumVariables() {
    return 0;
}

void ** GetVariables(void) {
    return NULL;
}

nameArray GetVariableNames(void) {
    return NULL;
}

void BMPFontClose() {
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


TS_BMPFont::TS_BMPFont(const char* file) {
    //mask = {255,255,255,255};
	SDL_RWops *fontfile;
	inheight = 0;
	fontfile = SDL_RWFromFile(file, "rb");
    if(!fontfile){
		printf("[bmpfontSDL] Error: Could not open file %s\n", file);   //the file should be checked BEFORE passing it to the constructor!
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

    printf("[bmpfontSDL] info: Font was compressed: %i\n", *compressed);

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

                if(*height>inheight){inheight = *height;}

                SDL_RWseek(fontfile, 28, SEEK_CUR);

                glyphs.push_back(SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, *width, *height, DEPTH, CHANNEL_MASKS));//SDL_ConvertSurface(tempglyph, tempglyph->format, tempglyph->flags));

                SDL_RWread(fontfile, glyphs.at(i)->pixels, BPP,(*width)*(*height));
            }
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
        return v8::ThrowException(v8::String::New("[bmpfontSDL] TS_LoadBMPFont Error: Called with no arguments."));
    }
    CHECK_ARG_STR(0);
    v8::HandleScope loadBMPfontscope;
	v8::Handle<v8::Value> external;
    //TS_BMPFont *font = NULL;
    v8::String::Utf8Value str(args[0]);
    const char *fontname = *str;

    SDL_RWops *fonttest = SDL_RWFromFile(string(TS_dirs->font).append(fontname).c_str(), "rb");
    if(!fonttest){
        SDL_RWclose(fonttest);
        //printf("[bmpfontSDL] TS_LoadBMPFont Error: Could not open rfn file %s\n", fontname);
        return v8::ThrowException(v8::String::New(string("[bmpfontSDL] TS_LoadFont Error: Could not load font ").append(fontname).c_str()));
    }
    SDL_RWclose(fonttest);

    TS_BMPFont *font MEMALIGN(4) = new TS_BMPFont(string(TS_dirs->font).append(fontname).c_str());

    external = v8::External::New(font);


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
        return v8::ThrowException(v8::String::New(string("[bmpfontSDL] TS_LoadSystemBMPFont Error: Could not open rfn file ").append(string(TS_dirs->system).append(TS_conf->systemfont)).c_str()));
    }
    TS_BMPFont *font MEMALIGN(8) = new TS_BMPFont(string(TS_dirs->system).append(TS_conf->systemfont).c_str());
    external = v8::External::New(font);
    printf("System Font is at address %p.\n", &font);
  	BMPFontInsttempl->SetInternalFieldCount(1);
	v8::Local<v8::Function> BMPFontctor = BMPFonttempl->GetFunction();
	v8::Local<v8::Object> BMPFontobj = BMPFontctor->NewInstance();
    v8::Persistent<v8::Object> BMPPfontobj = v8::Persistent<v8::Object>::New(BMPFontobj);
    BMPPfontobj.MakeWeak(font, TS_BMPFontFinalizer);
	BMPPfontobj->SetInternalField(0, external);
    return loadBMPfontscope.Close(BMPPfontobj);
}

void TS_BMPFont::drawText(int x, int y, const char *t) {
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

void TS_BMPFont::drawZoomedText(int x, int y, float f, const char* t) {
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

int TS_BMPFont::getStringWidth(const char *t) const{
    int curWidth = 0;
    int glyphnum;
    std::string text(t);
    for (size_t i=0; i < text.length(); i++){
        glyphnum = int(text.at(i));
        curWidth+=glyphs.at(glyphnum)->w;
    }
    return curWidth;
}

int TS_BMPFont::getStringHeight(const char *t) {
    int curWidth = 0;
    int maxHeight= 0;
    int glyphnum;
    std::string text = t;
    for (size_t i=0; i < text.length(); i++){
        glyphnum = int(text.at(i));
        curWidth+=glyphs.at(glyphnum)->w;
        if(glyphs.at(glyphnum)->h>maxHeight){
            maxHeight=glyphs.at(glyphnum)->h;
        }
    }
    return curWidth;
}

int TS_BMPFont::getHeight() {
    return inheight;
}

inline void TS_BMPFont::addline(const char **textlines, int *readylines, char *linebuffer) const{
    (*readylines)++;
    textlines = (const char **)realloc(textlines, (*readylines)*sizeof(const char *));
    textlines[(*readylines)-1] = STRDUP(linebuffer+(((linebuffer[0]==' ')||(linebuffer[0]=='\n'))?1:0));
    linebuffer = (char *)realloc(linebuffer, 1);
    *linebuffer = '\0';
}

const char **TS_BMPFont::wordWrapString(const char *t, int w, int* num) {

    intptr_t startOfWord    = (intptr_t)t; //NUMBER OF CHARACTERS
    const char **textlines  = (const char **)calloc(0, sizeof(const char *));
    int readylines          = 0;
    int lineWidth           = 0; //PIXEL WIDTH!
    int tlength             = strlen(t);
    char *linebuffer        = (char *)calloc(1, sizeof(char));
    //*linebuffer = '\0';

    for(int i = 0; i<tlength+1; i++){
        if((t[i]==' ')||(t[i]=='\n')||(i==tlength)){

            const int wordlen = (((intptr_t)t)+i)-startOfWord; //NUMBER OF CHARACTERS
            const char *tword = strncpy((char *)calloc(wordlen, sizeof(char)), (const char *)startOfWord, wordlen);
            const int twwidth = this->getStringWidth(tword);
            if(lineWidth+twwidth>w){
                this->addline(textlines, &readylines, linebuffer);
                lineWidth=twwidth;
                linebuffer = strcat(linebuffer, tword);
            }
            else{
                linebuffer = strcat(linebuffer, tword);
                linebuffer = (char *)realloc(linebuffer, (strlen(linebuffer)+wordlen+2)*sizeof(char)); //two extra bytes to handle if '\n' and ' ' in sequence.
                lineWidth+=twwidth;
                if(t[i]=='\n'){
                    this->addline(textlines, &readylines, linebuffer);
                    lineWidth = 0;
                }
            }

            /*
            bool newline = false;
            int wordlen = (((intptr_t)t)+i)-startOfWord; //NUMBER OF CHARACTERS
            const char *tword = strncpy((char *)calloc(wordlen, sizeof(char)), (const char *)startOfWord, wordlen);

            if(lineWidth+this->getStringWidth(tword)>w){
                this->addline(textlines, &readylines, linebuffer);
                lineWidth=this->getStringWidth(tword);
                newline   = true;
            }
            else{
                lineWidth+=this->getStringWidth(tword);
            }
            linebuffer = strcat(linebuffer, tword);
            linebuffer = (char *)realloc(linebuffer, (strlen(linebuffer)+wordlen)*sizeof(char));
            if((t[i]=='\n')&&(!newline)){
                this->addline(textlines, &readylines, linebuffer);
                lineWidth = 0;
            }
            /*
            else if(false&&i==tlength-1){
                if(lineWidth+this->getStringWidth((const char *)((intptr_t)t+i))<=w){
                    linebuffer = strcat(linebuffer, (const char *)((intptr_t)t+i));
                }
                else{
                    char * templines = STRDUP((const char *)((intptr_t)t+i));
                    this->addline(textlines, &readylines, templines);
                    free(templines);
                }
            }
            */

            free((void*)tword);
            startOfWord = ((intptr_t)t)+i;
        }
    }
    (readylines)++;
    textlines = (const char **)realloc(textlines, (readylines)*sizeof(const char *));
    textlines[(readylines)-1] = strdup(linebuffer+(((linebuffer[0]==' ')||(linebuffer[0]=='\n'))?1:0));
    free(linebuffer);
    (*num) = readylines;

    return textlines;
}

//                 0  1  2      3       4         5
//Font.drawTextBox(x, y, width, height, y_offset, text): Draw wrapped text, shifted up/down by y_offset, in the box (x, y, width, height).
v8Function TS_BMPdrawTextBox(V8ARGS) {
    if(args.Length()<2){
        THROWERROR("[bmpfontSDL] TS_BMPwordWrapString Error: Called with less than 2 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);
    CHECK_ARG_INT(4);
    CHECK_ARG_STR(5);
    if(args[2]->IntegerValue()<0){
        THROWERROR_RANGE("[bmpfontSDL] TS_BMPdrawTextBox Error: Argument 2 is negative.");
    }
    if(args[3]->IntegerValue()<0){
        THROWERROR_RANGE("[bmpfontSDL] TS_BMPdrawTextBox Error: Argument 3 is negative.");
    }

    v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();

    int x = args[0]->Int32Value();
    int y = args[1]->Int32Value();
    int w = args[2]->Int32Value();
    int h = args[3]->Int32Value();
    int y_offset = args[4]->Int32Value();
	v8::String::Utf8Value str(args[5]);
	int numlines = 1;

	if(str.length()==0){
        return v8::Undefined();
	}
	if(static_cast<TS_BMPFont*>(ptr)->getStringWidth((const char*)(*str))<=w){
        static_cast<TS_BMPFont*>(ptr)->drawText(x, y+y_offset, (const char*)(*str));
        return v8::Undefined();
	}



	const char ** lines = static_cast<TS_BMPFont*>(ptr)->wordWrapString((const char*)(*str), w, &numlines);
    int inheight = static_cast<TS_BMPFont*>(ptr)->getHeight();
    for(int i = 0; i<numlines; i++){
        static_cast<TS_BMPFont*>(ptr)->drawText(x, y+y_offset+(i*inheight), lines[i]);
        free((void*)lines[i]);
        if((y+y_offset+((i+1)*inheight))>h) {break;}
    }
    free(lines);
    return v8::Undefined();
}

v8Function TS_BMPwordWrapString(V8ARGS) {
    v8::HandleScope wordWrapStringscope;
    if(args.Length()<2){
        THROWERROR("[bmpfontSDL] TS_BMPwordWrapString Error: Called with less than 2 arguments.");
    }
    CHECK_ARG_STR(0);
    CHECK_ARG_INT(1);
    if(args[1]->IntegerValue()<0){
        THROWERROR_RANGE("[bmpfontSDL] TS_BMPwordWrapString Error: Argument 1 is negative.");
    }
	v8::String::Utf8Value str(args[0]);
	int w = args[1]->Int32Value();
	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int numlines = 1;
	const char ** lines = static_cast<TS_BMPFont*>(ptr)->wordWrapString((const char*)(*str), w, &numlines);

    v8::Local<v8::Object> linearray = v8::Array::New(numlines);

    for(uint32_t i = 0; i<numlines; i++){
        linearray->Set(i, v8::String::New(lines[i]));
        free((void*)lines[i]);
    }
    free(lines);
	return wordWrapStringscope.Close(linearray);


}

v8Function TS_BMPgetStringWidth(V8ARGS) {
    if(args.Length()<1){
        THROWERROR("[bmpfontSDL] TS_BMPgetStringWidth Error: Called with less than 1 argument.");
    }
    CHECK_ARG_STR(0);
	v8::String::Utf8Value str(args[0]);
	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	return v8::Integer::New(static_cast<TS_BMPFont*>(ptr)->getStringWidth((const char*)(*str)));
}

v8Function TS_BMPgetStringHeight(V8ARGS) {
    if(args.Length()<1){
        THROWERROR("[bmpfontSDL] TS_BMPgetStringHeight Error: Called with less than 1 argument.");
    }
    CHECK_ARG_STR(0);
	v8::String::Utf8Value str(args[0]);
	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	return v8::Integer::New(static_cast<TS_BMPFont*>(ptr)->getStringHeight((const char*)(*str)));
}

v8Function TS_BMPgetHeight(V8ARGS)
{
	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	return v8::Integer::New(static_cast<TS_BMPFont*>(ptr)->getHeight());
}

v8Function TS_BMPdrawTextSurface(V8ARGS){
	if(args.Length()<4){
		return v8::ThrowException(v8::String::New("[bmpfontSDL] TS_BMPdrawTextSurface Error: called with less than 3 parameters!"));
    }


	CHECK_ARG_OBJ(0);
	CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
	CHECK_ARG_STR(3);


}

v8Function TS_BMPdrawText(V8ARGS) {


	if(args.Length()<3){
		return v8::ThrowException(v8::String::New("[bmpfontSDL] TS_BMPdrawText Error: called with less than 3 parameters!"));
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);

	int x = args[0]->Int32Value();
	int y = args[1]->Int32Value();
	v8::String::Utf8Value str(args[2]);

	v8::Local<v8::Object> self = args.Holder();

	static_cast<TS_BMPFont*>(v8::Local<v8::External>::Cast(self->GetInternalField(0))->Value())->drawText(x,y,(const char*)(*str));

    self.Clear();

	return v8::Undefined();
}
