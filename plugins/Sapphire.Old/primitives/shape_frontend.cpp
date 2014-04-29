#include "shape.h"
#include "../frontend.h"
#include "../../common/plugin.h"

Turbo::JSObj<TS_Shape> ShapeObject       = Turbo::JSObj<TS_Shape>();

void ShapeInit(void){

    ShapeObject.SetTypeName("Sound");

    //ShapeObject.AddToProto("vertices", ShapeGetVertices);
}
