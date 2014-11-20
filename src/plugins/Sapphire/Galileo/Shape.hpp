#pragma once

#include "Vertex.hpp"
#include <vector>
#include <algorithm>
#include <color.h>
#include "../Image.hpp"
#include <v8.h>
#include <memory>

namespace Sapphire{
namespace Galileo {

class Shader;

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
      bool mDirty;
  public:

      virtual ~Operation();
      Operation();

      virtual int Draw() = 0;
      virtual bool IsPersistent(void){
          return true;
      }


      virtual void SetShader(Shader *aShader){}

      inline void MarkDirty(){mDirty = true;}
      inline bool IsDirty(){return mDirty;}
      virtual bool EndsScene(){return false;}

  };

  class Drawable : public Operation {
  public:
    typedef std::vector<Vertex>::const_iterator const_iterator;
    typedef std::vector<Vertex>::const_reverse_iterator const_reverse_iterator;

  protected:

      void InitGL(void); //Initializes the OpenGL components.
      void CloseGL(void);//Deletes the OpenGL components.

      std::vector<Vertex> mVertices;
      unsigned mVertexArray;
      unsigned mBuffer[3];

      virtual void FillGL(void) = 0; //Fills the OpenGL components given the vertices.

      std::shared_ptr<Image> mImage;
  public:

      Drawable()
      {InitGL();}
      Drawable(std::vector<Vertex> &aVertices){

          mVertices.resize(aVertices.size());

          std::copy(aVertices.begin(), aVertices.end(), mVertices.begin());

          InitGL();

      }

      Drawable(std::vector<Vertex> &aVertices, Image *aImage)
        : mImage(aImage) {

          mVertices.resize(aVertices.size());

          std::copy(aVertices.begin(), aVertices.end(), mVertices.begin());

          InitGL();

      }

      Drawable(int a)
      {mVertices.reserve(a);}

      virtual ~Drawable(){CloseGL();}

      virtual int Draw() = 0;

      virtual void SetShader(Shader *aShader) = 0;
      // Checks if the specified shader contains the attribtues and
      // uniforms this drawable expects.
      virtual bool CanUse(Shader *aShader) = 0;

      virtual void BindBuffer();
      virtual void BindArray();
      virtual void Bind() {BindBuffer(); BindArray(); mImage->Bind();}

      virtual inline const_iterator begin() const{
          return mVertices.begin();
      }

      virtual inline const_iterator end() const{
          return mVertices.end();
      }

      virtual inline const_reverse_iterator rbegin() const{
          return mVertices.rbegin();
      }

      virtual inline const_reverse_iterator rend() const{
          return mVertices.rend();
      }

      virtual inline size_t size() const{
          return mVertices.size();
      }

      virtual inline void push(const Vertex &aToPush){
          mVertices.push_back(aToPush);
          MarkDirty();
      }

      inline bool empty(void) const {
          return mVertices.empty();
      }

      virtual void ReplaceImage(Image *im){
          mImage.reset(im);
      }

      virtual Image *GetImage() const {
          return mImage.get();
      }

  };
}

class Shape : public GL::Drawable {
protected:

public:
    Shape(std::vector<Vertex> &aVertices, Image *aImage);

    virtual ~Shape() {}
    virtual bool CanUse(Shader *aShader);
    virtual void SetShader(Shader *aShader);

    virtual void FillGL();

    virtual int Draw();

};

}
}
