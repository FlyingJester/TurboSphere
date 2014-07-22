#include "Vertex.hpp"

namespace Sapphire{
namespace Galileo {

Vertex::Vertex(int _x, int _y, float _u, float _v){
    x = _x; y = _y;
    u = _u; v = _v;
    color.red = 255; color.green = 255; color.blue = 255; color.alpha = 255;
}

int Vertex::GetX(void){return x;}
int Vertex::GetY(void){return y;}

void Vertex::SetX(int _x){x = _x;}
void Vertex::SetY(int _y){y = _y;}

Vertex::Vertex(const Vertex& aVertex){

    x = aVertex.x;
    y = aVertex.x;
    u = aVertex.u;
    v = aVertex.u;

    color.red   = aVertex.color.red;
    color.green = aVertex.color.green;
    color.blue  = aVertex.color.blue;
    color.alpha = aVertex.color.alpha;
}

}
}
