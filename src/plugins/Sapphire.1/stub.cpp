#include "api.h"
#include <string>

using namespace Script;

    /////
    // Screen functions moved to screen/screen_frontend.cpp

    /*
    Turbo::JSFunction Script::FlipScreen(Turbo::JSArguments args){

    }


    Turbo::JSFunction Script::GetFrameRate(Turbo::JSArguments args){

        args.GetReturnValue().Set(v8::Integer::New(v8::Isolate::GetCurrent(), 0));

    }

    Turbo::JSFunction Script::SetFrameRate(Turbo::JSArguments args){
        int sig[] = {Turbo::Int, 0};

        Turbo::CheckArg::CheckSig(args, 1, sig);

    }



    /////
    // Screen Size

    Turbo::JSFunction Script::GetScreenWidth(Turbo::JSArguments args){

        args.GetReturnValue().Set(v8::Integer::New(v8::Isolate::GetCurrent(), 0));

    }

    Turbo::JSFunction Script::GetScreenHeight(Turbo::JSArguments args){

        args.GetReturnValue().Set(v8::Integer::New(v8::Isolate::GetCurrent(), 0));

    }

    /////
    // Clipping Rectangle

    Turbo::JSFunction Script::GetClippingRectangle(Turbo::JSArguments args){

        auto iso = v8::Isolate::GetCurrent();

        v8::Handle<v8::Object> obj = v8::Object::New(iso);
        obj->Set(v8::String::NewFromUtf8(iso, "x"), v8::Integer::New(iso, 0));
        obj->Set(v8::String::NewFromUtf8(iso, "y"), v8::Integer::New(iso, 0));
        obj->Set(v8::String::NewFromUtf8(iso, "w"), v8::Integer::New(iso, 0));
        obj->Set(v8::String::NewFromUtf8(iso, "h"), v8::Integer::New(iso, 0));

        args.GetReturnValue().Set(v8::Integer::New(iso, 0));

    }

    Turbo::JSFunction Script::SetClippingRectangle(Turbo::JSArguments args){

        int sig[] = {Turbo::Int, Turbo::Int, Turbo::Int, Turbo::Int, 0};

        Turbo::CheckArg::CheckSig(args, 4, sig);

    }
*/
    /////
    // Primititves

    Turbo::JSFunction Script::Point(Turbo::JSArguments args){

        int sig[] = {Turbo::Int, Turbo::Int, Turbo::Object, 0};

        Turbo::CheckArg::CheckSig(args, 3, sig);

        /////
        // TODO: verify color object.

    }

    Turbo::JSFunction Script::PointSeries(Turbo::JSArguments args){

        auto iso = v8::Isolate::GetCurrent();

        Turbo::JSValue XProp = v8::String::NewFromUtf8(iso, "x");
        Turbo::JSValue YProp = v8::String::NewFromUtf8(iso, "y");

        int sig[] = {Turbo::Array, Turbo::Object, 0};

        if(!Turbo::CheckArg::CheckSig(args, 2, sig))
            return;

        v8::Handle<v8::Array> pointarray = v8::Handle<v8::Array>::Cast(args[0]);

        size_t num_points = pointarray->Length();

        if(!num_points)
            return;

        for(size_t i = 0; i<num_points; i++){
            v8::Local<v8::Object> point = (pointarray->Get(i))->ToObject();

            if(point->Has(XProp)&&(point->Has(YProp)))
                continue;

            Turbo::SetError(args, (std::string("[" PLUGINNAME "] ").append(__func__).append("Error: Argument 0, Element ")+std::to_string(i).append(" does not have 'x' and 'y' properties.")).c_str());
            return;
        }

    }

    Turbo::JSFunction Script::StippleLine(Turbo::JSArguments args){}
    Turbo::JSFunction Script::Line(Turbo::JSArguments args){}
    Turbo::JSFunction Script::LineSeries(Turbo::JSArguments args){}
    Turbo::JSFunction Script::GradientLine(Turbo::JSArguments args){}

    Turbo::JSFunction Script::BezierCurve(Turbo::JSArguments args){}

    Turbo::JSFunction Script::Polygon(Turbo::JSArguments args){}

    Turbo::JSFunction Script::Triangle(Turbo::JSArguments args){}
    Turbo::JSFunction Script::GradientTriangle(Turbo::JSArguments args){}
    Turbo::JSFunction Script::OutlinedTriangle(Turbo::JSArguments args){}

    Turbo::JSFunction Script::Rectangle(Turbo::JSArguments args){}
    Turbo::JSFunction Script::GradientRectangle(Turbo::JSArguments args){}
    Turbo::JSFunction Script::OutlinedRectangle(Turbo::JSArguments args){}

    Turbo::JSFunction Script::FilledEllipse(Turbo::JSArguments args){}
    Turbo::JSFunction Script::OutlinedEllipse(Turbo::JSArguments args){}
    Turbo::JSFunction Script::GradientEllipse(Turbo::JSArguments args){}

    Turbo::JSFunction Script::FilledComplex(Turbo::JSArguments args){}
    Turbo::JSFunction Script::OutlinedComplex(Turbo::JSArguments args){}
    Turbo::JSFunction Script::GradientComplex(Turbo::JSArguments args){}

    /////
    // Colors moved to color/color.cpp

    extern Turbo::JSObj<TS_Color>       ColorObject;

    //Turbo::JSFunction Script::CreateColor(Turbo::JSArguments args){}

    /////
    // Color Matrices
    //
    // I don't really understand these. I need to read up on this kind of thing.
    //

    //Turbo::JSFunction Script::CreateColorMatrix(Turbo::JSArguments args){}



    /////
    // Images moved to image/image_frontend.cpp


    // Creation
    Turbo::JSFunction Script::LoadImage(Turbo::JSArguments args);
    Turbo::JSFunction Script::CreateImage(Turbo::JSArguments args);
    Turbo::JSFunction Script::GrabImage(Turbo::JSArguments args);

    // System images
    Turbo::JSFunction Script::GetSystemArrow(Turbo::JSArguments args);
    Turbo::JSFunction Script::GetSystemUpArrow(Turbo::JSArguments args);
    Turbo::JSFunction Script::GetSystemDownArrow(Turbo::JSArguments args);

    /////
    // Surfaces

    Turbo::JSFunction Script::LoadSurface(Turbo::JSArguments args){}
    Turbo::JSFunction Script::CreateSurface(Turbo::JSArguments args){}
    Turbo::JSFunction Script::GrabSurface(Turbo::JSArguments args){}

    /////
    // Animations

    Turbo::JSFunction Script::LoadAnimation(Turbo::JSArguments args){}
