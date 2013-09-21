#include "main.h"
#include "color.h"
#include <climits>
#include <cassert>

DECLARE_OBJECT_TEMPLATES(Color);

const uint64_t ColorGLMagic = 0x0707464C; //Bell, Bell, G, L.

void GLColor(TS_Color *color){
    glColor4ub(color->red, color->green, color->blue, color->alpha);
}

inline unsigned char CleanseColorComponent(int val){
    int ret = val;
    if(ret<0)
        ret=0;
    else if(ret>UCHAR_MAX)
        ret=UCHAR_MAX;

    return ret;
}

void ColorInit(void){

    INIT_OBJECT_TEMPLATES(Color);
    SET_CLASS_NAME(Color, "Color");

    ColorInsttempl->SetAccessor(v8::String::New("red"), GetRed, SetRed);
	ColorInsttempl->SetAccessor(v8::String::New("green"), GetGreen, SetGreen);
	ColorInsttempl->SetAccessor(v8::String::New("blue"), GetBlue, SetBlue);
	ColorInsttempl->SetAccessor(v8::String::New("alpha"), GetAlpha, SetAlpha);
}

void ColorClose(void){
    //Colortempl.Clear();
	//ColorInsttempl.Clear();
    //Colortempl.Dispose();
	//ColorInsttempl.Dispose();

}

v8Function CreateColor(V8ARGS) {
    BEGIN_OBJECT_WRAP_CODE

    if(args.Length()<3){
        THROWERROR("[" PLUGINNAME "] CreateColor Error: Called with fewer than 3 arguments.");
    }

    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);

    int r = CleanseColorComponent(args[0]->Int32Value());
    int g = CleanseColorComponent(args[1]->Int32Value());
    int b = CleanseColorComponent(args[2]->Int32Value());
    int a = 255;
    if(args.Length()>3){
        CHECK_ARG_INT(3);
        a = CleanseColorComponent(args[3]->Int32Value());
    }


    MINMEMALIGN TS_Color *color = new TS_Color(r, g, b, a);

    color->reserved = (ColorGLMagic<<32);

    END_OBJECT_WRAP_CODE(Color, color);
}

v8::Local<v8::Object> TS_SDL_GL_WrapTS_Color(TS_Color *c){
    assert(c!=NULL);
    BEGIN_OBJECT_WRAP_CODE;
    END_OBJECT_WRAP_CODE(Color, c);
}


TS_Color *TS_BlendColorsWeighted(TS_Color *c1, TS_Color *c2, int w1, int w2){

    if(w2<=0){
        return new TS_Color(c1->red, c1->green, c1->blue, c1->alpha);
    }

    if(w1<=0){
        return new TS_Color(c2->red, c2->green, c2->blue, c2->alpha);
    }

    unsigned int r = (c1->red*w1)+(c2->red*w2);
    unsigned int g = (c1->green*w1)+(c2->green*w2);
    unsigned int b = (c1->blue*w1)+(c2->blue*w2);
    unsigned int a = (c1->alpha*w1)+(c2->alpha*w2);

    r/=w1+w2;
    g/=w1+w2;
    b/=w1+w2;
    a/=w1+w2;

    uint8_t r8 = r;
    uint8_t g8 = g;
    uint8_t b8 = b;
    uint8_t a8 = a;

    return new TS_Color(r8, g8, b8, a8);

}

TS_Color *TS_BlendColorsWeighted(TS_Color *c1, TS_Color *c2, double w1, double w2){

    if(w2<=0){
        return new TS_Color(c1->red, c1->green, c1->blue, c1->alpha);
    }

    if(w1<=0){
        return new TS_Color(c2->red, c2->green, c2->blue, c2->alpha);
    }

    double r = (c1->red*w1)+(c2->red*w2);
    double g = (c1->green*w1)+(c2->green*w2);
    double b = (c1->blue*w1)+(c2->blue*w2);
    double a = (c1->alpha*w1)+(c2->alpha*w2);

    r/=w1+w2;
    g/=w1+w2;
    b/=w1+w2;
    a/=w1+w2;

    uint8_t r8 = (uint8_t) r;
    uint8_t g8 = (uint8_t) g;
    uint8_t b8 = (uint8_t) b;
    uint8_t a8 = (uint8_t) a;

    return new TS_Color(r8, g8, b8, a8);

}

void TS_ColorFinalizer(V8FINALIZERARGS) {
    TS_Color* color = (TS_Color*)parameter;
    delete color;
    object->Dispose();
}

v8Function GetRed(V8GETTERARGS) {
    int value = GET_ACCESSOR_SELF(TS_Color*)->red;
    return v8::Integer::New(value);
}

void SetRed(V8SETTERARGS) {
    auto r = CleanseColorComponent(value->Int32Value());
    GET_ACCESSOR_SELF(TS_Color*)->red = r;
}

v8Function GetGreen(V8GETTERARGS) {
    int value = GET_ACCESSOR_SELF(TS_Color*)->green;
    return v8::Integer::New(value);
}

void SetGreen(V8SETTERARGS) {
    auto g = CleanseColorComponent(value->Int32Value());
    GET_ACCESSOR_SELF(TS_Color*)->green = g;
}


v8Function GetBlue(V8GETTERARGS) {
    int value = GET_ACCESSOR_SELF(TS_Color*)->blue;
    return v8::Integer::New(value);
}

void SetBlue(V8SETTERARGS) {
    auto b = CleanseColorComponent(value->Int32Value());
    GET_ACCESSOR_SELF(TS_Color*)->blue = b;
}

v8Function GetAlpha(V8GETTERARGS) {
    int value = GET_ACCESSOR_SELF(TS_Color*)->alpha;
    return v8::Integer::New(value);
}

void SetAlpha(V8SETTERARGS) {
    auto a = CleanseColorComponent(value->Int32Value());
    GET_ACCESSOR_SELF(TS_Color*)->alpha = a;
}
