#pragma once

#include "Vertex.hpp"
#include "Shader.hpp"
#include <vector>
#include <algorithm>
#include <color.h>

namespace Sapphire{
namespace Galileo {

/////
// ContainerClass is intended to be a std::array, std::list, or std::vector.
//
namespace GL {

  union VertexU {
    float f[8];
    struct {
      float x;
      float y;
      float u;
      float v;
      float color[4];
    } s;
  };

  class Operation {

  protected:
      Shader *mShader;
  public:


      virtual void Draw() = 0;
      virtual void SetShader(Shader *aShader);

  };

  class Drawable : public Operation {

      void InitGL(void); //Initializes the OpenGL components.
      void CloseGL(void);//Deletes the OpenGL components.

  protected:
      std::vector<Vertex> mVertices;
      unsigned mVertexArray;
      unsigned mBuffer;

      // Extra preparation of OpenGL state, such as teture binding.
      virtual void PrepareForDraw() = 0;

      virtual void FillGL(void) = 0; //Fills the OpenGL components given the vertices.

  public:

      Drawable()
      {InitGL();}
      Drawable(std::vector<Vertex> &aVertices){

          mVertices.resize(aVertices.size());

          std::copy(aVertices.begin(), aVertices.end(), mVertices.begin());

          InitGL();

      }
      virtual ~Drawable(){CloseGL();}

      virtual void Draw() = 0;

      virtual void SetShader(Shader *aShader);
      // Checks if the specified shader contains the attribtues and
      // uniforms this drawable expects.
      virtual bool CanUse(Shader *aShader) = 0;

      virtual void BindBuffer();
      virtual void BindArray();
      virtual void Bind() {BindBuffer(); BindArray();}
  };
}

class Shape : public GL::Drawable {
protected:

public:

    virtual bool CanUse(Shader *aShader);
    virtual void SetShader(Shader *aShader);

    virtual void FillGL();

    virtual void Draw();

};

}
}
