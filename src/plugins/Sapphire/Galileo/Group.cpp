#include "Group.hpp"
#include "State.hpp"
#include <cassert>

#ifdef OS_X
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

namespace Sapphire{
namespace Galileo {

Group::Group()
  : mOffset{0.0f, 0.0f}
  , mRotOffset{0.0f, 0.0f}
  , mAngle(0.0f)
  , mShader(nullptr)
{

}

Group::~Group(){
}

int Group::Draw(void){

    std::for_each(begin(), end(), std::mem_fn(&GL::Operation::Draw));

    return 0;

}

int Group::DrawAll(concurrent_queue<GL::Operation *> *aSendTo){
    aSendTo->push(mShader.get());

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
                  1, mOffset, (ShaderParamChange::callback_t)glUniform2fv, 8));
    aSendTo->push(new ShaderParamChange(lRotPosition,
                  1, mRotOffset, (ShaderParamChange::callback_t)glUniform2fv, 8));
    aSendTo->push(new ShaderParamChange(lAngle,
                  1, &mAngle, (ShaderParamChange::callback_t)glUniform1fv, 4));

    aSendTo->push(this);

    return 0;

}

int Group::Draw(concurrent_queue<GL::Operation *> *aSendTo){
    aSendTo->push(this);
    return 0;
}


int Group::DrawRange(concurrent_queue<GL::Operation *> *aSendTo, iterator aFrom, iterator aTo){

    aSendTo->push(mShader.get());

    aSendTo->push(new ShaderParamChange(mShader->mUniforms.find(Shader::ShaderOffsetUniformName)->second,
                  1, mOffset, (ShaderParamChange::callback_t)glUniform2fv, 8));

    aSendTo->push(new ShaderParamChange(mShader->mUniforms.find(Shader::ShaderRotOffsetUniformName)->second,
                  1, mRotOffset, (ShaderParamChange::callback_t)glUniform2fv, 8));

    aSendTo->push(new ShaderParamChange(mShader->mUniforms.find(Shader::ShaderAngleUniformName)->second,
                  1, &mAngle, (ShaderParamChange::callback_t)glUniform1fv, 4));

    std::for_each(aFrom, aTo, std::mem_fn(&GL::Operation::Draw));

    return 0;

}

}
}
