#include "../package_spk.h"
#include "../t5.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cassert>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Tabs.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Slider.H>

bool SPK_FLTK_Assert(bool exp, const char * desc){
    if(!exp){
        fl_alert(desc);
        fprintf(stderr, "SPK_FLTK Assertion failed: %s\n", desc);
    }
    return exp;
}

struct GFX {
    uint32_t windowWidth;
    uint32_t windowHeight;
    char * windowTitle;
    uint32_t padding;
};

struct SPK_Configuration {
    int unpack;
    char *SPK;
    char *UnpackPath;
};

void ListContents(SPK_Configuration *conf){
    assert(conf);
    if((!SPK_FLTK_Assert(conf->SPK,         "You Must Select an Archive Name"))||
       (!SPK_FLTK_Assert(strlen(conf->SPK), "You Must Select an Archive Name")))
        return;
    int num_files = 0;
    const char **files = SPK_GetFileList(conf->SPK, &num_files);

    char *str = NULL;
    size_t l = 1;
    size_t ol = 1;
    for(int i = 0; i<num_files; i++){
        ol = l;
        l+=strlen(files[i]);
        str = (char *)realloc(str, l);
        memcpy(str+ol-1, files[i], l-ol);
        free((void *)files[i]);
        str[l-1] = '\n';
        l++;
    }
    str[l-2] = '\0';
    fl_alert("Files in %s:\n%s", conf->SPK, str);
    free(str);
    free(files);
}

void SPK_ListFiles_fltk(Fl_Widget *w, void *args){
    ListContents((SPK_Configuration *)args);
}

SPK_Configuration *Config;

void SPK_Unpack_fltk(Fl_Widget *w, void *args){
    SPK_Configuration *conf = (SPK_Configuration *)args;
    assert(conf->unpack);
    if((!SPK_FLTK_Assert(conf->SPK,         "You Must Select an Archive Name"))||
       (!SPK_FLTK_Assert(strlen(conf->SPK), "You Must Select an Archive Name"))||
       (!SPK_FLTK_Assert(conf->UnpackPath,  "You Must Select an Unpack Directory"))||
       (!SPK_FLTK_Assert(strlen(conf->UnpackPath), "You Must Select an Unpack Directory")))
        return;
    //int Error = TS_UnpackSPK1(conf->SPK, conf->UnpackPath);
    TS_SPK SPK;


    if(!T5_IsDir(conf->UnpackPath))
        TS_CreateDirectory(conf->UnpackPath);

    int Error = SPK_OpenSPK(SPK, conf->SPK);
    Error|= TS_UnpackSPK(SPK, conf->UnpackPath);

    if(!Error)
        fl_alert("Successfully Unpacked %s to %s", conf->SPK, conf->UnpackPath);
    SPK_FreeSPK(SPK);

}

void FileChooser(Fl_Widget *widget, void *text){
    Fl_Input *filefield = (Fl_Input *)text;
    char *file = fl_file_chooser("Pick a File", filefield->value(), "", 1);

    if(file==NULL){
        return;
    }

    filefield->value(file);

    filefield->do_callback();

}

void DirectoryChooser(Fl_Widget *widget, void *text){
    Fl_Input *dirfield = (Fl_Input *)text;
    char *dir = fl_dir_chooser("Pick a Directory", dirfield->value(), 1);

    if(dir==NULL){
        return;
    }

    dirfield->value(dir);

    dirfield->do_callback();

}

void inCallback(Fl_Widget *widget, void *args){
    SPK_Configuration *conf = (SPK_Configuration *)args;
    Fl_Input *inp = (Fl_Input *)widget;
    if(conf->SPK!=NULL)
        free(conf->SPK);
    conf->SPK = strdup(inp->value());
}

void outCallback(Fl_Widget *widget, void *args){
    assert(args);
    SPK_Configuration *conf = (SPK_Configuration *)args;
    Fl_Input *inp = (Fl_Input *)widget;
    conf->UnpackPath = strdup(inp->value());
}

int main(int argc, char *argv[]){

    GFX DefaultGFX = {600, 260, (char *)"TurboSphere SPK Archiver", 32};

    Config = (SPK_Configuration *)malloc(sizeof(SPK_Configuration));
    Config->SPK          = NULL;
    Config->UnpackPath   = NULL;

    if((argc>1)&&(TS_IsSPK(argv[1]))){
        Config->unpack = 1;
        Config->SPK = strdup(argv[1]);
        char *dest = (char *)malloc(strlen(argv[1])+1);
        size_t len = (size_t)strrchr(argv[1], '.');
        if(len)
            len -=(size_t)argv[1];
        else
            len = strlen(argv[1]);

        memcpy(dest, argv[1], len);

        dest[len] = '\0';
        Config->UnpackPath = dest;
    }
    else{
        Config->unpack = 1;
    }

    Fl_Window spkWindow(DefaultGFX.windowWidth, DefaultGFX.windowHeight, DefaultGFX.windowTitle);
    spkWindow.begin();

        Fl_Input    inText   (DefaultGFX.padding+80, DefaultGFX.padding+20, 180, 20, "SPK File");
        inText.when(FL_WHEN_CHANGED);
        inText.callback(inCallback, Config);
        if(Config->SPK)
            inText.value(Config->SPK);
        Fl_Button   chooseIn (DefaultGFX.padding+280, DefaultGFX.padding+20, 80, 20, "...");
        chooseIn.callback(FileChooser, &inText);
        Fl_Input    outText  (DefaultGFX.padding+80, DefaultGFX.padding+40+DefaultGFX.padding, 180, 20, "Directory");
        outText.when(FL_WHEN_CHANGED);
        outText.callback(outCallback, Config);
        if(Config->UnpackPath)
            outText.value(Config->UnpackPath);
        Fl_Button   chooseOut(DefaultGFX.padding+280, DefaultGFX.padding+40+DefaultGFX.padding, 80, 20, "...");
        chooseOut.callback(DirectoryChooser, &outText);


        Fl_Tabs tabs(DefaultGFX.padding+350+DefaultGFX.padding, DefaultGFX.padding-5, 160, 190);
        tabs.begin();
            Fl_Group unpackGroup(DefaultGFX.padding+350+DefaultGFX.padding, DefaultGFX.padding+20, 160, 160, "Unpack");
            unpackGroup.begin();
                Fl_Button   unpack(DefaultGFX.padding+370+DefaultGFX.padding, DefaultGFX.padding+40, 120, 20, "Unpack");
                unpack.callback(SPK_Unpack_fltk, Config);
                Fl_Button   list(DefaultGFX.padding+370+DefaultGFX.padding, DefaultGFX.padding+DefaultGFX.padding+40, 120, 20, "List Contents");
                list.callback(SPK_ListFiles_fltk, Config);
                Fl_Button   info(DefaultGFX.padding+370+DefaultGFX.padding, DefaultGFX.padding+DefaultGFX.padding+DefaultGFX.padding+40, 120, 20, "Game Info");
            unpackGroup.end();
            Fl_Group packGroup(DefaultGFX.padding+350+DefaultGFX.padding, DefaultGFX.padding+20, 160, 160, "Pack");
            packGroup.begin();
                Fl_Button   pack(DefaultGFX.padding+370+DefaultGFX.padding, DefaultGFX.padding+40, 120, 20, "Pack");
            packGroup.end();

        tabs.end();


    spkWindow.end();

    spkWindow.show();

    return Fl::run();

}
