#include <cstdio>
#include <t5.h>
#include <TSPR/dlopenwrap.h>
#include "loadplugin.hpp"

namespace Turbo {

bool loadAllPlugins(JSContext *ctx, const std::string &dir){
    t5::DataSource::StdOut()->WriteF("[Engine] Info loading plugins from ", dir, '\n');
    std::vector<const std::string> those;
    if(!listAllPlugins(those, dir)){
        t5::DataSource::StdOut()->WriteF("[Engine] Error could not iterate files in ", dir, '\n');
        return false;
    }
    
    for(std::vector<const std::string>::const_iterator iter = those.cbegin(); iter!=those.cend(); iter++){
        t5::DataSource::StdOut()->WriteF("[Engine] Info loading plugin '", *iter, '\'', '\n');
        loadPlugin(ctx, *iter);
    }
    
    return true;
    
}

bool listAllPlugins(std::vector<const std::string> &those, const std::string &dir){
    t5::fs::Entry *e = t5::fs::Entry::FromPath(dir);
    if(!e)
        return false;
    t5::fs::Directory *d = e->AsDirectory();
    if(!d)
        return false;
    
    const int suffix_len = strlen(LIBRARY_SUFFIX);
    
    for(t5::fs::Entry::iterator iter = d->begin(); iter!=d->end(); iter++){
        
        
        std::string last_path = iter->GetPath().substr(iter->GetPath().rfind('/')+1);
        t5::DataSource::StdOut()->WriteF("[Engine] Info loading plugin ", last_path, '\n');
        if((LIBRARY_PREFIX[0]!='\0') && (last_path.find(LIBRARY_PREFIX)!=0)){
            continue;
        }
        
        if(last_path.rfind(LIBRARY_SUFFIX)!=last_path.length()-suffix_len){
            continue;
        }
        
        those.push_back(iter->GetPath());
        
    }
        
    delete e;
    return true;
}

template<typename T, typename S>
inline bool GetFunction(T &out, const S &pluginname, fhandle plugin, const char *name){
    out = (T)DLOPENFUNCTION(plugin, name);
    if(!out){
        t5::DataSource::StdOut()->WriteF("[Engine] Error no ", name, " function in plugin ", pluginname);
        return false;
    }
    return true;
}

struct Plugin{
    
    const char *name;
    unsigned num_functions;
    unsigned num_variables;
    
    fhandle handle;
    
    struct {
        InitFunction Init;
        CloseFunction Close;
        NumFunction NumFunctions;
        GetFuncFunction GetFunction;
        GetNameFunction GetFunctionName;
        
        NumFunction NumVariables;
        GetVarFunction GetVariable;
        GetNameFunction GetVariableName;
        
    } API;

};

bool loadPlugin(JSContext *ctx, const std::string &path){
    
    JS::RootedObject global(ctx, JS::CurrentGlobalOrNull(ctx));
    
    if((!global) || OBJECT_TO_JSVAL(global).isNull()){
        // This is actually REALLY bad.
        t5::DataSource::StdOut()->WriteF("[Engine] Error could not get global object.\n");
        return false;
    }
    
    static unsigned ID = 0;
    
    struct Plugin plugin;
    plugin.handle = DLOPENFILE(path.c_str(), DL_NOW|DL_LOCAL);
    
    if(!GetFunction(plugin.API.Init, path, plugin.handle, "Init"))
        return false;
    
    plugin.name = plugin.API.Init(ctx, ++ID);
    
    #define LOAD_FUNC(NAME)\
    if(!GetFunction(plugin.API.NAME, plugin.name, plugin.handle, #NAME)) do{DLCLOSELIBRARY(plugin.handle); return false;}while(true)

    LOAD_FUNC(Close);
    LOAD_FUNC(NumFunctions);
    LOAD_FUNC(GetFunction);
    LOAD_FUNC(GetFunctionName);
    LOAD_FUNC(NumVariables);
    LOAD_FUNC(GetVariable);
    LOAD_FUNC(GetVariableName);
    
    #undef LOAD_FUNC
    
    plugin.num_functions = plugin.API.NumFunctions(ctx);
    plugin.num_variables = plugin.API.NumVariables(ctx);

    for(unsigned i = 0; i<plugin.num_functions; i++){
        JS_DefineFunction(ctx, global, plugin.API.GetFunctionName(ctx, i), plugin.API.GetFunction(ctx, i), 0, 0);
    }
    
    for(unsigned i = 0; i<plugin.num_variables; i++){
        JS::RootedValue val(ctx);
        plugin.API.GetVariable(ctx, i, &val);
        if(!JS_DefineProperty(ctx, global, plugin.API.GetVariableName(ctx, i), val, JSPROP_PERMANENT)){
            DLCLOSELIBRARY(plugin.handle); return false;
        }
    }

    return true;
}

}
