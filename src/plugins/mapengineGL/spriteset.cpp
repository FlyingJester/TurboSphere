#include "spriteset.h"
//#include "../graphicSDL_GL_threaded/image.h"
#include "../../common/dlopenwrap.h"

v8::Local<v8::Object> (*TS_SDL_GL_MakeV8SurfaceHandleFromPixelsDL)(int, int, void*);
v8::Local<v8::Object> (*TS_SDL_GL_MakeV8ImageHandleFromGLTextureDL)(int, int, GLuint);

static fhandle SDLGLhandle;

v8Function SpriteSetGetImages(V8GETTERARGS);

void SpriteSetSetImages(V8SETTERARGS);


DECLARE_OBJECT_TEMPLATES(SpriteSet);

void InitSpriteSet(void){
    INIT_OBJECT_TEMPLATES(SpriteSet);
    SET_CLASS_NAME(SpriteSet, "SpriteSet");

    SpriteSetInsttempl->SetAccessor(v8::String::New("images"), SpriteSetGetImages, SpriteSetSetImages);

    #ifdef _WIN32
		DWORD error;
	    SDLGLhandle = LoadLibrary("./plugin/SDL_GL_threaded.dll");
	    if(SDLGLhandle==NULL){
            SDLGLhandle = LoadLibrary("./plugin/SDL_GL.dll");
	    }
        if(SDLGLhandle!=NULL) {
				DLOPENFUNCTION(v8::Local<v8::Object>(*)(int, int, void *), TS_SDL_GL_MakeV8SurfaceHandleFromPixelsDL, SDLGLhandle, "TS_SDL_GL_MakeV8SurfaceHandleFromPixels", 0, 0, exit(0xDD));
				DLOPENFUNCTION(v8::Local<v8::Object>(*)(int, int, GLuint), TS_SDL_GL_MakeV8ImageHandleFromGLTextureDL, SDLGLhandle, "TS_SDL_GL_MakeV8ImageHandleFromGLTexture", 0, 0, exit(0xDD));
        }
        else{
			exit(0xD9);
        }
    #else

        char *error;
        SDLGLhandle = dlopen("./plugin/libSDL_GL_threaded.so", RTLD_GLOBAL|RTLD_NOW);
        if(SDLGLhandle==NULL)
            SDLGLhandle = dlopen("./plugin/libSDL_GL.so", RTLD_GLOBAL|RTLD_NOW);

        if(SDLGLhandle==NULL) {
            fprintf(stderr, "[" PLUGINNAME "] InitSpriteSet error: Could not open any known graphics plugins.\n");
            exit(0xFD);
        }
        else{
            //DLOPENFUNCTION(v8::Local<v8::Object>(*)(int, int, void *), TS_SDL_GL_MakeV8SurfaceHandleFromPixels, handle, "TS_SDL_GL_MakeV8SurfaceHandleFromPixels", "[" PLUGINNAME "] InitSpriteSet error: Could not load TS_SDL_GL_MakeV8SurfaceHandleFromPixels from any plugin.\n", error, exit(0xFE));
            TS_SDL_GL_MakeV8SurfaceHandleFromPixelsDL = (v8::Local<v8::Object>(*)(int, int, void *))dlsym(SDLGLhandle, "TS_SDL_GL_MakeV8SurfaceHandleFromPixels");
            if (((error = dlerror()) != NULL)||(TS_SDL_GL_MakeV8SurfaceHandleFromPixelsDL==NULL))  {
                fprintf (stderr, "[" PLUGINNAME "] InitSpriteSet error: Could not load TS_SDL_GL_MakeV8SurfaceHandleFromPixels from any plugin.\n\tReported error is: %s", error);
                exit(0xFE);
            }
            //DLOPENFUNCTION(v8::Local<v8::Object>(*)(int, int, GLuint), TS_SDL_GL_MakeV8ImageHandleFromGLTexture, handle, "TS_SDL_GL_MakeV8ImageHandleFromGLTexture", "[" PLUGINNAME "] InitSpriteSet error: Could not load % from any plugin.\n", error, exit(0xFE));
            TS_SDL_GL_MakeV8ImageHandleFromGLTextureDL = (v8::Local<v8::Object>(*)(int, int, GLuint))dlsym(SDLGLhandle, "TS_SDL_GL_MakeV8ImageHandleFromGLTexture");
            if (((error = dlerror()) != NULL)||(TS_SDL_GL_MakeV8ImageHandleFromGLTextureDL==NULL))  {
                fprintf (stderr, "[" PLUGINNAME "] InitSpriteSet error: Could not load TS_SDL_GL_MakeV8ImageHandleFromGLTexture from any plugin.\n\tReported error is: %s", error);
                exit(0xFE);
            }
        }
    #endif
}

void CloseSpriteSet(void){
    #ifdef _MSC_VER

    #else
    dlclose(SDLGLhandle);
    #endif
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


v8Function SpriteSetGetImages(V8GETTERARGS) {
    TS_SpriteSet *ss = GET_ACCESSOR_SELF(TS_SpriteSet*);
    TS_Texture *textures = ss->textures;
    int numTextures = ss->numTextures;

    v8::Local<v8::Array> imageArray = v8::Array::New(numTextures);

    for(int i = 0; i<numTextures; i++){
        imageArray->Set(i, TS_SDL_GL_MakeV8ImageHandleFromGLTextureDL(ss->textureWidth, ss->textureHeight, textures[i]));
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
