#ifndef SAPPHIRE_API_HEAD
#define SAPPHIRE_API_HEAD

#include "frontend.h"

#include "../common/plugin.h"
#include "../../graphiccommon/color.h"

    /////
    // API listing. A function is declared here if and only if it is exposed to script.
    //
    // Includes stubbed functions.
    //

namespace Script {

    /////
    // Adding functions to Sapphire:
    //
    // If the function is already in api.h but not implemented, remove the stub from ``stub.cpp''.
    //
    // Otherwise, add a forward declaration to api.h, add a pointer and JS name to frontend.cpp.



    /////
    // Video

    Turbo::JSFunction FlipScreen(       Turbo::JSArguments);
    Turbo::JSFunction GetFrameRate(     Turbo::JSArguments);
    Turbo::JSFunction SetFrameRate(     Turbo::JSArguments);

//  Defined as embedded script.
//    Turbo::JSFunction SetFrameRate(     Turbo::JSArguments);

    /////
    // Screen Size

    Turbo::JSFunction GetScreenWidth(   Turbo::JSArguments);
    Turbo::JSFunction GetScreenHeight(  Turbo::JSArguments);

    /////
    // Clipping Rectangle

    Turbo::JSFunction GetClippingRectangle(Turbo::JSArguments);
    Turbo::JSFunction SetClippingRectangle(Turbo::JSArguments);

    /////
    // Primititves

    Turbo::JSFunction Point(Turbo::JSArguments);
    Turbo::JSFunction PointSeries(Turbo::JSArguments);

    Turbo::JSFunction StippleLine(Turbo::JSArguments);
    Turbo::JSFunction Line(Turbo::JSArguments);
    Turbo::JSFunction LineSeries(Turbo::JSArguments);
    Turbo::JSFunction GradientLine(Turbo::JSArguments);

    Turbo::JSFunction BezierCurve(Turbo::JSArguments);

    Turbo::JSFunction Polygon(Turbo::JSArguments);

    Turbo::JSFunction Triangle(Turbo::JSArguments);
    Turbo::JSFunction GradientTriangle(Turbo::JSArguments);
    Turbo::JSFunction OutlinedTriangle(Turbo::JSArguments);

    Turbo::JSFunction Rectangle(Turbo::JSArguments);
    Turbo::JSFunction GradientRectangle(Turbo::JSArguments);
    Turbo::JSFunction OutlinedRectangle(Turbo::JSArguments);

//  Defined as embedded scripts.
//    Turbo::JSFunction FilledCircle(Turbo::JSArguments);
//    Turbo::JSFunction OutlinedCircle(Turbo::JSArguments);
//    Turbo::JSFunction GradientCircle(Turbo::JSArguments);

    Turbo::JSFunction FilledEllipse(Turbo::JSArguments);
    Turbo::JSFunction OutlinedEllipse(Turbo::JSArguments);
    Turbo::JSFunction GradientEllipse(Turbo::JSArguments);

    Turbo::JSFunction FilledComplex(Turbo::JSArguments);
    Turbo::JSFunction OutlinedComplex(Turbo::JSArguments);
    Turbo::JSFunction GradientComplex(Turbo::JSArguments);

    /////
    // Colors

    extern Turbo::JSObj<TS_Color>       ColorObject;

    Turbo::JSFunction CreateColor(Turbo::JSArguments);

    /////
    // Color Matrices
    //
    // I don't really understand these. I need to read up on this kind of thing.
    //

    Turbo::JSFunction CreateColorMatrix(Turbo::JSArguments);

    /////
    // Images

    // Creation
    Turbo::JSFunction LoadImage(Turbo::JSArguments);
    Turbo::JSFunction CreateImage(Turbo::JSArguments);
    Turbo::JSFunction GrabImage(Turbo::JSArguments);

    // System images
    Turbo::JSFunction GetSystemArrow(Turbo::JSArguments);
    Turbo::JSFunction GetSystemUpArrow(Turbo::JSArguments);
    Turbo::JSFunction GetSystemDownArrow(Turbo::JSArguments);

    /////
    // Surfaces

    Turbo::JSFunction LoadSurface(Turbo::JSArguments);
    Turbo::JSFunction CreateSurface(Turbo::JSArguments);
    Turbo::JSFunction GrabSurface(Turbo::JSArguments);

    /////
    // Animations

    Turbo::JSFunction LoadAnimation(Turbo::JSArguments);

    /////
    // Shaders

    Turbo::JSFunction CreateFragShader(Turbo::JSArguments);
    Turbo::JSFunction CreateVertShader(Turbo::JSArguments);
    Turbo::JSFunction CreateProgram(Turbo::JSArguments);
    Turbo::JSFunction LoadProgram(Turbo::JSArguments);

    Turbo::JSFunction UseProgram(Turbo::JSArguments);
    Turbo::JSFunction UseCompositeProgram(Turbo::JSArguments);

    Turbo::JSFunction GetSystemProgram(Turbo::JSArguments);
    Turbo::JSFunction GetSystemCompositeProgram(Turbo::JSArguments);
}

#endif
