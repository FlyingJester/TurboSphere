#include "backend.h"
#include "shader/shader.h"
#include "config.h"
#include "screen/flipscreen.h"


float whitef[] = {
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f
};

float tex[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
};

Configuration_t configl;


unsigned int ShaderID = 0;
unsigned int ShaderProgramID = 0;

void BackendInit(bool threaded, bool composite){
    FlipScreenInit(threaded, composite);
}

void BackendClose(void){
    FlipScreenClose();
}
