#ifndef SAPPHIRE_GALILEO_VERTEX_HEAD
#define SAPPHIRE_GALILEO_VERTEX_HEAD

#include <inttypes.h>
#include "../sapphire.h"

namespace Sapphire {

    namespace Galileo {

        class Vertex {
        public:
            int x;
            int y;
            uint32_t color;

        };

        void InitVertex(uint32_t ID, v8::Isolate *isol);
    }
    extern Turbo::JSObj<Galileo::Vertex> VertexObject;

}

#endif
