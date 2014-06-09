#include "shape.hpp"
#include "../color.hpp"
#include <utility>
#include <memory>

using namespace Sapphire;
Turbo::JSObj<Galileo::Shape> Sapphire::ShapeObject;

void TS_ShapeFinalizer(const v8::WeakCallbackData<v8::Object, Galileo::Shape> &args){
    delete args.GetParameter();
    args.GetValue().Clear();
}

Sapphire::PolygonOp::PolygonOp(){
        num_vertices = 0;
}

void Galileo::InitShape(uint32_t ID, v8::Isolate *isol) {
    Sapphire::ShapeObject = Turbo::JSObj<Galileo::Shape> ();

    printf("[" PLUGINNAME "] Info: Shape ID is %x\n", (ID<<16)|(0x0216u));

    ShapeObject.Finalize        = TS_ShapeFinalizer;
    Sapphire::ShapeObject.SetTypeName("Shape");
    Sapphire::ShapeObject.ID              = (ID<<16)|(0x0216u);
    Sapphire::ShapeObject.AddAccessor("texture", Galileo::Script::GetShapeTexture, Galileo::Script::SetShapeTexture);

    Sapphire::ShapeObject.AddToProto("draw", Galileo::Script::DrawShape);

}

void Galileo::Script::GetShapeTexture(Turbo::JSAccessorProperty property, Turbo::JSAccessorGetterInfo info) {
    Galileo::Shape *self = Turbo::GetAccessorSelf<Galileo::Shape>(info);

    Turbo::WrapObject(info, Sapphire::ImageObject, self->image);
    IncImageRef(self->image);
    return;

}

void Galileo::Script::SetShapeTexture(Turbo::JSAccessorProperty property, Turbo::JSValue value, Turbo::JSAccessorSetterInfo info) {

    SynchroEngineAndRender(); //Ensure that if we needed the old state, it will be done.

    Galileo::Shape *self = Turbo::GetAccessorSelf<Galileo::Shape>(info);
    DecImageRef(self->image);

    if(!ImageObject.IsA(value)) {
        info.GetReturnValue().Set(v8::Exception::TypeError(v8::String::NewFromUtf8(iso, "[" PLUGINNAME "] Galileo SetShapeTexture Error: rvalue is not an Image.")));
        return;
    }

    Sapphire::Image *im = static_cast<Sapphire::Image*>(v8::Local<v8::Object>::Cast(value)->GetAlignedPointerFromInternalField(0));
    assert(im);

    IncImageRef(im);
    self->image = im;

}

Turbo::JSFunction Galileo::Script::DrawShape(Turbo::JSArguments args){

    Galileo::Shape *shape = static_cast<Galileo::Shape*>(Turbo::GetSelf(args));

    assert(shape);

    PushDrawingOp(shape);

}

Turbo::JSFunction Galileo::Script::CreateShape(Turbo::JSArguments args){

    v8::Local<v8::Value> xprop = v8::String::NewFromUtf8(iso, "x");
    v8::Local<v8::Value> yprop = v8::String::NewFromUtf8(iso, "y");
    v8::Local<v8::Value> cprop = v8::String::NewFromUtf8(iso, "color");

    int sig[] = {Turbo::Array,Turbo::Object,0};

    if(!Turbo::CheckArg::CheckSig(args, 2, sig, true))
        return;

    if(!ImageObject.IsA(args[1])){
        args.GetReturnValue().Set(v8::Exception::TypeError(v8::String::NewFromUtf8(iso, "[" PLUGINNAME "] Galileo CreateShape Error: Argument 1 is not an Image.")));
        return;
    }

    Sapphire::Image *im = static_cast<Sapphire::Image*>(v8::Local<v8::Object>::Cast(args[1])->GetAlignedPointerFromInternalField(0));
    IncImageRef(im);

    const v8::Local<v8::Array> vertexarray = v8::Local<v8::Array>::Cast(args[0]);

    const int num_vertices = vertexarray->Length();

    std::unique_ptr<int[]> xp = std::unique_ptr<int[]> (new int[num_vertices]);
    std::unique_ptr<int[]> yp = std::unique_ptr<int[]> (new int[num_vertices]);
    std::unique_ptr<RGBA[]> cp = std::unique_ptr<RGBA[]> (new RGBA[num_vertices]);

    RGBA w = 0;
    w = ~w;

    // Validate and encode all the vertices to x and y point arrays.
    for(int i = 0; i<num_vertices; i++){
        const v8::Local<v8::Object> vertex = (vertexarray->Get(i))->ToObject();
        if((!vertex->Has(xprop))||(!vertex->Has(yprop))){

            char n[9];
            sprintf(n, "%i", i);
            std::string err = "[" PLUGINNAME "] Galileo CreateShape Error: Element ";
            err += n;
            err += " of Argument 0 is not a valid Vertex.";

            args.GetReturnValue().Set(v8::Exception::TypeError(v8::String::NewFromUtf8(iso, err.c_str())));
            return;
        }

        xp.get()[i] = vertex->Get(xprop)->Int32Value();
        yp.get()[i] = vertex->Get(yprop)->Int32Value();

        if(!vertex->Has(cprop)) //For efficiency with purely textured primitives.
            cp.get()[i] = w;
        else{
            if(!ColorObject.IsA(vertex->Get(cprop))){
                char n[9];
                sprintf(n, "%i", i);
                std::string err = "[" PLUGINNAME "] Galileo CreateShape Error: Element ";
                err += n;
                err += " of Argument 0 has an invalid color.";

                args.GetReturnValue().Set(v8::Exception::TypeError(v8::String::NewFromUtf8(iso, err.c_str())));
                return;
            }
            TS_Color *color = static_cast<TS_Color*>(v8::Local<v8::Object>::Cast(vertex->Get(cprop))->GetAlignedPointerFromInternalField(0));
            cp.get()[i] = color->toInt();
        }
    }

    Sapphire::PolygonOp *shape = new Sapphire::PolygonOp(num_vertices, xp.get(), yp.get(), cp.get(), im);

    Turbo::WrapObject(args, Sapphire::ShapeObject, shape);

}
