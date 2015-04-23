#include "Group.hpp"
#include "State.hpp"
#include <cassert>

#ifdef OS_X
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace Sapphire{
namespace Galileo {

Group::Group()
  : mShader(nullptr)
  , mOffset{0.0f, 0.0f}
  , mRotOffset{0.0f, 0.0f}
  , mAngle(0.0f)
{

}

Group::~Group(){
}

enum change_type {offset, rot_offset, angle};

int Group::Draw(void){
    
    const float m2Pi = 2.0f*3.14159265358979323846f;
    
    float lAngle = mAngle;
    // Obtain a normalized angle
    while(lAngle<0.0f) lAngle+=m2Pi;
    while(lAngle>m2Pi) lAngle-=m2Pi;
    
    // We cache the actual changes because simply reversing a translation is an order of magnitude cheaper than PushMatrix and PopMatrix.
    // Reversing a rotation is much more complex. We simply fall back on PushMatrix and PopMatrix for that.
    // TODO: Reverse the rotation rather than popping the matrix.
    const bool pos_changed = !((mOffset[0]<1.0f) && (mOffset[0]>-1.0f) && (mOffset[1]<1.0f) && (mOffset[1]>-1.0f)),
        rot_changed = (!((mRotOffset[0]<1.0f) && (mRotOffset[0]>-1.0f) && (mRotOffset[1]<1.0f) && (mRotOffset[1]>-1.0f))) && (lAngle<0.01f);
        
    if(rot_changed){
        glPushMatrix();
    }
    
    if(pos_changed)
        glTranslatef(mOffset[0], mOffset[1], 0.0f);
    if(rot_changed)
        glRotatef(mAngle, mRotOffset[0], mRotOffset[1], 0.0f);
    
    for(const_iterator i = mShapes.cbegin(); i!=mShapes.cend(); i++){
        (*i)->Draw();
    }
    
    if(rot_changed){
        glPopMatrix();
    }
    else if(pos_changed){
        glTranslatef(-mOffset[0], -mOffset[1], 0.0f);
    }
    
    return 0;

}

int Group::DrawAll(std::queue<GL::Operation *> *aSendTo){
    aSendTo->push(this);        
    return 0;
}

int Group::Draw(std::queue<GL::Operation *> *aSendTo){
    aSendTo->push(this);
    return 0;
}


int Group::DrawRange(std::queue<GL::Operation *> *aSendTo, iterator aFrom, iterator aTo){
    assert(false);
    return 0;
}

}
}
