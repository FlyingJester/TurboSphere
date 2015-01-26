#include <cstdio>
#include <t5.h>
#include <TSPR/dlopenwrap.h>
#include "loadplugin.hpp"

namespace Turbo {

bool loadAllPlugins(JSContext *ctx, const std::string &dir){
    std::vector<const std::string> those;
    if(!listAllPlugins(those, dir))
        return false;
    
    for(std::vector<const std::string>::const_iterator iter = those.cbegin(); iter!=those.cend(); iter++){
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
        
        std::string last_path = d->GetPath().substr(d->GetPath().rfind('/')+1);
        if((LIBRARY_PREFIX[0]!='\0') && (last_path.find(LIBRARY_PREFIX)!=0))
            continue;
        
        t5::DataSource::StdOut()->WriteF("[Engine] Info: Wanted last occurrence of ", LIBRARY_SUFFIX, " at ", 
        last_path.rfind(LIBRARY_SUFFIX), ", was at ", last_path.length()-suffix_len, '\n');
        if(last_path.rfind(LIBRARY_SUFFIX)!=last_path.length()-suffix_len){
            
            printf(" It didn't work.\n");
            
            continue;
        }
        
        putchar('\n');
        
        those.push_back(d->GetPath());
        
    }
        
    delete e;
    return true;
}

bool loadPlugin(JSContext *ctx, const std::string &path){
    
    fhandle plugin = DLOPENFILE(path.c_str(), DL_NOW|DL_LOCAL);
    
    InitFunction init = (InitFunction)DLOPENFUNCTION(plugin, "Init");
    if(!init){
        t5::DataSource::StdOut()->WriteF("[Engine] Error: No Init function in plugin ", path);
        return false;
    }
    return true;

}

}
