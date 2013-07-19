#include "spriteset.h"
#include "../graphicSDL_GL_threaded/image.h"

/*
struct TS_SpriteSetFrame{
    int delay;
    TS_Texture texture;
}

struct TS_SpriteSetDirection{
    const char *name;
    int numImages;
    TS_SpriteSetFrame *frames;
}

class TS_SpriteSet{
public:
    SDL_Rect base;
    int numDirections;
    TS_SpriteSetDirection *directions;
}
*/


v8Function SpriteSetGetImages(V8GETTERARGS);

void SpriteSetSetImages(V8SETTERARGS);


DECLARE_OBJECT_TEMPLATES(SpriteSet);

void InitSpriteSet(void){
    INIT_OBJECT_TEMPLATES(SpriteSet);
    SET_CLASS_NAME(SpriteSet, "SpriteSet");

    SpriteSetInsttempl->SetAccessor(v8::String::New("images"), SpriteSetGetImages, SpriteSetSetImages);
}

TS_SpriteSet::TS_SpriteSet(const char *filename){

    if(filename==NULL){
        printf("[" PLUGINNAME "] TS_SpriteSet(filename) Error: No spriteset filename passed.\n");   //the file should be checked BEFORE passing it to the constructor!
        return;
    }
    SDL_RWops *ssfile;
	ssfile = SDL_RWFromFile(filename, "rb");
    if(!ssfile){
		printf("[" PLUGINNAME "] TS_SpriteSet(filename) Error: Could not open file %s\n", filename);   //the file should be checked BEFORE passing it to the constructor!
        return;
	}

    numTextures = 0;

    unsigned char sig[5]    = "    ";
    unsigned short version[1];
	unsigned short numImages[1];
	unsigned short frameWidth[1];
	unsigned short frameHeight[1];
    unsigned short numDirs[1];
    unsigned short baseX1[1];
    unsigned short baseY1[1];
    unsigned short baseX2[1];
    unsigned short baseY2[1];

    SDL_RWread(ssfile, sig, 1, 4);

    SDL_RWread(ssfile, version, 2, 1);

    SDL_RWread(ssfile, numImages, 2, 1);

    SDL_RWread(ssfile, frameWidth, 2, 1);

    textureWidth = *frameWidth;

    SDL_RWread(ssfile, frameHeight, 2, 1);

    textureHeight = *frameHeight;

    SDL_RWread(ssfile, numDirs, 2, 1);

    numDirections = *numDirs;

    SDL_RWread(ssfile, baseX1, 2, 1);
    SDL_RWread(ssfile, baseY1, 2, 1);
    SDL_RWread(ssfile, baseX2, 2, 1);
    SDL_RWread(ssfile, baseY2, 2, 1);

    printf("Spriteset is version %i\n", *version);

	SDL_RWseek(ssfile, 128, SEEK_SET);

    if(*version==3){
        uint32_t *pixels = (uint32_t *)malloc((*frameWidth)*(*frameHeight)*4);
        textures = (TS_Texture *)calloc(*numImages, sizeof(TS_Texture));
        numTextures = *numImages;
        for(int i = 0; i<*numImages; i++){

            SDL_RWread(ssfile, pixels, (*frameWidth)*(*frameHeight), 4);

            glGenTextures(1, textures+(i*sizeof(TS_Texture)));
            glBindTexture(GL_TEXTURE_2D, textures[i]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *frameWidth, *frameHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        }

        directions = (TS_SpriteSetDirection *)calloc(numDirections, sizeof(TS_SpriteSetDirection));

        unsigned short numFrames[1];
        unsigned short frameIndex[1];
        unsigned short delay[1];
        unsigned short stringlength[1];

        for(int d = 0; d<numDirections; d++){

            SDL_RWread(ssfile, numFrames, 2, 1);

            directions[d].numFrames = *numFrames;

            directions[d].frames = (TS_SpriteSetFrame *)calloc(*numFrames, sizeof(TS_SpriteSetFrame));

            SDL_RWseek(ssfile, 6, SEEK_CUR);

            SDL_RWread(ssfile, stringlength, 2, 1);

            directions[d].name = (char *)malloc((*stringlength)+1);
            directions[d].name[*stringlength] ='\0';

            SDL_RWread(ssfile, directions[d].name, *stringlength, 1);

            for(int f = 0; f<*numFrames; f++){
                SDL_RWread(ssfile, frameIndex, 2, 1);
                SDL_RWread(ssfile, delay, 2, 1);
                SDL_RWseek(ssfile, 4, SEEK_CUR);

                directions[d].frames[f].imageIndex = *frameIndex;
                directions[d].frames[f].delay = *delay;
            }
        }
    }

}

/*
struct TS_SpriteSetFrame{
    int imageIndex;
    int delay;
};

struct TS_SpriteSetDirection{
    char *name;
    int numFrames;
    TS_SpriteSetFrame *frames;
};

class TS_SpriteSet{
public:
    TS_SpriteSet(const char *filename);

    TS_Texture *textures;

    SDL_Rect base;
    int numDirections;
    TS_SpriteSetDirection *directions;
*/


v8Function SpriteSetGetImages(V8GETTERARGS) {
    TS_SpriteSet *ss = GET_ACCESSOR_SELF(TS_SpriteSet*);
    TS_Texture *textures = ss->textures;
    int numTextures = ss->numTextures;

    v8::Local<v8::Array> imageArray = v8::Array::New(numTextures);

    for(int i = 0; i<numTextures; i++){
        BEGIN_OBJECT_WRAP_CODE;
        TS_Image *im = new TS_Image(textures[i], ss->textureWidth, ss->textureHeight);
        RETURN_OBJECT_WRAP_CODE(Image, im);
        imageArray->Set(i, GET_OBJECT_WRAP_CODE(Image));
    }

	return imageArray;
}

void SpriteSetSetImages(V8SETTERARGS) {
}

TS_SpriteSet::~TS_SpriteSet(){
    glDeleteTextures(numTextures, textures);
    free(textures);
    for(int i = 0; i<numDirections; i++){
        //free(directions[i];
    }
}

void TS_SpriteSetFinalizer(V8FINALIZERARGS) {
    TS_SpriteSet* ss = (TS_SpriteSet*)parameter;
    delete ss;
    object->Dispose();
}

v8Function LoadSpriteSet(V8ARGS){
    if(args.Length()<1){
        THROWERROR(" Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);
    BEGIN_OBJECT_WRAP_CODE

    TS_Directories *dirs = GetDirs();

    v8::String::Utf8Value str(args[0]);

    const char *cstr = *str;

    TS_SpriteSet *ss = new TS_SpriteSet(string(dirs->spriteset).append(cstr).c_str());

    if(ss==NULL){
        THROWERROR(string(" Error: Invalid spriteset '").append(string(dirs->spriteset).append(cstr)).append("'").c_str());
    }

    printf("This is just a test: %i\n", ss->numDirections);

    END_OBJECT_WRAP_CODE(SpriteSet, ss);
}
