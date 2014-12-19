#pragma once
#include <list>
#include <queue>
#include <memory>
#include <color.h>
#include "Shape.hpp"
#include "Shader.hpp"

namespace Sapphire{
namespace Galileo {

class Group : public GL::Operation {
public:
    typedef std::list<GL::Operation *>::iterator iterator;
    typedef std::list<GL::Operation *>::const_iterator const_iterator;
    typedef std::list<GL::Operation *>::reverse_iterator reverse_iterator;
    typedef std::list<GL::Operation *>::const_reverse_iterator const_reverse_iterator;

protected:
    enum eOffset {x, y, d};

    std::list<GL::Operation *> mShapes;

    std::shared_ptr<Shader> mShader;
    float mOffset[2];
    float mRotOffset[2];
    float mAngle;

    int ShaderOffsetUniformName, ShaderRotOffsetUniformName, ShaderAngleUniformName;

    int mLastDrawnShader;

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

    double GetX_d(){
        return mOffset[eOffset::x];
    }
    double GetY_d(){
        return mOffset[eOffset::y];
    }

    template<typename T>
    void SetRotX(T _x){
        mRotOffset[eOffset::x] = _x;
    }
    template<typename T>
    void SetRotY(T _y){
        mRotOffset[eOffset::y] = _y;
    }

    template<typename T = float>
    T GetRotX() const{
        return mRotOffset[eOffset::x];
    }
    template<typename T = float>
    T GetRotY() const{
        return mRotOffset[eOffset::y];
    }

    template<typename T>
    void SetRotationOffset(T _x, T _y){
        mRotOffset[eOffset::x] = _x;
        mRotOffset[eOffset::y] = _y;
    }

    template<typename T>
    void SetRotation(T _a){
        mAngle = _a;
    }

    template<typename T>
    T GetRotation(void) const{
        return mAngle;
    }

    template<typename T, typename U>
    inline void SetRotationAttr(U _x, U _y, T _a){
        SetRotation<T>(_a);
        SetRotationOffset<U>(_x, _y);
    }

    virtual inline iterator begin(){
        return mShapes.begin();
    }

    virtual inline iterator end(){
        return mShapes.end();
    }

    virtual inline const_iterator begin() const{
        return mShapes.begin();
    }

    virtual inline const_iterator end() const{
        return mShapes.end();
    }

    virtual inline reverse_iterator rbegin(){
        return mShapes.rbegin();
    }

    virtual inline reverse_iterator rend(){
        return mShapes.rend();
    }

    virtual inline const_reverse_iterator rbegin() const{
        return mShapes.rbegin();
    }

    virtual inline const_reverse_iterator rend() const{
        return mShapes.rend();
    }

    virtual inline size_t size() const{
        return mShapes.size();
    }

    virtual inline void push(GL::Operation *aToPush){
        mShapes.push_back(aToPush);
        aToPush->SetShader(mShader.get());
    }

    inline bool empty(void) const {
        return mShapes.empty();
    }

    template<typename i=iterator>
    i erase(i at){
        return mShapes.erase(at);
    }

    template<typename i=iterator>
    i erase(i at) const {
        return mShapes.erase(at);
    }

    virtual int Draw(void);
    virtual int Draw(std::queue<GL::Operation *> *aSendTo);
    virtual int DrawAll(std::queue<GL::Operation *> *aSendTo);
    virtual int DrawRange(std::queue<GL::Operation *> *aSendTo, iterator aFrom, iterator aTo);

    void SetShader(Shader *aShader){mShader = std::shared_ptr<Shader>(aShader);}

};

}
}
