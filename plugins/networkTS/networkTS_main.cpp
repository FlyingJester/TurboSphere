#include "../common/plugin.h"
#include "frontend.h"

#define NETWORKTS_INTERNAL
#define PLUGINNAME "networkTS"
#include "networkTS.h"

#define NUMFUNCS 1
#define NUMVARS 0

#ifdef _WIN32

#define STRDUP _strdup
#else

#include <cstring>
#define STRDUP strdup
#endif

void * GetHostNamePointer            = V8FUNCPOINTER(GetHostName);

int numerate(bool reset) {
    static int i = 0;
    if(reset) {
        i = 0;
        return 0;
    }
    i++;
    return i-1;
}

initFunction Init(void) {
	return (char *)PLUGINNAME;
}

void Close() {
}

int GetNumFunctions() {
    return NUMFUNCS;
}

functionArray GetFunctions() {
    numerate(true);
    functionArray funcs = (functionArray)calloc(GetNumFunctions(), sizeof(void*));
    funcs[numerate(false)] = GetHostNamePointer;
    return funcs;
}

nameArray GetFunctionNames() {
    numerate(true);
    nameArray names = (nameArray)calloc(GetNumFunctions(), sizeof(functionName));
    names[numerate(false)] = (functionName)"GetLocalName";
    return names;
}

int GetNumVariables() {
    return 0;
}

void ** GetVariables(void) {
    return NULL;
}

nameArray GetVariableNames(void) {
    return NULL;
}
