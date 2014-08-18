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
    mRotOffset[0] = 0.0f;
    mRotOffset[1] = 0.0f;
    mAngle = 0.0f;
}

Group::~Group(){
}

int Group::Draw(void){
    mShader->Bind();
    int lPosition = mShader->mUniforms.find(Shader::ShaderOffsetUniformName)->second;
    int lRotPosition = mShader->mUniforms.find(Shader::ShaderRotOffsetUniformName)->second;
    int lAngle = mShader->mUniforms.find(Shader::ShaderAngleUniformName)->second;
    glUniform2f(lPosition, mOffset[0], mOffset[1]);
    glUniform2f(lRotPosition, mRotOffset[0], mRotOffset[1]);
    glUniform1f(lAngle, mAngle);
    iterator lIter = begin();

    while(lIter!=end()){

        (*lIter)->Draw();
        lIter++;

    }

    return 0;

}

int Group::DrawAll(concurrent_queue<GL::Operation *> *aSendTo){
    aSendTo->push(mShader);

    auto o = mShader->mUniforms.find(Shader::ShaderOffsetUniformName);
    auto r = mShader->mUniforms.find(Shader::ShaderRotOffsetUniformName);
    auto a = mShader->mUniforms.find(Shader::ShaderAngleUniformName);

    assert(o!=mShader->mUniforms.end());
    assert(r!=mShader->mUniforms.end());
    assert(a!=mShader->mUniforms.end());

    int lPosition = o->second;
    int lRotPosition = r->second;
    int lAngle = a->second;

    assert(lPosition>=0);
    assert(lRotPosition>=0);
    assert(lAngle>=0);

    aSendTo->push(new ShaderParamChange(lPosition,
                  1, mOffset, (ShaderParamChange::callback_t)glUniform2fv, sizeof(float)*2));
    aSendTo->push(new ShaderParamChange(lRotPosition,
                  1, mRotOffset, (ShaderParamChange::callback_t)glUniform2fv, sizeof(float)*2));
    aSendTo->push(new ShaderParamChange(lAngle,
                  1, &mAngle, (ShaderParamChange::callback_t)glUniform1fv, sizeof(float)*1));

    for(iterator lIter = begin(); lIter!=end(); lIter++){
        aSendTo->push(*lIter);
    }

    return 0;

}

int Group::DrawRange(concurrent_queue<GL::Operation *> *aSendTo, iterator aFrom, iterator aTo){

    aSendTo->push(mShader);
    aSendTo->push(new ShaderParamChange(mShader->mUniforms.find(Shader::ShaderOffsetUniformName)->second,
                  1, mOffset, (ShaderParamChange::callback_t)glUniform2fv, sizeof(float)*2));

    aSendTo->push(new ShaderParamChange(mShader->mUniforms.find(Shader::ShaderRotOffsetUniformName)->second,
                  1, mRotOffset, (ShaderParamChange::callback_t)glUniform2fv, sizeof(float)*2));

    aSendTo->push(new ShaderParamChange(mShader->mUniforms.find(Shader::ShaderAngleUniformName)->second,
                  1, &mAngle, (ShaderParamChange::callback_t)glUniform1fv, sizeof(float)*1));

    for(iterator lIter = aFrom; lIter!=aTo; lIter++){
        aSendTo->push(*lIter);
    }

    return 0;

}

}
}
