
#include "../common/plugin.h"
#include "scriptfs.h"
#include "../../t5.h"
#include <stdio.h>

int numerate(bool reset){
    static int i = 0;
    if(reset) {
        i = 0;
        return 0;
    }
    i++;
    return i-1;
}

#define NUMFUNCS 3
#define NUMVARS 0

v8Function TS_OpenFile(V8ARGS);
v8Function TS_RemoveFile(V8ARGS);
v8Function TS_GetFileList(V8ARGS);

void * TS_OpenFilePointer          = V8FUNCPOINTER(TS_OpenFile);
void * TS_RemoveFilePointer        = V8FUNCPOINTER(TS_RemoveFile);
void * TS_GetFileListPointer       = V8FUNCPOINTER(TS_GetFileList);

DECLARE_OBJECT_TEMPLATES(ScriptFile);

void Close(){

}
initFunction Init(){
	T5_init(1, GetDirs()->save);
    INIT_OBJECT_TEMPLATES(ScriptFile);
//    ADD_TO_PROTO(ScriptFile, "read", TS_WSdrawWindow);
//    ADD_TO_PROTO(ScriptFile, "write", TS_WSdrawWindow);
//    ADD_TO_PROTO(ScriptFile, "flush", TS_WSdrawWindow);
//    ADD_TO_PROTO(ScriptFile, "close", TS_WSdrawWindow);
//    ADD_TO_PROTO(ScriptFile, "getNumKeys", TS_WSdrawWindow);
//    ADD_TO_PROTO(ScriptFile, "getKey", TS_WSdrawWindow);
	return (initFunction)"scriptfsT5";
}

int GetNumFunctions(){
	return NUMFUNCS;
}
functionArray GetFunctions(){
	numerate(true);
	functionArray funcs = (functionArray)calloc(NUMFUNCS, sizeof(void *));
	funcs[numerate(false)] = TS_OpenFilePointer;
	funcs[numerate(false)] = TS_RemoveFilePointer;
	funcs[numerate(false)] = TS_GetFileListPointer;
	return funcs;
}

const char ** GetFunctionNames(){
	numerate(true);
	nameArray funcs = (nameArray)calloc(NUMFUNCS, sizeof(TS_name));
	funcs[numerate(false)] = (TS_name)"File";
	funcs[numerate(false)] = (TS_name)"RemoveFile";
	funcs[numerate(false)] = (TS_name)"GetFileList";
	return funcs;
}

int GetNumVariables(void){return 0;}
void ** GetVariables(void){return NULL;}
const char ** CCALL GetVariableNames(void){return NULL;}

void TS_ScriptFileFinalizer(v8::Persistent<v8::Value> object, void* parameter) {
    T5_file* file = (T5_file*)parameter;
    T5_close(file);
	object.Dispose();
}


v8Function TS_OpenFile(V8ARGS){
	if(args.Length()<1){
        THROWERROR("TS_OpenFile Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0, "TS_OpenFile Error: Arg 0 is not a string.");

    BEGIN_OBJECT_WRAP_CODE

    v8::String::Utf8Value str(args[0]);
    const char *filename = *str;

	T5_file *file = T5_OpenFile(filename);
	if(file==NULL){
		//create a file. Not done!
	}

    END_OBJECT_WRAP_CODE(ScriptFile, file);
}
v8Function TS_RemoveFile(V8ARGS){
	if(args.Length()<1){
        THROWERROR("TS_RemoveFile Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0, "TS_RemoveFile Error: Arg 0 is not a string.");
	v8::String::Utf8Value str(args[0]);
    string filename = (*str);
	if(remove((string(GetDirs()->save)+filename).c_str()) != 0 ){
		THROWERROR_REFERENCE((string("TS_RemoveFile Error: No such file as ")+(string(GetDirs()->save)+filename)).c_str());
	}
	return v8::Undefined();
}

v8Function TS_GetFileList(V8ARGS){
    v8::HandleScope TS_GetFileListscope;
	filehandle dir;
    filedata data;

    const char ** filenames = NULL;
	int i = 0;

#ifdef _WIN32
	dir = FindFirstFile((string(GetDirs()->save)+"*.*").c_str(), &data);
	if (dir!=INVALID_HANDLE_VALUE){
        do{
#else
	if ((dir=opendir(GetDirs()->save))!=NULL){
	    while((data=readdir(dir))!=NULL){
#endif
			if(FILENAME(data)[0]!='.'){
				filenames = (const char **)realloc(filenames, (i+1)*sizeof(const char *));
				filenames[i] = STRDUP(FILENAME(data));
				i++;
			}
#ifdef _WIN32
        } while(FindNextFile(dir, &data));
        FindClose(dir);
	}
#else
        }
        closedir(dir);
	}
#endif
	v8::Local<v8::Array> files = v8::Array::New(i);
	for(int e = 0; e<i; e++){
		files->Set(e, v8::String::New(filenames[e]));
	}
	free((void *)filenames);
	return TS_GetFileListscope.Close(files);

}
