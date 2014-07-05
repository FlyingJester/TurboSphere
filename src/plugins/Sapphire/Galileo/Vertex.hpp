#pragma once
#include <color.h>

namespace Sapphire {
namespace Galileo{

class Vertex {
public:

    int x;
    int y;
    TS_Color color;

    Vertex(int _x, int _y);
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
