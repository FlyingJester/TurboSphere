#ifndef SAPPHIRE_DRAWQUEUE_HEAD
#define SAPPHIRE_DRAWQUEUE_HEAD

#include <inttypes.h>

typedef int (*DrawingCallback)(void *);

#ifdef __cplusplus
extern "C" {
#endif

void IncRenderFrame(void);
void IncEngineFrame(void);

uint32_t GetRenderFrame(void);
uint32_t GetEngineFrame(void);

void ResetRenderFrame(void);
void ResetEngineFrame(void);

void InitDrawQueue(void);
void CloseDrawQueue(void);

void SetThreadDie(int);
int  GetThreadDie(void);

// Synchronizes engine and renderer.
// This is the BIG LOCK. Avoid whenever possible, and use a synchro object.
void SynchroEngineAndRender(void);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

namespace Sapphire {

    class DrawingOp {
    public:
        DrawingOp(){}
        virtual ~DrawingOp(){}
        virtual int call(void) = 0;
        virtual void SetVertexAttribBinding(uint32_t vertex, uint32_t texcoor, uint32_t color){}

    };

    void PushDrawingOp(DrawingOp *);

}

#endif



#endif
