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


void * spritebatcherDebugPointer            = V8FUNCPOINTER(spritebatcherDebug);
void * NewSpriteBatcherPointer              = V8FUNCPOINTER(NewSpriteBatcher);
void * spritebatcherAddImagePointer              = V8FUNCPOINTER(spritebatcherAddImage);

void (APIENTRY * glFramebufferParameteri)(GLenum target, GLenum pname, GLint param) = NULL;

initFunction Init(int ID){

    GET_GL_FUNCTION(glFramebufferParameteri, (void (APIENTRY *)(GLenum target, GLenum pname, GLint param)));

    InitBatcher();

    return PLUGINNAME;
}

void Close(void){

}

int             GetNumFunctions(void){
    return 1;
}

functionArray   GetFunctions(void){
    functionArray funcs = (functionArray)calloc(2, sizeof(void*));
    funcs[0] = NewSpriteBatcherPointer;
    return funcs;
}

nameArray       GetFunctionNames(void){
    nameArray names = (nameArray)calloc(2, sizeof(functionName));
    names[0] = (functionName)"SpriteBatch";
    return names;
}

int             GetNumVariables(void){
    return 4;
}

v8FunctionArray GetVariables(void){

    v8FunctionArray vars = (v8FunctionArray)calloc(4, sizeof(v8Function));

    vars[0]=v8::Number::New(0);
    vars[1]=v8::Number::New(1);
    vars[2]=v8::Number::New(2);
    vars[3]=v8::Number::New(3);

    return vars;
}

nameArray       GetVariableNames(void){

    nameArray names = (nameArray)calloc(4, sizeof(functionName));
    names[0]=(functionName)"LINE";
    names[1]=(functionName)"TRIANGLE";
    names[2]=(functionName)"RECTANGLE";
    names[3]=(functionName)"CIRCLE";

    return names;
}
