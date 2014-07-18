#include "Group.hpp"
#include "State.hpp"
#include <cassert>
#include <OpenGL/gl3.h>

namespace Sapphire{
namespace Galileo {
/*
class Group : public GL::Operation {
protected:
    std::list<Shape *> mShapes;
    Shader *mShader;
    int mOffsetX, mOffsetY;
public:
*/

Group::Group(){
    mOffset[0] = 0.0f;
    mOffset[1] = 0.0f;
}

Group::~Group(){

}

int Group::Draw(void){
    mShader->Bind();
    int lPosition = mShader->mUniforms.find(Shader::ShaderOffsetUniformName)->second;
    glUniform2f(lPosition, mOffset[0], mOffset[1]);
    iterator lIter = begin();

    while(lIter!=end()){

        (*lIter)->Draw();
        lIter++;

    }

    return 0;

}

int Group::DrawAll(concurrent_queue<GL::Operation *> *aSendTo){
    aSendTo->push(mShader);
    auto r = mShader->mUniforms.find(Shader::ShaderOffsetUniformName);
    assert(r!=mShader->mUniforms.end());
    int lPosition = r->second;
    assert(lPosition>=0);
    aSendTo->push(new ShaderParamChange(lPosition,
                  1, mOffset, (ShaderParamChange::callback_t)glUniform2fv, sizeof(float)*2));
/*
    int loc = mShader->mUniforms[Shader::ShaderPositionName];
    GL::Operation *lOp = new ShaderParamChange(loc, 1, mOffset, (ShaderParamChange::callback_t)glUniform2fv, sizeof(float)*2);
    aSendTo->push(lOp);
*/
    for(iterator lIter = begin(); lIter!=end(); lIter++){
        aSendTo->push(*lIter);
    }

    return 0;

}

int Group::DrawRange(concurrent_queue<GL::Operation *> *aSendTo, iterator aFrom, iterator aTo){

    aSendTo->push(mShader);
    aSendTo->push(new ShaderParamChange(mShader->mUniforms.find(Shader::ShaderOffsetUniformName)->second,
                  1, mOffset, (ShaderParamChange::callback_t)glUniform2fv, sizeof(float)*2));

    for(iterator lIter = aFrom; lIter!=aTo; lIter++){
        aSendTo->push(*lIter);
    }

    return 0;

}

}
}
