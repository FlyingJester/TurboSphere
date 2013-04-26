#include "main.h"
#include "image.h"
#include "color.h"
#include <assert.h>
#include <math.h>


//pixel buffer object constants
#define PIXEL_PACK_BUFFER_EXT           0x88EB
#define PIXEL_UNPACK_BUFFER_EXT         0x88EC
#define PIXEL_PACK_BUFFER_BINDING_EXT   0x88ED
#define PIXEL_UNPACK_BUFFER_BINDING_EXT 0x88EF


DECLARE_OBJECT_TEMPLATES(Image);

TS_Color *fullmask;
GLint *texcoord;

static void (APIENTRY * glGenBuffers)(GLsizei, GLuint*) = NULL;
static void (APIENTRY * glBindBuffer)(GLenum,  GLuint) = NULL;
static void (APIENTRY * glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum) = NULL;


//void glGenBuffers(GLsizei n, GLuint *ids);

void ImageInit(void){
    fullmask = new TS_Color(0xFF, 0xFF, 0xFF, 0xFF);

    texcoord = (GLint*)calloc(8, sizeof(GLint));
    texcoord[2] = 1;
    texcoord[4] = 1;
    texcoord[5] = 1;
    texcoord[7] = 1;

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_INT, 0, texcoord);
//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    INIT_OBJECT_TEMPLATES(Image);
    SET_CLASS_NAME(Image, "Image");

    ADD_TO_PROTO(Image, "blit",             ImageBlit);
    ADD_TO_PROTO(Image, "zoomBlit",         ImageZoomBlit);
    ADD_TO_PROTO(Image, "stretchBlit",      ImageStretchBlit);
    ADD_TO_PROTO(Image, "rotateBlit",       ImageRotateBlit);
    ADD_TO_PROTO(Image, "transformBlit",    ImageTransformBlit);
    ADD_TO_PROTO(Image, "blitMask",         ImageBlitMask);
    ADD_TO_PROTO(Image, "zoomBlitMask",     ImageZoomBlitMask);
    ADD_TO_PROTO(Image, "stretchBlitMask",  ImageStretchBlitMask);
    ADD_TO_PROTO(Image, "rotateBlitMask",   ImageRotateBlitMask);
    ADD_TO_PROTO(Image, "transformBlitMask",ImageTransformBlitMask);

    ADD_TO_PROTO(Image, "createSurface",    ImageToSurface);
    ADD_TO_PROTO(Image, "clone",            ImageClone);

    ADD_TO_PROTO(Image, "save",             ImageSave);

    SET_CLASS_ACCESSOR(Image, "width",    ImageGetWidth,    ImageSetWidth);
    SET_CLASS_ACCESSOR(Image, "height",   ImageGetHeight,   ImageSetHeight);

    char *extensions = (char *)glGetString(GL_EXTENSIONS);
    if ((SDL_GL_GetProcAddress("glGenBuffers")!=NULL)&&(strstr(extensions, "GL_EXT_pixel_buffer_object"))){
        glGenBuffers = (void (APIENTRY *)(GLsizei, GLuint*)) SDL_GL_GetProcAddress("glGenBuffers");
        glBindBuffer = (void (APIENTRY *)(GLenum, GLuint))   SDL_GL_GetProcAddress("glBindBuffer");
        glBufferData = (void (APIENTRY *)(GLenum, GLsizeiptr, const GLvoid *, GLenum))   SDL_GL_GetProcAddress("glBufferData");
    }
    else{
        printf("[" PLUGINNAME "] Error: GL_EXT_pixel_buffer_object is not present.\n");
        exit(3);
    }

}

void ImageClose(){}

v8Function ImageGetWidth(V8GETTERARGS) {
    int value = GET_ACCESSOR_SELF(TS_Image*)->width;
    return v8::Integer::New(value);
}

void ImageSetWidth(V8SETTERARGS) {
//    GET_SELF(SDL_Surface*)->w = value->Int32Value();
}

v8Function ImageGetHeight(V8GETTERARGS) {
    int value = GET_ACCESSOR_SELF(TS_Image*)->height;
    return v8::Integer::New(value);
}

void ImageSetHeight(V8SETTERARGS) {
//    GET_SELF(SDL_Surface*)->h = value->Int32Value();
}


TS_Image::TS_Image(TS_Texture tex, int w, int h){
    texture = tex;
    assert(w>0);
    assert(h>0);
    width   = w;
    height  = h;
    dwidth  = w;
    dheight = h;
    diaglength = (int)ceil(sqrt(float(w*w)+float(h*h))/2);

    this->resetMask();

}

TS_Color *TS_Image::getMask(void){
    return mask;
}

void TS_Image::setMask(TS_Color c){
    hasInternalMask = true;
    mask = new TS_Color(c.red, c.blue, c.green, c.alpha);
}

SDL_Surface *TS_Image::CreateSurface(){

    MINMEMALIGN SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, width, height, DEPTH, CHANNEL_MASKS);

    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    return surface;
}

TS_Image *TS_ImageGrab(int x, int y, int w, int h){
    TS_Texture texture;

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glCopyTexImage2D(texture, 0, GL_RGBA, x, y, w, h, 0);
    MINMEMALIGN TS_Image *image = new TS_Image(texture, w, h);
    return image;
}

v8Function ImageToSurface(V8ARGS){
    BEGIN_OBJECT_WRAP_CODE;
    MINMEMALIGN SDL_Surface *surface = GET_SELF(TS_Image*)->CreateSurface();
    END_OBJECT_WRAP_CODE(Surface, surface);
}

v8Function ImageSave(V8ARGS){

    if(args.Length()<1){
        THROWERROR("[" PLUGINNAME "] ImageSave Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);

    TS_Directories *TS_dirs = GetDirs();

    MINMEMALIGN SDL_Surface *surface = GET_SELF(TS_Image*)->CreateSurface();

    v8::String::Utf8Value str(args[0]);
    const char *filename = string(TS_dirs->image).append(*str).c_str();
    if(SDL_SaveBMP(surface, filename)!=0){
        printf("[" PLUGINNAME "] ImageSave Error: Could not save image %s: %s\n", *str, SDL_GetError());
        SDL_ClearError();
        SDL_FreeSurface(surface);
        THROWERROR("[" PLUGINNAME "] ImageSave Error: Could not save image.");
    }

    SDL_FreeSurface(surface);
    return v8::Undefined();
}

v8Function ImageClone(V8ARGS){
    BEGIN_OBJECT_WRAP_CODE;

    MINMEMALIGN TS_Image *image = GET_SELF(TS_Image*)->Clone();

    END_OBJECT_WRAP_CODE(Image, image);
}

v8Function ImageGrab(V8ARGS){
    if(args.Length()<4){
        THROWERROR("[" PLUGINNAME "] ImageGrab Error: Called with fewer than 4 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);

    BEGIN_OBJECT_WRAP_CODE;
    int x = args[0]->Int32Value();
    int y = args[1]->Int32Value();
    int w = args[2]->Int32Value();
    int h = args[3]->Int32Value();

    if(w<0){
        x+=w;
        w=-w;
    }
    if(h<0){
        y+=h;
        h=-h;
    }

    MINMEMALIGN TS_Image *image = TS_ImageGrab(x, y, w, h);

    END_OBJECT_WRAP_CODE(Image, image);
}

TS_Image::~TS_Image(){
    glDeleteTextures(1, &texture);
    if (((mask!=fullmask)||hasInternalMask)&&(mask!=NULL)){
        delete mask;
    }
}

void TS_Image::resetMask(void){
    mask = fullmask;
    hasInternalMask = false;
}

void TS_ImageFinalizer(V8FINALIZERARGS) {

    TS_Image* im = (TS_Image*)parameter;
    delete im;
    object.Dispose();
}

TS_Image *TS_LoadTexture(const char *filename){

    TS_Texture texture;
    MINMEMALIGN SDL_Surface *surface = NULL;


    if(!(surface = IMG_Load(filename))){
        return NULL;
    }

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_FreeSurface(surface);

    MINMEMALIGN TS_Image* image = new TS_Image(texture, surface->w, surface->h);

    return image;
}

void TS_Image::blit(int x, int y){

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
        GLColor(mask);
        glTexCoord2i(0, 0);
        glVertex2i(x,       y);
        glTexCoord2i(1, 0);
        glVertex2i(x+width, y);
        glTexCoord2i(1, 1);
        glVertex2i(x+width, y+height);
        glTexCoord2i(0, 1);
        glVertex2i(x,       y+height);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

TS_Image *TS_Image::Clone(void){
    //BAD! Replace with FBO based cloning.
    uint32_t *pixels = (uint32_t *)calloc(width*height, sizeof(uint32_t));

    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    TS_Texture newtexture;
    glGenTextures(1, &newtexture);

    glBindTexture(GL_TEXTURE_2D, newtexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    free(pixels);

    MINMEMALIGN TS_Image *image = new TS_Image(texture, width, height);

    return image;

}

void TS_Image::zoomBlit(int x, int y, double factor){

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    double scaleWidth   = factor*dwidth;
    double scaleHeight  = factor*dheight;
    glBegin(GL_QUADS);
        GLColor(mask);
        glTexCoord2i(0, 0);
        glVertex2i(x,                   y);
        glTexCoord2i(1, 0);
        glVertex2i((int)(x+scaleWidth), y);
        glTexCoord2i(1, 1);
        glVertex2i((int)(x+scaleWidth), (int)(y+scaleHeight));
        glTexCoord2i(0, 1);
        glVertex2i(x,                   (int)(y+scaleHeight));
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void TS_Image::stretchBlit(int x, int y, double wfactor, double hfactor){

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    double scaleWidth   = hfactor*dwidth;
    double scaleHeight  = wfactor*dheight;
    glBegin(GL_QUADS);
        GLColor(mask);
        glTexCoord2i(0, 0);
        glVertex2i(x,                   y);
        glTexCoord2i(1, 0);
        glVertex2i((int)(x+scaleWidth), y);
        glTexCoord2i(1, 1);
        glVertex2i((int)(x+scaleWidth), (int)(y+scaleHeight));
        glTexCoord2i(0, 1);
        glVertex2i(x,                   (int)(y+scaleHeight));
    glEnd();
    glDisable(GL_TEXTURE_2D);
}


void TS_Image::rotateBlit(int x, int y, double angle){

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    int xmod1 = (int)ceil(cos(angle)*diaglength);
    int xmod2 = (int)ceil(cos(angle+1.5707)*diaglength);
    int xmod3 = (int)ceil(cos(angle+3.1415)*diaglength);
    int xmod4 = (int)ceil(cos(angle+4.7123)*diaglength);
    int ymod1 = (int)ceil(sin(angle+3.1415)*diaglength);
    int ymod2 = (int)ceil(sin(angle+4.7123)*diaglength);
    int ymod3 = (int)ceil(sin(angle)*diaglength);
    int ymod4 = (int)ceil(sin(angle+1.5707)*diaglength);
    glBegin(GL_QUADS);
        GLColor(mask);
        glTexCoord2i(0, 0);
        glVertex2i(x+xmod1, y+ymod1);
        glTexCoord2i(1, 0);
        glVertex2i(x+xmod2, y+ymod2);
        glTexCoord2i(1, 1);
        glVertex2i(x+xmod3, y+ymod3);
        glTexCoord2i(0, 1);
        glVertex2i(x+xmod4, y+ymod4);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void TS_Image::transformBlit(int x[4], int y[4]){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
        GLColor(mask);
        glTexCoord2i(0, 0);
        glVertex2i(x[0], y[0]);
        glTexCoord2i(1, 0);
        glVertex2i(x[1], y[1]);
        glTexCoord2i(1, 1);
        glVertex2i(x[2], y[2]);
        glTexCoord2i(0, 1);
        glVertex2i(x[3], y[3]);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

v8Function LoadImage(V8ARGS){
    if(args.Length()<1){
        THROWERROR("[" PLUGINNAME "] LoadImage Error: Called with no argument.");
    }
    CHECK_ARG_STR(0);

    BEGIN_OBJECT_WRAP_CODE;

    TS_Directories *TS_dirs = GetDirs();

    v8::String::Utf8Value str(args[0]);
    TS_Image *im = TS_LoadTexture(string(TS_dirs->image).append(*str).c_str());

    END_OBJECT_WRAP_CODE(Image, im);
}

v8Function ImageBlit(V8ARGS){
    if(args.Length()<2){
        THROWERROR("[" PLUGINNAME "] ImageBlit Error: Called with fewer than 2 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);

	int x = args[0]->Int32Value();
	int y = args[1]->Int32Value();

	GET_SELF(TS_Image*)->blit(x,y);

    return v8::Undefined();
}

v8Function ImageZoomBlit(V8ARGS){
    if(args.Length()<3){
        THROWERROR("[" PLUGINNAME "] ImageZoomBlit Error: Called with fewer than 3 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);

	int     x = args[0]->Int32Value();
	int     y = args[1]->Int32Value();
	double  f = args[2]->NumberValue();

	GET_SELF(TS_Image*)->zoomBlit(x,y,f);

    return v8::Undefined();
}


v8Function ImageStretchBlit(V8ARGS){
    if(args.Length()<4){
        THROWERROR("[" PLUGINNAME "] ImageStretchBlit Error: Called with fewer than 4 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);

	int     x = args[0]->Int32Value();
	int     y = args[1]->Int32Value();
	double  wf= args[2]->NumberValue();
	double  hf= args[3]->NumberValue();

	GET_SELF(TS_Image*)->stretchBlit(x,y,wf,hf);

    return v8::Undefined();
}

v8Function ImageRotateBlit(V8ARGS){
    if(args.Length()<3){
        THROWERROR("[" PLUGINNAME "] ImageRotateBlit Error: Called with fewer than 3 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);

	int     x = args[0]->Int32Value();
	int     y = args[1]->Int32Value();
	double  a = args[2]->NumberValue();

    a+=0.78539816339;

	GET_SELF(TS_Image*)->rotateBlit(x,y,a);

    return v8::Undefined();
}

v8Function ImageTransformBlit(V8ARGS){
    if(args.Length()<8){
        THROWERROR("[" PLUGINNAME "] ImageTransformBlit Error: Called with fewer than 8 arguments.");
    }

    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);
    CHECK_ARG_INT(4);
    CHECK_ARG_INT(5);
    CHECK_ARG_INT(6);
    CHECK_ARG_INT(7);

    int x[4] = {args[0]->Int32Value(), args[2]->Int32Value(), args[4]->Int32Value(), args[6]->Int32Value()};
    int y[4] = {args[1]->Int32Value(), args[3]->Int32Value(), args[5]->Int32Value(), args[7]->Int32Value()};

	GET_SELF(TS_Image*)->transformBlit(x, y);

    return v8::Undefined();
}


v8Function ImageBlitMask(V8ARGS){
    if(args.Length()<3){
        THROWERROR("[" PLUGINNAME "] ImageBlit Error: Called with fewer than 3 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_OBJ(2);

	int x = args[0]->Int32Value();
	int y = args[1]->Int32Value();

	v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[2]);
    TS_Color *color = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    TS_Color *originalMask = GET_SELF(TS_Image*)->mask;

	GET_SELF(TS_Image*)->mask = color;
	GET_SELF(TS_Image*)->blit(x,y);
	GET_SELF(TS_Image*)->mask = originalMask;

    return v8::Undefined();
}

v8Function ImageZoomBlitMask(V8ARGS){
    if(args.Length()<4){
        THROWERROR("[" PLUGINNAME "] ImageZoomBlitMask Error: Called with fewer than 4 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_OBJ(3);

	int     x = args[0]->Int32Value();
	int     y = args[1]->Int32Value();
	double  f = args[2]->NumberValue();

	v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[3]);
    TS_Color *color = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    TS_Color *originalMask = GET_SELF(TS_Image*)->mask;

	GET_SELF(TS_Image*)->mask = color;
	GET_SELF(TS_Image*)->zoomBlit(x,y,f);
	GET_SELF(TS_Image*)->mask = originalMask;

    return v8::Undefined();
}


v8Function ImageStretchBlitMask(V8ARGS){
    if(args.Length()<5){
        THROWERROR("[" PLUGINNAME "] ImageStretchBlitMask Error: Called with fewer than 5 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);
    CHECK_ARG_OBJ(4);

	int     x = args[0]->Int32Value();
	int     y = args[1]->Int32Value();
	double  wf= args[2]->NumberValue();
	double  hf= args[3]->NumberValue();

	v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[4]);
    TS_Color *color = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    TS_Color *originalMask = GET_SELF(TS_Image*)->mask;

	GET_SELF(TS_Image*)->mask = color;
	GET_SELF(TS_Image*)->stretchBlit(x,y,wf,hf);
	GET_SELF(TS_Image*)->mask = originalMask;

    return v8::Undefined();
}

v8Function ImageRotateBlitMask(V8ARGS){
    if(args.Length()<4){
        THROWERROR("[" PLUGINNAME "] ImageRotateBlitMask Error: Called with fewer than 4 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_OBJ(3);

	int     x = args[0]->Int32Value();
	int     y = args[1]->Int32Value();
	double  a = args[2]->NumberValue();
    a+=0.78539816339;
	v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[3]);
    TS_Color *color = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    TS_Color *originalMask = GET_SELF(TS_Image*)->mask;

	GET_SELF(TS_Image*)->mask = color;
	GET_SELF(TS_Image*)->rotateBlit(x,y,a);
	GET_SELF(TS_Image*)->mask = originalMask;

    return v8::Undefined();
}

v8Function ImageTransformBlitMask(V8ARGS){
    if(args.Length()<9){
        THROWERROR("[" PLUGINNAME "] ImageTransformBlitMask Error: Called with fewer than 9 arguments.");
    }

    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);
    CHECK_ARG_INT(4);
    CHECK_ARG_INT(5);
    CHECK_ARG_INT(6);
    CHECK_ARG_INT(7);
    CHECK_ARG_OBJ(8);

    int x[4] = {args[0]->Int32Value(), args[2]->Int32Value(), args[4]->Int32Value(), args[6]->Int32Value()};
    int y[4] = {args[1]->Int32Value(), args[3]->Int32Value(), args[5]->Int32Value(), args[7]->Int32Value()};

	v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[8]);
    TS_Color *color = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    TS_Color *originalMask = GET_SELF(TS_Image*)->mask;

	GET_SELF(TS_Image*)->mask = color;
	GET_SELF(TS_Image*)->transformBlit(x, y);
	GET_SELF(TS_Image*)->mask = originalMask;

    return v8::Undefined();
}
