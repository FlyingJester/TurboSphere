#ifdef _WIN32
#include <windows.h>
//#include <WinBase.h>
#else
#include <dirent.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <string>

#include "engine.h"
#include "functionload.h"
#include "loadplugins.h"
#include "variableregister.h"
#include "configmanager/opengame.h"

using namespace std;

#ifdef _WIN32
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

int getNumPlugins(void);

#ifdef _WIN32

int getNumPlugins(){
    HANDLE dir;
    WIN32_FIND_DATA data;
    int num = 0;
    dir = FindFirstFile("plugin/*.*", &data);
	if (dir!=INVALID_HANDLE_VALUE){
        do{
            num++;
        } while(FindNextFile(dir, &data));
        FindClose(dir);
	}
    else{
        printf("[Engine] getNumPlugins Error: No Plugins.\n");
        exit(1);
    }
	if (num==0){
	    printf("[Engine] getNumPlugins Error: No Plugins.\n");
        exit(1);

	}
    return num;
}


const char ** getPluginNames(){
    HANDLE dir;
    WIN32_FIND_DATA data;

	int numplugins = getNumPlugins();
    const char ** pluginnames = (const char **)calloc(numplugins, sizeof(const char *));

    dir = FindFirstFile("plugin/*.*", &data);
	if (dir!=INVALID_HANDLE_VALUE){
		int i = 0;
        do{
            pluginnames[i] = STRDUP(data.cFileName);
            i++;
        } while(FindNextFile(dir, &data));
        FindClose(dir);
    }
    else{
        printf("[Engine] getNumPlugins Error: No Plugins.\n");
        exit(1);
    }
    return pluginnames;
}
#else

int getNumPlugins(){
    DIR *dir;
    struct dirent *ent;
    int num = 0;
    if ((dir=opendir("plugin"))!=NULL){
        while((ent=readdir(dir))!=NULL){
            num++;
        }
        closedir(dir);
    }
    else{
        printf("[Engine] getNumPlugins Error:cannot find plugin directory.\n");
        exit(1);
    }
    return num;
}


const char ** getPluginNames(){
	DIR *dir;
    struct dirent *ent;

	int numplugins = getNumPlugins();
    const char ** pluginnames = (const char **)calloc(numplugins, sizeof(const char *));


	if ((dir=opendir("plugin"))!=NULL){
        int i = 0;
        while((ent=readdir(dir))!=NULL){
            pluginnames[i] = strdup(ent->d_name);
            i++;
        }
        closedir(dir);
    }
    else{
        printf("[Engine] getPluginNames Error: cannot find plugin directory.\n");
        exit(1);
    }
    return pluginnames;
}
#endif


int loadAllPlugins(){
	TS_Config *TS_conf = GetConfig();
    const char ** plugins = getPluginNames();
    int numplugins = getNumPlugins();
    for(int i = 0; i<numplugins; i++){
        //printf("%s\n", plugins[i]);
    }
    for(int i = 0; i<numplugins; i++){
        if(plugins[i][0]=='.'){
            continue;
        }

        bool skip = true;

        for(int e = 0; e < TS_conf->fixedplugins; e++){
            if(strcmp(plugins[i], TS_conf->plugins[e])==0){
            skip = false;
            }
            else{
                //printf("Skipping plugin number %i, %s\n", i, plugins[i]);
            }
        }
        if(skip){
            continue;
        }


        #ifdef _WIN32
		DWORD error;
	    HINSTANCE handle = LoadLibrary(string("plugin\\").append(string(plugins[i])).c_str());
        if(handle!=NULL) {
            printf("Plugin %s is open.\n", plugins[i]);
            grabFuncsFromLibrary(handle);
        }
        else{
            printf("Plugin %s is not open.\n", plugins[i]);
			error = GetLastError();
            printf("Error: %i\n", error);
        }
        #else
        void * handle = dlopen(string("plugin/").append(string(plugins[i])).c_str(), RTLD_GLOBAL|RTLD_NOW);
        if(handle!=NULL) {
            printf("Plugin %s is open.\n", plugins[i]);
            grabFuncsFromLibrary(handle);
        }
        else{
            printf("Plugin %s is not open.\n", plugins[i]);
            printf("Error: %s\n", dlerror());
        }
        #endif

    }

    for(int i = 0; i<numplugins; i++){
        //printf("Plugin %i is %s\n", i, plugins[i]);
        if(plugins[i][0]=='.'){
            continue;
        }
        bool skip = false;
        for(int e = 0; e < TS_conf->fixedplugins; e++){
            if(strcmp(plugins[i], TS_conf->plugins[e])==0){
                //printf("Skipping plugin number %i, %s", i, plugins[i]);
            skip = true;
            }
        }
        if(skip){
            continue;
        }

        #ifdef _WIN32
		DWORD error;
	    HINSTANCE handle = LoadLibrary(string("plugin\\").append(string(plugins[i])).c_str());
        if(handle!=NULL) {
            printf("Plugin %s is open.\n", plugins[i]);
            grabFuncsFromLibrary(handle);
        }
        else{
            printf("Plugin %s is not open.\n", plugins[i]);
			error = GetLastError();
            printf("Error: %i\n", error);
        }
        #else
        void * handle = dlopen(string("plugin/").append(plugins[i]).c_str(), RTLD_GLOBAL|RTLD_NOW);
        if(handle!=NULL) {
            printf("Plugin %s is open.\n", plugins[i]);
            grabFuncsFromLibrary(handle);
        }
        else{
            printf("Plugin %s is not open.\n", plugins[i]);
            printf("Error: %s\n", dlerror());
        }
        #endif

    }

    for(int i = 0; i<numplugins; i++){
        free((void*)plugins[i]);
    }
    free((void*)plugins);
    //
    //
    return 0;
}


#ifdef _WIN32

void grabFuncsFromLibrary(HINSTANCE handle){
    //printf("We'll try.\n");
    DWORD   error = NULL;
    char   *(*dlInit)(void);
    void    (*dlClose)(void);
    int     (*dlGetNumFuncs)(void);
    FunctionArray   (*dlGetFuncs)(void);
    const char**    (*dlGetFuncNames)(void);
    int     (*dlGetNumVars)(void);
    VariableArray   (*dlGetVars)(void);
    const char**    (*dlGetVarNames)(void);
    DLOPENFUNCTONPRESET(char *(*)(void), dlInit, handle, "Init", error, return);
    DLOPENFUNCTONPRESET(void(*)(void), dlClose, handle, "Close", error, return);
    const char * repname = dlInit();
    //DEBUG_VERBOSE
    if(repname==NULL){
        printf("Problem Loading Plugin: %s\n", error);
        return;
    }
	void(*dlClosep)(void) = dlClose;
    atexit(dlClosep);

    //printf("Loaded plugin %s.\n", repname);

    DLOPENFUNCTONPRESET(int(*)(void), dlGetNumFuncs, handle, "GetNumFunctions", error, return);
    DLOPENFUNCTONPRESET(void **(*)(void), dlGetFuncs, handle, "GetFunctions", error, return);
    DLOPENFUNCTONPRESET(const char **(*)(void), dlGetFuncNames, handle, "GetFunctionNames", error, return);

    DLOPENFUNCTONPRESET(int(*)(void), dlGetNumVars, handle, "GetNumVariables", error, return);
    DLOPENFUNCTONPRESET(VariableArray (*)(void), dlGetVars, handle, "GetVariables", error, return);
    DLOPENFUNCTONPRESET(const char **(*)(void), dlGetVarNames, handle, "GetVariableNames", error, return);

    int numFuncs = dlGetNumFuncs();
    FunctionArray Funcs = (FunctionArray) calloc (numFuncs, sizeof(void *));

    Funcs = dlGetFuncs();
    const char ** FuncNames = (const char **)calloc(numFuncs, sizeof(const char*));

    FuncNames = dlGetFuncNames();


    for(int i = 0; i<numFuncs; i++){
        //DEBUG_VERBOSE

        addFunctionToList(FuncNames[i], (v8::Handle<v8::Value> (*)(const v8::Arguments& args))Funcs[i]);
    }

    int numVars = dlGetNumVars();
    if(numVars>0){
		VariableArray Vars = (VariableArray)calloc(numVars, sizeof(v8::Handle<v8::Value>));

		Vars = dlGetVars();
		const char ** VarNames = (const char **)calloc(numVars, sizeof(const char*));

		VarNames = dlGetVarNames();


		for(int i = 0; i<numVars; i++){
			addVariableToList(VarNames[i], (v8::Handle<v8::Value>)Vars[i]);
		}

    }

}
#else

//void grabFuncsFromLibrary(void *handle, const char *name = "Unkown"){
void grabFuncsFromLibrary(void *handle){
    //printf("We'll try.\n");
    char *error;
    char   *(*dlInit)(void);
    void    (*dlClose)(void);
    int     (*dlGetNumFuncs)(void);
    FunctionArray   (*dlGetFuncs)(void);
    const char**    (*dlGetFuncNames)(void);
    int     (*dlGetNumVars)(void);
    VariableArray   (*dlGetVars)(void);
    const char**    (*dlGetVarNames)(void);
    DLOPENFUNCTONPRESET(char *(*)(void), dlInit, handle, "Init", error, return);
    DLOPENFUNCTONPRESET(void(*)(void), dlClose, handle, "Close", error, return);
    const char * repname = dlInit();
    //DEBUG_VERBOSE
    if(repname==NULL){
        printf("Problem Loading Plugin: %s\n", error);
        return;
    }

    atexit(dlClose);

    //printf("Loaded plugin %s.\n", repname);

    DLOPENFUNCTONPRESET(int(*)(void), dlGetNumFuncs, handle, "GetNumFunctions", error, return);
    DLOPENFUNCTONPRESET(void **(*)(void), dlGetFuncs, handle, "GetFunctions", error, return);
    DLOPENFUNCTONPRESET(const char **(*)(void), dlGetFuncNames, handle, "GetFunctionNames", error, return);

    DLOPENFUNCTONPRESET(int(*)(void), dlGetNumVars, handle, "GetNumVariables", error, return);
    DLOPENFUNCTONPRESET(VariableArray (*)(void), dlGetVars, handle, "GetVariables", error, return);
    DLOPENFUNCTONPRESET(const char **(*)(void), dlGetVarNames, handle, "GetVariableNames", error, return);

    int numFuncs = dlGetNumFuncs();
    FunctionArray Funcs = (FunctionArray) calloc (numFuncs, sizeof(void *));

    Funcs = dlGetFuncs();
    const char ** FuncNames = (const char **)calloc(numFuncs, sizeof(const char*));

    FuncNames = dlGetFuncNames();


    for(int i = 0; i<numFuncs; i++){
            if(FuncNames[i]==NULL){
                printf("Problem with plugin %s: Function %i has no name.\n", repname, i);
            }
            else{
                addFunctionToList(FuncNames[i], (v8::Handle<v8::Value> (*)(const v8::Arguments& args))Funcs[i]);
            }
    }

    int numVars = dlGetNumVars();
    if(numVars>0){
        VariableArray Vars = (VariableArray)calloc(numVars, sizeof(v8::Handle<v8::Value>));

        Vars = dlGetVars();
        const char ** VarNames = (const char **)calloc(numVars, sizeof(const char*));

        VarNames = dlGetVarNames();


        for(int i = 0; i<numVars; i++){
            if(VarNames[i]==NULL){
                printf("Problem with plugin %s: Variable %i has no name.\n", repname, i);
            }
            else{
                addVariableToList(VarNames[i], (v8::Handle<v8::Value>)Vars[i]);
            }
        }

        free(VarNames);
        free(Vars);
    }

    free(FuncNames);
    free(Funcs);
    printf("Plugin %s successfully loaded.\n", repname);
}


#endif
