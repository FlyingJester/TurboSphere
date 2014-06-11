#ifndef SAPPHIRE_COLOR_HEAD
#define SAPPHIRE_COLOR_HEAD

#include "../../../graphiccommon/color.h"
#include "../../common/plugin.h"

extern Turbo::JSObj<TS_Color> ColorObject;
void TS_ColorFinalizer(const v8::WeakCallbackData<v8::Object, TS_Color> &args);

void InitColor(uint32_t ID);

namespace Sapphire{

    namespace Color{

        typedef unsigned char ColorComponent;

        template<typename T>
        inline ColorComponent CleanseColorComponent(T in){
            T out = in;
            if(out>0xFF)
                out = 0xFF;
            else if(out<0)
                out = 0;
            return out;

        }
    }
}

#endif
