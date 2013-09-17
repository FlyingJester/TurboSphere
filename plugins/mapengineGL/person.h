#ifndef MAPENGINE_PR_HEAD
#define MAPENGINE_PR_HEAD

#include "typedef.h"

#include <vector>

extern std::vector<TS_Person*> personList;

struct TS_MEPlayerConfig {
    int MenuKey;
    int UpKey;
    int DownKey;
    int LeftKey;
    int RightKey;
    int AKey;
    int BKey;
    int XKey;
    int YKey;
};

struct TS_MEControlConfig {
    int numPlayers;
    TS_MEPlayerConfig *PlayerControls;
};

void InitMEControlConfig(void);

TS_MEControlConfig *GetMEControlConfig(void);

struct TS_Camera{
    TS_Camera(int, int);
    TS_Camera();
    int x;
    int y;
};

class TS_Person{
public:

    TS_Person(const char *n, TS_SpriteSet *ss, bool dWM);
    TS_Person(const char *name, int _x, int _y, int _baseX, int _baseY, int _baseWidth, int _baseHeight);
    TS_Person(const char *name, int _baseX, int _baseY, int _baseWidth, int _baseHeight);

    const char* name;

    int layer;

    int x;
    int y;

    uint32_t tick;

    TS_SpriteSet *spriteset;
    int moveIndex;
    int direction;

    bool destroyWithMap;
    void BaseDraw();
    std::vector<const char*> ignoring;

    int step;
};

void TS_AttachInput(const char *name);
void TS_AttachInput(int index);

int GetNumPersonsOnLayer(int l);
TS_Person **GetPersonsOnLayer(int l);

void TS_ClearTemporaryPersons();

struct TS_MEPersonConfig{
    TS_Person *controlPerson;
    TS_Person *cameraPerson;
};

v8Function CreatePerson(V8ARGS);

TS_MEPersonConfig *TS_GetPersonConfig(void);
#endif
