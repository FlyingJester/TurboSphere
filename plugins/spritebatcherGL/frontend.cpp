#include "frontend.h"
#include "batcher.h"

#define CHECK_FOR_PROCESS(NAME){\
        if(SDL_GL_GetProcAddress(NAME)==NULL){\
        printf("[" PLUGINNAME "] Init Error: " NAME " is not present in OpenGL library.\n");\
        exit(1);\
    }\
}

#define GET_GL_FUNCTION(NAME, TYPING)\
CHECK_FOR_PROCESS( #NAME );\
NAME = TYPING SDL_GL_GetProcAddress( #NAME )

void (APIENTRY * glFramebufferParameteri)(GLenum target, GLenum pname, GLint param) = NULL;

initFunction Init(int ID){

    GET_GL_FUNCTION(glFramebufferParameteri, (void (APIENTRY *)(GLenum target, GLenum pname, GLint param)));

    return PLUGINNAME;
}

void Close(void){

}

int             GetNumFunctions(void){
    return 0;
}

functionArray   GetFunctions(void){
    return NULL;
}

nameArray       GetFunctionNames(void){
    return NULL;
}

int             GetNumVariables(void){
    return 0;
}

v8FunctionArray GetVariables(void){
    return NULL;
}

nameArray       GetVariableNames(void){
    return NULL;
}
