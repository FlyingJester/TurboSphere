#ifndef SDL_GL_SURFACE_HEAD
#define SDL_GL_SURFACE_HEAD
#include "main.h"
#include "image.h"

extern SDL_atomic_t SurfaceThreadNearDeath;

#define READY_SURFACE(_surface){\
    if(SDL_LockMutex(SurfaceQueueNeedMutex)<0)\
        exit(0x10B);\
\
\
    while(SDL_AtomicGet(&NeedSurface)){\
        if(SDL_UnlockMutex(SurfaceQueueNeedMutex)<0)\
            exit(0x10C);\
\
\
\
\
\
        if(SDL_LockMutex(SurfaceQueueNeedMutex)<0)\
            exit(0x10B);\
    }\
\
    SDL_AtomicSet(&NeedSurface, 1);\
    SurfaceNeeded = _surface;\
\
    do{\
        if(SDL_UnlockMutex(SurfaceQueueNeedMutex)<0)\
            exit(0x10D);\
\
\
\
\
\
        if(SDL_LockMutex(SurfaceQueueNeedMutex)<0)\
            exit(0x10E);\
    }while(SDL_AtomicGet(&NeedSurface));\
\
\
    if(SDL_UnlockMutex(SurfaceQueueNeedMutex)<0)\
        exit(0x10C);\
}

int SurfaceThreadFunction(void *data);

struct TS_GenericSurfaceFunction{
    TS_GenericSurfaceFunction(void (*func)(void *), void * arg, SDL_Surface *src, SDL_Surface *dst);
    ~TS_GenericSurfaceFunction();
    void (*func)(void *);
    void *arg;
    SDL_Surface *srcsurface; //We must finish working on older requests for this surface before we can perform this operation.
    SDL_Surface *dstsurface; //This operation must be finished before this surface can be used by newer requests.
};

//This is the devil.
int CallGenericFunction(void (*)(void *), void *arg);

//The SurfaceQueue is a FILO, upward queue.

//The number of remaining operations is:
/*
    //if QueueOperatingPosition is greater than QueuePlacingPosition:
    //  SurfaceQueueSize - (QueueOperatingPosition-QueuePlacingPosition)
    //else if QueueOperatingPosition is less than QueuePlacingPosition:
    //  QueuePlacingPosition-QueueOperatingPosition
    //else if QueueOperatingPosition=QueuePlacingPosition:
    //  SurfaceQueueSize
*/
//In the last case, there is no guarantee that any surface operation will not be blocking until at least one surface operation is actually performed.


//Size of the surface operation queue.
extern const size_t SurfaceQueueSize;

//These two values range from 0x0 to the SurfaceQueueSize (default 0xFF), and wrap around to 0 at overflow.
//The position at which we draw the next operation from.
extern size_t QueueOperatingPosition;
//The position at which we can place a new operation.
extern size_t QueuePlacingPosition;

//This mutex must be held in order to use the SurfaceQueue or change the position variables.
extern SDL_mutex *SurfaceQueueMutex;

//This flag must be held in order to change the needed surface variables.
extern SDL_mutex *SurfaceQueueNeedMutex;

//This mutex stops the surface drawing thread from continuing through the Queue independantly.
//The surface thread will still update surfaces that are referenced by the SurfaceNeeded variable.

//This flag is used in conjunction with the SurfaceNeeded mutex so that you can request a surface
//be fully updated, and then access it.
//A value of 1 is locked (no independant operation), 0 is unlocked.
extern SDL_atomic_t SurfaceQueueIndependantFlag;

//The legendary SurfaceQueue, fabled in song and story.
extern TS_GenericSurfaceFunction** SurfaceQueue;

extern SDL_atomic_t NeedSurface;
extern SDL_Surface* SurfaceNeeded;

EXTERN_OBJECT_TEMPLATES(Surface);

void SurfaceInit(void);
void SurfaceClose(void);

//This struct must be passed to the primitives object file.
//Soft Line Argument structure
typedef struct {SDL_Surface *surface; TS_Color *color; int x1; int y1; int x2; int y2; } SoftLineArgs;


v8Function NewSurface(V8ARGS);

v8Function SurfaceBlit(V8ARGS);
v8Function SurfaceBlitSurface(V8ARGS);

v8Function SurfaceToImage(V8ARGS);
v8Function SurfaceClone(V8ARGS);
v8Function SurfaceCloneSection(V8ARGS);
v8Function SurfaceGrab(V8ARGS);

v8Function SurfaceSave(V8ARGS);

v8Function SurfaceSetAlpha(V8ARGS);
v8Function SurfaceGetClippingRectangle(V8ARGS);
v8Function SurfaceSetClippingRectangle(V8ARGS);

v8Function SurfaceRectangle(V8ARGS);

void TS_SurfaceFinalizer(V8FINALIZERARGS);

v8Function SurfaceSetPixel(V8ARGS);
v8Function SurfaceGetPixel(V8ARGS);

v8Function SurfaceGetWidth(V8GETTERARGS);
void SurfaceSetWidth(V8SETTERARGS);
v8Function SurfaceGetHeight(V8GETTERARGS);
void SurfaceSetHeight(V8SETTERARGS);

#endif
