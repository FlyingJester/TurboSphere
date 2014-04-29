#ifndef SAPPHIRE_GALILEO_GROUP_HEAD
#define SAPPHIRE_GALILEO_GROUP_HEAD

#include <vector>
#include <utility>
#include "shape.hpp"

namespace Sapphire {

    class Shader;

    namespace Galileo {

        typedef unique_ptr<Shape> shapeptr;

        class Group {
        public:
            Group(){
                ResetOffset();
                ResetRotationOffset();
                ResetRotation();
            }
            ~Group(){

            }

            inline void ResetOffset(void){
                offsetX = 0;
                offsetY = 0;
            }
            inline void ResetRotationOffset(void){
                rotationX = 0;
                rotationY = 0;
            }
            inline void ResetRotation(void){
                rotation = 0.0f;
            }

            inline void push_back(Shape *shape){
                Shapes.push_back(shapeptr(shape));
            }
            inline void push_back(shapeptr shape){
                Shapes.push_back(move(shape));
            }

        private:

            Shader *shader;

            float rotation;

            int rotationX;
            int rotationY;
            int offsetX;
            int offsetY;
            std::vector<shapeptr>Shapes;
        }

    }

}

#endif
