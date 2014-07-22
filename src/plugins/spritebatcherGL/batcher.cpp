#include "batcher.h"
#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <cstdint>

void (*TS_SDL_GL_ResetOrthoDL)(void);
v8::Local<v8::Object> (*TS_SDL_GL_WrapTS_ColorDL)(TS_Color*);
uint32_t *(*TS_SDL_GL_GetSurfaceIDDL)(void);
uint32_t *(*TS_SDL_GL_GetImageIDDL)(void);
TS_Texture(*TS_SDL_GL_GetTextureFromImageDL)(void *);
v8Function (*TS_SDL_GL_MakeV8ImageHandleFromGLTextureCoordDL)(int w, int h, float coords[8], GLuint tex);

#include "../../common/dlopenwrap.h"

static fhandle SDLGLhandle;

DECLARE_OBJECT_TEMPLATES(SpriteBatch);
DECLARE_OBJECT_TEMPLATES(SpriteBatchOp);
TS_SpriteBatch *batch = NULL;

uint32_t DEFAULT_WIDTH = 1;
uint32_t DEFAULT_HEIGHT = 1;

void InitBatcher(void){

    //Set the textures to be the closest POT size to the maximum.
    int maxTexSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
    while(DEFAULT_WIDTH*4<(unsigned int)maxTexSize)
        DEFAULT_WIDTH<<=1;

    DEFAULT_HEIGHT = DEFAULT_WIDTH;
    printf("[" PLUGINNAME "] Info: Limit on size are %i. Making textures at %i.\n", maxTexSize, DEFAULT_WIDTH);


    //Set up the SpriteBatch script-side objects.
    INIT_OBJECT_TEMPLATES(SpriteBatch);
    INIT_OBJECT_TEMPLATES(SpriteBatchOp);
    SET_CLASS_ACCESSOR(SpriteBatchOp, "Color", SpriteBatchOpGetColor, SpriteBatchOpSetColor);

    SET_CLASS_NAME(SpriteBatch, "SpriteBatch");
    SET_CLASS_NAME(SpriteBatchOp, "SpriteBatchOp");
    ADD_TO_PROTO(SpriteBatch, "addTexture", spritebatcherAddImage);
    ADD_TO_PROTO(SpriteBatch, "getImages", SpriteBatchGetImages);


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

            TS_SDL_GL_MakeV8ImageHandleFromGLTextureCoordDL = (v8Function(*)(int, int, float[8], TS_Texture))dlsym(SDLGLhandle, "TS_SDL_GL_MakeV8ImageHandleFromGLTextureCoord");
            if (((error = dlerror()) != NULL)||(TS_SDL_GL_MakeV8ImageHandleFromGLTextureCoordDL==NULL))  {
                fprintf (stderr, "[" PLUGINNAME "] %s error: Could not load TS_SDL_GL_MakeV8ImageHandleFromGLTextureCoord from any plugin.\n\tReported error is: %s", __func__, error);
                exit(0xDE);

            }
            TS_SDL_GL_ResetOrthoDL = (void(*)(void))dlsym(SDLGLhandle, "ResetOrtho");
            if (((error = dlerror()) != NULL)||(TS_SDL_GL_ResetOrthoDL==NULL))  {
                fprintf (stderr, "[" PLUGINNAME "] %s error: Could not load ResetOrtho from any plugin.\n\tReported error is: %s", __func__, error);
                exit(0xDE);
            }
            TS_SDL_GL_WrapTS_ColorDL = (v8::Local<v8::Object>(*)(TS_Color*))dlsym(SDLGLhandle, "TS_SDL_GL_WrapTS_Color");
            if (((error = dlerror()) != NULL)||(TS_SDL_GL_WrapTS_ColorDL==NULL))  {
                fprintf (stderr, "[" PLUGINNAME "] InitSpriteSet error: Could not load TS_SDL_GL_WrapTS_Color from any plugin.\n\tReported error is: %s", error);
                exit(0xFE);
            }

        }
    #endif

}

v8Function spritebatcherAddImage(V8ARGS){
    if(args.Length()==0){
        THROWERROR(" Error: Called with fewer than one argument.");
    }

    TS_SpriteBatchTextureType type = TS_SpriteBatchTextureType::INVALID;

    SDL_Surface *surf = NULL;
    TS_Texture tex = 0;

    v8::Local<v8::Object> textureObj = v8::Local<v8::Object>::Cast(args[0]);

        //Check if the parameter can be checked as a valid, TurboSphere object.
    if(textureObj->InternalFieldCount()<2){
        fprintf(stderr, "[" PLUGINNAME "] %s Error: Invalid object for argument 0. Using wrong graphics plugin?\n", __func__);
        THROWERROR(" Error: Argument 0 is not an Image or Surface.");
    }

        //Check what kind of object the parameter is.
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
        fprintf(stderr, "ID was %i. Expected %i (Surface) or %i (Image).\n", textureObj->GetInternalField(1)->Uint32Value(), *TS_SDL_GL_GetSurfaceIDDL(), *TS_SDL_GL_GetImageIDDL());
        THROWERROR(" Error: Argument 0 is not an Image or Surface.");
    }

        //Add the parameter.
    if(type==TS_SpriteBatchTextureType::SDL_SURFACE){
        GET_SELF(TS_SpriteBatch *)->AddImage(surf->w, surf->h, surf->pixels, true);
    }
    else if(type==TS_SpriteBatchTextureType::TS_IMAGE){
        GET_SELF(TS_SpriteBatch *)->AddImage(-1, -1, tex);
    }
    else{
        fprintf(stderr, "%s Internal Error: Unkown texture type. This is an internal error.\n", __func__);
        exit(0x20F);
    }

    return v8::Undefined();
}

TS_SpriteBatch::TS_SpriteBatch(){

    numImages = 0;
    curwidth  = 0;
    curheight = 0;
    rowheight = 0;

    width = DEFAULT_WIDTH;
    height = DEFAULT_HEIGHT;
    widthf = DEFAULT_WIDTH;
    heightf = DEFAULT_HEIGHT;
    glEnable(GL_TEXTURE_2D);

    coords = std::vector<TS_SpriteTextureCoord>();
    operations = std::vector<TS_SpriteBatchOp*>();
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Set up the color buffer of framebuffer.
    glGenTextures(1, &fbotex);

    glBindTexture(GL_TEXTURE_2D, fbotex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //Reserve the graphics memory for it.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbotex, 0);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //TS_SDL_GL_ResetOrthoDL();
    glDisable(GL_TEXTURE_2D);
    //glPopAttrib();

}

TS_SpriteBatchError TS_SpriteBatch::PushOperation(TS_SpriteBatchOp *op){
    if(op->texture==0)
        return TS_SpriteBatchError::BADTEXTURE;

    TS_SpriteBatchOp *newop = (TS_SpriteBatchOp *)malloc(sizeof(TS_SpriteBatchOp));
    memcpy(newop, op, sizeof(TS_SpriteBatchOp));

    operations.push_back(newop);

    return TS_SpriteBatchError::NOERROR;
}

inline void TS_SpriteBatch::SetOrtho(void){
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glViewport(0, 0, width, height);
    //glScissor(0, 0, GetScreenWidth()*scaleSize, GetScreenHeight()*scaleSize);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();


    glOrtho(0, width, height, 0, 1, -1);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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

    //coords.push_back({curwidth, curheight, (uint32_t)w, (uint32_t)h, ((float)curwidth)/((float)width), ((float)curheight)/((float)height), ((float)(curwidth+w))/((float)width), ((float)(curheight+h))/((float)height)});
    coords.push_back({curwidth, curheight, (uint32_t)w, (uint32_t)h, ((float)(curwidth))/((float)width), ((float)curheight)/((float)height), ((float)((curwidth+w)))/((float)width), ((float)(curheight+h))/((float)height)});

    SetOrtho();

    TS_Texture tex;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    const GLint   vertexData[]   = {width-curwidth, curheight, width-(curwidth+w), curheight, width-(curwidth+w), curheight+h, width-curwidth, curheight+h};
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

    TS_SDL_GL_ResetOrthoDL();

    curwidth+=w;
    glDeleteTextures(1, &tex);
    numImages++;

    return TS_SpriteBatchError::NOERROR;
}


TS_SpriteBatchError TS_SpriteBatch::AddImage(int w, int h, TS_Texture tex){

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

    SetOrtho();

    glBindTexture(GL_TEXTURE_2D, tex);

    //if((w<=0)||(h<=0)){
        //int w1 = 0;
        //int h1 = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
        //w = w1;
        //h = h1;
    //}

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

    //coords.push_back({curwidth, curheight, (uint32_t)w, (uint32_t)h, ((float)curwidth)/((float)width), ((float)curheight)/((float)height), ((float)(curwidth+w))/((float)width), ((float)(curheight+h))/((float)height)});
    coords.push_back({curwidth, curheight, (uint32_t)w, (uint32_t)h, 0.0f, 1.0f, 0.0f, 1.0f});
    //coords.push_back({curwidth, curheight, (uint32_t)w, (uint32_t)h, curwidth, curheight, curwidth+w, curheight+h});
    //coords.push_back({curwidth, curheight, (uint32_t)w, (uint32_t)h, ((float)(width-curwidth))/((float)width), ((float)curheight)/((float)height), ((float)(width-(curwidth+w)))/((float)width), ((float)(curheight+h))/((float)height)});

    //printf("X\t%i\tY\t%i\tw\t%i\th\t%i\n", curwidth, curheight, w, h);
    //const GLint   vertexData[]   = {curwidth, curheight, curwidth+w, curheight, curwidth+w, curheight+h, curwidth, curheight+h};


    const GLint   vertexData[]   = {width-curwidth, curheight, width-(curwidth+w), curheight, width-(curwidth+w), curheight+h, width-curwidth, curheight+h};
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
    TS_SDL_GL_ResetOrthoDL();

    curwidth+=w;
    numImages++;
    return TS_SpriteBatchError::NOERROR;
}

TS_SpriteBatchOp *TS_CreateSpriteBatchOp(void){

    TS_SpriteBatchOp *sbop =  (TS_SpriteBatchOp*)malloc(sizeof(TS_SpriteBatchOp));

    sbop->texture = 0;

    return sbop;
}
void TS_FreeSpriteBatchOp(TS_SpriteBatchOp* op){
    assert(op);
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

void TS_SpriteBatchOpFinalizer(V8FINALIZERARGS) {

    TS_SpriteBatchOp* sbop = (TS_SpriteBatchOp*)parameter;
    TS_FreeSpriteBatchOp(sbop);
    object->Dispose();
}


v8Function SpriteBatchOpGetColor(V8GETTERARGS) {
    union {uint32_t i; char c[4];} color;
    color.i = (GET_ACCESSOR_SELF(TS_SpriteBatchOp*)->Color[0]);
    TS_Color *c = new TS_Color(color.c[0], color.c[1], color.c[2], color.c[3]);
    return TS_SDL_GL_WrapTS_ColorDL(c);
}

void SpriteBatchOpSetColor(V8SETTERARGS) {
    uint32_t *colors = GET_ACCESSOR_SELF(TS_SpriteBatchOp*)->Color;

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(value);

    TS_Color* c = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    for(int i = 0; i<4; i++){
        colors[i] = c->toInt();
    }
}

v8Function NewSpriteBatcher(V8ARGS){

    BEGIN_OBJECT_WRAP_CODE;

    TS_SpriteBatch *sb = new TS_SpriteBatch();

    END_OBJECT_WRAP_CODE(SpriteBatch, sb)

}

v8Function NewSpriteBatchOp(V8ARGS){

    BEGIN_OBJECT_WRAP_CODE;

    TS_SpriteBatchOp *sbop =  TS_CreateSpriteBatchOp();

    END_OBJECT_WRAP_CODE(SpriteBatchOp, sbop)
}

v8Function SpriteBatchGetImages(V8ARGS){

    v8::HandleScope handle_scope;

    const TS_SpriteBatch *sb = GET_SELF(TS_SpriteBatch*);
    size_t num_images = sb->numImages;
    v8::Handle<v8::Array> arr = v8::Array::New(num_images);
    float *texcoordtemp = (float*)malloc(8*(sizeof(float)));
    for(size_t i = 0; i<num_images; i++){

        texcoordtemp[6] = (sb->width-float(sb->coords[i].x))/sb->widthf;
        texcoordtemp[1] = (sb->height-float(sb->coords[i].y))/sb->heightf;

        texcoordtemp[0] = (sb->width-float(sb->coords[i].x))/sb->widthf;
        texcoordtemp[7] = (sb->height-float(sb->coords[i].y+sb->coords[i].h))/sb->heightf;

        texcoordtemp[2] = (sb->width-float(sb->coords[i].x+sb->coords[i].w))/sb->widthf;
        texcoordtemp[5] = (sb->height-float(sb->coords[i].y+sb->coords[i].h))/sb->heightf;

        texcoordtemp[4] = (sb->width-float(sb->coords[i].x+sb->coords[i].w))/sb->widthf;
        texcoordtemp[3] = (sb->height-float(sb->coords[i].y))/sb->heightf;

        arr->Set(i, TS_SDL_GL_MakeV8ImageHandleFromGLTextureCoordDL(sb->coords[i].w, sb->coords[i].h, texcoordtemp, sb->fbotex));

    }
    free(texcoordtemp);
    return handle_scope.Close(arr);
}
