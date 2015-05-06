#include "Group.hpp"
#include "State.hpp"
#include <cassert>
#include <screen.h>

#ifdef OS_X
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

namespace Sapphire{
namespace Galileo {

Group::Group()
  : mShader(nullptr)
  , clip_rectangle()
  , mOffset{0.0f, 0.0f}
  , mRotOffset{0.0f, 0.0f}
  , mAngle(0.0f)
{

}

Group::~Group(){
}

int Group::Draw(void){

    return 0;

}

int Group::DrawAll(std::queue<GL::Operation *> *aSendTo){
    aSendTo->push(mShader.get());

    int lPosition = mShader->FindUniform(ShaderProgram::ShaderOffsetUniformName);
    int lRotPosition = mShader->FindUniform(ShaderProgram::ShaderRotOffsetUniformName);
    int lAngle = mShader->FindUniform(ShaderProgram::ShaderAngleUniformName);

    assert(lPosition>=0);
    assert(lRotPosition>=0);
    assert(lAngle>=0);

    aSendTo->push(new ShaderParamChange(lPosition,
                  1, mOffset, (ShaderParamChange::callback_t)glUniform2fv, 8));
    aSendTo->push(new ShaderParamChange(lRotPosition,
                  1, mRotOffset, (ShaderParamChange::callback_t)glUniform2fv, 8));
    aSendTo->push(new ShaderParamChange(lAngle,
                  1, &mAngle, (ShaderParamChange::callback_t)glUniform1fv, 4));
    
    aSendTo->push(&clip_rectangle);
    
    for(container::iterator iter = mShapes.begin(); iter!=mShapes.end(); iter++){
        aSendTo->push(*iter);
    }
        
    return 0;

}

int Group::Draw(std::queue<GL::Operation *> *aSendTo){
    aSendTo->push(this);
    return 0;
}


int Group::DrawRange(std::queue<GL::Operation *> *aSendTo, iterator aFrom, iterator aTo){

    aSendTo->push(mShader.get());

    aSendTo->push(new ShaderParamChange(mShader->FindUniform(ShaderProgram::ShaderOffsetUniformName),
                  1, mOffset, (ShaderParamChange::callback_t)glUniform2fv, 8));

    aSendTo->push(new ShaderParamChange(mShader->FindUniform(ShaderProgram::ShaderRotOffsetUniformName),
                  1, mRotOffset, (ShaderParamChange::callback_t)glUniform2fv, 8));

    aSendTo->push(new ShaderParamChange(mShader->FindUniform(ShaderProgram::ShaderAngleUniformName),
                  1, &mAngle, (ShaderParamChange::callback_t)glUniform1fv, 4));

    std::for_each(aFrom, aTo, std::mem_fn(&GL::Operation::Draw));

    return 0;

}

}
}
