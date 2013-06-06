#define GRAPHIC_INTERNAL
#include "main.h"
#include "primitives.h"
#include "image.h"
#include "color.h"
#include <sstream>
#include <assert.h>

#define XPROP v8::String::New("x")
#define YPROP v8::String::New("y")


#define GLSL(version, shader)  "#version " #version "\n" #shader

const GLchar *vert = GLSL(150,
    in vec2 position;

    void main()
    {
        gl_Position = vec4( position.x, position.y, 0.0, 1.0 );
    }
);

const GLchar *frag = GLSL(150,
    out vec4 outColor;
    void main()
    {
        outColor = vec4( 0.0, 1.0, 1.0, 0.01 );
    }
);

GLuint priInd;
GLuint triVBO;
GLuint rctVBO;
GLuint linVBO;

void PrimitivesInit(void){
    const GLubyte primitiveInd[] = {0, 1, 2, 3, 4, 5};

    glGenBuffers(1, &triVBO);
    glGenBuffers(1, &rctVBO);
    glGenBuffers(1, &linVBO);
    glGenBuffers(1, &priInd);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, priInd);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(primitiveInd), primitiveInd, GL_STATIC_DRAW);
}

void PutPrimitiveElementArray(GLuint numArrays){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, priInd);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, sizeof(GLint) * 2, 0);
}

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

    /*
    const GLint vertexData[] = {x, y, x+w, y, x+w, y+h, x, y+h};

    glBindBuffer(GL_ARRAY_BUFFER, rctVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_DYNAMIC_DRAW);

    PutPrimitiveElementArray(4);

    GLColor(color);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, 0);
    glDisableVertexAttribArray(0);
    */
    /*
    glBegin(GL_QUADS);
        GLColor(color);
        glVertex2i(x,   y);
        glVertex2i(x+w, y);
        glVertex2i(x+w, y+h);
        glVertex2i(x,   y+h);
    glEnd();
    */
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
    glBegin(GL_POINTS);
        GLColor(color);
        glVertex2i(x, y);
    glEnd();
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

	SDL_Surface* surface = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA, w, h, DEPTH, CHANNEL_MASKS);
	SDL_FillRect(surface, &locrect, color);
	SDL_BlitSurface(surface, NULL, destination, &rect);
    SDL_FreeSurface(surface);
}

void TS_SoftLine(int ax1, int ay1, int ax2, int ay2, TS_Color *c, SDL_Surface *destination){

    uint32_t color = c->toInt();

    int w = abs(ax1-ax2);
    int h = abs(ay1-ay2);

    SDL_Rect rect = {(short int)min(ax1, ax2), (short int)min(ay1, ay2), (short unsigned int)w, (short unsigned int)h};

    if(ax1-ax2<=1&&ax1-ax2>=-1){
        SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, 1, h, DEPTH, CHANNEL_MASKS);
        SDL_Rect allsurf = {0, 0, 1, (short unsigned int)h};
        SDL_FillRect(surface, &allsurf, color);

        SDL_BlitSurface(surface, NULL, screen, &rect);
        SDL_FreeSurface(surface);

        return;
    }
    else if(ay1-ay2<=1&&ay1-ay2>=-1){
        SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w, 1, DEPTH, CHANNEL_MASKS);
        SDL_Rect allsurf = {0, 0, (short unsigned int)w, 1};
        SDL_FillRect(surface, &allsurf, color);

        SDL_BlitSurface(surface, NULL, screen, &rect);
        SDL_FreeSurface(surface);

        return;
    }

    int x1 = ax1-min(ax1, ax2);
    int y1 = ay1-min(ay1, ay2);
    int x2 = ax2-min(ax1, ax2);
    int y2 = ay2-min(ay1, ay2);

    SDL_Rect temprect = {(short int)x1, (short int)y1, 1, 1};

	SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w+1, h+1, DEPTH, CHANNEL_MASKS);

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

    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA, rad*2, rad*2, DEPTH, CHANNEL_MASKS);

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

    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA, rad*2, rad*2, DEPTH, CHANNEL_MASKS);

    Uint32 *field = (Uint32 *)surface->pixels;
    int w = rad*2;
    double dis= 0;
    int xd2 = 0;
    int yd2 = 0;
    double r  = rad;
    int inf = (rad>4)?((rad>>2)+rad-floor((double)(((double)rad)/sqrt(2.0)))):(rad);

    for(int xt = 0; xt<=rad; xt++){
        xd2 = rad-xt;
        xd2*= xd2;
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
