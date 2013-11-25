#include "batcher.h"
#include <cstdlib>

#include <cstdio>

uint32_t *(*TS_SDL_GL_GetSurfaceIDDL)(void);
uint32_t *(*TS_SDL_GL_GetImageIDDL)(void);
TS_Texture(*TS_SDL_GL_GetTextureFromImageDL)(void *);

#include "../../common/dlopenwrap.h"

static fhandle SDLGLhandle;

DECLARE_OBJECT_TEMPLATES(SpriteBatch);
TS_SpriteBatch *batch = NULL;

void InitBatcher(void){
    printf("Limit on size are %i.\n", GL_MAX_TEXTURE_SIZE);

    INIT_OBJECT_TEMPLATES(SpriteBatch);
    SET_CLASS_NAME(SpriteBatch, "SpriteBatch");
    ADD_TO_PROTO(SpriteBatch, "addTexture", spritebatcherAddImage);
    ADD_TO_PROTO(SpriteBatch, "blitBuffer", spritebatcherBlitBuffer);


    #ifdef _WIN32
		DWORD error;
	    SDLGLhandle = LoadLibrary("./plugin/SDL_GL_threaded.dll");
	    if(SDLGLhandle==NULL){
            SDLGLhandle = LoadLibrary("./plugin/SDL_GL.dll");
	    }
        if(SDLGLhandle!=NULL) {
				DLOPENFUNCTION(uint32_t *(*)(void), TS_SDL_GL_GetSurfaceIDDL, SDLGLhandle, "TS_SDL_GL_GetSurfaceID", 0, 0, exit(0xDD));
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
            fprintf(stderr, "[" PLUGINNAME "] %s Error: Could not open any known graphics plugins.\n", __func__);
            exit(0xFD);
        }
        else{
            TS_SDL_GL_GetSurfaceIDDL = (uint32_t *(*)(void))dlsym(SDLGLhandle, "TS_SDL_GL_GetSurfaceID");
            if (((error = dlerror()) != NULL)||(TS_SDL_GL_GetSurfaceIDDL==NULL))  {
                fprintf (stderr, "[" PLUGINNAME "] %s error: Could not load TS_SDL_GL_GetSurfaceID from any plugin.\n\tReported error is: %s", __func__, error);
                exit(0xDD);
            }
            TS_SDL_GL_GetImageIDDL = (uint32_t *(*)(void))dlsym(SDLGLhandle, "TS_SDL_GL_GetImageID");
            if (((error = dlerror()) != NULL)||(TS_SDL_GL_GetImageIDDL==NULL))  {
                fprintf (stderr, "[" PLUGINNAME "] %s error: Could not load TS_SDL_GL_GetImageID from any plugin.\n\tReported error is: %s", __func__, error);
                exit(0xDD);
            }
            TS_SDL_GL_GetTextureFromImageDL = (TS_Texture(*)(void *))dlsym(SDLGLhandle, "TS_SDL_GL_GetTextureFromImage");
            if (((error = dlerror()) != NULL)||(TS_SDL_GL_GetTextureFromImageDL==NULL))  {
                fprintf (stderr, "[" PLUGINNAME "] %s error: Could not load TS_SDL_GL_GetTextureFromImage from any plugin.\n\tReported error is: %s", __func__, error);
                TS_SDL_GL_GetTextureFromImageDL=NULL;
            }

        }
    #endif

}

v8Function spritebatcherDebug(V8ARGS){
    if(args.Length()<2){
        THROWERROR(" Error: Called with fewer than 2 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);

    batch->blitDebug(args[0]->Int32Value(), args[1]->Int32Value());

    return v8::Undefined();

}

v8Function spritebatcherAddImage(V8ARGS){
    if(args.Length()==0){
        THROWERROR(" Error: Called with fewer than one argument.");
    }

    TS_SpriteBatchTextureType type = TS_SpriteBatchTextureType::INVALID;

    SDL_Surface *surf = NULL;
    TS_Texture tex = 0;

    v8::Local<v8::Object> textureObj = v8::Local<v8::Object>::Cast(args[0]);

    if(textureObj->InternalFieldCount()<2){
        fprintf(stderr, "[" PLUGINNAME "] %s Error: Invalid object for argument 0. Using wrong graphics plugin?\n", __func__);
        THROWERROR(" Error: Argument 0 is not an Image or Surface.");
    }

    if(textureObj->GetInternalField(1)->Uint32Value()==*(TS_SDL_GL_GetSurfaceIDDL())){
        type = TS_SpriteBatchTextureType::SDL_SURFACE;
        surf = (SDL_Surface*)textureObj->GetAlignedPointerFromInternalField(0);
    }
    else if(textureObj->GetInternalField(1)->Uint32Value()==*(TS_SDL_GL_GetImageIDDL())){
        if(TS_SDL_GL_GetTextureFromImageDL==NULL){
            fprintf(stderr, " %s Error: Adding SpriteBatch images from Images is not supported without a compatible graphics plugin.", __func__);
            THROWERROR(" Error: Adding SpriteBatch images from Images is not supported without a compatible graphics plugin.");
        }
        type = TS_SpriteBatchTextureType::TS_IMAGE;
        tex = TS_SDL_GL_GetTextureFromImageDL(textureObj->GetAlignedPointerFromInternalField(0));
    }
    else{
        fprintf(stderr, "ID was %i. Expected %i or %i.\n", textureObj->GetInternalField(1)->Uint32Value(), *TS_SDL_GL_GetSurfaceIDDL(), *TS_SDL_GL_GetImageIDDL());
        THROWERROR(" Error: Argument 0 is not an Image or Surface.");
    }

    if(type==TS_SpriteBatchTextureType::SDL_SURFACE){
        GET_SELF(TS_SpriteBatch *)->AddImage(surf->w, surf->h, surf->pixels, true);
    }
    else{
        fprintf(stderr, "WARNING: Adding TS_Images as to TS_SpriteBatches is not fully implemented yet.\n");
        GET_SELF(TS_SpriteBatch *)->AddImage(1, 1, tex);
    }


    return v8::Undefined();
}

v8Function spritebatcherBlitBuffer(V8ARGS){
    if(args.Length()<2){
        THROWERROR(" Error: Called with fewer than 2 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);

    GET_SELF(TS_SpriteBatch *)->blitDebug(args[0]->Int32Value(), args[1]->Int32Value());

    return v8::Undefined();

}

TS_SpriteBatch::TS_SpriteBatch(){

    curwidth = 0;
    curheight = 0;
    rowheight = 0;

    width = DEFAULT_WIDTH;
    height = DEFAULT_HEIGHT;
    widthf = DEFAULT_WIDTH;
    heightf = DEFAULT_HEIGHT;
    glEnable(GL_TEXTURE_2D);

    coords = std::vector<TS_SpriteTextureCoord>();
    glGenFramebuffers(1, &framebuffer);
    glGenTextures(1, &texture);

    //Set up the color buffer of framebuffer.
    glGenTextures(1, &fbotex);

    glBindTexture(GL_TEXTURE_2D, fbotex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //Reserve the graphics memory for it.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbotex, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_TEXTURE_2D);
    //glPopAttrib();

}

TS_SpriteBatch::~TS_SpriteBatch(){
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &texture);
    glDeleteTextures(1, &fbotex);
}

TS_SpriteBatchError TS_SpriteBatch::AddImage(int w, int h, void *pixels, bool RGBA){

    if((w>width)||(h>height)){
        return TS_SpriteBatchError::TOOBIG;
    }

    if(curwidth+w>width){
        curheight+=rowheight;
        rowheight = 0;
        curwidth = 0;
    }

    if(curheight+h>height){
        return TS_SpriteBatchError::NOROOM;
    }

    if(h>rowheight)
        rowheight=h;

    //TODO: The floats are dummy values.
    coords.push_back({curwidth, curheight, (uint32_t)w, (uint32_t)h, ((float)curwidth)/((float)width), ((float)curheight)/((float)height), ((float)(curwidth+w))/((float)width), ((float)(curheight+w))/((float)height)});

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    TS_Texture tex;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    const GLint   vertexData[]   = {curwidth, curheight, curwidth+w, curheight, curwidth+w, curheight+h, curwidth, curheight+h};
    const GLint   texcoordData[] = {0, 0, 1, 0, 1, 1, 0, 1};
    const GLuint  colorData[]    = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

    glTexCoordPointer(2, GL_INT, 0, texcoordData);
    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);

    glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, texture);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    curwidth+=w;
    glDeleteTextures(1, &tex);

    return TS_SpriteBatchError::NOERROR;
}


void TS_SpriteBatch::blit(int x, int y){

}

void TS_SpriteBatch::blitDebug(int x, int y){

    glBindTexture(GL_TEXTURE_2D, fbotex);

    const GLint   vertexData[]   = {x, y, x+width, y, x+width, y+height, x, y+height};
    const GLint   texcoordData[] = {1, 1, 0, 1, 0, 0, 1, 0};
    const GLuint  colorData[]    = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

    glTexCoordPointer(2, GL_INT, 0, texcoordData);
    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);

    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, texture);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);


}

TS_SpriteBatchError TS_SpriteBatch::AddImage(int w, int h, TS_Texture tex){

//  coords.push_back(TS_SpriteTextureCoord());
    coords.push_back({curwidth, curheight, (uint32_t)w, (uint32_t)h, 1.0f, 1.0f, 1.0f, 1.0f});
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);



    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    return TS_SpriteBatchError::NOERROR;
}

void *TS_SpriteBatch::getPixels(){
    void *pixels = malloc(4*width*height);
    return pixels;
}

void TS_SpriteBatchFinalizer(V8FINALIZERARGS) {

    TS_SpriteBatch* sb = (TS_SpriteBatch*)parameter;
    delete sb;
    object->Dispose();
}

v8Function NewSpriteBatcher(V8ARGS){

    BEGIN_OBJECT_WRAP_CODE;

    TS_SpriteBatch *sb = new TS_SpriteBatch();

    END_OBJECT_WRAP_CODE(SpriteBatch, sb)

}
