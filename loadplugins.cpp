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

#include "common/dlopenwrap.h"

#include "engine.h"
#include "functionload.h"
#include "loadplugins.h"
#include "variableregister.h"
#include "configmanager/opengame.h"

using namespace std;

typedef void(*PluginCloseFunction)(void);

int *GetNumPluginsToClose(){
	static int Plugins = 0;
	return &Plugins;
}

int IncNumPluginsToClose(){
	int *Plugins = GetNumPluginsToClose();
	(*Plugins)++;
	//printf("Plugins:\t%i\n", *Plugins);
	return *Plugins;
}

int DecNumPluginsToClose(){
	int *Plugins = GetNumPluginsToClose();
	(*Plugins)--;
	return *Plugins;
}

void ClrNumPluginsToClose(){
	int *Plugins = GetNumPluginsToClose();
	(*Plugins)= 0;
}

PluginCloseFunction *RegisterPluginCloseFunction(void(*close)(void)){

	static PluginCloseFunction* CloseFuncs = NULL;

	if(close!=NULL){

		int numplugs = IncNumPluginsToClose();
		CloseFuncs = (PluginCloseFunction*)realloc(CloseFuncs, numplugs*sizeof(PluginCloseFunction));
		CloseFuncs[numplugs-1] = close;
	//printf("Plugins:\t%i\n", numplugs);

	}
	return CloseFuncs;

}

int CloseAllPlugins(){
	PluginCloseFunction *funcs = RegisterPluginCloseFunction(NULL);
	int numplugs = *(GetNumPluginsToClose());
	int i = 0;
	//printf("Plugins:\t%i\n", numplugs);
	while(i<numplugs){
		//printf("Plugin Closed:\t%i\n", i);
		((PluginCloseFunction)(funcs[i]))();
		i++;
	}
	printf("All Plugins Closed.\n");
	return i;
}

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
			const char *lowername = data.cFileName+(strlen(data.cFileName)-3);
			if(strcmp(lowername, "dll")!=0)
					continue;
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
			const char *lowername = data.cFileName+(strlen(data.cFileName)-3);
			if(strcmp(lowername, "dll")!=0)
				continue;
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
	//TS_Config *TS_conf = GetConfig();
    const char ** plugins = getPluginNames();
    int numplugins = getNumPlugins();
    for(int i = 0; i<numplugins; i++){
        //printf("%s\n", plugins[i]);
    }
    for(int i = 0; i<numplugins; i++){
        if(plugins[i][0]=='.'){
            continue;
        }

        //bool skip = true;
        /*
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
        */

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
            //printf("Plugin %s is open.\n", plugins[i]);
            grabFuncsFromLibrary(handle);
        }
        else{
            printf("[Engine] Error: Plugin %s is not open.\n", plugins[i]);
            printf("[Engine] Error: %s\n", dlerror());
        }
        #endif

    }

    for(int i = 0; i<numplugins; i++){
        //printf("Plugin %i is %s\n", i, plugins[i]);
        if(plugins[i][0]=='.'){
            continue;
        }
        /*
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
        */

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
            //printf("Plugin %s is open.\n", plugins[i]);
            grabFuncsFromLibrary(handle);
        }
        else{
            printf("[Engine] Error: Plugin %s is not open.\n", plugins[i]);
            printf("[Engine] Error: %s\n", dlerror());
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
    RegisterPluginCloseFunction(dlClosep);

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

        addFunctionToList(FuncNames[i], (v8::Handle<v8::Value> (*)(const v8::FunctionCallbackInfo<v8::Value> &args))Funcs[i]);
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
    if(!handle){
        fprintf(stderr, "[Engine] Error: Bad handle.\n");
        exit(EXIT_FAILURE);
    }

    //printf("We'll try.\n");
    static int PluginNum = 1;
    char *error;
    char   *(*dlInit)(int);
    void    (*dlClose)(void);
    int     (*dlGetNumFuncs)(void);
    FunctionArray   (*dlGetFuncs)(void);
    const char**    (*dlGetFuncNames)(void);
    int     (*dlGetNumVars)(void);
    VariableArray   (*dlGetVars)(void);
    const char**    (*dlGetVarNames)(void);
    dlInit = (char *(*)(int))DLOPENFUNCTION(handle, "Init");
    dlClose = (void(*)(void))DLOPENFUNCTION(handle, "Close");
    const char * repname = dlInit(PluginNum);
    //DEBUG_VERBOSE
    if(repname==NULL){
        printf("Problem Loading Plugin: %s\n", error);
        return;
    }

    RegisterPluginCloseFunction(dlClose);

    //printf("Loaded plugin %s.\n", repname);

    dlGetNumFuncs   = (int(*)(void))DLOPENFUNCTION(handle, "GetNumFunctions");

    if(!dlGetNumFuncs){
        fprintf(stderr, "[Engine] Error: could not load GetNumFunctions from plugin.\n");
        exit(EXIT_FAILURE);
    }

    dlGetFuncs      = (void **(*)(void))DLOPENFUNCTION(handle, "GetFunctions");

    if(!dlGetFuncs){
        fprintf(stderr, "[Engine] Error: could not load GetFunctions from plugin.\n");
        exit(EXIT_FAILURE);
    }

    dlGetFuncNames  = (const char **(*)(void))DLOPENFUNCTION(handle, "GetFunctionNames");

    if(!dlGetFuncNames){
        fprintf(stderr, "[Engine] Error: could not load GetFunctionNames from plugin.\n");
        exit(EXIT_FAILURE);
    }

    dlGetNumVars    = (int(*)(void))DLOPENFUNCTION(handle, "GetNumVariables");

    if(!dlGetNumVars){
        fprintf(stderr, "[Engine] Error: could not load GetNumVariables from plugin.\n");
        exit(EXIT_FAILURE);
    }

    dlGetVars       = (VariableArray (*)(void))DLOPENFUNCTION(handle, "GetVariables");

    if(!dlGetVars){
        fprintf(stderr, "[Engine] Error: could not load GetVariables from plugin.\n");
        exit(EXIT_FAILURE);
    }

    dlGetVarNames   = (const char **(*)(void))DLOPENFUNCTION(handle, "GetVariableNames");

    if(!dlGetVarNames){
        fprintf(stderr, "[Engine] Error: could not load GetVarNames from plugin.\n");
        exit(EXIT_FAILURE);
    }


    int numFuncs = dlGetNumFuncs();
    FunctionArray Funcs;// = (FunctionArray) calloc (numFuncs, sizeof(void *));

    Funcs = dlGetFuncs();
    const char ** FuncNames;// = (const char **)calloc(numFuncs, sizeof(const char*));

    FuncNames = dlGetFuncNames();


    for(int i = 0; i<numFuncs; i++){
            if(FuncNames[i]==NULL){
                printf("Problem with plugin %s: Function %i has no name.\n", repname, i);
            }
            else{
                addFunctionToList(FuncNames[i], (void (*)(const v8::FunctionCallbackInfo<v8::Value> &args))Funcs[i], v8::Isolate::GetCurrent());
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
                addVariableToList(VarNames[i], (v8::Handle<v8::Value>)Vars[i], v8::Isolate::GetCurrent());
            }
        }

//        free(VarNames);
//        free(Vars);
    }

//    free(FuncNames);
//    free(Funcs);
    printf("[Engine] Info: Plugin %s successfully loaded.\n", repname);
}


#endif
