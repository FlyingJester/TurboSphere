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


    double    (*sin_override)(double);
    double    (*cos_override)(double);
    double    (*floor_override)(double);
    double    (*ceil_override)(double);
    double    (*max_override)(double, double);
    double    (*min_override)(double, double);
    double    (*sqrt_override)(double);
    double tempmin(double a, double b){
        return min(a, b);
    }
    double tempmax(double a, double b){
        return max(a, b);
    }


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
        }
        else{
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
	int x = args[0]->v8::Value::Int32Value();
	int y = args[1]->v8::Value::Int32Value();
	int w = args[2]->v8::Value::Int32Value();
	int h = args[3]->v8::Value::Int32Value();

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[4]);

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

	int x1 = args[0]->v8::Value::Int32Value();
	int y1 = args[1]->v8::Value::Int32Value();
	int x2 = args[2]->v8::Value::Int32Value();
	int y2 = args[3]->v8::Value::Int32Value();

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[4]);

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

	int x = args[0]->v8::Value::Int32Value();
	int y = args[1]->v8::Value::Int32Value();

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[2]);

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

	int x1 = args[0]->v8::Value::Int32Value();
	int y1 = args[1]->v8::Value::Int32Value();
	int x2 = args[2]->v8::Value::Int32Value();
	int y2 = args[3]->v8::Value::Int32Value();
	int x3 = args[4]->v8::Value::Int32Value();
	int y3 = args[5]->v8::Value::Int32Value();

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[6]);

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

    uint32_t numpoints = 0;
	v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[1]);
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

    TS_Color* c1 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    color = v8::Local<v8::Object>::Cast(args[5]);

    TS_Color* c2 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    color = v8::Local<v8::Object>::Cast(args[6]);

    TS_Color* c3 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    color = v8::Local<v8::Object>::Cast(args[7]);

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

    TS_Color* c1 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

	color = v8::Local<v8::Object>::Cast(args[5]);

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

    TS_Color* c1 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    color = v8::Local<v8::Object>::Cast(args[7]);

    TS_Color* c2 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    color = v8::Local<v8::Object>::Cast(args[8]);

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

    TS_Color* c1 = (TS_Color*)color->GetAlignedPointerFromInternalField(0);
    color = v8::Local<v8::Object>::Cast(args[4]);
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

v8Function OutlinedCircle(V8ARGS);

void TS_Rectangle(int x, int y, int w, int h, TS_Color *color){

    const GLint   vertexData[] = {x, y, x+w, y, x+w, y+h, x, y+h};
    const GLuint  colorData[]  = {
        color->toInt(),
        color->toInt(),
        color->toInt(),
        color->toInt()
    };

    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

}

void TS_Line(int x1, int y1, int x2, int y2, TS_Color *color){

    const GLint vertexData[] = {x1, y1, x2, y2};
    const GLuint  colorData[]  = {
        color->toInt(),
        color->toInt()
    };
    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_LINES, 0, 2);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

}

void TS_Pixel(int x, int y, TS_Color *color){

}

void TS_Triangle(int x1, int y1, int x2, int y2, int x3, int y3, TS_Color *color){

    const GLint   vertexData[] = {x1, y1, x2, y2, x3, y3};
    const GLuint  colorData[]  = {
        color->toInt(),
        color->toInt(),
        color->toInt()
    };
    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

}

void TS_Polygon(int numPoints, int *x, int *y, TS_Color *color){
    GLint *vertexData = (GLint*)calloc(numPoints*2, sizeof(GLint));
    GLuint *colorData = (GLuint*)calloc(numPoints, sizeof(GLuint));
    for(int i = 0; i<numPoints; i++){
        vertexData[i*2]     = x[i];
        vertexData[(i*2)+1] = y[i];
        colorData[i] = color->toInt();
    }
    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_POLYGON, 0, numPoints);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    free(vertexData);
    free(colorData);
}


void TS_GradientRectangle(int x, int y, int w, int h, TS_Color *color1, TS_Color *color2, TS_Color *color3, TS_Color *color4){

    const GLint   vertexData[] = {x, y, x+w, y, x+w, y+h, x, y+h};
    const GLuint  colorData[]  = {
        color1->toInt(),
        color2->toInt(),
        color3->toInt(),
        color4->toInt()
    };

    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void TS_GradientLine(int x1, int y1, int x2, int y2, TS_Color *color1, TS_Color *color2){

    const GLint vertexData[] = {x1, y1, x2, y2};
    const GLuint  colorData[]  = {
        color1->toInt(),
        color2->toInt()
    };
    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_LINES, 0, 2);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}
void TS_GradientTriangle(int x1, int y1, int x2, int y2, int x3, int y3, TS_Color *color1, TS_Color *color2, TS_Color *color3){

    const GLint   vertexData[] = {x1, y1, x2, y2, x3, y3};
    const GLuint  colorData[]  = {
        color1->toInt(),
        color2->toInt(),
        color3->toInt()
    };
    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void TS_GradientPolygon(int numPoints, int *x, int *y, TS_Color **color){
    GLint *vertexData = (GLint*)calloc(numPoints*2, sizeof(GLint));
    GLuint *colorData = (GLuint*)calloc(numPoints, sizeof(GLuint));
    for(int i = 0; i<numPoints; i++){
        vertexData[i*2]     = x[i];
        vertexData[(i*2)+1] = y[i];
        colorData[i] = color[i]->toInt();
    }
    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_POLYGON, 0, numPoints);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    free(vertexData);
    free(colorData);

}

void TS_OutlinedRectangle(int x, int y, int w, int h, TS_Color *color, int thickness){
//This is easier if thickness == 1.
    assert(thickness>=0);

    if(thickness==0)    return;

    if(thickness==1){
        const GLint vertexData[] = {x, y, x+w, y, x+w, y+h, x, y+h};

        const GLuint  colorData[]  = {
            color->toInt(),
            color->toInt(),
            color->toInt(),
            color->toInt()
        };

        glVertexPointer(2, GL_INT, 0, vertexData);
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        return;
    }

/*
    3--------5
    |\   __//|
    |\\ /  / |
    || 4--6  |
    | \|  |\ |
    |  2--8 ||
    | /10  \\|
    |/      \|
    19-------7

    So the last 2 vertices are duped, but it's good enough.
*/

    const GLint vertexData[] = {
        x,              y+h, //1
        x+thickness,   (y+h)-thickness, //2
        x,              y, //3
        x+thickness,    y+thickness, //4
        x+w,            y, //5
       (x+w)-thickness, y+thickness, //6
        x+w,            y+h, //7
       (x+w)-thickness, y+h, //8
        x,              y+h, //9
        x+thickness,   (y+h)-thickness //10
    };

    GLuint colorData[10];
    std::fill(colorData, colorData+sizeof(colorData), (int)(color->toInt()));
    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

}

void TS_OutlinedGradientRectangle(int x, int y, int w, int h, TS_Color **color, int thickness){
//This is easier if thickness == 1.
    assert(thickness>=0);

    if(thickness==0)    return;

    if(thickness==1){
        const GLint vertexData[] = {x, y, x+w, y, x+w, y+h, x, y+h};

        const GLuint  colorData[]  = {
            color[0]->toInt(),
            color[1]->toInt(),
            color[2]->toInt(),
            color[3]->toInt()
        };

        glVertexPointer(2, GL_INT, 0, vertexData);
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        return;
    }

/*
    3--------5
    |\   __//|
    |\\ /  / |
    || 4--6  |
    | \|  |\ |
    |  2--8 ||
    | /10  \\|
    |/      \|
    19-------7

    So the last 2 vertices are duped, but it's good enough.
*/

    const GLint vertexData[] = {
        x,              y+h, //1
        x+thickness,   (y+h)-thickness, //2
        x,              y, //3
        x+thickness,    y+thickness, //4
        x+w,            y, //5
       (x+w)-thickness, y+thickness, //6
        x+w,            y+h, //7
       (x+w)-thickness, y+h, //8
        x,              y+h, //9
        x+thickness,   (y+h)-thickness //10
    };

    const GLuint colorData[] = {
        color[3]->toInt(), //1
        color[3]->toInt(), //2
        color[0]->toInt(), //3
        color[0]->toInt(), //4
        color[1]->toInt(), //5
        color[1]->toInt(), //6
        color[2]->toInt(), //7
        color[2]->toInt(), //8
        color[3]->toInt(), //9
        color[3]->toInt(), //10
    };

    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

}

void TS_OutlinedPolygon(int numPoints, int *x, int *y, TS_Color *color){
    GLint *vertexData = (GLint*)calloc(numPoints*2, sizeof(GLint));
    GLuint *colorData = (GLuint*)calloc(numPoints, sizeof(GLuint));
    for(int i = 0; i<numPoints; i++){
        vertexData[i*2]     = x[i];
        vertexData[(i*2)+1] = y[i];
        colorData[i] = color->toInt();
    }
    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_LINE_LOOP, 0, numPoints);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    free(vertexData);
    free(colorData);
}

//Software Primitives

void TS_SoftRectangle(int x, int y, int w, int h, TS_Color *c, SDL_Surface *destination){

    uint32_t color = c->toInt();

    SDL_Rect rect = {(short int)x, (short int)y, (short unsigned int)w, (short unsigned int)h};
    SDL_Rect locrect = {0, 0, (short unsigned int)w, (short unsigned int)h};

	SDL_Surface* surface = SDL_CreateRGBSurface(0, w, h, DEPTH, CHANNEL_MASKS);
	SDL_FillRect(surface, &locrect, color);
	SDL_BlitSurface(surface, NULL, destination, &rect);
    SDL_FreeSurface(surface);
}

//IMPORTANT! Include the centroid in the number of segments.
//Also important. Really understand what is being returned.

#define CIRCLE_POINT_X(array, num) array[num*2]
#define CIRCLE_POINT_Y(array, num) array[(num*2)+1]

inline void TS_CalcCirclePoints(int r, int numSegments, GLint *points){
    const double pi2 = 2.0f*3.14159265358979323846f;

    assert(numSegments);

    //points[(numSegments*2)] = 'j';
    //printf("Poked.\n");
    double ns = numSegments;
    for(int i = 0; i<numSegments; i++){
        double theta=double(((double)i)/ns)*pi2;
            CIRCLE_POINT_X(points, i) = (int)round(((double)r)*cos(theta));
            CIRCLE_POINT_Y(points, i) = (int)round(((double)r)*sin(theta));
    }
}

#undef CIRCLE_POINT_X
#undef CIRCLE_POINT_Y

void TS_OutlinedCircle(int x, int y, int r, TS_Color *c, bool AA){

    const double localPi = 3.14159265358979323846f;

    if(r==0){
        return;
    }
    if(r<0){
        r*=-1;
    }


    glPushMatrix();

    #ifndef __ANDROID__
    if(AA){
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_LINE_SMOOTH);
    }
    #endif

    glTranslatef((float)x, (float)y, 0.0f);



    int numSegments = (int)ceil(localPi*r*2.0f);
    if(numSegments<4){
        numSegments = 4;
    }
    else if(numSegments%2)
        numSegments++;

    GLint *points = (GLint*)calloc(numSegments, sizeof(GLint)*2);
    TS_CalcCirclePoints(r, numSegments, points);
    points[0] = r;
    points[1] = 0;

    GLuint *colorData = (GLuint*)calloc(numSegments, 4);
    for(int i = 0; i<numSegments; i++){
        colorData[i] = c->toInt();
    }

    glVertexPointer(2, GL_INT, 0, points);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_LINE_LOOP, 0, numSegments);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    if(AA)
        glDisable(GL_LINE_SMOOTH);

    glPopMatrix();

    free(points);
    free(colorData);


}


void TS_FilledCircle(int x, int y, int r, TS_Color *c, bool AA){
    TS_GradientCircle(x, y, r, c, c, AA);
}


void TS_GradientCircle(int x, int y, int r, TS_Color *c1, TS_Color *c2, bool AA){

    const double localPi = 3.14159265358979323846f;

    if(r==0){
        return;
    }
    if(r<0){
        r*=-1;
    }

    glPushMatrix();

    #ifndef __ANDROID__
    if(AA){
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_POLYGON_SMOOTH);
    }
    #endif

    glTranslatef((float)x, (float)y, 0.0f);

    int numSegments = (int)ceil(localPi*r*2.0f);
    if(numSegments<4){
        numSegments = 4;
    }
    else if(numSegments%2)
        numSegments++;

    GLint *points = (GLint*)calloc(numSegments, sizeof(GLint)*2);
    TS_CalcCirclePoints(r, numSegments, points);
    //points = (GLint*)calloc(numSegments+1, sizeof(GLint)*2);
    points[(numSegments*2)-2] = points[2];
    points[(numSegments*2)-1] = points[3];
    points[0] = 0;
    points[1] = 0;
    //points[2] = r;
    //points[3] = 0;

    GLuint *colorData = (GLuint*)calloc(numSegments, sizeof(GLuint));
    for(int i = 1; i<numSegments; i++){
        colorData[i] = c2->toInt();
    }

    colorData[0] = c1->toInt();

    glVertexPointer(2, GL_INT, 0, points);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments);
    if(!AA)
        glDisableClientState(GL_COLOR_ARRAY);

    glDisableClientState(GL_VERTEX_ARRAY);

    if(AA){
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_LINE_SMOOTH);

        glVertexPointer(2, GL_INT, 0, points);
        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_LINE_LOOP, 0, numSegments);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_LINE_SMOOTH);
    }

    glPopMatrix();

    free(points);
    free(colorData);

}

//typedef struct {SDL_Surface *surface; TS_Color *color; int x1; int y1; int x2; int y2; } SoftLineArgs;

//void TS_SoftLine(int ax1, int ay1, int ax2, int ay2, TS_Color *c, SDL_Surface *destination){
void TS_SoftLine(void *arg){
    SoftLineArgs *carg = (SoftLineArgs*)arg;
    int ax1 = carg->x1;
    int ay1 = carg->y1;
    int ax2 = carg->x2;
    int ay2 = carg->y2;

    TS_Color *c = carg->color;
    SDL_Surface *destination = carg->surface;

    free(carg);

    uint32_t color = c->toInt();

    int w = abs(ax1-ax2);
    int h = abs(ay1-ay2);

    SDL_Rect rect = {(short int)min(ax1, ax2), (short int)min(ay1, ay2), (short unsigned int)w, (short unsigned int)h};

    if(ax1-ax2<=1&&ax1-ax2>=-1){
        SDL_Surface *surface = SDL_CreateRGBSurface(0, 1, h, DEPTH, CHANNEL_MASKS);
        SDL_Rect allsurf = {0, 0, 1, (short unsigned int)h};
        SDL_FillRect(surface, &allsurf, color);

        SDL_BlitSurface(surface, NULL, destination, &rect);
        SDL_FreeSurface(surface);

        return;
    }
    else if(ay1-ay2<=1&&ay1-ay2>=-1){
        SDL_Surface *surface = SDL_CreateRGBSurface(0, w, 1, DEPTH, CHANNEL_MASKS);
        SDL_Rect allsurf = {0, 0, (short unsigned int)w, 1};
        SDL_FillRect(surface, &allsurf, color);

        SDL_BlitSurface(surface, NULL, destination, &rect);
        SDL_FreeSurface(surface);

        return;
    }

    int x1 = ax1-min(ax1, ax2);
    int y1 = ay1-min(ay1, ay2);
    int x2 = ax2-min(ax1, ax2);
    int y2 = ay2-min(ay1, ay2);

    SDL_Rect temprect = {(short int)x1, (short int)y1, 1, 1};

	SDL_Surface *surface = SDL_CreateRGBSurface(0, w+1, h+1, DEPTH, CHANNEL_MASKS);

    int sx;
    int sy;
    if (x1<x2){
        sx  = 1;
    }
    else {
        sx  = -1;
    }
    if (y1<y2){
        sy  = 1;
    }
    else{
        sy  = -1;
    }
    float err = (float)w-h;

    float e2;
    while(true){
        temprect.x = x1;
        temprect.y = y1;

        SDL_FillRect(surface, &temprect, color);

        if ((x1==x2)&&(y1==y2)){break;}
        e2 = 2*err;

        if (e2>-h){

            err-= h;
            x1 += sx;

        }
        if (e2<w){

            err += w;
            y1  += sy;

        }
    }

	SDL_BlitSurface(surface, NULL, destination, &rect);
    SDL_FreeSurface(surface);
}

//Labelled horiz and vert for the direction in which the color changes.

void TS_SoftHorizGradRectangle(int x, int y, int w, int h, TS_Color *c1, TS_Color *c2, SDL_Surface *destination){
    SDL_Rect rect       = {(short int)x, (short int)y, (short unsigned int)w, (short unsigned int)h};
    SDL_Rect temprect   = {0, 0, 1, (short unsigned int)h};
    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w, h, DEPTH, CHANNEL_MASKS);

    TS_Color *color =  new TS_Color(c1->red, c1->green, c1->blue, c1->alpha);

    uint8_t redDif = c1->red  - c2->red;
    uint8_t grnDif = c1->green- c2->green;
    uint8_t bluDif = c1->blue - c2->blue;
    uint8_t alpDif = c1->alpha- c2->alpha;

    uint_fast32_t step = 0;

    for (int i = 0; i<w; i++){

        if(i!=0){
            step = i/w;
        }
        else{
            step = 0;
        }
        temprect.x = i;
        color->red   = c1->red  + (redDif*step);
        color->green = c1->green+ (grnDif*step);
        color->blue  = c1->blue + (bluDif*step);
        color->alpha = c1->alpha+ (alpDif*step);
        SDL_FillRect(surface, &temprect, color->toInt());
    }
	SDL_BlitSurface(surface, NULL, destination, &rect);
    SDL_FreeSurface(surface);
    delete color;
}

void TS_SoftVertGradRectangle(int x, int y, int w, int h, TS_Color *c1, TS_Color *c2, SDL_Surface *destination){
    SDL_Rect rect       = {(short int)x, (short int)y, (short unsigned int)w, (short unsigned int)h};
    SDL_Rect temprect   = {0, 0, (unsigned short int)w, 1};
    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w, h, DEPTH, CHANNEL_MASKS);

    TS_Color *color =  new TS_Color(c1->red, c1->green, c1->blue, c1->alpha);

    uint8_t redDif = c1->red  - c2->red;
    uint8_t grnDif = c1->green- c2->green;
    uint8_t bluDif = c1->blue - c2->blue;
    uint8_t alpDif = c1->alpha- c2->alpha;

    uint_fast32_t step = 0;

    for (int i = 0; i<h; i++){

        if(i!=0){
            step = i/h;
        }
        else{
            step = 0;
        }
        temprect.y = i;
        color->red   = c1->red  + (redDif*step);
        color->green = c1->green+ (grnDif*step);
        color->blue  = c1->blue + (bluDif*step);
        color->alpha = c1->alpha+ (alpDif*step);
        SDL_FillRect(surface, &temprect, color->toInt());
    }
	SDL_BlitSurface(surface, NULL, destination, &rect);
    SDL_FreeSurface(surface);
    delete color;
}

void TS_SoftGradientRectangle(int x, int y, int w, int h, TS_Color *c1, TS_Color *c2, TS_Color *c3, TS_Color *c4, SDL_Surface *destination){


    if(
        ((c1->toInt()==c2->toInt())||((c1->alpha==0)&&(c2->alpha==0))) &&
        ((c3->toInt()==c4->toInt())||((c3->alpha==0)&&(c4->alpha==0)))
    ){
        TS_SoftVertGradRectangle(x, y, w, h, c1, c3, destination);
        return;
    }
    else if(
        ((c1->toInt()==c4->toInt())||((c1->alpha==0)&&(c4->alpha==0))) &&
        ((c3->toInt()==c2->toInt())||((c3->alpha==0)&&(c2->alpha==0)))
    ){
        TS_SoftHorizGradRectangle(x, y, w, h, c1, c2, destination);
        return;
    }

    SDL_Rect rect       = {(short int)x, (short int)y, (short unsigned int)w, (short unsigned int)h};
    SDL_Rect temprect   = {0, 0, 1, 1};
    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w, h, DEPTH, CHANNEL_MASKS);

    TS_Color color(0, 0, 0, 0);

    int rDeltaTop = c2->red   - c1->red;
    int gDeltaTop = c2->green - c1->green;
    int bDeltaTop = c2->blue  - c1->blue;
    int aDeltaTop = c2->alpha - c1->alpha;

    int rDeltaLow = c3->red   - c4->red;
    int gDeltaLow = c3->green - c4->green;
    int bDeltaLow = c3->blue  - c4->blue;
    int aDeltaLow = c3->alpha - c4->alpha;

    color.red   = c1->red;
    color.green = c1->green;
    color.blue  = c1->blue;
    color.alpha = c1->alpha;

    int rLineDelta;
    int gLineDelta;
    int bLineDelta;
    int aLineDelta;

    TS_Color topColor = *c1;
    TS_Color lowColor = *c4;

    for(int tx = 0; tx<w; tx++){
        rLineDelta = lowColor.red   -topColor.red;
        gLineDelta = lowColor.green -topColor.green;
        bLineDelta = lowColor.blue  -topColor.blue;
        aLineDelta = lowColor.alpha -topColor.alpha;
        for(int ty = 0; ty<h; ty++){

            SDL_FillRect(surface, &temprect, color.toInt());
            color.red  =topColor.red  + (rLineDelta*ty/h);
            color.green=topColor.green+ (gLineDelta*ty/h);
            color.blue =topColor.blue + (bLineDelta*ty/h);
            color.alpha=topColor.alpha+ (aLineDelta*ty/h);
            temprect.y=ty;
        }

        topColor.red  =c1->red  + (rDeltaTop*tx/w);
        topColor.green=c1->green+ (gDeltaTop*tx/w);
        topColor.blue =c1->blue + (bDeltaTop*tx/w);
        topColor.alpha=c1->alpha+ (aDeltaTop*tx/w);

        lowColor.red  =c4->red  + (rDeltaLow*tx/w);
        lowColor.green=c4->green+ (gDeltaLow*tx/w);
        lowColor.blue =c4->blue + (bDeltaLow*tx/w);
        lowColor.alpha=c4->alpha+ (aDeltaLow*tx/w);

        temprect.x=tx;
    }

	SDL_BlitSurface(surface, NULL, destination, &rect);
	SDL_FreeSurface(surface);
}

void TS_SoftFilledCircle(int x, int y, int rad, TS_Color *c, SDL_Surface *destination){

    uint32_t color = c->toInt();

    //Repair poor parameters.
    if(rad<0){
        rad*=-1;
    }
    else if(rad>0){

    }
    else{
        return;
    }

    SDL_Surface* surface = SDL_CreateRGBSurface(0, rad*2, rad*2, DEPTH, CHANNEL_MASKS);

    int w = rad*2;
    double dis= 0;
    int xd2 = 0;
    int yd2 = 0;
    double r  = rad;

    int inf = (rad>4)?((rad>>3)+rad-floor((double)(((double)rad)/sqrt(2.0)))):(rad);

    SDL_Rect dest = {1, 1, 1, 1};

    for(int xt = 0; xt<=rad; xt++){
        xd2 = rad-xt;
        xd2*= xd2;
        for(int yt = 0; yt<inf; yt++){
            yd2 = rad-yt;
            yd2*= yd2;
            dis = floor(sqrt((float)(yd2+xd2)));
            if(dis<r){
                dest.x = yt;
                dest.y = xt;
                dest.w = (rad-yt)*2;
                SDL_FillRect(surface, &dest, color);

                dest.y=((rad*2)-dest.y);
                SDL_FillRect(surface, &dest, color);

                dest.x = xt;
                dest.y = yt;
                dest.w = (rad-xt)*2;
                SDL_FillRect(surface, &dest, color);

                dest.y=((rad*2)-dest.y);
                SDL_FillRect(surface, &dest, color);
                break;
            }
        }
    }

    SDL_Rect rect = {(short int)(x-rad), (short int)(y-rad), (short unsigned int)(w), (short unsigned int)(w)};
    SDL_BlitSurface(surface, NULL, destination, &rect);
    SDL_FreeSurface(surface);

}


void TS_SoftOutlinedCircle(int x, int y, int rad, TS_Color *c, SDL_Surface* destination){

    uint32_t color = c->toInt();

    //Repair poor parameters.
    if(rad<0){
        rad*=-1;
    }
    else if(rad>0){

    }
    else{
        return;
    }

    SDL_Surface* surface = SDL_CreateRGBSurface(0, rad*2, rad*2, DEPTH, CHANNEL_MASKS);

    Uint32 *field = (Uint32 *)surface->pixels;
    int w = rad*2;
    double dis= 0;
    int xd2 = 0;
    int yd2 = 0;
    double r  = rad;
    int inf = (rad>4)?((rad>>2)+rad-floor((double)(((double)rad)/sqrt(2.0)))):(rad);

    for(int xt = 0; xt<=rad; xt++){
        xd2 = rad-xt;
        xd2 *= xd2;
        for(int yt = 0; yt<inf; yt++){
            yd2 = rad-yt;
            yd2*= yd2;
            dis = floor(sqrt(double(yd2+xd2)));
            if(dis<r){
                field[(yt*surface->w)+xt] = color;
                field[((w-yt)*surface->w)+(w-xt)] = color;
                field[(xt*surface->w)+yt] = color;
                field[((w-xt)*surface->w)+(w-yt)] = color;

                field[((w-yt)*surface->w)+xt] = color;
                field[((w-xt)*surface->w)+yt] = color;
                field[(yt*surface->w)+(w-xt)] = color;
                field[(xt*surface->w)+(w-yt)] = color;
                break;
            }
        }
    }

    SDL_Rect rect = {(short int)(x-rad), (short int)(y-rad), (short unsigned int)(w), (short unsigned int)(w)};
    SDL_BlitSurface(surface, NULL, destination, &rect);
    SDL_FreeSurface(surface);

}
