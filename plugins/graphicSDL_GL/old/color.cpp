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
	//Colorproto->Set("toString", v8::FunctionTemplate::New(TS_surfaceBlit));

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

void TS_ColorFinalizer(V8FINALIZERARGS) {
    TS_Color* color = (TS_Color*)parameter;
    delete color;
    object.Dispose();
    object.Clear();
}

v8Function GetRed(V8GETTERARGS) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    int value = static_cast<TS_Color*>(ptr)->red;
    return v8::Integer::New(value);
}

void SetRed(V8SETTERARGS) {
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<TS_Color*>(ptr)->red = value->Int32Value();
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
