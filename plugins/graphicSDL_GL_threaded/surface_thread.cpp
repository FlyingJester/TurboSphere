#include "main.h"
#include "surface.h"
#include "color.h"
#include "primitives.h"
#include <functional>
#include <assert.h>

//Likely Very buggy, but with the possibilty of ultimate power.

//Contains the functions for the surface thread.


TS_GenericSurfaceFunction::TS_GenericSurfaceFunction(void (*_func)(void *), void * _arg, SDL_Surface *src, SDL_Surface *dst){
    func = _func;
    arg = _arg;
    srcsurface = src;
    dstsurface = dst;
}

TS_GenericSurfaceFunction::~TS_GenericSurfaceFunction(){
}

//Thread control
const size_t SurfaceQueueSize    = 0xFFFF;
TS_GenericSurfaceFunction** SurfaceQueue = NULL;

SDL_atomic_t NeedSurface;
SDL_Surface* SurfaceNeeded = NULL;

//Generic Function Calling
using namespace std::placeholders;

inline intptr_t get_addr_diff(size_t *sizes, int index){

    assert(index>=0);

    intptr_t r = 0;

    for(int i = 0; i<index; i++)
        r+=sizes[i];

    return r;
}

#define CGF_ARG(n) *(args+get_addr_diff(args_sizes, n))

int CallGenericFunction(void (*func)(void *), void *arg){

    if(func==NULL){
        exit(11);
    }
    func(arg);
    return 0;
}

void TS_NeedSurface(SDL_Surface *surface){
    if(QueuePlacingPosition==QueueOperatingPosition+1){
        return;
    }
    for(unsigned int i = QueueOperatingPosition; i!=QueuePlacingPosition; (++i)%=SurfaceQueueSize){
        //This would be ver strange indeed.
        //The only case it would make sense is blitting a surface onto itself--but in that case,
        //the source and destination are guaranteed to be at the same stage!
        //assert(SurfaceQueue[i]->dstsurface!=SurfaceQueue[i]->srcsurface);

        if(SurfaceQueue[i]==NULL)
            continue;

        if(SurfaceQueue[i]->srcsurface==surface){
            if(SurfaceQueue[i]->dstsurface!=NULL){
                TS_NeedSurface(SurfaceQueue[i]->dstsurface);
            }
            CallGenericFunction(SurfaceQueue[i]->func, SurfaceQueue[i]->arg);
            delete SurfaceQueue[i];
            SurfaceQueue[i] = NULL;
        }
        else if(false&&(SurfaceQueue[i]->dstsurface==surface)){
            CallGenericFunction(SurfaceQueue[i]->func, SurfaceQueue[i]->arg);
            delete SurfaceQueue[i];
            SurfaceQueue[i] = NULL;
        }
    }
}

//Thread Function

int SurfaceThreadFunction(void *data){
    //static bool first = true;
	int die = 1;
    while(true){
		die = SDL_AtomicGet(&SurfaceThreadNearDeath);
		//printf("%i\t", die);
		if(die==2){
            printf("[Surface Thread] Info: Dying as ordered.\n");
			return 0;
		}
        if(SDL_LockMutex(SurfaceQueueMutex)<0)
            exit(0xD);

        //Process the needed surface.
        if(SDL_LockMutex(SurfaceQueueNeedMutex)<0)
            exit(0xB);

            if(SDL_AtomicGet(&NeedSurface)){
                if(SurfaceNeeded==NULL)
                    exit(0xF);

                TS_NeedSurface(SurfaceNeeded);
                SurfaceNeeded = NULL;
                SDL_AtomicSet(&NeedSurface, 0);
            }

        if(SDL_UnlockMutex(SurfaceQueueNeedMutex)<0)
            exit(0xC);

        //If we don't get this mutex, we restart. This forces us to update all surfaces that need updating.
        if(SDL_AtomicGet(&SurfaceQueueIndependantFlag)){
            goto endsub;
        }
        if(((QueueOperatingPosition+1)%SurfaceQueueSize)==QueuePlacingPosition){
            goto endsub; //No comments on using goto!
        }
        //Process the next surface operation.
        while(SurfaceQueue[QueueOperatingPosition]==NULL){
            if(((QueueOperatingPosition+1)%SurfaceQueueSize)==QueuePlacingPosition){
                break;
            }
            QueueOperatingPosition++;
            QueueOperatingPosition%=SurfaceQueueSize;
        }

        //printf("Placing is at %i, Operating is at %i.\n", QueuePlacingPosition, QueueOperatingPosition);

        if((QueueOperatingPosition!=QueuePlacingPosition)&&(SurfaceQueue[QueueOperatingPosition]!=NULL)){
        //We now are at a valid operating position. Operate, my pretties!
            //printf("We are doing something!!\n");
            SurfaceQueue[QueueOperatingPosition]->func(SurfaceQueue[QueueOperatingPosition]->arg);
            //printf("The something has been done!!\n");
            delete SurfaceQueue[QueueOperatingPosition];
            SurfaceQueue[QueueOperatingPosition] = NULL;
        }
        else{
            printf("[Surface Thread] Info: Done!\n");
            SDL_Delay(0);
        }
        SDL_Delay(0);
    endsub:
        if(SDL_UnlockMutex(SurfaceQueueMutex)<0)
            exit(0xE);
	}
}
