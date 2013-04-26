#define GRAPHIC_INTERNAL
#include "main.h"
#include "primitives.h"
#include "color.h"
#include <sstream>

#define XPROP v8::String::New("x")
#define YPROP v8::String::New("y")

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
        THROWERROR("["PLUGINNAME "] Triangle Error: Called with fewer than 7 arguments.");
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
        THROWERROR("["PLUGINNAME "] Polygon Error: Called with fewer than 2 arguments.");
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
        THROWERROR("["PLUGINNAME "] GradientLine Error: Called with fewer than 6 arguments.");
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
        THROWERROR("["PLUGINNAME "] GradientTriangle Error: Called with fewer than 9 arguments.");
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

    if(args.Length()<6){
        CHECK_ARG_INT(5);
        thickness = args[5]->v8::Value::Int32Value();
    }

	v8::Local<v8::Object> color = v8::Local<v8::Object>::Cast(args[4]);

    TS_Color* c = (TS_Color*)color->GetAlignedPointerFromInternalField(0);

    TS_OutlinedRectangle(x, y, w, h, c, thickness);

    return v8::Undefined();
}
v8Function OutlinedPolygon(V8ARGS){

    if(args.Length()<2){
        THROWERROR("["PLUGINNAME "] OutlinedPolygon Error: Called with fewer than 2 arguments.");
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
    glBegin(GL_QUADS);
        GLColor(color);
        glVertex2i(x,   y);
        glVertex2i(x+w, y);
        glVertex2i(x+w, y+h);
        glVertex2i(x,   y+h);
    glEnd();
}

void TS_Line(int x1, int y1, int x2, int y2, TS_Color *color){
    glBegin(GL_LINES);
        GLColor(color);
        glVertex2i(x1, y1);
        glVertex2i(x2, y2);
    glEnd();
}

void TS_Pixel(int x, int y, TS_Color *color){
    glBegin(GL_POINTS);
        GLColor(color);
        glVertex2i(x, y);
    glEnd();
}

void TS_Triangle(int x1, int y1, int x2, int y2, int x3, int y3, TS_Color *color){
    glBegin(GL_TRIANGLES);
        GLColor(color);
        glVertex2i(x1, y1);
        glVertex2i(x2, y2);
        glVertex2i(x3, y3);
    glEnd();
}

void TS_Polygon(int numPoints, int *x, int *y, TS_Color *color){
    glBegin(GL_POLYGON);
            GLColor(color);
        for(int i = 0; i<numPoints; i++){
            glVertex2i(x[i], y[i]);
        }
    glEnd();
}


void TS_GradientRectangle(int x, int y, int w, int h, TS_Color *color1, TS_Color *color2, TS_Color *color3, TS_Color *color4){
    glBegin(GL_QUADS);
        GLColor(color1);
        glVertex2f(x,   y);
        GLColor(color2);
        glVertex2f(x+w, y);
        GLColor(color3);
        glVertex2f(x+w, y+h);
        GLColor(color4);
        glVertex2f(x,   y+h);
    glEnd();
}

void TS_GradientLine(int x1, int y1, int x2, int y2, TS_Color *color1, TS_Color *color2){
    glBegin(GL_LINES);
        GLColor(color1);
        glVertex2f(x1,  y1);
        GLColor(color2);
        glVertex2f(x2,  y2);
    glEnd();
}
void TS_GradientTriangle(int x1, int y1, int x2, int y2, int x3, int y3, TS_Color *color1, TS_Color *color2, TS_Color *color3){
    glBegin(GL_TRIANGLES);
        GLColor(color1);
        glVertex2f(x1,  y1);
        GLColor(color2);
        glVertex2f(x2,  y2);
        GLColor(color3);
        glVertex2f(x3,  y3);
    glEnd();
}

void TS_GradientPolygon(int numPoints, int *x, int *y, TS_Color **color){
    glBegin(GL_POLYGON);
        for(int i = 0; i<numPoints; i++){
            GLColor(color[i]);
            glVertex2i(x[i], y[i]);
        }
    glEnd();
}

void TS_OutlinedRectangle(int x, int y, int w, int h, TS_Color *color, int thickness){
    glBegin(GL_QUADS);
        GLColor(color);
        glVertex2i(x,   y);
        glVertex2i(x+w, y);
        glVertex2i(x+w, y+thickness);
        glVertex2i(x,   y+thickness);

        glVertex2i(x,   y+h-thickness);
        glVertex2i(x+w, y+h-thickness);
        glVertex2i(x+w, y+h);
        glVertex2i(x,   y+h);

        glVertex2i(x,             y-thickness);
        glVertex2i(x+thickness,   y-thickness);
        glVertex2i(x+thickness,   y+h-thickness);
        glVertex2i(x,             y+h-thickness);

        glVertex2i(x+w-thickness, y+thickness);
        glVertex2i(x+w,           y+thickness);
        glVertex2i(x+w,           y+h-thickness);
        glVertex2i(x+w-thickness, y+h-thickness);
        /*
        for(int i = 0; i<thickness; i++){
            glVertex2i(x,   y+i);
            glVertex2i(x+w, y+i);

            glVertex2i(x,   y+h-i);
            glVertex2i(x+w, y+h-i);

            glVertex2i(x+i,   y+thickness);
            glVertex2i(x+i,   y+h-thickness);

            glVertex2i(x+w-i, y+thickness);
            glVertex2i(x+w-i, y+h-thickness);
        }
        */
    glEnd();
}

void TS_OutlinedPolygon(int numPoints, int *x, int *y, TS_Color *color){
    glBegin(GL_LINE_LOOP);
        GLColor(color);
        for(int i = 0; i<numPoints; i++){
            glVertex2i(x[i], y[i]);
        }
    glEnd();
}
