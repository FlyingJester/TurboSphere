#ifndef SAPPHIRE_FLIPSCREEN_HEAD
#define SAPPHIRE_FLIPSCREEN_HEAD

#include "drawqueue.hpp"
#include "error.h"

#include "sync_object.hpp"

#include <SDL2/SDL.h>

namespace Sapphire{
/*
    class FlipScreenSynchro : public Sapphire::PollSynchro {
public:
        FlipScreenSynchro(){}
        virtual ~FlipScreenSynchro(){}

        // A function to call until the synchro has completed.
        virtual bool isDone(void);

        int retval;

    };
*/

    class UnthrottledDirectFlipScreenOp : public Sapphire::DrawingOp {
public:
        UnthrottledDirectFlipScreenOp(){}
        virtual ~UnthrottledDirectFlipScreenOp(){}
        virtual int call(void);

    };

    class UnthrottledCompositeFlipScreenOp : public Sapphire::DrawingOp {
public:
        UnthrottledCompositeFlipScreenOp(){}
        virtual ~UnthrottledCompositeFlipScreenOp(){}
        virtual int call(void);

    };

    class ThrottledDirectFlipScreenOp : public Sapphire::DrawingOp {
public:
        ThrottledDirectFlipScreenOp(){}
        virtual ~ThrottledDirectFlipScreenOp(){}
        virtual int call(void);

    };

    class ThrottledCompositeFlipScreenOp : public Sapphire::DrawingOp {
public:
        ThrottledCompositeFlipScreenOp(){}
        virtual ~ThrottledCompositeFlipScreenOp(){}
        virtual int call(void);

    };

    // Shorthand.

    namespace Throttled{
        namespace Direct {
            typedef ThrottledDirectFlipScreenOp FlipScreenOp;
        }
        namespace Composite {
            typedef ThrottledCompositeFlipScreenOp FlipScreenOp;
        }
    }
    namespace Unthrottled{
        namespace Direct {
            typedef UnthrottledDirectFlipScreenOp FlipScreenOp;
        }
        namespace Composite {
            typedef UnthrottledCompositeFlipScreenOp FlipScreenOp;
        }
    }

    class FrameRateOp : public Sapphire::DrawingOp {
        FrameRateOp(){}
        virtual ~FrameRateOp(){}
        virtual int call(void);
    };

    void SetWindowToFlip(SDL_Window *window);

    // Get/Set the frame throttling. Setting to 0 disables throttling.
    void     SetFrameRate(unsigned fps);
    unsigned GetFrameRate(void);

    // Get/Set whether the engine will try to compensate for periods of lag by
    // increasing the framerate for a short while afterwards, to make the
    // average framerate equal to what has been set.
    void     SetFrameRateComeback(bool);
    bool     SetFrameRateComeback(void);

    // Get/Set the max difference between the engine and render frames.
    void     SetFrameFloat(unsigned f);
    unsigned GetFrameFloat(void);

    void FlipScreen(void);

    namespace Script{
        void FlipScreen(Turbo::JSArguments args);
    }

}

#endif
