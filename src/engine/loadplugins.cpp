#include <vector>
#include <string>
#include <functional>
#include <cstdlib>
#include <cstddef>
#include <TSPR/dlopenwrap.h>
#include <t5.h>
#include <opengame.h>
#include "engine.hpp"
#include "functionload.hpp"
#include "loadplugins.hpp"
#include "variableregister.hpp"

#ifdef _WIN32
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

const std::string plugin_directory_name = "plugin";

struct plugin_t {
    const char *(*Init)(int);
    void (*Close)(void);

    int (*GetNumFunctions)(void);
    FunctionArray (*GetFunctions)(void);
    NameArray (*GetFunctionNames)(void);
    int (*GetNumVariables)(void);
    VariableArray (*GetVariables)(void);
    NameArray (*GetVariableNames)(void);

    int num_functions;
    int num_variables;
    FunctionArray functions;
    NameArray function_names;
    VariableArray variables;
    NameArray variable_names;

    std::string name;

};

static std::vector<struct plugin_t> Plugins;
void ClosePlugin_t(const struct plugin_t &p){
    p.Close();
}

void CloseAllPlugins(){
    std::for_each(Plugins.begin(), Plugins.end(), ClosePlugin_t);
    Plugins.clear();
}

template<typename T, typename ...Args>
inline void LoadPluginMethod(fhandle handle, T(* &into)(Args...), const char *name, bool &worked){
    into = (T(*)(Args...))DLOPENFUNCTION(handle, name);
    if(!into)
        worked = false;
}

bool InitializePlugin(struct plugin_t &plugin, fhandle &handle){
    static int ID = 1;
    bool worked = true;
    LoadPluginMethod(handle, plugin.Init, "Init", worked);
    LoadPluginMethod(handle, plugin.Close, "Close", worked);
    LoadPluginMethod(handle, plugin.GetNumFunctions, "GetNumFunctions", worked);
    LoadPluginMethod(handle, plugin.GetFunctions, "GetFunctions", worked);
    LoadPluginMethod(handle, plugin.GetFunctionNames, "GetFunctionNames", worked);
    LoadPluginMethod(handle, plugin.GetNumVariables, "GetNumVariables", worked);
    LoadPluginMethod(handle, plugin.GetVariables, "GetVariables", worked);
    LoadPluginMethod(handle, plugin.GetVariableNames, "GetVariableNames", worked);

    if(plugin.Init!=nullptr){
        plugin.name = plugin.Init(ID++);
    }

    return worked;
}

void LoadPluginMembers(struct plugin_t plugin, v8::Isolate *isolate){

    plugin.num_functions = plugin.GetNumFunctions();
    plugin.functions = plugin.GetFunctions();
    plugin.function_names = plugin.GetFunctionNames();

    plugin.num_variables = plugin.GetNumVariables();
    plugin.variables = plugin.GetVariables();
    plugin.variable_names = plugin.GetVariableNames();

    for(int i = 0; i<plugin.num_functions; i++){
        addFunctionToList(plugin.function_names[i], plugin.functions[i], isolate);
    }

    for(int i = 0; i<plugin.num_variables; i++){
        addVariableToList(plugin.variable_names[i], plugin.variables[i], isolate);
    }

}

void LoadAllPlugins(v8::Isolate *isolate){

    t5::fs::Directory *plugin_directory = t5::fs::Entry::FromPath(plugin_directory_name)->AsDirectory();
    if((!t5::IsDir(plugin_directory_name.c_str()))||(plugin_directory==nullptr)){
        fprintf(stderr, "[Engine] Error: No plugin directory '%s'.\n", plugin_directory_name.c_str());
    }


    for(t5::fs::Directory::iterator i = plugin_directory->begin(); i!=plugin_directory->end(); i= i+1){
        const char *path = i->GetPath().c_str();
        fhandle handle = DLOPENFILE(path, DL_NOW|DL_GLOBAL);
        if(!handle){
            fprintf(stderr, "[Engine] Error: Could not load plugin binary %s.\n", path);
            continue;
        }

        struct plugin_t plugin;
        if(!InitializePlugin(plugin, handle)){
                if(plugin.name.empty())
                    fprintf(stderr, "[Engine] Error: Could not open plugin %s.\n", path);
                else
                    fprintf(stderr, "[Engine] Error: Could not open plugin %s.\n", Plugins.back().name.c_str());
            continue;
        }

        LoadPluginMembers(plugin, isolate);

        Plugins.push_back(plugin);

    }

}
