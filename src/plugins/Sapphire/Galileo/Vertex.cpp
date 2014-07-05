#include "Vertex.hpp"

namespace Sapphire{
namespace Galileo {

Vertex::Vertex(int _x, int _y){x = _x; y = _y;}

int Vertex::GetX(void){return x;}
int Vertex::GetY(void){return y;}

void Vertex::SetX(int _x){x = _x;}
void Vertex::SetY(int _y){y = _y;}

Vertex::Vertex(const Vertex& aVertex){

    x = aVertex.x;
    y = aVertex.x;

    color.red   = aVertex.color.red;
    color.green = aVertex.color.green;
    color.blue  = aVertex.color.blue;
    color.alpha = aVertex.color.alpha;
}

}
}
