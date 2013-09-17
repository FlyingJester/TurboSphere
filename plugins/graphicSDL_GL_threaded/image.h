#ifndef SDL_GL_IMAGE_HEAD
#define SDL_GL_IMAGE_HEAD

typedef GLuint TS_Texture;
typedef GLuint TS_TexVertexBuffer;
typedef GLuint TS_TextureBuffer;
/*
void (APIENTRY * glGenBuffers)(GLsizei, GLuint*);
void (APIENTRY * glBindBuffer)(GLenum,  GLuint);
void (APIENTRY * glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum);
*/


extern TS_Color *fullmask;

EXTERN_OBJECT_TEMPLATES(Image);

void ImageInit(void);
void ImageClose(void);

class MINMEMALIGN TS_Image {
public:
    TS_Image(TS_Texture, int, int);
    TS_Image();
    ~TS_Image();
    TS_Texture texture;
    int width;
    int height;
    double dwidth;  //double width to avoid casts.
    double dheight; //double height to avoid casts.
    int diaglength; //distance from the center to a corner of the image.

    TS_Color *mask;

    TS_TextureBuffer pixelBuffer;
    GLuint texCoordBuffer;

    SDL_Surface *CreateSurface(void);
    TS_Image *Clone(void);

    void resetMask(void);

    void blit(int, int);
    void stretchBlit(int x, int y, double wfactor, double hfactor);
    void zoomBlit(int x, int y, double factor);
    void rotateBlit(int x, int y, double angle);
    void transformBlit(int x[4], int y[4]);

    TS_Color *getMask(void);
    void setMask(TS_Color c);

    GLuint buffer;

private:
    bool hasInternalMask;
};

TS_Image *TS_ImageGrab(int x, int y, int w, int h);

v8Function CreateImage(V8ARGS);
v8Function LoadImage(V8ARGS);
v8Function ImageBlit(V8ARGS);
v8Function ImageZoomBlit(V8ARGS);
v8Function ImageStretchBlit(V8ARGS);
v8Function ImageRotateBlit(V8ARGS);
v8Function ImageTransformBlit(V8ARGS);
v8Function ImageBlitMask(V8ARGS);
v8Function ImageZoomBlitMask(V8ARGS);
v8Function ImageStretchBlitMask(V8ARGS);
v8Function ImageRotateBlitMask(V8ARGS);
v8Function ImageTransformBlitMask(V8ARGS);

v8Function ImageToSurface(V8ARGS);
v8Function ImageClone(V8ARGS);
v8Function ImageGrab(V8ARGS);

v8Function ImageSave(V8ARGS);

void TS_ImageFinalizer(V8FINALIZERARGS);

v8Function ImageGetWidth(V8GETTERARGS);
void ImageSetWidth(V8SETTERARGS);
v8Function ImageGetHeight(V8GETTERARGS);
void ImageSetHeight(V8SETTERARGS);

#endif
