#ifndef SAPPHIRE_INIT_HEAD
#define SAPPHIRE_INIT_HEAD
#include "sapphire.h"
#include <stdint.h>
#include "../common/plugin.h"
#include <list>


namespace Sapphire {

    void Init(uint32_t ID);


    Turbo::ScriptObjectList<Turbo::JSCallback> *GetFunctions(void);
    Turbo::ScriptObjectList<Turbo::JSValue>    *GetVariables(void);

}


#endif
