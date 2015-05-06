#include "Shape.hpp"
#include "Shader.hpp"

#ifdef OS_X
#include <OpenGL/gl3.h>
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
    glGenVertexArrays(1, &mVertexArray);
    assert(mVertexArray);

    glGenBuffers(3, mBuffer);
    assert(mBuffer[0]);
    assert(mBuffer[1]);
    assert(mBuffer[2]);
}

void Drawable::CloseGL(void){
    glDeleteVertexArrays(1, &mVertexArray);
    glDeleteBuffers(3, mBuffer);
}

template<typename T>
inline float lNormalizeChannel(T aIn){float f = aIn; return f/255.0f;}

void Drawable::BindBuffer(){
}
void Drawable::BindArray(){
    glBindVertexArray(mVertexArray);
}

}

Shape::Shape(std::vector<Vertex> &aVertices, const std::shared_ptr<Image> aImage)
  : Drawable(aVertices, aImage)
{

}

void Shape::FillGL(void){
    assert(sizeof(float)*8 == sizeof(GL::VertexU));
    assert(sizeof(float)==4);

    vertex_size = mVertices.size();

    // Buffers
    float *Vertex  = (float*)alloca(sizeof(float)*vertex_size*2);
    float *Texcoord= (float*)alloca(sizeof(float)*vertex_size*2);
    float *Color   = (float*)alloca(sizeof(float)*vertex_size*4);

    // Buffer the information to be passed, packing it tight as possible.
    for(int i = 0; i<vertex_size; i++){
        Vertex[(i*2)+0] = mVertices[i].x;
        Vertex[(i*2)+1] = mVertices[i].y;
        Texcoord[(i*2)+0] = mVertices[i].u;
        Texcoord[(i*2)+1] = mVertices[i].v;

        Color[(i*4)+0] = GL::lNormalizeChannel<uint8_t>(mVertices[i].color.red);
        Color[(i*4)+1] = GL::lNormalizeChannel<uint8_t>(mVertices[i].color.green);
        Color[(i*4)+2] = GL::lNormalizeChannel<uint8_t>(mVertices[i].color.blue);
        Color[(i*4)+3] = GL::lNormalizeChannel<uint8_t>(mVertices[i].color.alpha);

    }

    glBindBuffer(GL_ARRAY_BUFFER, mBuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, 4*vertex_size*2,
                 Vertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, mBuffer[1]);
    glBufferData(GL_ARRAY_BUFFER, 4*vertex_size*2,
                 Texcoord, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, mBuffer[2]);
    glBufferData(GL_ARRAY_BUFFER, 4*vertex_size*4,
                 Color, GL_STATIC_DRAW);


    gl_mode = GL_TRIANGLE_STRIP;
    if(vertex_size==4)
    gl_mode = GL_TRIANGLE_FAN;
    else if(vertex_size==2)
      gl_mode = GL_LINE_STRIP;
    else if(vertex_size==1)
      gl_mode = GL_POINTS;

}

bool Shape::CanUse(ShaderProgram *aShader){
    return
    (aShader->FindAttribute(ShaderProgram::ShaderPositionName)>=0) &&
    (aShader->FindAttribute(ShaderProgram::ShaderTextureUVName)>=0) &&
    (aShader->FindAttribute(ShaderProgram::ShaderColorName)>=0) &&
    (aShader->FindUniform(ShaderProgram::ShaderOffsetUniformName)>=0);

}

void Shape::SetShader(std::shared_ptr<ShaderProgram> aShader){
    mShader = aShader;

    assert(CanUse(aShader.get()));

    //Update Vertex Array!
    Bind();

    int lShaderPosition  = aShader->FindAttribute(ShaderProgram::ShaderPositionName);
    int lShaderTextureUV = aShader->FindAttribute(ShaderProgram::ShaderTextureUVName);
    int lShaderColor     = aShader->FindAttribute(ShaderProgram::ShaderColorName);

    assert(lShaderPosition>=0);
    assert(lShaderTextureUV>=0);
    assert(lShaderColor>=0);

    glEnableVertexAttribArray(lShaderPosition);
    glEnableVertexAttribArray(lShaderTextureUV);
    glEnableVertexAttribArray(lShaderColor);

    glBindBuffer(GL_ARRAY_BUFFER, mBuffer[0]);
    glVertexAttribPointer(lShaderPosition, 2, GL_FLOAT, GL_FALSE,
                          0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, mBuffer[1]);
    glVertexAttribPointer(lShaderTextureUV, 2, GL_FLOAT, GL_FALSE,
                          0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, mBuffer[2]);
    glVertexAttribPointer(lShaderColor, 4, GL_FLOAT, GL_FALSE,
                          0, NULL);
}

int Shape::Draw(){

    assert(mImage.get()!=nullptr);

    BindArray();
    mImage->Bind();

    glDrawArrays(gl_mode, 0, vertex_size);

    return 0;
}

}
}
