#define GRAPHIC_INTERNAL
#include "main.h"
#include "primitives.h"
#include "image.h"
#include "color.h"
#include "surface.h"
#include <sstream>
#include <assert.h>
#include "../../SDL2/SDL_opengl.h"


#ifdef USE_AMDLIBM

    #define sin sin_override
    #define cos cos_override
    #define floor floor_override
    #define ceil  ceil_override
    #define min min_override
    #define max max_override
    #define sqrt sqrt_override
#endif


void TS_Rectangle(int x, int y, int w, int h, TS_Color *color){

    const GLint   vertexData[] = {x, y, x+w, y, x+w, y+h, x, y+h};
    const GLuint  colorData[]  = {
        color->toInt(),
        color->toInt(),
        color->toInt(),
        color->toInt()
    };

    glBindTexture(GL_TEXTURE_2D, TS_EmptyTexture);
    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_COLOR_ARRAY);

}

void TS_Line(int x1, int y1, int x2, int y2, TS_Color *color){

    const GLint vertexData[] = {x1, y1, x2, y2};
    const GLuint  colorData[]  = {
        color->toInt(),
        color->toInt()
    };
    glBindTexture(GL_TEXTURE_2D, TS_EmptyTexture);
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
    glBindTexture(GL_TEXTURE_2D, TS_EmptyTexture);
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
    glBindTexture(GL_TEXTURE_2D, TS_EmptyTexture);
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


    //glPushMatrix();
    const uint32_t color = c->toInt();
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
        colorData[i] = color;
    }

    glBindTexture(GL_TEXTURE_2D, TS_EmptyTexture);
    glVertexPointer(2, GL_INT, 0, points);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_LINE_LOOP, 0, numSegments);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    if(AA)
        glDisable(GL_LINE_SMOOTH);

    glTranslatef(-(float)x, -(float)y, 0.0f);
    //glPopMatrix();

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

    //glPushMatrix();

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

    glBindTexture(GL_TEXTURE_2D, TS_EmptyTexture);
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

    glTranslatef(-(float)x, -(float)y, 0.0f);
    //glPopMatrix();

    free(points);
    free(colorData);

}
