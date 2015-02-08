#pragma once
#include <color.h>

namespace Sapphire {
namespace Galileo{

class Vertex {
public:

    int x;
    int y;
    float u;
    float v;
    TS_Color color;

    Vertex(int _x, int _y, float _u, float _v);
    Vertex(const Vertex&);
    Vertex(){};
    ~Vertex() {}

    int GetX(void);
    int GetY(void);

    void SetX(int);
    void SetY(int);
};





}
}
