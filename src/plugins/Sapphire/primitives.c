#include "stacksteal.h"
#include "primitives.h"
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
//#include "glextra/gl_extra.h"


void InitPrimitives(unsigned int ID){

}

inline int *ApproximateEllipseGL(const int x, const int y, const unsigned int rx,const unsigned int ry, const int step, int *const points/*OUT, should be at least step<<3 bytes*/){
    const float frx = rx;
    const float fry = ry;
    //= malloc(step<<3);
    float angle = 0;

    float inc = (3.1415f)/((float)(step-2));

    int i = 0;

    while(i<step){
        points[(i<<1)  ] = ((((float)(cosf(angle)))*(frx))+0.5f)+x;
        points[(i<<1)+1] = ((((float)(sinf(angle)))*(fry))+0.5f)+y;
        angle+=inc;
        i++;
    }
    return points;
}

/*
void TS_FilledComplex(int x, int y, unsigned int w, unsigned int h, unsigned int radius, uint32_t color);


void TS_GradientComplex(int x, int y, unsigned int w, unsigned int h, unsigned int radius, uint32_t color[2]);
*/

void TS_GradientLine(int x[2], int y[2], uint32_t color[2]){

    static const float texcoord[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    const float colors[8] = {
        ((float)((color[0]>>24)&0xFF))/255.0f,
        ((float)((color[0]>>16)&0xFF))/255.0f,
        ((float)((color[0]>>8 )&0xFF))/255.0f,
        ((float)((color[0]    )&0xFF))/255.0f,

        ((float)((color[1]>>24)&0xFF))/255.0f,
        ((float)((color[1]>>16)&0xFF))/255.0f,
        ((float)((color[1]>>8 )&0xFF))/255.0f,
        ((float)((color[1]    )&0xFF))/255.0f,
    };

    const int vertex[4] = {x[0], y[0], x[1], y[1]};

    glColorPointer(4, GL_FLOAT, 0, colors);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoord);
    glVertexPointer(2, GL_INT, 0, vertex);

    glDrawArrays(GL_LINE_LOOP, 0, 2);

}

void TS_OutlinedEllipse(int x, int y, unsigned int hradius, unsigned int vradius, float angle, uint32_t color){
    int step = 3*(hradius+vradius);
    int   *points;
    float *texcoord;
    float *colors;
    step>>=1;
    if((step<<10)<TS_STACK_STEAL){
        points   = alloca(step<<3);
        texcoord = alloca(step<<3);
        colors   = alloca(step<<4);
    }
    else{
        points   = malloc(step<<3);
        texcoord = malloc(step<<3);
        colors   = malloc(step<<4);
    }

    ApproximateEllipseGL(x, y, hradius, vradius, step, points);
    //TS_ShaderSetColorInt(color);

    const float colorf[4] = {
        ((float)((color>>24)&0xFF))/255.0f,
        ((float)((color>>16)&0xFF))/255.0f,
        ((float)((color>>8 )&0xFF))/255.0f,
        ((float)((color    )&0xFF))/255.0f,
    };

    for(int i = 0; i<step; i++){
        texcoord[(i<<1)]    = 0.0f;
        texcoord[(i<<1)+1]  = 0.0f;

        memcpy(colors+(i*sizeof(float)<<3), colorf, sizeof(float)<<2);

    }

    glColorPointer(4, GL_FLOAT, 0, colors);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoord);
    glVertexPointer(2, GL_INT, 0, points);

    //glBindTexture(GL_TEXTURE_2D, EmptyTexture);

    glDrawArrays(GL_LINE_LOOP, 0, step);



    if((step<<10)>=TS_STACK_STEAL){
        free(points   );
        free(texcoord );
        free(colors   );
    }

}


void TS_GradientEllipse(int x, int y, unsigned int hradius, unsigned int vradius, float angle, uint32_t color[2]){
    int step = 3*(hradius+vradius);
    int   *points;
    float *texcoord;
    float *colors;
    step>>=1;
    step++;
    if((step<<10)<TS_STACK_STEAL){
        points   = alloca(step<<3);
        texcoord = alloca(step<<3);
        colors   = alloca(step<<4);
    }
    else{
        points   = malloc(step<<3);
        texcoord = malloc(step<<3);
        colors   = malloc(step<<4);
    }

    ApproximateEllipseGL(x, y, hradius, vradius, step-1, points+8);

    points[0] = x;
    points[0] = y;
    //TS_ShaderSetColorInt(color);

    const float colorf[4] = {
        ((float)((*color>>24)&0xFF))/255.0f,
        ((float)((*color>>16)&0xFF))/255.0f,
        ((float)((*color>>8 )&0xFF))/255.0f,
        ((float)((*color    )&0xFF))/255.0f,
    };

    colors[0] = ((float)((color[1]>>24)&0xFF))/255.0f;
    colors[1] = ((float)((color[1]>>16)&0xFF))/255.0f;
    colors[2] = ((float)((color[1]>>8 )&0xFF))/255.0f;
    colors[3] = ((float)((color[1]    )&0xFF))/255.0f;

    texcoord[0] = 0.0f;
    texcoord[1] = 0.0f;
    for(int i = 1; i<step-1; i++){
        texcoord[(i<<1)]    = 0.0f;
        texcoord[(i<<1)+1]  = 0.0f;

        memcpy(colors+(i*sizeof(float)<<3), colorf, sizeof(float)<<2);
    }

    glColorPointer(4, GL_FLOAT, 0, colors);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoord);
    glVertexPointer(2, GL_INT, 0, points);

    //glBindTexture(GL_TEXTURE_2D, EmptyTexture);

    glDrawArrays(GL_LINE_LOOP, 0, step);



    if((step<<10)>=TS_STACK_STEAL){
        free(points   );
        free(texcoord );
        free(colors   );
    }
}
/*
void TS_GradientRectangle(int x, int y, unsigned int w, unsigned int h, uint32_t color[4]);
void TS_GradientTriangle(int x[3], int y[3], uint32_t color[3]);
void TS_Point(int x, int y, uint32_t color);
*/
