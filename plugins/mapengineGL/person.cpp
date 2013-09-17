#include "person.h"
#include "spriteset.h"
#include "mapengine.h"

#define DEFAULT_NUM_PLAYERS 4

std::vector<TS_Person*> personList;

TS_Camera::TS_Camera(int _x, int _y){
    x = _x;
    y = _y;
}

TS_Camera::TS_Camera(void){
    x = 0;
    y = 0;
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
    printf("We are drawing at %i, %i\n", x, y);
    /*
    const GLint   texcoordData[] = {0, 0, 1, 0, 1, 1, 0, 1};
    const GLuint  colorData[]    = {
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF
    };
    glTexCoordPointer(2, GL_INT, 0, texcoordData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    */
    const GLint   vertexData[]   = {x, y,
                                    x+spriteset->textureWidth, y,
                                    x+spriteset->textureWidth, y+spriteset->textureHeight,
                                    x, y+spriteset->textureHeight};

                glVertexPointer(2, GL_INT, 0, vertexData);
                glBindTexture(GL_TEXTURE_2D, spriteset->textures[0]);
                /*glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_COLOR_ARRAY);*/
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                /*glDisableClientState(GL_COLOR_ARRAY);
                glDisableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/
}

void TS_ClearTemporaryPersons(){
    for(size_t i = personList.size()-1; i>=0; i--){
        if(personList[i]->destroyWithMap){
            TS_DestroyPerson(i);
        }
    }
}


v8Function CreatePerson(V8ARGS){
    if(args.Length()<1){
        THROWERROR(BRACKNAME "DestroyPerson Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);
    CHECK_ARG_STR(1);
    CHECK_ARG_BOOL(2);

    v8::String::Utf8Value name(args[0]);
    v8::String::Utf8Value sset(args[1]);
    bool destroyWithMap = args[2]->BooleanValue();

    TS_Directories *dirs = GetDirs();
    TS_CreatePerson(*name, new TS_SpriteSet(string(dirs->spriteset).append(*sset).c_str()), destroyWithMap);

    return v8::Undefined();
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
