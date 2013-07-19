
#define PLUGINNAME "scriptfs"
#include "../common/plugin.h"
#include "scriptfs.h"
#include "../../t5.h"
#include <stdio.h>
#include <fstream>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <assert.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#define SET_BINARY_MODE(file)
#endif

#define CHUNK 0xFFFF

#define DEBUG 0

int numerate(bool reset){
    static int i = 0;
    if(reset) {
        i = 0;
        return 0;
    }
    i++;
    return i-1;
}

#define NUMFUNCS 5
#define NUMVARS 0

v8Function TS_fileRead(V8ARGS);
v8Function TS_fileWrite(V8ARGS);
v8Function TS_fileFlush(V8ARGS);
v8Function TS_fileClose(V8ARGS);
v8Function TS_OpenRawFile(V8ARGS);
v8Function TS_OpenFile(V8ARGS);
v8Function TS_RemoveFile(V8ARGS);
v8Function TS_GetFileList(V8ARGS);
v8Function TS_DoesFileExist(V8ARGS);

void * TS_OpenFilePointer          = V8FUNCPOINTER(TS_OpenFile);
void * TS_OpenRawFilePointer          = V8FUNCPOINTER(TS_OpenRawFile);
void * TS_RemoveFilePointer        = V8FUNCPOINTER(TS_RemoveFile);
void * TS_GetFileListPointer       = V8FUNCPOINTER(TS_GetFileList);
void * TS_DoesFileExistPointer     = V8FUNCPOINTER(TS_DoesFileExist);

DECLARE_OBJECT_TEMPLATES(ScriptFile);
DECLARE_OBJECT_TEMPLATES(RawFile);


void Close(){
}
initFunction Init(){
	T5_init(1, GetDirs()->save);
    INIT_OBJECT_TEMPLATES(ScriptFile);
    SET_CLASS_NAME(ScriptFile, "File");
    INIT_OBJECT_TEMPLATES(RawFile);
    SET_CLASS_NAME(RawFile, "RawFile");

    ADD_TO_PROTO(ScriptFile, "read", TS_fileRead);
    ADD_TO_PROTO(ScriptFile, "write", TS_fileWrite);
    ADD_TO_PROTO(ScriptFile, "flush", TS_fileFlush);
    ADD_TO_PROTO(ScriptFile, "close", TS_fileClose);
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
	funcs[numerate(false)] = TS_DoesFileExistPointer;
	return funcs;
}

const char ** GetFunctionNames(){
	numerate(true);
	nameArray funcs = (nameArray)calloc(NUMFUNCS, sizeof(TS_name));
	funcs[numerate(false)] = (TS_name)"File";
	funcs[numerate(false)] = (TS_name)"RawFile";
	funcs[numerate(false)] = (TS_name)"RemoveFile";
	funcs[numerate(false)] = (TS_name)"GetFileList";
	funcs[numerate(false)] = (TS_name)"DoesFileExist";
	return funcs;
}

int GetNumVariables(void){return 0;}
void ** GetVariables(void){return NULL;}
const char ** CCALL GetVariableNames(void){return NULL;}

void TS_ScriptFileFinalizer(V8FINALIZERARGS) {
    T5_file* file = (T5_file*)parameter;
    T5_close(file);
	object->Dispose();
}


v8Function TS_OpenFile(V8ARGS){
	if(args.Length()<1){
        THROWERROR("[scriptfs] TS_OpenFile Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);

    BEGIN_OBJECT_WRAP_CODE

    v8::String::Utf8Value str(args[0]);
    const char *filename = *str;

	T5_file *file = T5_OpenFile(filename);
	if(file==NULL){
        T5_close(file);
        FILE *newfile;
        newfile = fopen(filename, "w");
        if(newfile==NULL){
            THROWERROR((((string)"[scriptfs] TS_OpenFile Error: Could not create file ")+string(filename)+((string)".")).c_str());
        }
        fclose(newfile);
        file = T5_OpenFile(filename);
	}

    END_OBJECT_WRAP_CODE(ScriptFile, file);
}
v8Function TS_RemoveFile(V8ARGS){
	if(args.Length()<1){
        THROWERROR("[scriptfs] TS_RemoveFile Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);
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

	const char *directory;

	if(args.Length()>0){
			CHECK_ARG_STR(0);
		    v8::String::Utf8Value str(args[0]);


			if(strnlen(*str, 1022)<1){
				directory = "";
			}
			else{
				if(DEBUG) printf("[scriptfs] GetFileList Info: The relative directory was %s\n", *str);
				char * tdir = (char *)malloc(strlen(*str)+1);
				if(DEBUG) printf("[scriptfs] GetFileList Info: tdir = %s\n", tdir);
				tdir = strcat(tdir, *str);
                char * ls = strrchr(tdir, '/');
                if(ls[1]!='\0'){
                    strcat(tdir, "/");
                }
                if(DEBUG) printf("[scriptfs] GetFileList Info: The directory as concated was %s\n", tdir);

                directory = STRDUP(tdir);
                free(tdir);
			}
		}
	else{
		directory = "";
	}

    const char ** filenames = NULL;
	int i = 0;
	const char *fulldir = STRDUP((string(GetDirs()->save).append(directory)).c_str());
	if(DEBUG) printf("[scriptfs] GetFileList Info: The directory specified is %s\n", fulldir);
	#ifdef _WIN32

	const char *fulldirlist = STRDUP((string(GetDirs()->save).append(directory)+"*.*").c_str());
	DWORD attribs = ::GetFileAttributesA(fulldir);
	if ((attribs != INVALID_FILE_ATTRIBUTES) && (attribs & FILE_ATTRIBUTE_DIRECTORY)) {
		if(DEBUG) printf("[scriptfs] GetFileList Info: the directory was valid.");
		dir = FindFirstFile(fulldirlist, &data);

		if (dir!=INVALID_HANDLE_VALUE){
			do{
#else
	    if ((dir=opendir(GetDirs()->save))!=NULL){
	        while((data=readdir(dir))!=NULL){
#endif
				if((!ISDIRECTORY(data))&&(FILENAME(data)[0]!='.')){
					filenames = (const char **)realloc(filenames, (i+1)*sizeof(const char *));
					filenames[i] = STRDUP(FILENAME(data));
					i++;
				}
#ifdef _WIN32
		    } while(FindNextFile(dir, &data));
		    FindClose(dir);
		}//dir!=INVALID_HANDLE_VALUE
		free((void*)fulldirlist);
	}//attribs!=INVALID_FILE_ATTRIBUTES
#if DEBUG
	else{
		printf("[scriptfs] GetFileList Info: Directory does not exist.\n");
	}
#endif
#else
        }
        closedir(dir);
	}
#endif
	v8::Local<v8::Array> files = v8::Array::New(i);
	for(int e = 0; e<i; e++){
		files->Set(e, v8::String::New(filenames[e]));
        free((void*)filenames[e]);
	}
	free(filenames);
	free((void*)fulldir);
	if(directory!=NULL){
		free((void*)directory);
	}
	return TS_GetFileListscope.Close(files);

}

v8Function TS_DoesFileExist(V8ARGS){
    if(args.Length()<1){
        THROWERROR("[scriptfs] TS_DoesFileExist Error: Called with fewer than 2 arguments!\n");
    }
    CHECK_ARG_STR(0);
    v8::String::Utf8Value str(args[0]);
    const char *path = (string(GetDirs()->save)+string(*str)).c_str();

    FILE *filep = fopen(path, "r");

    if(filep!=NULL){
        fclose(filep);
        return v8::True();
    }
    else{
        return v8::False();
    }
}

v8Function TS_fileRead(V8ARGS)
{
    if(args.Length()<2){
        THROWERROR("[scriptfs] TS_fileRead Error: Called with fewer than 2 arguments!\n");
    }
    CHECK_ARG_STR(0);
	//CHECK_ARG_STR(1);
    v8::String::AsciiValue key(args[0]);
    v8::String::AsciiValue def(args[1]);
	//v8::Local<v8::Object> self = args.Holder();
	//v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	//void* ptr = wrap->Value();
	const char * keyval = GET_SELF(T5_file*)->getValue(*key);
	if(keyval==NULL){
        //static_cast<T5_file*>(ptr)->writeValue(*key, *def);
        GET_SELF(T5_file*)->writeValue(*key, *def);
        return v8::String::New(*def);
	}
	if(args[1]->IsNumber()){
		return v8::Number::New(atof(keyval));
	}
	else{
		return v8::String::New(keyval);
	}
}

v8Function TS_fileWrite(V8ARGS)
{
    if(args.Length()<2){
        THROWERROR("[scriptfs] TS_fileWrite Error: Called with fewer than 2 arguments!\n");
    }
    CHECK_ARG_STR(0);
    CHECK_ARG_STR(1);
    v8::String::AsciiValue key(args[0]);
    v8::String::AsciiValue val(args[1]);
	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
    static_cast<T5_file*>(ptr)->writeValue(*key, *val);
	return v8::Undefined();
}

v8Function TS_fileFlush(V8ARGS)
{
	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
    static_cast<T5_file*>(ptr)->flush();
	return v8::Undefined();
}

v8Function TS_fileClose(V8ARGS)
{
	v8::Local<v8::Object> self = args.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
    T5_close(static_cast<T5_file*>(ptr));
	return v8::Undefined();
}

v8Function TS_OpenRawFile(V8ARGS){
    return v8::Undefined();
}
/*
RawData TS_DeflateRawData(RawData data){
    RawData out;
    out.size = 0;
    out.compressed = 0;
    out.data = NULL;
    int flush = Z_NULL;
    z_stream strm;

    unsigned int leftToRead = 0;
    unsigned int alreadyRead = 0;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    if(deflateInit(&strm, (data.size<=CHUNK)?Z_BEST_SPEED:Z_BEST_COMPRESSION)!=Z_OK){
        return out;
    }

    leftToRead = data.size;

    while(flush!=Z_FINISH){

        assert(leftToRead==data.size-alreadyRead);

        strm.avail_in = ((unsigned int)data.size)-alreadyRead;

        flush = (alreadyRead>=data.size)?Z_FINISH:Z_NO_FLUSH;

        strm.next_in = (unsigned char*)((intptr_t)data.data+(intptr_t)alreadyRead);

        do{
            strm.avail_out = CHUNK;

            out.data = realloc((void *)out.data, alreadyRead+CHUNK);

            strm.next_out = (unsigned char *)out.data;

            deflate(&strm, flush);

            alreadyRead = CHUNK - strm.avail_out;
            leftToRead = data.size - alreadyRead;

        }while(strm.avail_out==0);
    }
    deflateEnd(&strm);
    assert(leftToRead==0);
    out.data = realloc((void *)out.data, data.size);
    return out;
}

RawData TS_InflateRawData(RawData data){

return RawData();
}

*/
