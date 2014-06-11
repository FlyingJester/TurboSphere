
#include "primitives_script.hpp"
#include "primitive_types.h"
#include "primitives.h"

#include "galileo/vertex.hpp"
#include "galileo/shape.hpp"
#include "galileo/group.hpp"

using namespace Sapphire;

void Sapphire::Script::FlipScreen(Turbo::JSArguments){

    IncEngineFrame();
}

void Sapphire::PolygonOp::SetVertexAttribBinding(uint32_t vertex, uint32_t texcoord, uint32_t color){
    glBindVertexArray(Array);
    glBindBuffer(GL_ARRAY_BUFFER, GetVertex());
    glVertexAttribPointer(vertex,   2, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, GetTexcoord());
    glVertexAttribPointer(texcoord, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, GetColor());
    glVertexAttribPointer(color,    4, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
