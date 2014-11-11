#import <Foundation/NSThread.h>
#import <assert.h>
#import <unistd.h>

@interface TS_Thread : NSThread {
    void *(*FuncToCall)(void *);
    void *Arg;
    void *ToPutReturn;
}
 - (void)main;
 - (void)SetFunc:(void *(*)(void *)) aFuncToCall;
 - (void)SetArg:(void *)aArg;
 - (void*)GetReturn;
@end

@implementation TS_Thread

 - (void)main{

    assert(FuncToCall!=NULL);

    ToPutReturn = FuncToCall(Arg);
 }

 - (void)SetFunc:(void *(*)(void *)) aFuncToCall{
    FuncToCall = aFuncToCall;
 }

 - (void)SetArg:(void *)aArg{
    Arg = aArg;
 }
 - (void*)GetReturn{
    return ToPutReturn;
 }

@end


TS_Thread *TS_CreateThread(void *(*aFunc)(void *), void *aArg){
    TS_Thread *rThread = [[TS_Thread alloc] init];

    [rThread SetFunc:aFunc];
    [rThread SetArg:aArg];

    [rThread start];

    return rThread;

}

void *TS_WaitThread(TS_Thread *aThread){
    while([aThread isExecuting]){
        usleep(10000);
    }

    return [aThread GetReturn];

}


void TS_SetThreadPriority(TS_Thread *aThread, int aPriority){
    assert(aThread);
    assert(aPriority>=0 && aPriority<10);

    double alPriority = ((double)(aPriority-1))/8.0;

    [aThread setThreadPriority:alPriority];

}

int TS_GetThreadPriority(TS_Thread *aThread){
    assert(aThread);
    double p = [aThread threadPriority];
    return (p*8.0)+1.01;
}

void TS_SetCurrentThreadPriority(int aPriority){
    assert(aPriority>=0 && aPriority<10);

    double alPriority = ((double)(aPriority-1))/8.0;

    [NSThread setThreadPriority:alPriority];

}

int TS_GetCurrentThreadPriority(){
    double p = [NSThread threadPriority];
    return (p*8.0)+1.01;
}
