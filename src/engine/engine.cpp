#include <string>
#include <cstdlib>
#include <jsapi.h>
#include <opengame.h>
#include <openscript.h>
#include <t5.h>
#include "loadplugin.hpp"
#include "intrinsics.hpp"
#include "engine.hpp"

static bool inited = false;

const JSClass ts_global_class = {
    "global",
    JSCLASS_GLOBAL_FLAGS,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr, 
    nullptr, 
    nullptr, 
    nullptr,
    JS_GlobalObjectTraceHook
};

void TS_ReportError(JSContext *ctx, const char *msg, JSErrorReport *report) {
    
    std::string filename = "NULL";
    
    if(report->filename)
        filename = report->filename;
    
    const char * error_type = "Generic Error";
    if(JSREPORT_IS_WARNING(report->flags))
        error_type="Warning";
    else if(JSREPORT_IS_EXCEPTION(report->flags))
        error_type="Exception";
    else if(JSREPORT_IS_STRICT_MODE_ERROR(report->flags))
        error_type="StrictError";
    
    t5::DataSource::StdOut()->Put('\n');
    t5::DataSource::StdOut()->WriteF("[Engine] ", JSREPORT_IS_STRICT(report->flags)?"Strict ":"", error_type, " in file ", filename, " line ", report->lineno+1, '\n', msg, '\n');
    t5::DataSource::StdOut()->Put('\n');
    
}

inline void TS_PathSugar_Dir(std::string &that){
    if(that.empty())
        that.push_back('.');
    if(that.back()!='/')
        that.push_back('/');
}

inline std::string TS_PathFromCstring(const char *that){
    std::string path = that;
    TS_PathSugar_Dir(path);
    return path;
}

bool RunGame(const char *path, const char *root_path){
    
    // Init JS if not already done
    if(!inited){
        inited = true;
        if(!JS_Init())
            return false;
        
        atexit(JS_ShutDown);
        
        t5::DataSource::StdOut()->WriteF("[Engine] Info Inited JS VM\n");
    }
    
    std::string ts_root, sgm_file, game_folder, system_folder;
    
    ts_root = TS_PathFromCstring(root_path);
        
    // Validate input and prepare configuration input
    if(!t5::IsDir(ts_root)){
        t5::DataSource::StdOut()->WriteF("[Engine] Error root directory ", ts_root, " is not a directory.\n");
        return false;
    }
    
    system_folder = ts_root+"system/";
    
    if(t5::IsFile(path)){
        sgm_file = path;
    }
    else if(t5::IsDir(path)){
        sgm_file = TS_PathFromCstring(path);
        sgm_file += "game.sgm";
    }
    else{
        t5::DataSource::StdOut()->WriteF("[Engine] Error path ", path, " is not a file or directory.\n");
        return false;
    }
    
    if(!t5::IsFile(sgm_file)){
        t5::DataSource::StdOut()->WriteF("[Engine] Error finding SGM file ", sgm_file, '\n');
        return false;
    }
    
    // Initialize a TS_Environment for our game.
    long long last_slash = sgm_file.rfind('/');
    if(last_slash!=std::string::npos)
        game_folder = sgm_file.substr(0, last_slash);
    
    TS_PathSugar_Dir(game_folder);
    
    // These _should_ have already been verified above.
    assert(t5::IsFile(sgm_file));
    assert(t5::IsDir(system_folder));
    assert(t5::IsDir(game_folder));
    
    // Prepare a TS_Environment
    struct TS_GameEnvironment *game_env = static_cast<struct TS_GameEnvironment*>(malloc(sizeof(struct TS_GameEnvironment)));
    game_env->directories = TS_InitDirectories(game_folder.c_str());
    game_env->system = TS_LoadSystem(system_folder.c_str(), nullptr);
    game_env->config = TS_LoadConfig(sgm_file.c_str());
    
    if(game_env->directories==nullptr)
        t5::DataSource::StdOut()->WriteF("[Engine] Error could not open game folder ", game_folder, '\n');
        
    if(game_env->system==nullptr)
        t5::DataSource::StdOut()->WriteF("[Engine] Error could not open system ", system_folder, '\n');
    
    if(game_env->config==nullptr)
        t5::DataSource::StdOut()->WriteF("[Engine] Error could not open SGM file ", sgm_file, '\n');
    
    if(!(game_env->directories && game_env->system && game_env->config))
        return false;
    
#ifndef RELEASE_NO_PRINTING
    TS_PrintDirectories(game_env->directories, stdout);
    TS_PrintSystem(game_env->system, stdout);
    TS_PrintConfig(game_env->config, stdout);
#endif
    // Prepare a JSRuntime if none exist.
    std::unique_ptr<JSRuntime, void(*)(JSRuntime *)> runtime_ptr(JS_NewRuntime(8L * 1024L * 1024L), JS_DestroyRuntime);
    JSRuntime *runtime = runtime_ptr.get();
    
    // Prepare a JSContext and a compartment, including the global object
    std::unique_ptr<JSContext, void(*)(JSContext *)> context_ptr(JS_NewContext(runtime, 8192), JS_DestroyContext);
    JSContext *ctx = context_ptr.get();
    
    if(!ctx){
        t5::DataSource::StdOut()->WriteF("[Engine] Error could not create context\n");
        return false;
    }
    
    JS_SetErrorReporter(runtime, TS_ReportError);
    JSAutoRequest request(ctx);
    JS::RootedObject global(ctx, JS_NewGlobalObject(ctx, &ts_global_class, nullptr, JS::FireOnNewGlobalHook));
    
    if(!global){
        t5::DataSource::StdOut()->WriteF("[Engine] Error could not initialize JS global object\n");
        return false;
    }
    
    JSAutoCompartment compartment(ctx, global);
    if(!JS_InitStandardClasses(ctx, global)){
        t5::DataSource::StdOut()->WriteF("[Engine] Error could not initialize JS standard classes\n");
        return false;
    }
    
    // Associate the context and the environment for plugin use
    TS_AssociateContextEnvironment(ctx, game_env);
    
    // Initialize the engine intrinsics
    Turbo::initializeIntrinsicFunctions(ctx, global);
    
    // Initialize plugins
    Turbo::loadAllPlugins(ctx, game_env->system->plugin);
    
    // Execute main script
    
    std::string main_script = std::string(game_env->directories->script) + game_env->config->mainscript;
    if(!TS_LoadScript(ctx, main_script.c_str())){
        t5::DataSource::StdOut()->WriteF("[Engine] Error could not run main script ", game_env->config->mainscript, '\n');
        return false;
    }

    // Call game function
    JS::Rooted<JS::Value> rval(ctx);
    if(!JS::Call(ctx, global, game_env->config->gamefunc, JS::HandleValueArray::empty(), &rval)){
        t5::DataSource::StdOut()->WriteF("[Engine] Error could not call gamefunc ", game_env->config->gamefunc, '\n');
        return false;
    }
    
    // Dissociate the JSContext and environment for plugin use
    TS_RemoveContextEnvironment(nullptr, game_env);
    
    // Free the environment
    TS_FreeEnvironment(game_env);
    
    // Clean up the JSContext
    
    // Free the JSContext
    JS_DestroyContext(ctx);
    
    return true;
    
}

int main(int argc, char *argv[]){
    std::string game = "startup";
    if(argc>1){
        game = argv[argc-1];
    }
    
    if(!RunGame(game.c_str(), "")){
        t5::DataSource::StdOut()->WriteF("[Engine] Error running game ", game, '\n');
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
    
}
