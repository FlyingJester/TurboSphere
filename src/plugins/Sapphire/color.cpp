#include "color.hpp"

Turbo::JSObj<TS_Color> ColorObject;

using Sapphire::Color::CleanseColorComponent;
using Sapphire::Color::ColorComponent;

void TS_ColorFinalizer(const v8::WeakCallbackData<v8::Object, TS_Color> &args){
    delete args.GetParameter();
    args.GetValue().Clear();
}

void ColorRedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
void ColorGreenSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
void ColorBlueSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
void ColorAlphaSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

void ColorRedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
void ColorGreenGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
void ColorBlueGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
void ColorAlphaGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);


Turbo::JSFunction CreateColor(Turbo::JSArguments args){

    int sig[] = {Turbo::Int,Turbo::Int,Turbo::Int,Turbo::Int,0};

    if(!Turbo::CheckArg::CheckSig(args, 3, sig, true))
        return;

    unsigned char alpha = 0xFF;

    if(!Turbo::CheckArg::CheckSig(args, 4, sig, false))
        alpha = CleanseColorComponent(args[3]->IntegerValue());


    TS_Color *color = new TS_Color(CleanseColorComponent(args[0]->IntegerValue()), CleanseColorComponent(args[1]->IntegerValue()), CleanseColorComponent(args[2]->IntegerValue()), alpha);

    Turbo::WrapObject(args, ColorObject, color);

    printf("[" PLUGINNAME "] Info: Making an object as %x.\n", ColorObject.ID);



}


Turbo::JSFunction ColorToInt(Turbo::JSArguments args){

    int sig[] = {Turbo::Object,0};
    if(!Turbo::CheckArg::CheckSig(args, 1, sig, true))
        return;

    TS_Color *c = NULL;
    assert(ColorObject.IsA(args[0]));

    v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[0]);
    c = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    printf("Color was %x\n", c->toInt());

}

Turbo::JSFunction RedMember(Turbo::JSArguments args){

    args.GetReturnValue().Set((uint32_t)(255));
}

void InitColor(uint32_t ID){

    printf("[" PLUGINNAME "] Info: Color ID is %x\n", (ID<<16)|(0xFE24u));

    ColorObject                 = Turbo::JSObj<TS_Color>();
    ColorObject.Finalize        = TS_ColorFinalizer;
    ColorObject.SetTypeName("Color");
    ColorObject.ID              = (ID<<16)|(0xFE24u);
    ColorObject.AddAccessor("red",   ColorRedGetter,    ColorRedSetter);
    ColorObject.AddAccessor("green", ColorGreenGetter,  ColorGreenSetter);
    ColorObject.AddAccessor("blue",  ColorBlueGetter,   ColorBlueSetter);
    ColorObject.AddAccessor("alpha", ColorAlphaGetter,  ColorAlphaSetter);
    ColorObject.AddToProto("RedMember", RedMember);
}

/////
// Color Object accessor getters
//

void ColorRedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info){

    assert(info.Holder()->InternalFieldCount()>=2);
    assert(info.Holder()->GetInternalField(1)->Uint32Value()==ColorObject.ID);

    printf("Lol\n");

    info.GetReturnValue().Set((uint32_t)((Turbo::GetAccessorSelf<TS_Color>(info))->red));
}
void ColorGreenGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info){

    assert(info.Holder()->InternalFieldCount()>=2);
    assert(info.Holder()->GetInternalField(1)->Uint32Value()==ColorObject.ID);

    info.GetReturnValue().Set((uint32_t)((Turbo::GetAccessorSelf<TS_Color>(info))->green));
}
void ColorBlueGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info){

    assert(info.Holder()->InternalFieldCount()>=2);
    assert(info.Holder()->GetInternalField(1)->Uint32Value()==ColorObject.ID);

    info.GetReturnValue().Set((uint32_t)((Turbo::GetAccessorSelf<TS_Color>(info))->blue));
}
void ColorAlphaGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info){

    assert(info.Holder()->InternalFieldCount()>=2);
    assert(info.Holder()->GetInternalField(1)->Uint32Value()==ColorObject.ID);

    info.GetReturnValue().Set((uint32_t)((Turbo::GetAccessorSelf<TS_Color>(info))->alpha));
}

/////
// Color Object accessor setters
//

void ColorRedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info){

    assert(info.Holder()->InternalFieldCount()>=2);
    assert(info.Holder()->GetInternalField(1)->Uint32Value()==ColorObject.ID);

    if(!value->IsNumber())
        info.GetReturnValue().Set(v8::Exception::TypeError(v8::String::NewFromUtf8(iso, "[" PLUGINNAME "] ColorRedSetter Error: Value is not a number.")));
    else
        (Turbo::GetAccessorSelf<TS_Color>(info))->red = CleanseColorComponent(value->IntegerValue());
}
void ColorGreenSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info){

    assert(info.Holder()->InternalFieldCount()>=2);
    assert(info.Holder()->GetInternalField(1)->Uint32Value()==ColorObject.ID);

    if(!value->IsNumber())
        info.GetReturnValue().Set(v8::Exception::TypeError(v8::String::NewFromUtf8(iso, "[" PLUGINNAME "] ColorGreenSetter Error: Value is not a number.")));
    else
        (Turbo::GetAccessorSelf<TS_Color>(info))->green = CleanseColorComponent(value->IntegerValue());
}
void ColorBlueSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info){

    assert(info.Holder()->InternalFieldCount()>=2);
    assert(info.Holder()->GetInternalField(1)->Uint32Value()==ColorObject.ID);

    if(!value->IsNumber())
        info.GetReturnValue().Set(v8::Exception::TypeError(v8::String::NewFromUtf8(iso, "[" PLUGINNAME "] ColorBlueSetter Error: Value is not a number.")));
    else
        (Turbo::GetAccessorSelf<TS_Color>(info))->blue = CleanseColorComponent(value->IntegerValue());
}
void ColorAlphaSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info){

    assert(info.Holder()->InternalFieldCount()>=2);
    assert(info.Holder()->GetInternalField(1)->Uint32Value()==ColorObject.ID);

    if(!value->IsNumber())
        info.GetReturnValue().Set(v8::Exception::TypeError(v8::String::NewFromUtf8(iso, "[" PLUGINNAME "] ColorAlphaSetter Error: Value is not a number.")));
    else
        (Turbo::GetAccessorSelf<TS_Color>(info))->alpha = CleanseColorComponent(value->IntegerValue());
}
