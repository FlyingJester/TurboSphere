#ifndef SAPPHIRE_GALILEO_SHAPE_HEAD
#define SAPPHIRE_GALILEO_SHAPE_HEAD

#include "../drawqueue.hpp"
#include "../sapphire.h"
#include "../primitives_script.hpp"
#include "../../common/plugin.h"

/////
// Galileo Shape object.
//
// If a shape is modified in script, two things happen. The shape is marked as
//  Dirty (IsDirty flag in a PolygonOp), and a synchro is needed between the
//  render thread and the engine thread.
//
// TODO: Work out how to avoid the synchro on modification. I can't think of a
//  way that does not involve intensive management, so a sunchro is likely the
//  simplest way, short of something...clever.
//
// Keep in mind that only the engine thread can mark a Shape dirty, and only
//  the render thread can mark a Shape clean.
//

namespace Sapphire{
    namespace Galileo {
        typedef PolygonOp Shape;
        void InitShape(uint32_t ID, v8::Isolate *isol);

        namespace Script{
            Turbo::JSFunction DrawShape(Turbo::JSArguments args);
            Turbo::JSFunction CreateShape(Turbo::JSArguments args);

            void GetShapeTexture(Turbo::JSAccessorProperty property, Turbo::JSAccessorGetterInfo info);
            void SetShapeTexture(Turbo::JSAccessorProperty property, Turbo::JSValue value, Turbo::JSAccessorSetterInfo info);
        }
    }
    extern Turbo::JSObj<Galileo::Shape> ShapeObject;
}

#endif
