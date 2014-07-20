#include "Shape.hpp"
#include "Shader.hpp"

#ifdef OS_X
#include <OpenGL/gl3.h>
#endif // OS_X

#include "../Sapphire.hpp"

#include <cassert>
#include <v8.h>

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
    //glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
}
void Drawable::BindArray(){
    glBindVertexArray(mVertexArray);
}

}

Shape::Shape(std::vector<Vertex> &aVertices, Image *aImage)
  : Drawable(aVertices)
{
    mImage = aImage;
}

void Shape::FillGL(void){
    assert(sizeof(float)*8 == sizeof(GL::VertexU));
    //GL::VertexU *lVertexData = new GL::VertexU[mVertices.size()];
/*
    std::vector<GL::VertexU> lVertexArray;

    GL::VertexU lVertex;
*/
    float *Vertex  = (float*)alloca(sizeof(float)*mVertices.size()*2);
    float *Texcoord= (float*)alloca(sizeof(float)*mVertices.size()*2);
    float *Color   = (float*)alloca(sizeof(float)*mVertices.size()*4);

    for(int i = 0; i<mVertices.size(); i++){
        Vertex[(i*2)+0] = mVertices[i].x;
        Vertex[(i*2)+1] = mVertices[i].y;
        Texcoord[(i*2)+0] = mVertices[i].u;
        Texcoord[(i*2)+1] = mVertices[i].v;

        Color[(i*4)+0] = GL::lNormalizeChannel<uint8_t>(mVertices[i].color.red);
        Color[(i*4)+1] = GL::lNormalizeChannel<uint8_t>(mVertices[i].color.green);
        Color[(i*4)+2] = GL::lNormalizeChannel<uint8_t>(mVertices[i].color.blue);
        Color[(i*4)+3] = GL::lNormalizeChannel<uint8_t>(mVertices[i].color.alpha);

    }

    /*

    for(auto lIterator = mVertices.begin(); lIterator!=mVertices.end(); lIterator++){

        lVertex.s.x = lIterator->GetX();
        lVertex.s.y = lIterator->GetY();
        lVertex.s.u = lIterator->u;
        lVertex.s.v = lIterator->v;
        lVertex.s.color[0] =GL::lNormalizeChannel<uint8_t>(lIterator->color.red);
        lVertex.s.color[1] =GL::lNormalizeChannel<uint8_t>(lIterator->color.green);
        lVertex.s.color[2] =GL::lNormalizeChannel<uint8_t>(lIterator->color.blue);
        lVertex.s.color[3] =GL::lNormalizeChannel<uint8_t>(lIterator->color.alpha);

        //printf(BRACKNAME " Info: X:%f\tY:%f\tU:%f\tV:%f\n", lVertex.s.x, lVertex.s.y, lVertex.s.u, lVertex.s.v);

        //lVertexArray.push_back(lVertex);


    }
    */
    //Bind();
/*
    GL::VertexU *Vertices = (GL::VertexU *)alloca(lVertexArray.size()*sizeof(GL::VertexU));
    memcpy(Vertices, &(lVertexArray[0]), lVertexArray.size()*sizeof(GL::VertexU));

    float *Check = (float *)(&(Vertices[1]));

    printf("The second vertex(these numbers should be equal:"
                              "\n\t%f\t%f"
                              "\n\t%f\t%f"
                              "\n\t%f\t%f"
                              "\n\t%f\t%f"
                              "\n\t%f\t%f"
                              "\n\t%f\t%f"
                              "\n\t%f\t%f"
                              "\n\t%f\t%f\n",
                              Vertices[1].s.x, Check[0],
                              Vertices[1].s.y, Check[1],
                              Vertices[1].s.u, Check[2],
                              Vertices[1].s.v, Check[3],
                              Vertices[1].s.color[0], Check[4],
                              Vertices[1].s.color[1], Check[5],
                              Vertices[1].s.color[2], Check[6],
                              Vertices[1].s.color[3], Check[7]);
*/

    glBindBuffer(GL_ARRAY_BUFFER, mBuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mVertices.size()*2,
                 Vertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, mBuffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mVertices.size()*2,
                 Texcoord, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, mBuffer[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mVertices.size()*4,
                 Color, GL_STATIC_DRAW);

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
    glEnableVertexAttribArray(lShaderPosition);
    glEnableVertexAttribArray(lShaderTextureUV);
    glEnableVertexAttribArray(lShaderColor);

}

int Shape::Draw(){

    assert(mImage!=nullptr);

    //glEnable(GL_TEXTURE_2D);
    BindArray();

    mImage->Bind();

    //SetShader(mShader);

    static auto lmode = (mVertices.size()>=3)?GL_TRIANGLE_FAN:((mVertices.size()==2)?GL_LINE_LOOP:GL_POINTS);

    //printf(BRACKNAME " Info: Drawing a primitive with %lu size.\n", mVertices.size());


    glDrawArrays(lmode, 0, mVertices.size());

    return 0;
}

}
}
