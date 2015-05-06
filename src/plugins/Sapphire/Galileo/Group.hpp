#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <color.h>
#include "Shape.hpp"
#include "State.hpp"
#include "Shader.hpp"
#include "../libyyymonitor/monitor.hpp"

namespace Sapphire{
namespace Galileo {

class Group : public GL::Operation {
public:
    typedef std::vector<GL::Operation *> container;
    typedef container::iterator iterator;
    typedef container::const_iterator const_iterator;
    typedef container::reverse_iterator reverse_iterator;
    typedef container::const_reverse_iterator const_reverse_iterator;

protected:
    enum eOffset {x, y, d};

    container mShapes;

    std::shared_ptr<ShaderProgram> mShader;
    ClippingRectangle clip_rectangle;
    float mOffset[2];
    float mRotOffset[2];
    float mAngle;

    int ShaderOffsetUniformName, ShaderRotOffsetUniformName, ShaderAngleUniformName;

    int mLastDrawnShader;
    
    mutable Turbo::Monitor monitor;
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
    void SetClipX(T x_){
        clip_rectangle.x = (x_)?x_:0;
    }
    template<typename T>
    void SetClipY(T y_){
        clip_rectangle.y = (y_)?y_:0;
    }
    template<typename T>
    void SetClipW(T w_){
        clip_rectangle.w = (w_<0xFFFF)?w_:0xFFFF;
    }
    template<typename T>
    void SetClipH(T h_){
        clip_rectangle.h = (h_<0xFFFF)?h_:0xFFFF;
    }
    
    template<typename T>
    void SetClippingRectangle(T x_, T y_, T w_, T h_){
        SetClipX(x_);
        SetClipY(y_);
        SetClipW(w_);
        SetClipH(h_);
    }
    
    unsigned GetClipX() const { return clip_rectangle.x; }
    unsigned GetClipY() const { return clip_rectangle.y; }
    unsigned GetClipW() const { return clip_rectangle.w; }
    unsigned GetClipH() const { return clip_rectangle.h; }
    
    inline void UnsetClippingRectangle(){
        clip_rectangle.x = clip_rectangle.y = 0;
        clip_rectangle.w = 0xFFFF;
        clip_rectangle.h = 0xFFFF;
    }
    
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

    virtual inline void lock() const {
        monitor.lock();
    }
    
    virtual inline void unlock() const {
        monitor.unlock();
    }
    
    virtual inline void wait() const {
        monitor.Wait();
    }
    
    virtual inline void notify() const {
        monitor.Notify();
    }
    
    virtual inline void notifyAll() const {
        monitor.NotifyAll();
    }

    virtual inline iterator begin(){
        return mShapes.begin();
    }

    virtual inline struct Turbo::Monitor::Locker createLocker(){
        return monitor.CreateLocker();
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
        aToPush->SetShader(mShader);
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

    template<typename i=iterator>
    i insert(i at, GL::Operation *aOp){
        aOp->SetShader(mShader);
        return mShapes.insert(at, aOp);
    }
    
    virtual int Draw(void);
    virtual int Draw(std::queue<GL::Operation *> *aSendTo);
    virtual int DrawAll(std::queue<GL::Operation *> *aSendTo);
    virtual int DrawRange(std::queue<GL::Operation *> *aSendTo, iterator aFrom, iterator aTo);

    void SetShader(std::shared_ptr<ShaderProgram> aShader){mShader = aShader;}

};

}
}
