#include "flipscreen.h"
#include "../operation_queue.h"

static void FS(void *unused){
    SwapBuffers();
}

void EnqueueFlipScreen(void *atomic_flag){
    Sapphire_EnqueueFunction(FS, NULL);
}

