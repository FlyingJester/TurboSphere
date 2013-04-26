#include "main.h"
#include "color.h"

DECLARE_OBJECT_TEMPLATES(Color);

void GLColor(TS_Color *color){
    glColor4ub(color->red, color->green, color->blue, color->alpha);
}

void ColorInit(void){
    Colortempl     = *v8::FunctionTemplate::New();
	ColorInsttempl = *Colortempl->InstanceTemplate();
    Colorproto     = *Colortempl->PrototypeTemplate();
    Colortempl->SetClassName(v8::String::New("Color"));

    ColorInsttempl->SetAccessor(v8::String::New("red"), GetRed, SetRed);
	ColorInsttempl->SetAccessor(v8::String::New("green"), GetGreen, SetGreen);
	ColorInsttempl->SetAccessor(v8::String::New("blue"), GetBlue, SetBlue);
	ColorInsttempl->SetAccessor(v8::String::New("alpha"), GetAlpha, SetAlpha);
}

void ColorClose(void){
    Colortempl.Clear();
	ColorInsttempl.Clear();
    Colortempl.Dispose();
	ColorInsttempl.Dispose();

}

v8Function CreateColor(V8ARGS) {
  v8::HandleScope loadcolorscope;

  MINMEMALIGN TS_Color *color = new TS_Color(255, 255, 255, 255);


    if(args.Length()<3){
        THROWERROR("[" PLUGINNAME "] CreateColor Error: Called with fewer than 3 arguments.");
    }

    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);

    int r = args[0]->Int32Value();
    int g = args[1]->Int32Value();
    int b = args[2]->Int32Value();
    int a = 255;
    if(args.Length()>3){
        if(args[3]->IsNumber()){
            a = args[3]->Int32Value();
        }
    }

    color->red      = r;
    color->green    = g;
    color->blue     = b;
    color->alpha    = a;


  	ColorInsttempl->SetInternalFieldCount(1);
	v8::Handle<v8::Function> colorctor = Colortempl->GetFunction();
	v8::Local<v8::Object> colorobj = colorctor->NewInstance();

	v8::Persistent<v8::Object> Pcolorobj = v8::Persistent<v8::Object>::New(colorobj);

    Pcolorobj.MakeWeak(color, TS_ColorFinalizer);
    Pcolorobj->SetAlignedPointerInInternalField(0, color);

    Pcolorobj->GetConstructorName()=v8::String::New("Color");

	return loadcolorscope.Close(Pcolorobj);
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
    object.Dispose();
    object.Clear();
}

v8Function GetRed(V8GETTERARGS) {
    int value = GET_ACCESSOR_SELF(TS_Color*)->red;
    return v8::Integer::New(value);
}

void SetRed(V8SETTERARGS) {
    GET_ACCESSOR_SELF(TS_Color*)->red = value->Int32Value();
}

v8Function GetGreen(V8GETTERARGS) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    int value = static_cast<TS_Color*>(ptr)->green;
    return v8::Integer::New(value);
}

void SetGreen(V8SETTERARGS) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<TS_Color*>(ptr)->green = value->Int32Value();
}


v8Function GetBlue(V8GETTERARGS) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    int value = static_cast<TS_Color*>(ptr)->blue;
    return v8::Integer::New(value);
}

void SetBlue(V8SETTERARGS) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<TS_Color*>(ptr)->blue = value->Int32Value();
}

v8Function GetAlpha(V8GETTERARGS) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    int value = static_cast<TS_Color*>(ptr)->alpha;
    return v8::Integer::New(value);
}

void SetAlpha(V8SETTERARGS) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<TS_Color*>(ptr)->alpha = value->Int32Value();
}
