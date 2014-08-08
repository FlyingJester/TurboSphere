#pragma once
#pragma once
#include <color.h>
#include <list>
#include <TSPR/concurrent_queue.h>
#include "Shape.hpp"
#include "Shader.hpp"

namespace Sapphire{
namespace Galileo {

class Group : public GL::Operation {
public:
    typedef std::list<GL::Operation *>::iterator iterator;
    typedef std::list<GL::Operation *>::reverse_iterator reverse_iterator;

protected:
    enum eOffset {x, y, d};
    std::list<GL::Operation *> mShapes;
    Shader *mShader;
    float mOffset[2];
    float mRotationOffset[2];
    float mRotationAngle;
public:

    Group();
    virtual ~Group();

    // a Group can function in one of two ways:
    //
    // Operation mode, which assumes that Draw is called on the thread that renders,
    // and Pipelined mode, which assumes that DrawAll is called, and will send
    // required operations to the concurrent_queue given.
    //

    template<typename T>
    void SetOffset(T _x, T _y){
        mOffset[eOffset::x] = _x;
        mOffset[eOffset::y] = _y;
    }

    template<typename T>
    void SetX(T _x){
        mOffset[eOffset::x] = _x;
    }
    template<typename T>
    void SetY(T _y){
        mOffset[eOffset::y] = _y;
    }

    template<typename T = float>
    T GetX() const{
        return mOffset[eOffset::x];
    }
    template<typename T = float>
    T GetY() const{
        return mOffset[eOffset::y];
    }


    template<typename T>
    void SetRotationOffset(T _x, T _y){
        mRotationOffset[eOffset::x] = _x;
        mRotationOffset[eOffset::y] = _y;
    }
    template<typename T>
    void SetRotation(T _a){
        mRotationAngle = _a;
    }

    virtual inline iterator begin(){
        return mShapes.begin();
    }

    virtual inline iterator end(){
        return mShapes.end();
    }

    virtual inline reverse_iterator rbegin(){
        return mShapes.rbegin();
    }

    virtual inline reverse_iterator rend(){
        return mShapes.rend();
    }

    virtual inline size_t size(){
        return mShapes.size();
    }

    virtual inline void push(GL::Operation *aToPush){
        mShapes.push_back(aToPush);
        aToPush->SetShader(mShader);
    }

    virtual int Draw(void);
    virtual int DrawAll(concurrent_queue<GL::Operation *> *aSendTo);
    virtual int DrawRange(concurrent_queue<GL::Operation *> *aSendTo, iterator aFrom, iterator aTo);

    void SetShader(Shader *aShader){mShader = aShader;}

};

}
}
