#define NO_TTF_FONT_DECL

#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <cmath>

#define BMPFONT_INTERNAL
#define PLUGINNAME "bmpfontGL"
#include "bmpfont.h"

#define NUMFUNCS 2
#define NUMVARS 0

#ifdef _WIN32

#define STRDUP _strdup
#else

#include <cstring>
#define STRDUP strdup
#endif


#define CHECK_FOR_PROCESS(NAME){\
        if(SDL_GL_GetProcAddress(NAME)==NULL){\
        printf("[" PLUGINNAME "] Init Error: " NAME " is not present in OpenGL library.\n");\
        exit(1);\
    }\
    else\
    printf("[" PLUGINNAME "] Init Info: " NAME " is present in OpenGL library.\n");\
}

#define GET_GL_FUNCTION(NAME, TYPING)\
CHECK_FOR_PROCESS( #NAME );\
NAME = TYPING SDL_GL_GetProcAddress( #NAME )

#include "../../common/dlopenwrap.h"

v8::Local<v8::Object> (*TS_SDL_GL_WrapTS_ColorDL)(TS_Color*);

static fhandle SDLGLhandle;

void GetPluginInfo(TS_PluginInfo *info){
    info->name      = "bmpfontGL";
    info->version   = "v0.3";
    info->author    = "Martin McDonough";
    info->date      = __DATE__;
    info->description = ".rfn Sphere font reading, saving, and drawing plugin.";
}

using namespace std;

void (APIENTRY * glGenFramebuffersEXT)(GLsizei, GLuint*);

DECLARE_OBJECT_TEMPLATES(BMPFont);

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

void GLColor(TS_Color *color){
    glColor4ub(color->red, color->green, color->blue, color->alpha);
}

bool HAS_FRAMEBUFFERS = (strstr((char *)glGetString(GL_EXTENSIONS),"GL_EXT_framebuffer_object"));

int numerate(bool reset) {
    static int i = 0;
    if(reset) {
        i = 0;
        return 0;
    }
    i++;
    return i-1;
}

static GLuint *cachedTextLines;


initFunction Init(void) {
    if(false&&HAS_FRAMEBUFFERS){
        GET_GL_FUNCTION(glGenFramebuffersEXT, (void (APIENTRY *)(GLsizei, GLuint*)));
        cachedTextLines = (GLuint*)calloc(MAX_CACHED_TEXTLINES, sizeof(GLuint));
        glGenFramebuffersEXT(MAX_CACHED_TEXTLINES, cachedTextLines);
    }
    INIT_OBJECT_TEMPLATES(BMPFont);

    SET_CLASS_NAME(BMPFont, "Font");
    ADD_TO_PROTO(BMPFont, "drawText",        TS_BMPdrawText);
    ADD_TO_PROTO(BMPFont, "drawZoomedText",  TS_BMPdrawZoomedText);
    ADD_TO_PROTO(BMPFont, "getStringWidth",  TS_BMPgetStringWidth);
    ADD_TO_PROTO(BMPFont, "getStringHeight", TS_BMPgetStringHeight);
    ADD_TO_PROTO(BMPFont, "getHeight",       TS_BMPgetHeight);
    ADD_TO_PROTO(BMPFont, "drawTextBox",     TS_BMPdrawTextBox);
    ADD_TO_PROTO(BMPFont, "wordWrapString",  TS_BMPwordWrapString);
    ADD_TO_PROTO(BMPFont, "setColorMask",    TS_BMPsetColorMask);
    ADD_TO_PROTO(BMPFont, "getColorMask",    TS_BMPgetColorMask);


    #ifdef _WIN32
		DWORD error;
	    SDLGLhandle = LoadLibrary("./plugin/SDL_GL_threaded.dll");
	    if(SDLGLhandle==NULL){
            SDLGLhandle = LoadLibrary("./plugin/SDL_GL.dll");
	    }

        if(SDLGLhandle==NULL) {
            fprintf(stderr, "[" PLUGINNAME "] InitSpriteSet error: Could not open any known graphics plugins.\n");
            exit(0xFD);
        }
        else{
			DLOPENFUNCTION(v8::Local<v8::Object>(*)(TS_Color*), TS_SDL_GL_WrapTS_ColorDL, SDLGLhandle, "TS_SDL_GL_WrapTS_Color", 0, 0, exit(0xDD));
        }
    #else

        char *error;
        SDLGLhandle = dlopen("./plugin/libSDL_GL_threaded.so", RTLD_LOCAL|RTLD_NOW);
        if(SDLGLhandle==NULL)
            SDLGLhandle = dlopen("./plugin/libSDL_GL.so", RTLD_LOCAL|RTLD_NOW);

        if(SDLGLhandle==NULL) {
            fprintf(stderr, "[" PLUGINNAME "] InitSpriteSet error: Could not open any known graphics plugins.\n");
            exit(0xFD);
        }
        else{
            printf("The address is %p\n.", dlsym(SDLGLhandle, "TS_SDL_GL_WrapTS_Color"));
            TS_SDL_GL_WrapTS_ColorDL = (v8::Local<v8::Object>(*)(TS_Color*))dlsym(SDLGLhandle, "TS_SDL_GL_WrapTS_Color");
            if (((error = dlerror()) != NULL)||(TS_SDL_GL_WrapTS_ColorDL==NULL))  {
                fprintf (stderr, "[" PLUGINNAME "] InitSpriteSet error: Could not load TS_SDL_GL_WrapTS_Color from any plugin.\n\tReported error is: %s", error);
                exit(0xFE);
            }

        }
    #endif

	return (char *)PLUGINNAME;
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
    CLOSE_OBJECT_TEMPLATES(BMPFont);
}


void TS_BMPFontFinalizer(V8FINALIZERARGS) {
    TS_BMPFont* font = (TS_BMPFont*)parameter;
    delete font;
    object->Dispose();
}

/*
TS_BMPGlyph::TS_BMPGlyph(TS_Texture tex, int w, int h){
    texture = tex;
    width = w;
    height = h;
}
*/
TS_BMPGlyph::TS_BMPGlyph(uint32_t *pixels, int w, int h){

    width = w;
    height = h;
    dwidth = (double)w;
    dheight = (double)h;

    glGenTextures(1, &texture);
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, DEPTH, CHANNEL_MASKS);
    memcpy(surface->pixels, pixels, w*h*sizeof(uint32_t));

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

TS_BMPGlyph::~TS_BMPGlyph(){
    glDeleteTextures(1, &texture);
}

TS_BMPFont::TS_BMPFont(){
    mask = new TS_Color(255, 255, 255, 255);
    fontname = NULL;
}

int TS_BMPFont::Load(const char* file) {
    glGenTextures(1, &textureAtlas);
    mask = new TS_Color(0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RWops *fontfile;
	inheight = 0;
	fontfile = SDL_RWFromFile(file, "rb");
    if(!fontfile){
		printf("[" PLUGINNAME "] Error: Could not open file %s\n", file);   //the file should be checked BEFORE passing it to the constructor!
        return TS_BMPERROR_BADFILE;
	}
	if(fontfile->size(fontfile)<(long long)256){
        printf("[" PLUGINNAME "] Error: File %s is not a valid .rfn file: no file header.\n", file);
        SDL_RWclose(fontfile);
        return TS_BMPERROR_HEADER;
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

	SDL_RWseek(fontfile, 256, SEEK_SET);
    glyphs.reserve(numchars[0]);

    unsigned short totalwidth = 0;

	if(fontfile->size(fontfile)<(long long)256+(4*(*numchars))){
	    //Benefit of the doubt--an rfn could possibly contain a bunch of empty characters.
        printf("[" PLUGINNAME "] Error: File %s is not a valid .rfn file: no glyph data.\n", file);
        return TS_BMPERROR_ENDDATA;
	}
    if(version[0]==1){

    }
    if(version[0]==2){
        long long current_pos = 256;
        uint32_t *pixels = (uint32_t*)calloc(0, sizeof(uint32_t));
        if(*compressed==0){
            for(unsigned short i = 0; i<numchars[0]; i++){
                unsigned short   width[1];
                unsigned short  height[1];
                current_pos += SDL_RWread(fontfile,  width, 1, 2);
                current_pos += SDL_RWread(fontfile, height, 1, 2);

                if(fontfile->size(fontfile)<(long long)((*width)*(*height)*sizeof(uint32_t))+(long long)28+current_pos){
                    printf("[" PLUGINNAME "] Error: File %s is not a valid .rfn file: unexpected end of glyph data.\n", file);
                    free(pixels);
                    SDL_RWclose(fontfile);
                    return TS_BMPERROR_ENDDATA;
                }

                pixels = (uint32_t *)realloc(pixels, (*width)*(*height)*sizeof(uint32_t));

                if(*height>inheight)    inheight = *height;

                SDL_RWseek(fontfile, 28, SEEK_CUR);

                current_pos += 28+(sizeof(uint32_t)*SDL_RWread(fontfile, pixels, sizeof(uint32_t),(*width)*(*height)));
                totalwidth+=*width;
                glyphs.push_back(new TS_BMPGlyph(pixels, *width, *height));
            }
        }

        surfaceAtlas = SDL_CreateRGBSurface(SDL_SWSURFACE, totalwidth, inheight, DEPTH, CHANNEL_MASKS);
        SDL_SetSurfaceBlendMode(surfaceAtlas, SDL_BLENDMODE_NONE);
        widths = (unsigned short *)malloc((sizeof(unsigned short)*glyphs.size())+1);
        SDL_Rect dest = {0, 0, 0, 0};
        for(size_t i = 0; i<glyphs.size(); i++){
            dest.w = glyphs[i]->surface->w;
            widths[i] = dest.x;
            dest.h = glyphs[i]->surface->h;
                for(int e = 0; e<glyphs[i]->surface->h; e++){
                    memcpy((dest.x*sizeof(uint32_t))+(char *)(surfaceAtlas->pixels)+(e*surfaceAtlas->pitch), (char *)(glyphs[i]->surface->pixels)+(e*glyphs[i]->surface->pitch), glyphs[i]->surface->pitch);
                }
            dest.x+=dest.w;
        }
        widths[glyphs.size()] = totalwidth;

        glBindTexture(GL_TEXTURE_2D, textureAtlas);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, totalwidth, inheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, surfaceAtlas->pixels);

        SDL_RWclose(fontfile);
        free(pixels);
    }

    return TS_BMPERROR_NOERROR;

}

TS_BMPFont::~TS_BMPFont(){
    for(unsigned int i = 0; i<glyphs.size(); i++){
        delete glyphs[i];
    }
}

v8Function LoadBMPFont(const v8::Arguments& args) {

	TS_Directories *TS_dirs = GetDirs();

    if(args.Length()<1){
        THROWERROR("[" PLUGINNAME "] TS_LoadBMPFont Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);

    BEGIN_OBJECT_WRAP_CODE;

    v8::String::Utf8Value str(args[0]);
    const char *fontname = *str;

    SDL_RWops *fonttest = SDL_RWFromFile(string(TS_dirs->font).append(fontname).c_str(), "rb");
    if(!fonttest){
        SDL_RWclose(fonttest);
        //printf("[bmpfontSDL] TS_LoadBMPFont Error: Could not open rfn file %s\n", fontname);
        return v8::ThrowException(v8::String::New(string("[" PLUGINNAME "] TS_LoadFont Error: Could not load font ").append(fontname).c_str()));
    }
    SDL_RWclose(fonttest);

    MINMEMALIGN TS_BMPFont *font = new TS_BMPFont();
    int error = font->Load(string(TS_dirs->font).append(fontname).c_str());

    if(error){
        THROWERROR(string("Error: Could not load bmp font.").append(fontname).c_str());
    }

    END_OBJECT_WRAP_CODE(BMPFont, font);
}

v8Function LoadSystemBMPFont(V8ARGS) {

    BEGIN_OBJECT_WRAP_CODE;

	TS_Config *TS_conf = GetConfig();
	TS_Directories *TS_dirs = GetDirs();

    SDL_RWops *fonttest = SDL_RWFromFile(string(TS_dirs->system).append(TS_conf->systemfont).c_str(), "rb");
    if(!fonttest){
        SDL_RWclose(fonttest);
        return v8::ThrowException(v8::String::New(string("[" PLUGINNAME "] TS_LoadSystemBMPFont Error: Could not open rfn file ").append(string(TS_dirs->system).append(TS_conf->systemfont)).c_str()));
    }

    MEMALIGN(8) TS_BMPFont *font = new TS_BMPFont();
    int error = font->Load(string(TS_dirs->system).append(TS_conf->systemfont).c_str());

    if(error){
        THROWERROR("Error: Could not load system font.");
    }

    END_OBJECT_WRAP_CODE(BMPFont, font);
}

v8Function TS_BMPsetColorMask(V8ARGS){
    if(args.Length()<1){
        THROWERROR(" Error: Called with no arguments.");
    }
	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[0]);

    TS_Color* c = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    GET_SELF(TS_BMPFont*)->setColorMask(c);

    return v8::Undefined();
}

v8Function TS_BMPgetColorMask(V8ARGS){

    TS_Color* c = GET_SELF(TS_BMPFont*)->getColorMask();

    //Make the returned color the mask in value only, not actually point to the mask's address.
    TS_Color*c2 = new TS_Color(c->red, c->green, c->blue, c->alpha);
    return TS_SDL_GL_WrapTS_ColorDL(c2);
}

void TS_BMPGlyph::blit(int x, int y, TS_Color *mask){
    const GLint vertexData[] = {x, y, x+width, y, x+width, y+height, x, y+height};
    glVertexPointer(2, GL_INT, 0, vertexData);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

int TS_BMPGlyph::zoomBlit(int x, int y, double factor, TS_Color *mask){

    int scaleWidth   = factor*dwidth;
    int scaleHeight  = factor*dheight;
    const GLint vertexData[] = {x, y, x+(int)scaleWidth, y, x+scaleWidth, y+scaleHeight, x, y+scaleHeight};
    glVertexPointer(2, GL_INT, 0, vertexData);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    return scaleWidth;
}

void TS_BMPFont::setColorMask(TS_Color *c){
    delete mask;
    mask = new TS_Color(c->red, c->green, c->blue, c->alpha);
}

TS_Color *TS_BMPFont::getColorMask(void){
    return mask;
}

void TS_BMPFont::drawText(int x, int y, const char *t) {
    int curWidth = 0;
    int glyphnum = 0;
    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    size_t i = 0;

    while(t[i]!=0){

        glyphnum = int(t[i]);
        const int width = glyphs[glyphnum]->width;
        float x0 = (float)(widths[glyphnum])/(float)widths[glyphs.size()];
        float x1 = (float)(widths[glyphnum+1])/float(widths[glyphs.size()]);
        const GLfloat texcoordData[] = {x0, 0.0f, x1, 0.0f, x1, 1.0f, x0, 1.0f};
        const GLint vertexData[] = {x+curWidth, y, x+curWidth+width, y, x+curWidth+width, y+inheight, x+curWidth, y+inheight};
        glTexCoordPointer(2, GL_FLOAT, 0, texcoordData);
        glVertexPointer(2, GL_INT, 0, vertexData);
        glBindTexture(GL_TEXTURE_2D, textureAtlas);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        //glyphs[glyphnum]->blit(x+curWidth, y, mask);
        curWidth+=glyphs[glyphnum]->width;
        i++;
    }


    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);

}

void TS_BMPFont::drawZoomedText(int x, int y, double f, const char* t) {
    double curWidth = 0;
    int glyphnum = 0;
    std::string text = t;

    glEnable(GL_TEXTURE_2D);
    const GLint   texcoordData[] = {0, 0, 1, 0, 1, 1, 0, 1};

    const GLuint  colorData[]    = {
        mask->toInt(),
        mask->toInt(),
        mask->toInt(),
        mask->toInt()
    };

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glTexCoordPointer(2, GL_INT, 0, texcoordData);



        for (size_t i=0; i < text.length(); i++){
            glyphnum = int(text.at(i));
            curWidth+=glyphs[glyphnum]->zoomBlit(x+curWidth, y, f, mask);
        }

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);

}

int TS_BMPFont::getStringWidth(const char *t) const{
    int curWidth = 0;
    int glyphnum;
    std::string text(t);
    for (size_t i=0; i < text.length(); i++){
        glyphnum = int(text.at(i));
        curWidth+=glyphs[glyphnum]->width;
    }
    return curWidth;
}

int TS_BMPFont::getCharWidth(char c) const{
    return glyphs[c]->width;
}

int TS_BMPFont::getStringHeight(const char *t) {
    int curWidth = 0;
    int maxHeight= 0;
    int glyphnum;
    std::string text = t;
    for (size_t i=0; i < text.length(); i++){
        glyphnum = int(text.at(i));
        curWidth+=glyphs[glyphnum]->width;
        if(glyphs[glyphnum]->height>maxHeight){
            maxHeight=glyphs[glyphnum]->height;
        }
    }
    return maxHeight;
}

int TS_BMPFont::getHeight() {
    return inheight;
}

inline const char ** TS_BMPFont::addline(const char ** __restrict textlines, int * __restrict readylines, char ** __restrict linebuffer) const{
    (*readylines)++;
    textlines = (const char **)realloc(textlines, (*readylines)*sizeof(const char *));
    textlines[(*readylines)-1] = STRDUP((char *)((*linebuffer)+((((*linebuffer)[0]==' ')||((*linebuffer)[0]=='\n'))?1:0)));
	(*linebuffer)  = (char *)realloc((*linebuffer), 1);
	(*linebuffer)[0] = '\0';
    return textlines;
}

const char **TS_BMPFont::wordWrapString(const char * __restrict t, int w, int* __restrict num) {

    static TS_BMPWordWrapResult *strings = (TS_BMPWordWrapResult *)calloc(sizeof(TS_BMPWordWrapResult), MAX_CACHED_WORDWRAPS);
    static int stringLoc = 0;
    for(int i = 0; i<MAX_CACHED_WORDWRAPS; i++){
        if(strings[i].str==NULL)
            continue;
        if(strcmp(t, strings[i].str)==0){
            (*num) = strings[i].numlines;
            return strings[i].lines;
        }
    }

    intptr_t startOfWord    = (intptr_t)t; //NUMBER OF CHARACTERS
    const char **textlines  = NULL;// (const char **)calloc(0, sizeof(const char *));
    int readylines          = 0;
    int lineWidth           = 0; //PIXEL WIDTH!
    int tlength             = strlen(t);
    char *linebuffer        = (char *)calloc(1, sizeof(char));

    for(int i = 0; i<tlength+1; i++){
        if((t[i]==' ')||(t[i]=='\n')||(i==tlength)){
            const int wordlen = (((intptr_t)t)+i)-startOfWord; //NUMBER OF CHARACTERS
            const char *tword = strncpy((char *)calloc(wordlen+1, sizeof(char)), (const char *)startOfWord, wordlen);
            const int twwidth = this->getStringWidth(tword);
            if(lineWidth-this->getCharWidth(' ')+twwidth>w){
                textlines   = this->addline(textlines, &readylines, &linebuffer);
                lineWidth=twwidth;

				linebuffer = (char *)realloc(linebuffer,(wordlen+2)*sizeof(char));
                linebuffer = strcat(linebuffer, tword);
            }
            else{
				linebuffer = (char *)realloc(linebuffer, (strnlen(linebuffer, tlength+1)+wordlen+2)*sizeof(char)); //two extra bytes to handle if '\n' and ' ' in sequence.
                linebuffer = strcat(linebuffer, tword);
                lineWidth+=twwidth;
                if(t[i]=='\n'){
                    textlines = this->addline(textlines, &readylines, &linebuffer);
                    lineWidth = 0;
                }
            }
            free((void*)tword);
            startOfWord = ((intptr_t)t)+i;
        }
    }
    (readylines)++;
    textlines = (const char **)realloc(textlines, (readylines)*sizeof(const char *));
    textlines[(readylines)-1] = STRDUP(linebuffer+(((linebuffer[0]==' ')||(linebuffer[0]=='\n'))?1:0));
    free(linebuffer);
    (*num) = readylines;

    //Free up the location for the result to be cached, if necessary.
    if(strings[stringLoc].str!=NULL)
        free((void *)strings[stringLoc].str);

    if(strings[stringLoc].lines!=NULL){
        for(int i = 0; i<strings[stringLoc].numlines; i++){
            free((void*)strings[stringLoc].lines[i]);
        }
        free((void *)strings[stringLoc].lines);
    }
    //Cache the result.

    strings[stringLoc].numlines = *num;
    strings[stringLoc].str = STRDUP(t);
    strings[stringLoc].lines = textlines;
    (stringLoc+=1)%=MAX_CACHED_WORDWRAPS;
    printf("[" PLUGINNAME "] TS_wordWrapString Info: String %s cached to slot %i.\n", t, stringLoc);
    return textlines;
}

//                 0  1  2      3       4         5
//Font.drawTextBox(x, y, width, height, y_offset, text): Draw wrapped text, shifted up/down by y_offset, in the box (x, y, width, height).
v8Function TS_BMPdrawTextBox(V8ARGS) {
    if(args.Length()<2){
        THROWERROR("[" PLUGINNAME "] TS_BMPwordWrapString Error: Called with less than 2 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);
    CHECK_ARG_INT(4);
    CHECK_ARG_STR(5);
    if(args[2]->IntegerValue()<0){
        THROWERROR_RANGE("[" PLUGINNAME "] TS_BMPdrawTextBox Error: Argument 2 is negative.");
    }
    if(args[3]->IntegerValue()<0){
        THROWERROR_RANGE("[" PLUGINNAME "] TS_BMPdrawTextBox Error: Argument 3 is negative.");
    }

    TS_BMPFont *font = GET_SELF(TS_BMPFont*);
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
	if(font->getStringWidth((const char*)(*str))<=w){
        font->drawText(x, y+y_offset, (const char*)(*str));
        return v8::Undefined();
	}

	const char ** lines = font->wordWrapString((const char*)(*str), w, &numlines);
    int inheight = font->getHeight();
    int i = 0;
    while(i<numlines){
        if((y_offset+((i+1)*inheight))<=h)
            font->drawText(x, y+y_offset+(i*inheight), lines[i]);
        i++;
    }

    return v8::Undefined();
}

v8Function TS_BMPwordWrapString(V8ARGS) {
    v8::HandleScope wordWrapStringscope;
    if(args.Length()<2){
        THROWERROR("[" PLUGINNAME "] TS_BMPwordWrapString Error: Called with less than 2 arguments.");
    }
    CHECK_ARG_STR(0);
    CHECK_ARG_INT(1);
    if(args[1]->IntegerValue()<0){
        THROWERROR_RANGE("[" PLUGINNAME "] TS_BMPwordWrapString Error: Argument 1 is negative.");
    }
	v8::String::Utf8Value str(args[0]);
	int w = args[1]->Int32Value();
	int numlines = 1;
	const char ** lines = GET_SELF(TS_BMPFont*)->wordWrapString((const char*)(*str), w, &numlines);

    v8::Local<v8::Object> linearray = v8::Array::New(numlines);

    for(int32_t i = 0; i<numlines; i++){
        linearray->Set(i, v8::String::New(lines[i]));
        free((void*)lines[i]);
    }
    free(lines);
	return wordWrapStringscope.Close(linearray);


}

v8Function TS_BMPgetStringWidth(V8ARGS) {
    if(args.Length()<1){
        THROWERROR("[" PLUGINNAME "] TS_BMPgetStringWidth Error: Called with less than 1 argument.");
    }
    CHECK_ARG_STR(0);
	v8::String::Utf8Value str(args[0]);
	return v8::Integer::New(GET_SELF(TS_BMPFont*)->getStringWidth((const char*)(*str)));
}

v8Function TS_BMPgetStringHeight(V8ARGS) {
    if(args.Length()<1){
        THROWERROR("[" PLUGINNAME "] TS_BMPgetStringHeight Error: Called with less than 1 argument.");
    }
    CHECK_ARG_STR(0);
	v8::String::Utf8Value str(args[0]);
	return v8::Integer::New(GET_SELF(TS_BMPFont*)->getStringHeight((const char*)(*str)));
}

v8Function TS_BMPgetHeight(V8ARGS)
{
	return v8::Integer::New(GET_SELF(TS_BMPFont*)->getHeight());
}

v8Function TS_BMPdrawTextSurface(V8ARGS){
	if(args.Length()<4){
		return v8::ThrowException(v8::String::New("[" PLUGINNAME "] TS_BMPdrawTextSurface Error: called with less than 3 parameters!"));
    }

	CHECK_ARG_OBJ(0);
	CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
	CHECK_ARG_STR(3);

    return v8::Undefined();

}

v8Function TS_BMPdrawText(V8ARGS) {


	if(args.Length()<3){
		return v8::ThrowException(v8::String::New("[" PLUGINNAME "] TS_BMPdrawText Error: called with less than 3 parameters!"));
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);

	int x = args[0]->Int32Value();
	int y = args[1]->Int32Value();
	v8::String::Utf8Value str(args[2]);

	GET_SELF(TS_BMPFont*)->drawText(x,y,(const char*)(*str));

	return v8::Undefined();
}

v8Function TS_BMPdrawZoomedText(V8ARGS) {


	if(args.Length()<4){
		return v8::ThrowException(v8::String::New("[" PLUGINNAME "] TS_BMPdrawZoomedText Error: called with less than 3 parameters!"));
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);

	int x = args[0]->Int32Value();
	int y = args[1]->Int32Value();
	double zoom = args[2]->NumberValue();
	v8::String::Utf8Value str(args[3]);

	GET_SELF(TS_BMPFont*)->drawZoomedText(x, y, zoom, (const char*)(*str));

	return v8::Undefined();
}
