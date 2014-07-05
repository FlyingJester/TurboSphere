#include "Shape.hpp"

#ifdef OS_X
#include <OpenGL/gl3.h>

#endif // OS_X

#include <cassert>

namespace Sapphire{
namespace Galileo {

namespace GL {
void Drawable::InitGL(void){
    glGenVertexArrays(1, &mVertexArray);
    assert(mVertexArray);

    glGenBuffers(1, &mBuffer);
    assert(mBuffer);
}

void Drawable::CloseGL(void){
    glDeleteVertexArrays(1, &mVertexArray);
    glDeleteBuffers(1, &mBuffer);
}

template<typename T>
inline float lNormalizeChannel(T aIn){float f = aIn; return f/255.0f;}

void Drawable::BindBuffer(){
    glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
}
void Drawable::BindArray(){
    glBindVertexArray(mVertexArray);
}

}

void Shape::FillGL(void){
    assert(mBuffer);
    assert(sizeof(float)*8 == sizeof(GL::VertexU));
    //GL::VertexU *lVertexData = new GL::VertexU[mVertices.size()];

    std::vector<GL::VertexU> lVertexArray;

    GL::VertexU lVertex;
    for(auto lIterator = mVertices.begin(); lIterator!=mVertices.end(); lIterator++){
        lVertex.s.x = lIterator->GetX();
        lVertex.s.y = lIterator->GetY();
        lVertex.s.color[0] =GL::lNormalizeChannel<uint8_t>(lIterator->color.red);
        lVertex.s.color[1] =GL::lNormalizeChannel<uint8_t>(lIterator->color.green);
        lVertex.s.color[2] =GL::lNormalizeChannel<uint8_t>(lIterator->color.blue);
        lVertex.s.color[3] =GL::lNormalizeChannel<uint8_t>(lIterator->color.alpha);

        lVertexArray.push_back(lVertex);
    }

    Bind();

    glBufferData(GL_ARRAY_BUFFER, lVertexArray.size()*sizeof(float)*8,
                 &(lVertexArray[0]), GL_STATIC_DRAW);

}

bool Shape::CanUse(Shader *aShader){

    return
    (aShader->mAttributes.find(Shader::ShaderPositionName)        != aShader->mAttributes.end()) &&
    (aShader->mAttributes.find(Shader::ShaderTextureUVName)       != aShader->mAttributes.end()) &&
    (aShader->mAttributes.find(Shader::ShaderColorName)           != aShader->mAttributes.end()) &&
    (aShader->mUniforms.find(Shader::ShaderOffsetUniformName)     != aShader->mUniforms.end()  ) &&
    true;

}

void Shape::SetShader(Shader *aShader){
    mShader = aShader;

    assert(CanUse(aShader));

    //Update Vertex Array!
    Bind();

    int lShaderPosition  = aShader->mAttributes.find(Shader::ShaderPositionName  )->second;
    int lShaderTextureUV = aShader->mAttributes.find(Shader::ShaderTextureUVName )->second;
    int lShaderColor     = aShader->mAttributes.find(Shader::ShaderColorName     )->second;

    glEnableVertexAttribArray(lShaderPosition);
    glVertexAttribPointer(lShaderPosition, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float)*6, (void*)(sizeof(float)*0));

    glEnableVertexAttribArray(lShaderTextureUV);
    glVertexAttribPointer(lShaderTextureUV, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float)*6, (void*)(sizeof(float)*2));

    glEnableVertexAttribArray(lShaderColor);
    glVertexAttribPointer(lShaderColor, 4, GL_FLOAT, GL_FALSE,
                          sizeof(float)*4, (void*)(sizeof(float)*4));

}


}
}
