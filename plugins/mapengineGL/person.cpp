#include "person.h"
#include "spriteset.h"
#include "mapengine.h"

#define DEFAULT_NUM_PLAYERS 4

std::vector<TS_Person*> personList;

TS_Camera::TS_Camera(int _x, int _y){
    x = _x;
    y = _y;
}

TS_Person::TS_Person(const char *n, TS_SpriteSet *ss, bool dWM){

    x = 0;
    y = 0;

    spriteset = ss;
    name = n;
    destroyWithMap = dWM;

    tick = 0;

    layer = 0;

}


inline void TS_DestroyPerson(int at){
    personList.erase(personList.begin()+at);
}

void TS_CreatePerson(const char *name, TS_SpriteSet *spriteset, bool destroyWithMap){
    TS_Person *ss = new TS_Person(name, spriteset, destroyWithMap);
    personList.push_back(ss);
}

void TS_Person::BaseDraw(){

}

void TS_ClearTemporaryPersons(){
    for(size_t i = personList.size()-1; i>=0; i--){
        if(personList[i]->destroyWithMap){
            TS_DestroyPerson(i);
        }
    }
}

v8Function DestroyPerson(V8ARGS){
    if(args.Length()<1){
        THROWERROR(BRACKNAME "DestroyPerson Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);

    return v8::Undefined();
}

void InitMEControlConfig(){
    TS_MEControlConfig *config = GetMEControlConfig();
    //TEST VALUES!
    config->numPlayers = DEFAULT_NUM_PLAYERS;
    config->PlayerControls = new TS_MEPlayerConfig[DEFAULT_NUM_PLAYERS];

    config->PlayerControls[0].UpKey    = SDLK_UP;
    config->PlayerControls[0].DownKey  = SDLK_DOWN;
    config->PlayerControls[0].LeftKey  = SDLK_LEFT;
    config->PlayerControls[0].RightKey = SDLK_RIGHT;

    config->PlayerControls[0].AKey = SDLK_SPACE;
    config->PlayerControls[0].BKey = SDLK_LSHIFT;
    config->PlayerControls[0].XKey = SDLK_z;
    config->PlayerControls[0].YKey = SDLK_x;

    int k = 96;

    for(int i = 1; i<DEFAULT_NUM_PLAYERS; i++){
        config->PlayerControls[i].UpKey    = k++;
        config->PlayerControls[i].DownKey  = k++;
        config->PlayerControls[i].LeftKey  = k++;
        config->PlayerControls[i].RightKey = k++;

        config->PlayerControls[i].AKey = k++;
        config->PlayerControls[i].BKey = k++;
        config->PlayerControls[i].XKey = k++;
        config->PlayerControls[i].YKey = k++;
    }

}

TS_MEPersonConfig *TS_GetPersonConfig(void){
    static TS_MEPersonConfig config;
    return &config;
}

void CloseMEControlConfig(){
    TS_MEControlConfig *config = GetMEControlConfig();
    delete[] config->PlayerControls;

}

TS_MEControlConfig *GetMEControlConfig(){
    static TS_MEControlConfig config;
    return &config;
}

void TS_AttachInput(const char *name){
    for(size_t i = 0; i<personList.size(); i++){
        if((name==personList[i]->name)||strcmp(personList[i]->name, name)==0){
            TS_AttachInput((int)i);
            return;
        }
    }
}

void TS_AttachInput(int index){
    if((index>0)&&(index<personList.size())){
        TS_GetPersonConfig()->controlPerson = personList[index];
    }
}

int GetNumPersonsOnLayer(int l){
    int num = 0;
    for(int i = 0; i<personList.size(); i++){
        if(personList[i]->layer==l) num++;
    }
    return num;
}

TS_Person **GetPersonsOnLayer(int l){
    TS_Person **tempPersonList = (TS_Person**)calloc(GetNumPersonsOnLayer(l), sizeof(TS_Person*));
    int num = 0;
    for(int i = 0; i<personList.size(); i++){
        if(personList[i]->layer==l){
        tempPersonList[num] = personList[i];
        num++;
        }
    }
    return tempPersonList;
}
