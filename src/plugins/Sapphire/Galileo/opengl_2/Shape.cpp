#include "Shape.hpp"
#include "Shader.hpp"

#ifdef OS_X
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "../Sapphire.hpp"

#include <cassert>

namespace Sapphire{
namespace Galileo {

namespace GL {

  Operation::~Operation(){}
  Operation::Operation(){}

void Drawable::InitGL(void){

}

void Drawable::CloseGL(void){

}

template<typename T>
inline float lNormalizeChannel(T aIn){float f = aIn; return f/255.0f;}

void Drawable::BindBuffer(){
}
void Drawable::BindArray(){
}

}

Shape::Shape(std::vector<Vertex> &aVertices, const std::shared_ptr<Image> aImage)
  : Drawable(aVertices, aImage)
{
    
}

void Shape::FillGL(void){
    
    vertex_size = mVertices.size();

    gl_mode = GL_TRIANGLE_STRIP;
    if(vertex_size==4)
    gl_mode = GL_TRIANGLE_FAN;
    else if(vertex_size==2)
      gl_mode = GL_LINE_STRIP;
    else if(vertex_size==1)
      gl_mode = GL_POINTS;

}

bool Shape::CanUse(Shader *aShader){
    return true;
}

void Shape::SetShader(std::shared_ptr<Shader> aShader){

}

static void DrawVertex_(const Vertex &vertex){
    glColor4ub(vertex.color.red, vertex.color.green, vertex.color.blue, vertex.color.alpha);
    glTexCoord2f(vertex.u, vertex.v);
    glVertex2i(vertex.x, vertex.y);
}

int Shape::Draw(){

    assert(mImage.get()!=nullptr);

    if(!mVertices.empty()){
        mImage->Bind();
        glBegin(gl_mode);
        std::for_each(mVertices.cbegin(), mVertices.cend(), DrawVertex_);    
        glEnd();
    }
    return 0;
}

}
}
