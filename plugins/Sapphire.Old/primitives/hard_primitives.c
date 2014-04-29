#include "../backend.h"
#include "../loadfunc/gl_extra.h"
#include <stdlib.h>

#ifdef __linux__
#include <alloca.h>
#endif

#include "circle.h"

#define NotVisible(a, b, c, d) {}
/*
inline bool NotVisible(int x, int y, int w, int h){

    if(x+w<ClipRectX)
        return true;
    if(y+h<ClipRectY)
        return true;
    if(x>ClipRectX+ClipRectW)
        return true;
    if(y>ClipRectY+ClipRectH)
        return true;

    return false;

}
*/


void ImmediateDrawPoint(int x, int y, RGBA color){

    glBindTexture(GL_TEXTURE_2D, EmptyTexture);

    glColorPointer(4, GL_UNSIGNED_BYTE, 0, &color);
    glBindBuffer(GL_ARRAY_BUFFER, ZeroTexCoordBuffer);
    glTexCoordPointer(2, GL_FLOAT, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    {
        GLint vertex[] = {x, y};

        glVertexPointer(2, GL_INT, 0, vertex);
    }

    glDrawArrays(GL_POINTS, 0, 1);

}

void ImmediateDrawPointSeries(int** points, int length, RGBA color){

    if(!length)
        return;

    if(length<MAX_STACK_STEAL)
        goto onstack;
    {
        RGBA *colors = NULL;
        GLint *vertex = NULL;

        colors = malloc(4*length);

        vertex = malloc(8*length);

        goto fill;

    onstack:;


        colors = alloca(4*length);

        vertex = alloca(8*length);

    fill:;
        {
            GLint *texCoords = calloc(8, length);
            int i = 0;

            for(i = 0; i<length; ++i){
                colors[i] = color;
                vertex[(i*2) ] = points[i][0];
                vertex[(i*2)+1] = points[i][1];
            }
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
            glTexCoordPointer(2, GL_INT, 0, texCoords);
            glVertexPointer(2, GL_INT, 0, vertex);
            glBindTexture(GL_TEXTURE_2D, EmptyTexture);

            glDrawArrays(GL_POINTS, 0, length);

            free(texCoords);
        }
        if(length<MAX_STACK_STEAL)
            return;

        free(colors);
        free(vertex);

    }
}


void ImmediateDrawGradientLine(int x[2], int y[2], RGBA colors[2]){

    GLint vertex[] = {x[0], y[0], x[1], y[1]};

    glBindTexture(GL_TEXTURE_2D, EmptyTexture);

    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
    glBindBuffer(GL_ARRAY_BUFFER, ZeroTexCoordBuffer);
    glTexCoordPointer(2, GL_FLOAT, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glVertexPointer(2, GL_INT, 0, vertex);

    glDrawArrays(GL_LINES, 0, 2);
}

void ImmediateDrawLine(int x[2], int y[2], RGBA color){

    RGBA colors[] = {color, color};
    ImmediateDrawGradientLine(x, y, colors);
}

void ImmediateDrawLineSeries(int** points, int length, RGBA color, int type){

    if(!length)
        return;

    {
        GLenum gltype = GL_LINES;

        switch (type){
            case 1:
                gltype = GL_LINE_STRIP;
                break;
            case 2:
                gltype = GL_LINE_LOOP;
        }

        {
            RGBA *colors = NULL;
            GLint *vertex = NULL;

            if(length<MAX_STACK_STEAL)
                goto onstack;

            colors = malloc(4*length);

            vertex = malloc(8*length);

            goto fill;

        onstack:;


            colors = alloca(4*length);

            vertex = alloca(8*length);

        fill:;
            {
                GLint *texCoords = calloc(8, length);
                int i = 0;

                for(; i<length; ++i){
                    colors[i] = color;
                    vertex[(i*2) ] = points[i][0];
                    vertex[(i*2)+1] = points[i][1];
                }
                glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
                glTexCoordPointer(2, GL_INT, 0, texCoords);
                glVertexPointer(2, GL_INT, 0, vertex);

                glBindTexture(GL_TEXTURE_2D, EmptyTexture);

                glDrawArrays(gltype, 0, length);

                free(texCoords);
            }
            if(length<MAX_STACK_STEAL)
                return;

            free(colors);
            free(vertex);
        }
    }
}


void ImmediateDrawGradientTriangle(int x[3], int y[3], RGBA colors[3]){
    //if(!(Colored(colors[0])|Colored(colors[1])|Colored(colors[2])))
    // return;
    GLint vertex[] = {x[0], y[0], x[1], y[1], x[2], y[2]};

    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);

    glBindBuffer(GL_ARRAY_BUFFER, ZeroTexCoordBuffer);
    glTexCoordPointer(2, GL_FLOAT, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glVertexPointer(2, GL_INT, 0, vertex);

    glBindTexture(GL_TEXTURE_2D, EmptyTexture);



    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);


}

void ImmediateDrawTriangle(int x[3], int y[3], RGBA color){

    RGBA colors[] = {color, color, color};

    ImmediateDrawGradientTriangle(x, y, colors);
}

void ImmediateDrawPolygon(int** points, int length, int invert, RGBA color){

    if(!length)
        return;

    if(length<MAX_STACK_STEAL)
        goto onstack;
    {
        RGBA *colors = NULL;
        GLuint *vertex = NULL;

        colors = malloc(4*length);

        vertex = malloc(8*length);

        goto fill;

    onstack:;


        colors = alloca(4*length);

        vertex = alloca(8*length);

    fill:;
        {
            GLint *texCoords = calloc(8, length);
            int i = 0;

            for(; i<length; ++i){
                colors[i] = color;
                vertex[(i*2) ] = points[i][0];
                vertex[(i*2)+1] = points[i][1];
            }
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
            glTexCoordPointer(2, GL_INT, 0, texCoords);
            glVertexPointer(2, GL_INT, 0, vertex);

            glBindTexture(GL_TEXTURE_2D, EmptyTexture);



            glDrawArrays(GL_TRIANGLE_STRIP, 0, length);




            free(texCoords);
        }
        if(length<MAX_STACK_STEAL)
            return;

        free(colors);
        free(vertex);
    }
}

void ImmediateDrawGradientRectangle(int x, int y, int w, int h, RGBA colors[4]){
    /*
    if(NotVisible(x, y, w, h)){
        return;
    }
    */

    glBindTexture(GL_TEXTURE_2D, EmptyTexture);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);

    glBindBuffer(GL_ARRAY_BUFFER, ZeroTexCoordBuffer);
    glTexCoordPointer(2, GL_FLOAT, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    {
        GLint vertex[] = {x, y, x+w, y, x+w, y+h, x, y+h};
        glVertexPointer(2, GL_INT, 0, vertex);
    }

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

}

void ImmediateDrawRectangle(int x, int y, int w, int h, RGBA color){

    RGBA colors[] = {color, color, color, color};

    ImmediateDrawGradientRectangle(x, y, w, h, colors);
}



void ImmediateDrawOutlinedRectangle(int x, int y, int w, int h, int size, RGBA color){

    /*
    if(NotVisible(x, y, w, h))
        return;
    */
    if(!size)
        return;

    {
        GLuint colors[] = {color, color, color, color, color, color, color, color};
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
    }

    glBindTexture(GL_TEXTURE_2D, EmptyTexture);
    glBindBuffer(GL_ARRAY_BUFFER, SeriousCoordBuffer);
    glTexCoordPointer(2, GL_FLOAT, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    {
        GLint vertex[] = {
            x, y,
            x+size, y+size,
            x, y+h,
            x+size, y+h-size,
            x+w, y+h,
            x+w-size, y+h-size,
            x+w, y,
            x+w-size, y+size
        };

        glVertexPointer(2, GL_INT, 0, vertex);
    }


    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);


}

void ImmediateDrawOutlinedEllipse(int x, int y, int rx, int ry, RGBA color){

    const unsigned int step = (unsigned)(SPI*(float)((rx>ry)?rx:ry));
    GLint * vertex = NULL;
    if(step<5){
        ImmediateDrawPoint(x, y, color);
        return;
    }

    vertex = ApproximateEllipseGL(x, y, rx, ry, step);
    {
        RGBA *colors = NULL;

        if(step<MAX_STACK_STEAL)
        goto onstack;

        colors = malloc(step<<2);

        goto fill;

    onstack:;

        colors = alloca(step<<2);

    fill:;
        {
            GLint *texCoords = calloc(8, step);
            unsigned int i = 0;

            for(; i<step; i++){
                colors[i] = color;
            }

            glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
            glTexCoordPointer(2, GL_INT, 0, texCoords);
            glVertexPointer(2, GL_INT, 0, vertex);

            glBindTexture(GL_TEXTURE_2D, EmptyTexture);

            glDrawArrays(GL_LINE_LOOP, 0, step);

            FreeEllipsePointsGL(vertex);

            free(texCoords);
        }
        if(step<MAX_STACK_STEAL)
            return;

        free(colors);
    }
}
void ImmediateDrawFilledEllipse(int x, int y, int rx, int ry, RGBA color){

    const unsigned int step = (unsigned)(SPI*(float)((rx>ry)?rx:ry));

    GLint *vertex = NULL;

    if(step<5){
        ImmediateDrawPoint(x, y, color);
        return;
    }

    vertex = ApproximateEllipseGL(x, y, rx, ry, step);

    {
        RGBA *colors = NULL;

        if(step<MAX_STACK_STEAL)
        goto onstack;

        colors = malloc(step<<2);

        goto fill;

    onstack:;

        colors = alloca(step<<2);

    fill:;
        {
            GLint *texCoords = calloc(8, step);
            unsigned int i = 0;

            for(; i<step; i++){
                colors[i] = color;
            }

            glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
            glTexCoordPointer(2, GL_INT, 0, texCoords);
            glVertexPointer(2, GL_INT, 0, vertex);

            glBindTexture(GL_TEXTURE_2D, EmptyTexture);

            glDrawArrays(GL_TRIANGLE_FAN, 0, step);

            FreeEllipsePointsGL(vertex);

            free(texCoords);
        }
        if(step<MAX_STACK_STEAL)
        return;

        free(colors);
    }
}

void ImmediateDrawOutlinedCircle(int x, int y, int r, RGBA color, int antialias){

    const unsigned int step = (unsigned)(SPI*((float)r));

GLint * vertex = NULL;

    if(r<3){
        ImmediateDrawPoint(x, y, color);
        return;
    }

    vertex = ApproximateCircleGL(x, y, r, step);
    {
        RGBA *colors = NULL;

        if(step<MAX_STACK_STEAL)
            goto onstack;

        colors = malloc(step<<2);

        goto fill;

    onstack:;

        colors = alloca(step<<2);

    fill:;
        {
            GLint *texCoords = calloc(8, step);
            unsigned int i = 0;

            for(; i<step; i++){
                colors[i] = color;
            }

            glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
            glTexCoordPointer(2, GL_INT, 0, texCoords);
            glVertexPointer(2, GL_INT, 0, vertex);

            glDrawArrays(GL_LINE_LOOP, 0, step);

            FreeEllipsePointsGL(vertex);

            free(texCoords);
        }
        if(step<MAX_STACK_STEAL)
        return;

        free(colors);
    }
}

void ImmediateDrawFilledCircle(int x, int y, int r, RGBA color, int antialias){

    const unsigned int step = (unsigned)(SPI*((float)r));
    GLint * vertex = NULL;

    if(r<3){
        ImmediateDrawPoint(x, y, color);
        return;
    }

    vertex = ApproximateCircleGL(x, y, r, step);
    {
        RGBA *colors = NULL;

        if(step<MAX_STACK_STEAL)
        goto onstack;

        colors = malloc(step<<2);
        goto fill;

        onstack:;

        colors = alloca(step<<2);

        fill:;
        {
            GLint *texCoords = calloc(8, step);
            unsigned int i = 0;

            for(; i<step; i++){
                colors[i] = color;
            }

            glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
            glTexCoordPointer(2, GL_INT, 0, texCoords);
            glVertexPointer(2, GL_INT, 0, vertex);

            glBindTexture(GL_TEXTURE_2D, EmptyTexture);



            glDrawArrays(GL_TRIANGLE_FAN, 0, step);



            FreeEllipsePointsGL(vertex);

            free(texCoords);
        }

        if(step<MAX_STACK_STEAL)
            return;

        free(colors);
    }
}

void ImmediateDrawGradientCircle(int x, int y, int r, RGBA color[2], int antialias){

    GLint * vertex = NULL;
    unsigned int step = (unsigned)(SPI*((float)r));

    if(r<3){
        ImmediateDrawPoint(x, y, *color);
        return;
    }


    vertex = ApproximateCircleGL(x, y, r, step);

    step++;

    vertex[0] = x;
    vertex[1] = y;
    {
        RGBA *colors = NULL;

        if(step<MAX_STACK_STEAL)
        goto onstack;

        colors = malloc(step<<2);

        goto fill;

        onstack:;

        colors = alloca(step<<2);

        fill:;
        {
            GLint *texCoords = calloc(8, step);
            unsigned int i = 1;
            *colors = color[1];

            for(; i<step; i++){
                colors[i] = *color;
            }

            glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
            glTexCoordPointer(2, GL_INT, 0, texCoords);
            glVertexPointer(2, GL_INT, 0, vertex);
            glBindTexture(GL_TEXTURE_2D, EmptyTexture);

            glDrawArrays(GL_TRIANGLE_FAN, 0, step);

            FreeEllipsePointsGL(vertex);

            free(texCoords);
        }
        if(step<MAX_STACK_STEAL)
        return;

        free(colors);
    }
}
