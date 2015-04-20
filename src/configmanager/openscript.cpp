#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <jsapi.h>
#include <t5.h>
#define CONFIGMGR_INTERNAL
#include "openscript.h"
#include "opengame.h"

#ifdef _WIN32
#define STRDUP _strdup
#else
#include <cstring>
#define STRDUP strdup
#endif

struct SystemScriptTracker{
    JSContext *ctx;
    std::vector<std::string> scripts; 
};

std::vector<struct SystemScriptTracker> system_script_tracker;
std::mutex system_script_tracker_mutex;

bool TS_ScriptWasEvaluated(JSContext *ctx, const std::string &script, bool add_if_not_there = true){
    
    system_script_tracker_mutex.lock();
    struct SystemScriptTracker *tracker = nullptr;
    for(std::vector<struct SystemScriptTracker>::iterator iter = system_script_tracker.begin(); iter!=system_script_tracker.end(); iter++){
        if(iter->ctx==ctx){
            tracker = &(*iter);
            break;
        }
    }
    
    if(tracker==nullptr){
        struct SystemScriptTracker that = {ctx};
        system_script_tracker.push_back(that);
        tracker = &(system_script_tracker.back());
    }
    
    bool matches = false;
    
    for(std::vector<std::string>::const_iterator c_iter = tracker->scripts.cbegin(); c_iter!=tracker->scripts.cend(); c_iter++){
        if((*c_iter)==script){
            matches = true;
            break;
        }
    }
    
    if((!matches) && add_if_not_there)
        tracker->scripts.push_back(script);
    
    system_script_tracker_mutex.unlock();

    if(matches){
        return true;
    }
    
#ifndef RELEASE_NO_PRINTING
    t5::DataSource::StdOut()->WriteF("[ConfigManager] Info considering script ", script, ", ", (matches?"not ":""), "compiled.\n");
#endif

    return false;
    
}

bool TS_LoadScript(JSContext *ctx, const char *filename, bool only_once){
    
    if(!filename){
        t5::DataSource::StdOut()->WriteF("[ConfigManager] ", __func__, " Error bad filename NULL\n");
        return false;
    }
    
    if(!t5::IsFile(filename)){
        t5::DataSource::StdOut()->WriteF("[ConfigManager] ", __func__, " Error no such file ", filename, "\n");
        return false;
    }
    
    {
        std::string file = filename;
        if(only_once && TS_ScriptWasEvaluated(ctx, file))
            return true;
    }
    
    std::unique_ptr<t5::DataSource> source(t5::DataSource::FromPath(t5::DataSource::eRead, filename));
    
    if(source==nullptr){
        t5::DataSource::StdOut()->WriteF("[ConfigManager] ", __func__, " Error could not open main script ", filename, '\n');
        return false;
    }
    
    size_t len = source->Length();
    char * script = (char *)malloc(len+1);
    script[len] = '\0';
    
    source->Read(script, len);
    bool r = TS_ExecuteStringL(ctx, filename, script, len);
    
    free(script);
    
    return r;

}

bool TS_ExecuteStringL(JSContext *ctx, const char *filename, const char *source,  size_t len){

    JS::OwningCompileOptions options(ctx);
    options.setUTF8(true);
    options.setFileAndLine(ctx, filename, 0);
    options.setIntroductionScript(nullptr);
    options.setVersion(JSVERSION_LATEST);
    JS::RootedObject scope(ctx, JS::CurrentGlobalOrNull(ctx));
    
    if((!scope) || OBJECT_TO_JSVAL(scope).isNull())
        return false;
    
    JS::RootedScript script(ctx);
    JS::RootedValue rval(ctx);

    if(!JS::Compile(ctx, scope, options, source, len, &script)){
        t5::DataSource::StdOut()->WriteF("[ConfigManager] ", __func__, " Error could not compile script ", filename, '\n');
        return false;
    }
    
    JS_MaybeGC(ctx);
    
    if(!JS_ExecuteScript(ctx, scope, script, &rval)){
        t5::DataSource::StdOut()->WriteF("[ConfigManager] ", __func__, " Error could not execute script ", filename, '\n');
        return false;
    }
    
    return true;

}

bool ExecuteString(JSContext *ctx, const char *filename, const char *source){
    return TS_ExecuteStringL(ctx, filename, source, strlen(source));
}

// NULL prefix for no prefix.
bool TS_LoadScriptPrefixed_JS(JSContext *ctx, unsigned argc, JS::Value *vp, const char * prefix, bool only_once){
    
    std::string file;
    
    if(prefix){
        
        file = prefix;
    
        if((file.size()!=0) && (file.back()!='/'))
            file.push_back('/');
    
    }
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    args.rval().setUndefined();
    
    if(args.length()<1){
        const char error_str[] = "[ConfigManager] TS_LoadScriptPrefixed_JS Error called with no arguments";
        JS::RootedValue error(ctx, STRING_TO_JSVAL(JS_NewStringCopyN(ctx, error_str, sizeof(error_str)-1)));
        JS_SetPendingException(ctx, error);
        return false;
    }
    
    if(!args[0].isString()){
        const char error_str[] = "[ConfigManager] TS_LoadScriptPrefixed_JS Error argument 0 is not a string";
        JS::RootedValue error(ctx, STRING_TO_JSVAL(JS_NewStringCopyN(ctx, error_str, sizeof(error_str)-1)));
        JS_SetPendingException(ctx, error);
        return false;
    }
    
    {
        char buffer[1024]; // No path can be even this large according to both Posix and Win32 
        size_t len = JS_EncodeStringToBuffer(ctx, args[0].toString(), buffer, sizeof(buffer));
        
        if(len==sizeof(buffer)-1){
            const char error_str[] = "[ConfigManager] TS_LoadScriptPrefixed_JS Error argument 0 string is too long";
            JS::RootedValue error(ctx, STRING_TO_JSVAL(JS_NewStringCopyN(ctx, error_str, sizeof(error_str)-1)));
            JS_SetPendingException(ctx, error);
            return false;
        }
        
        buffer[len] = '\0';
        file+=buffer;
        
        if(only_once && TS_ScriptWasEvaluated(ctx, file))
            return true;
        
        t5::DataSource *source = t5::DataSource::FromPath(t5::DataSource::eRead, file.c_str());
        if(!source){
            std::string err = "[ConfigManager] TS_LoadScriptPrefixed_JS Error could open script "; err+=file;
            JS::RootedValue error(ctx, STRING_TO_JSVAL(JS_NewStringCopyN(ctx, err.c_str(), err.length())));
            JS_SetPendingException(ctx, error);
            return false;
        }
        
        int source_len = source->Length();
        std::unique_ptr<char, void(*)(void *)> script_source((char *)malloc(source_len+1), free);
        script_source.get()[source_len] = '\0';
        source->Read(script_source.get(), source_len);
        
        if(!TS_ExecuteStringL(ctx, file.c_str(), script_source.get(), source_len)){
            if(!JS_ReportPendingException(ctx)){
                std::string err = "[ConfigManager] TS_LoadScriptPrefixed_JS Error could not run script "; err+=file;
                JS::RootedValue error(ctx, STRING_TO_JSVAL(JS_NewStringCopyN(ctx, err.c_str(), err.length())));
                JS_SetPendingException(ctx, error);
            }
            return false;
        }
    }
    return true;
}

