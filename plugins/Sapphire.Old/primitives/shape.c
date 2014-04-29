#include "shape.h"
#include "../loadfunc/gl_extra.h"
#include "../shader/shader.h"

void DrawShape(TS_Shape *shape){
    glBindVertexArray(shape->VertexArray);
    glBindTexture(GL_TEXTURE_2D, shape->texture);
    switch(shape->NumVertices){
        case 1:
            glDrawArrays(GL_POINTS, 0, 1);
            break;
        case 2:
            glDrawArrays(GL_LINES, 0, 2);
            break;
        default:
            glDrawArrays(GL_TRIANGLE_STRIP, 0, shape->NumVertices);
    }

    glBindVertexArray(0);

}

CurrentColorAttrib;
CurrentVertexAttrib;
CurrentTexcoordAttrib;

TS_Shape* CreateShape(size_t length, float *vertices, float *texcoords, float *colors, GLuint texture){
    TS_Shape *shape = malloc(sizeof(TS_Shape));

    shape->texture = texture;
    glGenVertexArrays(1, &(shape->VertexArray));
    glGenBuffers(3, shape->Buffers);
    glBindVertexArray(shape->VertexArray);


    glBindBuffer(GL_ARRAY_BUFFER, shape->Buffers[VERTEX_B]);
    glBufferData(GL_ARRAY_BUFFER, length<<3, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(CurrentVertexAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, shape->Buffers[COLORS_B]);
    glBufferData(GL_ARRAY_BUFFER, length<<4, colors, GL_STATIC_DRAW);
    glVertexAttribPointer(CurrentColorAttrib, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, shape->Buffers[TEXCRD_B]);
    glBufferData(GL_ARRAY_BUFFER, length<<3, texcoords, GL_STATIC_DRAW);
    glVertexAttribPointer(CurrentTexcoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return shape;
}
