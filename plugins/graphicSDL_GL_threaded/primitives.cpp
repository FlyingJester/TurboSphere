#define GRAPHIC_INTERNAL
#include "main.h"
#include "primitives.h"
#include "image.h"
#include "color.h"
#include "surface.h"
#include <sstream>
#include <assert.h>
#include "../../SDL2/SDL_opengl.h"

#ifdef _MSC_VER //Good job, Microsoft.

#include <cmath>

double round(double in){
	return floor(in+0.5);
}

#endif

#define XPROP v8::String::New("x")
#define YPROP v8::String::New("y")

#ifdef USE_AMDLIBM
    #ifdef _WIN32
        #warning AMD LibM cannot be used for TurboSphere on Windows.
        #error If you really want to try, go ahead and remove this error. But I warned you.
    #endif
#endif

#ifdef USE_AMDLIBM


    double    (*sin_override)(double) = NULL;
    double    (*cos_override)(double) = NULL;
    double    (*floor_override)(double) = NULL;
    double    (*ceil_override)(double) = NULL;
    double    (*max_override)(double, double) = NULL;
    double    (*min_override)(double, double) = NULL;
    double    (*sqrt_override)(double) = NULL;



#endif

void PrimitivesInit(void){
    #ifdef USE_AMDLIBM
        #ifdef _WIN32
		DWORD error;
	    HINSTANCE handle = LoadLibrary("libamdlibm.so");
        if(handle!=NULL) {
            #warning this will cause problems.
        }
        else{
            sin_override = sin;
            cos_override = cos;
        }
        #else

        char *error;
        void * handle = dlopen("./libamdlibm.so", RTLD_GLOBAL|RTLD_NOW);
        if(handle==NULL) {
            sin_override = sin;
            cos_override = cos;
            floor_override= floor;
            ceil_override = ceil;
            sqrt_override = sqrt;
            min_override = tempmin;
            max_override = tempmax;
            printf("[" PLUGINNAME "] Info: Could not open AMD libm. Using system libm.\n");
        }
        else{
            printf("[" PLUGINNAME "] Info: Using AMD libm.\n");
            sin_override = (double(*)(double))dlsym(handle, "amd_sin");
            if ((error = dlerror()) != NULL)  {
                fprintf (stderr, "[" PLUGINNAME "] PrimitivesInit error: Could not load amd_sin from AMD libm.\n\tReverting to normal libm sin function.\n\tReported error is: %s", error);
                sin_override = sin;
            }
            cos_override = (double(*)(double))dlsym(handle, "amd_cos");
            if ((error = dlerror()) != NULL)  {
                fprintf (stderr, "[" PLUGINNAME "] PrimitivesInit error: Could not load amd_cos from AMD libm.\n\tReverting to normal libm cos function.\n\tReported error is: %s", error);
                cos_override = cos;
            }
            floor_override = (double(*)(double))dlsym(handle, "amd_floor");
            if ((error = dlerror()) != NULL)  {
                fprintf (stderr, "[" PLUGINNAME "] PrimitivesInit error: Could not load amd_floor from AMD libm.\n\tReverting to normal libm floor function.\n\tReported error is: %s", error);
                floor_override = floor;
            }
            ceil_override = (double(*)(double))dlsym(handle, "amd_ceil");
            if ((error = dlerror()) != NULL)  {
                fprintf (stderr, "[" PLUGINNAME "] PrimitivesInit error: Could not load amd_ceil from AMD libm.\n\tReverting to normal libm ceil function.\n\tReported error is: %s", error);
                ceil_override = ceil;
            }
            min_override = (double(*)(double, double))dlsym(handle, "amd_fmin");
            if ((error = dlerror()) != NULL)  {
                fprintf (stderr, "[" PLUGINNAME "] PrimitivesInit error: Could not load amd_fmin from AMD libm.\n\tReverting to normal libm min function.\n\tReported error is: %s", error);
                min_override = tempmin;
            }
            max_override = (double(*)(double, double))dlsym(handle, "amd_fmax");
            if ((error = dlerror()) != NULL)  {
                fprintf (stderr, "[" PLUGINNAME "] PrimitivesInit error: Could not load amd_fmax from AMD libm.\n\tReverting to normal libm max function.\n\tReported error is: %s", error);
                max_override = tempmax;
            }
            sqrt_override = (double(*)(double))dlsym(handle, "amd_sqrt");
            if ((error = dlerror()) != NULL)  {
                fprintf (stderr, "[" PLUGINNAME "] PrimitivesInit error: Could not load amd_sqrt from AMD libm.\n\tReverting to normal libm sqrt function.\n\tReported error is: %s", error);
                sqrt_override = sqrt;
            }
        }
        #endif
    #endif

}


#ifdef USE_AMDLIBM

    #define sin sin_override
    #define cos cos_override
    #define floor floor_override
    #define ceil  ceil_override
    #define min min_override
    #define max max_override
    #define sqrt sqrt_override
#endif

v8Function Rectangle(V8ARGS)
{
    if(args.Length()<5){
        return v8::ThrowException(v8::String::New("[" PLUGINNAME "] TS_Rectangle Error: Called with fewer than 5 arguments."));
	}
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);
    CHECK_ARG_OBJ(4);

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[4]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 4 is not a color.");
    }

	int x = args[0]->v8::Value::Int32Value();
	int y = args[1]->v8::Value::Int32Value();
	int w = args[2]->v8::Value::Int32Value();
	int h = args[3]->v8::Value::Int32Value();

    TS_Color* c = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

	TS_Rectangle(x, y, w, h, c);

	return v8::Undefined();
}

v8Function Line(V8ARGS){

    if(args.Length()<5){
        THROWERROR("[" PLUGINNAME "] Line Error: Called with fewer than 5 arguments.");
    }

    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);
    CHECK_ARG_OBJ(4);

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[4]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 4 is not a color.");
    }

	int x1 = args[0]->v8::Value::Int32Value();
	int y1 = args[1]->v8::Value::Int32Value();
	int x2 = args[2]->v8::Value::Int32Value();
	int y2 = args[3]->v8::Value::Int32Value();

    TS_Color* c = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    TS_Line(x1, y1, x2, y2, c);

    return v8::Undefined();
}

v8Function Point(V8ARGS){

    if(args.Length()<3){
        THROWERROR("[" PLUGINNAME "] Point Error: Called with fewer than 3 arguments.")
    }

    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_OBJ(2);

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[2]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 2 is not a color.");
    }

	int x = args[0]->v8::Value::Int32Value();
	int y = args[1]->v8::Value::Int32Value();

    TS_Color* c = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    TS_Pixel(x, y, c);

    return v8::Undefined();

}

v8Function Triangle(V8ARGS){
    if(args.Length()<7){
        THROWERROR("[" PLUGINNAME "] Triangle Error: Called with fewer than 7 arguments.");
    }

    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);
    CHECK_ARG_INT(4);
    CHECK_ARG_INT(5);
    CHECK_ARG_OBJ(6);

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[6]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 6 is not a color.");
    }

	int x1 = args[0]->v8::Value::Int32Value();
	int y1 = args[1]->v8::Value::Int32Value();
	int x2 = args[2]->v8::Value::Int32Value();
	int y2 = args[3]->v8::Value::Int32Value();
	int x3 = args[4]->v8::Value::Int32Value();
	int y3 = args[5]->v8::Value::Int32Value();

    TS_Color* c = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    TS_Triangle(x1, y1, x2, y2, x3, y3, c);

    return v8::Undefined();
}

v8Function Polygon(V8ARGS){

    if(args.Length()<2){
        THROWERROR("[" PLUGINNAME "] Polygon Error: Called with fewer than 2 arguments.");
    }
    CHECK_ARG_ARRAY(0);
    CHECK_ARG_OBJ(1);

	v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[1]);
    if(colorobj->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 1 is not a color.");
    }

    uint32_t numpoints = 0;
    TS_Color *color = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    v8::Handle<v8::Array> pointarray = v8::Handle<v8::Array>::Cast(args[0]);

    numpoints = pointarray->Length();

    if(numpoints==0){
        return v8::Undefined();
    }
    else if(numpoints==1){
        v8::Local<v8::Object> point = (pointarray->Get(0))->ToObject();

        if(!point->Has(XPROP)){
        //    const char * String = (static_cast<ostringstream*>( &(ostringstream() << i) )->str())->c_str();
            THROWERROR("[" PLUGINNAME "] Polygon Error: element 0 has no property 'x'.");
        }
        if(!point->Has(YPROP)){
        //    const char * String = (static_cast<ostringstream*>( &(ostringstream() << i) )->str())->c_str();
            THROWERROR("[" PLUGINNAME "] Polygon Error: element 0 has no property 'y'.");
        }


        TS_Pixel(point->Get(XPROP)->Int32Value(), point->Get(YPROP)->Int32Value(), color);
        return v8::Undefined();
    }
    else if(numpoints==2){
        v8::Local<v8::Object> pointA = (pointarray->Get(0))->ToObject();
        v8::Local<v8::Object> pointB = (pointarray->Get(1))->ToObject();

        if(!pointA->Has(XPROP)){
        //    const char * String = (static_cast<ostringstream*>( &(ostringstream() << i) )->str())->c_str();
            THROWERROR("[" PLUGINNAME "] Polygon Error: element 0 has no property 'x'.");
        }
        if(!pointA->Has(YPROP)){
        //    const char * String = (static_cast<ostringstream*>( &(ostringstream() << i) )->str())->c_str();
            THROWERROR("[" PLUGINNAME "] Polygon Error: element 0 has no property 'y'.");
        }

        if(!pointB->Has(XPROP)){
        //    const char * String = (static_cast<ostringstream*>( &(ostringstream() << i) )->str())->c_str();
            THROWERROR("[" PLUGINNAME "] Polygon Error: element 1 has no property 'x'.");
        }
        if(!pointB->Has(YPROP)){
        //    const char * String = (static_cast<ostringstream*>( &(ostringstream() << i) )->str())->c_str();
            THROWERROR("[" PLUGINNAME "] Polygon Error: element 1 has no property 'y'.");
        }


        TS_Line(pointA->Get(XPROP)->Int32Value(), pointA->Get(XPROP)->Int32Value(), pointB->Get(XPROP)->Int32Value(), pointB->Get(YPROP)->Int32Value(), color);
        return v8::Undefined();
    }

    int *x = (int *)calloc(numpoints, sizeof(int));
    int *y = (int *)calloc(numpoints, sizeof(int));

    for(uint32_t i = 0; i < numpoints; i++){
        v8::Local<v8::Object> point = (pointarray->Get(i))->ToObject();
        if(!point->Has(XPROP)){

            std::ostringstream errorname;
            errorname << "[" PLUGINNAME "] Polygon Error: element ";
            errorname << i;
            errorname << " has no property 'x'.";

            THROWERROR_TYPE((errorname.str()).c_str());
        }
        if(!point->Has(YPROP)){

            std::ostringstream errorname;
            errorname << "[" PLUGINNAME "] Polygon Error: element ";
            errorname << i;
            errorname << " has no property 'y'.";

            THROWERROR_TYPE((errorname.str()).c_str());
        }
        x[i] = point->Get(XPROP)->Int32Value();
        y[i] = point->Get(YPROP)->Int32Value();
    }

    TS_Polygon(numpoints, x, y, color);

    free(x);
    free(y);

    return v8::Undefined();
}

v8Function GradientRectangle(V8ARGS)
{
    if(args.Length()<5){
        return v8::ThrowException(v8::String::New("[" PLUGINNAME "] TS_Rectangle Error: Called with fewer than 5 arguments."));
	}
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);
    CHECK_ARG_OBJ(4);
    CHECK_ARG_OBJ(5);
    CHECK_ARG_OBJ(6);
    CHECK_ARG_OBJ(7);
	int x = args[0]->v8::Value::Int32Value();
	int y = args[1]->v8::Value::Int32Value();
	int w = args[2]->v8::Value::Int32Value();
	int h = args[3]->v8::Value::Int32Value();

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[4]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 4 is not a color.");
    }

    TS_Color* c1 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    color = v8::Local<v8::Object>::Cast(args[5]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 5 is not a color.");
    }

    TS_Color* c2 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    color = v8::Local<v8::Object>::Cast(args[6]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 6 is not a color.");
    }

    TS_Color* c3 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    color = v8::Local<v8::Object>::Cast(args[7]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 7 is not a color.");
    }

    TS_Color* c4 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

	TS_GradientRectangle(x, y, w, h, c1, c2, c3, c4);

	return v8::Undefined();
}


v8Function GradientLine(V8ARGS){

    if(args.Length()<6){
        THROWERROR("[" PLUGINNAME "] GradientLine Error: Called with fewer than 6 arguments.");
    }

    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);
    CHECK_ARG_OBJ(4);
    CHECK_ARG_OBJ(5);

	int x1 = args[0]->v8::Value::Int32Value();
	int y1 = args[1]->v8::Value::Int32Value();
	int x2 = args[2]->v8::Value::Int32Value();
	int y2 = args[3]->v8::Value::Int32Value();

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[4]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 4 is not a color.");
    }

    TS_Color* c1 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

	color = v8::Local<v8::Object>::Cast(args[5]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 5 is not a color.");
    }

    TS_Color* c2 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    TS_GradientLine(x1, y1, x2, y2, c1, c2);

    return v8::Undefined();
}

v8Function GradientTriangle(V8ARGS){
    if(args.Length()<7){
        THROWERROR("[" PLUGINNAME "] GradientTriangle Error: Called with fewer than 9 arguments.");
    }

    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);
    CHECK_ARG_INT(4);
    CHECK_ARG_INT(5);
    CHECK_ARG_OBJ(6);
    CHECK_ARG_OBJ(7);
    CHECK_ARG_OBJ(8);

	int x1 = args[0]->v8::Value::Int32Value();
	int y1 = args[1]->v8::Value::Int32Value();
	int x2 = args[2]->v8::Value::Int32Value();
	int y2 = args[3]->v8::Value::Int32Value();
	int x3 = args[4]->v8::Value::Int32Value();
	int y3 = args[5]->v8::Value::Int32Value();

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[6]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 6 is not a color.");
    }

    TS_Color* c1 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    color = v8::Local<v8::Object>::Cast(args[7]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 7 is not a color.");
    }

    TS_Color* c2 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    color = v8::Local<v8::Object>::Cast(args[8]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 8 is not a color.");
    }


    TS_Color* c3 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    TS_GradientTriangle(x1, y1, x2, y2, x3, y3, c1, c2, c3);

    return v8::Undefined();
}

v8Function OutlinedRectangle(V8ARGS){
    if(args.Length()<5){
        return v8::ThrowException(v8::String::New("[" PLUGINNAME "] TS_Rectangle Error: Called with fewer than 5 arguments."));
	}
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);
    CHECK_ARG_OBJ(4);
	int x = args[0]->v8::Value::Int32Value();
	int y = args[1]->v8::Value::Int32Value();
	int w = args[2]->v8::Value::Int32Value();
	int h = args[3]->v8::Value::Int32Value();

    int thickness = 1;

    if(args.Length()>=6){
        CHECK_ARG_INT(5);
        thickness = args[5]->v8::Value::Int32Value();
    }

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[4]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 4 is not a color.");
    }


    TS_Color* c = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    TS_OutlinedRectangle(x, y, w, h, c, thickness);

    return v8::Undefined();

}

v8Function OutlinedCircle(V8ARGS){
    if(args.Length()<4){
        return v8::ThrowException(v8::String::New("[" PLUGINNAME "] OutlinedCircle Error: Called with fewer than 4 arguments."));
	}
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_OBJ(3);
	int x = args[0]->v8::Value::Int32Value();
	int y = args[1]->v8::Value::Int32Value();
	int r = args[2]->v8::Value::Int32Value();

    bool AA = false;

    if(args.Length()>=5){
        CHECK_ARG_BOOL(4);
        AA = args[4]->BooleanValue();
    }

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[3]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 3 is not a color.");
    }


    TS_Color* c = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    TS_OutlinedCircle(x, y, r, c, AA);

    return v8::Undefined();

}

v8Function FilledCircle(V8ARGS){
    if(args.Length()<4){
        return v8::ThrowException(v8::String::New("[" PLUGINNAME "] FilledCircle Error: Called with fewer than 4 arguments."));
	}
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_OBJ(3);
	int x = args[0]->v8::Value::Int32Value();
	int y = args[1]->v8::Value::Int32Value();
	int r = args[2]->v8::Value::Int32Value();
    bool AA = false;

    if(args.Length()>=5){
        CHECK_ARG_BOOL(4);
        AA = args[4]->BooleanValue();
    }

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[3]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 3 is not a color.");
    }


    TS_Color* c = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    TS_FilledCircle(x, y, r, c, AA);

    return v8::Undefined();

}

v8Function GradientCircle(V8ARGS){
    if(args.Length()<4){
        return v8::ThrowException(v8::String::New("[" PLUGINNAME "] GradientCircle Error: Called with fewer than 5 arguments."));
	}
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_OBJ(3);
    CHECK_ARG_OBJ(4);
	int x = args[0]->v8::Value::Int32Value();
	int y = args[1]->v8::Value::Int32Value();
	int r = args[2]->v8::Value::Int32Value();
	bool AA = false;

    if(args.Length()>=6){
        CHECK_ARG_BOOL(5);
        AA = args[5]->BooleanValue();
    }

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[3]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 3 is not a color.");
    }


    TS_Color* c1 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);
    color = v8::Local<v8::Object>::Cast(args[4]);
    if(color->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 4 is not a color.");
    }

    TS_Color* c2 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    TS_GradientCircle(x, y, r, c1, c2, AA);

    return v8::Undefined();

}

v8Function OutlinedPolygon(V8ARGS){

    if(args.Length()<2){
        THROWERROR("[" PLUGINNAME "] OutlinedPolygon Error: Called with fewer than 2 arguments.");
    }
    CHECK_ARG_ARRAY(0);
    CHECK_ARG_OBJ(1);

    uint32_t numpoints = 0;
	v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[1]);
    if(colorobj->GetInternalField(1)->Uint32Value()!=ColorID){
        THROWERROR_TYPE(" Error: Argument 1 is not a color.");
    }

    TS_Color *color = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    v8::Handle<v8::Array> pointarray = v8::Handle<v8::Array>::Cast(args[0]);

    numpoints = pointarray->Length();

    if(numpoints==0){
        return v8::Undefined();
    }
    else if(numpoints==1){
        v8::Local<v8::Object> point = (pointarray->Get(0))->ToObject();

        if(!point->Has(XPROP)){
        //    const char * String = (static_cast<ostringstream*>( &(ostringstream() << i) )->str())->c_str();
            THROWERROR("[" PLUGINNAME "] OutlinedPolygon Error: element 0 has no property 'x'.");
        }
        if(!point->Has(YPROP)){
        //    const char * String = (static_cast<ostringstream*>( &(ostringstream() << i) )->str())->c_str();
            THROWERROR("[" PLUGINNAME "] OutlinedPolygon Error: element 0 has no property 'y'.");
        }


        TS_Pixel(point->Get(XPROP)->Int32Value(), point->Get(YPROP)->Int32Value(), color);
        return v8::Undefined();
    }
    else if(numpoints==2){
        v8::Local<v8::Object> pointA = (pointarray->Get(0))->ToObject();
        v8::Local<v8::Object> pointB = (pointarray->Get(1))->ToObject();

        if(!pointA->Has(XPROP)){
        //    const char * String = (static_cast<ostringstream*>( &(ostringstream() << i) )->str())->c_str();
            THROWERROR("[" PLUGINNAME "] OutlinedPolygon Error: element 0 has no property 'x'.");
        }
        if(!pointA->Has(YPROP)){
        //    const char * String = (static_cast<ostringstream*>( &(ostringstream() << i) )->str())->c_str();
            THROWERROR("[" PLUGINNAME "] Polygon Error: element 0 has no property 'y'.");
        }

        if(!pointB->Has(XPROP)){
        //    const char * String = (static_cast<ostringstream*>( &(ostringstream() << i) )->str())->c_str();
            THROWERROR("[" PLUGINNAME "] OutlinedPolygon Error: element 1 has no property 'x'.");
        }
        if(!pointB->Has(YPROP)){
        //    const char * String = (static_cast<ostringstream*>( &(ostringstream() << i) )->str())->c_str();
            THROWERROR("[" PLUGINNAME "] OutlinedPolygon Error: element 1 has no property 'y'.");
        }


        TS_Line(pointA->Get(XPROP)->Int32Value(), pointA->Get(XPROP)->Int32Value(), pointB->Get(XPROP)->Int32Value(), pointB->Get(YPROP)->Int32Value(), color);
        return v8::Undefined();
    }

    int *x = (int *)calloc(numpoints, sizeof(int));
    int *y = (int *)calloc(numpoints, sizeof(int));

    for(uint32_t i = 0; i < numpoints; i++){
        v8::Local<v8::Object> point = (pointarray->Get(i))->ToObject();
        if(!point->Has(XPROP)){

            std::ostringstream errorname;
            errorname << "[" PLUGINNAME "] OutlinedPolygon Error: element ";
            errorname << i;
            errorname << " has no property 'x'.";

            THROWERROR_TYPE((errorname.str()).c_str());
        }
        if(!point->Has(YPROP)){

            std::ostringstream errorname;
            errorname << "[" PLUGINNAME "] OutlinedPolygon Error: element ";
            errorname << i;
            errorname << " has no property 'y'.";

            THROWERROR_TYPE((errorname.str()).c_str());
        }
        x[i] = point->Get(XPROP)->Int32Value();
        y[i] = point->Get(YPROP)->Int32Value();
    }

    TS_OutlinedPolygon(numpoints, x, y, color);

    free(x);
    free(y);

    return v8::Undefined();

}
