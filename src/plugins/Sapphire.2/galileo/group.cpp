#include "group.hpp"

using namespace Sapphire;
Turbo::JSObj<Galileo::Group> Sapphire::GroupObject;

void Sapphire::Galileo::InitGroup(uint32_t ID, v8::Isolate *isol){
    Sapphire::GroupObject = Turbo::JSObj<Galileo::Group> ();



}
