#include <string>
#include <array>
#include <vector>
#include <memory>
#include <mutex>
#include <cassert>
#include <t5.h>
#include <opengame.h>

static std::mutex context_association_mutex;

struct TS_ContextAssociation_t {
    void *ctx;
    TS_GameEnvironment *env;
};

typedef std::vector<std::unique_ptr<TS_ContextAssociation_t> > cav_t;

cav_t context_association_vector;

void TS_AssociateContextEnvironment(void *ctx, struct TS_GameEnvironment *env){
    context_association_mutex.lock();
    context_association_vector.push_back(std::unique_ptr<TS_ContextAssociation_t>(new TS_ContextAssociation_t()));
    context_association_vector.back()->ctx = ctx;
    context_association_vector.back()->env = env;
    context_association_mutex.unlock();
}


struct TS_GameEnvironment *TS_GetContextEnvironment(void *ctx){
    
    struct TS_GameEnvironment *that = nullptr;
    
    context_association_mutex.lock();
    
    for(cav_t::const_iterator iter = context_association_vector.cbegin(); iter!=context_association_vector.cend(); iter++){
        if((*iter)->ctx==ctx){
            that = (*iter)->env;
            break;
        }
    }
    context_association_mutex.unlock();
    
    return that;
    
}

void TS_RemoveContextEnvironment(void *ctx, struct TS_GameEnvironment *env){
    
    assert((ctx || env) && !(ctx && env));
    
    context_association_mutex.lock();
    
    for(cav_t::iterator iter = context_association_vector.begin(); iter!=context_association_vector.end(); iter++){
        if(((*iter)->ctx==ctx) || ((*iter)->env==env)){
            context_association_vector.erase(iter);
            break;
        }
    }
    
    context_association_mutex.unlock();
}

void TS_FreeEnvironment(struct TS_GameEnvironment *env){
    TS_FreeDirectories(env->directories);
    TS_FreeSystem(env->system);
    TS_FreeConfig(env->config);
//  TS_FreeWilly    
}

void TS_ResetContextEnvironment(void){
    context_association_mutex.lock();
    context_association_vector.clear();
    context_association_mutex.unlock();
}

struct TS_GameDirectories *TS_InitDirectories(const char *path){
    
    assert(t5::IsDir(path));
    
    struct TS_GameDirectories *that = 
        static_cast<struct TS_GameDirectories *>(malloc(sizeof(struct TS_GameDirectories)));
    
    {
        std::string  root_path = path;
        if(root_path.back()!='/')
            root_path.push_back('/');
        
        that->root = strdup(root_path.c_str());
    }
    
    return TS_FillDirectories(that);

}

struct TS_GameDirectories *TS_FillDirectories(TS_GameDirectories *that){
    
    assert(t5::IsDir(that->root));
    
    std::string root_path = that->root;
    if(root_path.back()!='/'){
        root_path.push_back('/');
        
        free((void *)that->root);
        that->root = strdup(root_path.c_str());
        
    }
    
    #define ROOT_SUFFIX(NAME)\
        that->NAME = strdup((root_path+ #NAME + '/').c_str());
    #define ROOT_SUFFIX_S(NAME)\
        that->NAME = strdup((root_path+ #NAME + 's' + '/').c_str());
    
    ROOT_SUFFIX_S(image);
    ROOT_SUFFIX_S(font);
    ROOT_SUFFIX_S(script);
    ROOT_SUFFIX_S(sound);
    ROOT_SUFFIX_S(map);
    ROOT_SUFFIX_S(spriteset);
    ROOT_SUFFIX_S(animation);
    ROOT_SUFFIX_S(windowstyle);
    ROOT_SUFFIX_S(soundfont);
    ROOT_SUFFIX_S(shader);
    ROOT_SUFFIX(other);
    ROOT_SUFFIX(save);
    
    #undef ROOT_SUFFIX
    #undef ROOT_SUFFIX_S
    
    return that;

}

void TS_FreeDirectories(struct TS_GameDirectories *that){
    
    free((void *)that->root);
    
    free((void *)that->image);
    free((void *)that->font);
    free((void *)that->script);
    free((void *)that->sound);
    free((void *)that->map);
    free((void *)that->spriteset);
    free((void *)that->animation);
    free((void *)that->windowstyle);
    free((void *)that->soundfont);
    free((void *)that->shader);
    free((void *)that->other);
    free((void *)that->save);
    
    free(that);
}

struct TS_GameSystem *TS_LoadSystem(const char *system_folder_, const char *system_file_){
    assert(t5::IsDir(system_folder_));
    
    std::string system_folder = system_folder_;
    std::string system_file;
    
    if(system_folder.back()!='/'){
        system_folder.push_back('/');
    }
    
    if(system_file_==nullptr){
        
        
        std::array<std::string, 2> system_file_defaults = {{
            system_folder+"system.json",
            system_folder+"system.ini"
        }};
        
        auto i = system_file_defaults.cbegin();
        do{
            if(t5::IsFile(i->c_str())){
                system_file = *i;
                break;
            }
        }while(++i!=system_file_defaults.cend());
        
        // If we go past the end, we failed.
        if(i==system_file_defaults.cend()){
            return nullptr;
        }
        
    }
    else{
        assert(t5::IsFile(system_file_));
        system_file = system_file_;
    }
    
    struct TS_GameSystem *that = static_cast<struct TS_GameSystem *>(malloc(sizeof(struct TS_GameSystem)));
    
    std::unique_ptr<t5::DataSource>system_source(t5::DataSource::FromPath(t5::DataSource::eRead, system_file.c_str()));
    assert(system_source!=nullptr);
    
    t5::map::Group group(nullptr);
    group.ReadDataSource(system_source.get());
    
    that->fullscreen  = group.GetBool("fullscreen", false);
    that->compositing = group.GetBool("compositing", false);
    that->scale       = group.GetInt("scale", 1);
    
    that->system = strdup(system_folder.c_str());
    
    #define SYS_FOLDER(VAR, NAME)\
    that->system##VAR = strdup((system_folder + NAME).c_str())
    
    SYS_FOLDER(font, "system.rfn");
    SYS_FOLDER(ttffont, "DejaVuSans.ttf");
    SYS_FOLDER(windowstyle, "system.rws");
    SYS_FOLDER(arrow, "pointer.png");
    SYS_FOLDER(uparrow, "up_arrow.png");
    SYS_FOLDER(downarrow, "down_arrow.png");
    SYS_FOLDER(soundfont, "hollywood.sf2");
    
    SYS_FOLDER(script, "scripts");
    SYS_FOLDER(shader, "shaders");
    
    #undef SYS_FOLDER
    
    that->plugin = strdup("plugin");
    
    return that;
    
}

void TS_FreeSystem(struct TS_GameSystem *that){
    
    free((void *)that->systemfont);
    free((void *)that->systemttffont);
    free((void *)that->systemwindowstyle);
    free((void *)that->systemarrow);
    free((void *)that->systemuparrow);
    free((void *)that->systemdownarrow);
    free((void *)that->systemsoundfont);
    free((void *)that->systemscript);
    free((void *)that->systemshader);
    
    free(that);
}

struct TS_GameConfig *TS_LoadConfig(const char *path){
    assert(t5::IsFile(path));
    
    struct TS_GameConfig *that = static_cast<struct TS_GameConfig *>(malloc(sizeof(struct TS_GameConfig)));
    
    std::unique_ptr<t5::DataSource>system_source(t5::DataSource::FromPath(t5::DataSource::eRead, path));
    assert(system_source!=nullptr);
    
    t5::map::Group group(nullptr);
    group.ReadDataSource(system_source.get());
    
    that->gamefunc = "game";
    that->sgmname  = "game.sgm";
    
    that->gamename     = group.GetString("name");
    that->description  = group.GetString("description");
    that->author       = group.GetString("author");
    that->screenwidth  = group.GetInt("screen_width", 800);
    that->screenheight = group.GetInt("screen_height", 640);
    that->mainscript   = group.GetString("script");
    that->version      = group.GetInt("version", 1);
    
    if(that->mainscript==nullptr){
        free(that);
        return nullptr;
    }
    
    #define CPY_MEM_STR(VAR)\
    if(that->VAR==nullptr) {char *c = (char *)malloc(1); c[0] = '\0'; that->VAR = c;}\
    else that->VAR = strdup(that->VAR)
    
    CPY_MEM_STR(gamefunc);
    CPY_MEM_STR(sgmname);
    CPY_MEM_STR(gamename);
    CPY_MEM_STR(description);
    CPY_MEM_STR(author);
    CPY_MEM_STR(mainscript);
    
    return that;
    
    #undef CPY_MEM_STR
    
}

void TS_FreeConfig(struct TS_GameConfig *that){
    
    free((void *)that->mainscript);
    free((void *)that->gamefunc);
    free((void *)that->sgmname);
    free((void *)that->gamename);
    free((void *)that->author);
    
    free(that);
}

#define COMPONENT "Directory"
#define WRITE_COMPONENT(NAME)\
out->WriteF("[ConfigManager] " COMPONENT " " #NAME " - ", that->NAME, '\n');

void TS_PrintDirectories(struct TS_GameDirectories *that, void *c_file_handle){
    t5::DataSourcePusher *out = t5::DataSource::OneWayFromCFile<t5::DataSourcePusher>(c_file_handle);
    
    WRITE_COMPONENT(root);
    WRITE_COMPONENT(image);
    WRITE_COMPONENT(font);
    WRITE_COMPONENT(script);
    WRITE_COMPONENT(sound);
    WRITE_COMPONENT(map);
    WRITE_COMPONENT(other);
    WRITE_COMPONENT(save);
    WRITE_COMPONENT(spriteset);
    WRITE_COMPONENT(animation);
    WRITE_COMPONENT(windowstyle);
    WRITE_COMPONENT(soundfont);
    WRITE_COMPONENT(shader);
    
    delete out;
}

#undef COMPONENT
#define COMPONENT "System"
void TS_PrintSystem(struct TS_GameSystem *that, void *c_file_handle){
    t5::DataSourcePusher *out = t5::DataSource::OneWayFromCFile<t5::DataSourcePusher>(c_file_handle);
    
    WRITE_COMPONENT(fullscreen);
    WRITE_COMPONENT(compositing);
    WRITE_COMPONENT(scale);
    WRITE_COMPONENT(system);
    WRITE_COMPONENT(systemscript);
    WRITE_COMPONENT(systemshader);
    WRITE_COMPONENT(systemfont);
    WRITE_COMPONENT(systemttffont);
    WRITE_COMPONENT(systemwindowstyle);
    WRITE_COMPONENT(systemarrow);
    WRITE_COMPONENT(systemuparrow);
    WRITE_COMPONENT(systemdownarrow);
    WRITE_COMPONENT(systemsoundfont);
    WRITE_COMPONENT(plugin);

    delete out;
}

#undef COMPONENT
#define COMPONENT "Config"
void TS_PrintConfig(struct TS_GameConfig *that, void *c_file_handle){
    t5::DataSourcePusher *out = t5::DataSource::OneWayFromCFile<t5::DataSourcePusher>(c_file_handle);
    
    WRITE_COMPONENT(gamefunc);
    WRITE_COMPONENT(sgmname);
    WRITE_COMPONENT(gamename);
    WRITE_COMPONENT(mainscript);
    WRITE_COMPONENT(screenwidth);
    WRITE_COMPONENT(screenheight);
    WRITE_COMPONENT(author);
    WRITE_COMPONENT(description);
    WRITE_COMPONENT(version);
    
    delete out;
    
}
#undef COMPONENT
#undef WRITE_COMPONENT
