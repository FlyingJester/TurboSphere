#include "batcher.h"

/*
class TS_BatchImageOperation

    TS_BatchImageOperation();
    ~TS_BatchImageOperation();

    void Set(int x, int y, float coords[8], TS_Texture tex, uint32_t color[4]);
    virtual void Destroy(void);

    TS_Texture tex;
    float coords[8];
    int32_t x, y;
    uint32_t color[4];
*/

TS_BatchImageOperation::TS_BatchImageOperation(){
    colorSize  = 4;
    coordsSize = 8;
    vertexSize = 8;

    coords   = (float    *)malloc(coordsSize*4);
    color    = (uint32_t *)malloc(colorSize*4);
    vertices = (int32_t *)malloc(vertexSize*4);

}

TS_BatchImageOperation::~TS_BatchImageOperation(){
    free(coords);
    free(color);
    free(vertices);
}

void TS_BatchImageOperation::Set(int _x, int _y, float _coords[8], TS_Texture _tex, uint32_t _color[4], int32_t _vertices[8]){

    x = _x;
    y = _y;

    memcpy(coords, _coords,    coordsSize*4);
    memcpy(color,  _color,     colorSize*4);
    memcpy(vertices, _vertices, vertexSize*4);

    tex = _tex;

}
