#define T5_INTERNAL
#include "t5.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifdef _WIN32
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

static std::vector<const char*> T5_Directories(0);

void T5CALL T5_init(int ndirs, ...){   //load T5_Directories, and init T5.
    T5_Directories.resize(0);
    va_list Dirs;
    va_start(Dirs, ndirs);
    for(int i = 0; i<ndirs; i++){
        T5_Directories.push_back(va_arg(Dirs, const char*));
    }
    va_end(Dirs);
}

void T5_AddDir(const char *dir){
    T5_Directories.push_back(dir);
}

T5_file *T5_OpenFile(const char* file){ //Make T5_ifile from filename
    for(unsigned int i = 0; i<T5_Directories.size(); i++){
    std::fstream testfile(std::string(T5_Directories[i]).append(file).c_str());
        if (testfile)
        {
            return new T5_file(std::string(T5_Directories[i]).append(file).c_str());
        }
    }
    return NULL;
}

T5_file::T5_file(const char*file){
    values.resize(0);
    stream.open(file, std::fstream::in|std::fstream::out|std::fstream::app);
    std::string buffer;
    std::string val = "";
    std::string key = "";
    std::string sec = "";
    bool isSec  = false;
    bool isKey  = true;
    bool secDone= false;
    while(stream.good()){
        getline(stream, buffer);
		if(buffer.empty()){
            continue;
        }

        key     = "";
        val     = "";

        isSec   = false;
        isKey   = true;
        secDone = false;

        for (size_t i=0; i < buffer.length(); i++){
            if(buffer.at(i)<30){
            continue;
            }
            if(buffer.at(i)=='['){
                isSec = true;
                isKey = false;
                key = "";
                continue;
            }
            if(isSec){
                if(buffer.at(i)==']'){
                    secDone = true;
                    break;
                }
                val+=buffer.at(i);
            }
            else if(isKey){
                if(buffer.at(i)=='='){
                isKey=false;
                continue;
                }
                key+=buffer.at(i);
            }
            else{
                val+=buffer.at(i);
            }

        }


        if(isSec&&secDone){
            sec = STRDUP(val.c_str());
                #ifdef VERBOSE
                printf("Section Found: %s", sec.c_str());
                #endif
        }
        else{
        values.push_back(new INIvalue(STRDUP(key.c_str()), STRDUP(val.c_str()), STRDUP(sec.c_str())));
        }
    }
}

T5_file::T5_file(){

}

T5_file::~T5_file(){
    stream.flush();
    stream.close();
}

void T5_close(T5_file *file){
    delete file;
}

const char* T5_file::getValue(const char* key){
    for(size_t i = 0; i<values.size(); i++){
            #ifdef VERBOSE
            printf("Value %s, Section %s\n", values[i]->val, values[i]->sec);
            #endif
        if(strcmp(values[i]->key, key)==0){
            return values[i]->val;
        }
    }
    return NULL;
}

const char* T5_file::getValueFromSection(const char* key, const char *sec){
	for(unsigned int i = 0; i<values.size(); i++){
		if(strcmp(values[i]->sec, sec)!=0){
            continue;
        }
        if(strcmp(values[i]->key, key)==0){
            return values[i]->val;
        }
    }
    return NULL;
}

void T5_file::writeValue(const char *value, const char *key){
    stream.seekg(std::ios::beg);
    stream<<key;
    stream<<"=";
    stream<<value;
    stream<<"\n";

    stream.flush();
}

void T5_file::writeValueToSection(const char *value, const char *_key, const char *_sec){
    stream.seekg(std::ios::beg);

    std::string buffer;
    std::string val = "";
    std::string key = "";
    std::string sec = "";
    bool isSec  = false;
    bool isKey  = true;
    bool secDone= false;

    while(stream.good()){
        getline(stream, buffer);
		if(buffer.empty()){
            continue;
        }

        key     = "";
        val     = "";

        isSec   = false;
        isKey   = true;
        secDone = false;

        for (size_t i=0; i < buffer.length(); i++){
            if(buffer.at(i)<30){
            continue;
            }
            if(buffer.at(i)=='['){
                isSec = true;
                isKey = false;
                key = "";
                continue;
            }
            if(isSec){
                if(buffer.at(i)==']'){
                    secDone = true;
                    break;
                }
                val+=buffer.at(i);
            }
            else if(isKey){
                if(buffer.at(i)=='='){
                isKey=false;
                continue;
                }
                key+=buffer.at(i);
            }
            else{
                val+=buffer.at(i);
            }

        }


        if(isSec&&secDone){
            if(strcmp(sec.c_str(), _sec)==0){
                stream<<"\n";
                stream<<_key;
                stream<<"=";
                stream<<value;
                return;
            }
        }
    }
    stream.seekg(std::ios::beg);
    stream<<"[";
    stream<<sec;
	stream<<"]";
	stream<<key;
    stream<<"=";
    stream<<value;
    stream<<"\n";

    stream.flush();
}

INIvalue::INIvalue(const char *keyname, const char *value, const char *section){
    key = keyname;
    val = value;
    sec = section;
}
