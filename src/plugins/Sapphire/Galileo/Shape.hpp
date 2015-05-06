#pragma once

#include <vector>
#include <TSPR/concurrent_queue.h>
#include <algorithm>
#include <memory>
#include <color.h>
#include "Vertex.hpp"
#include "../Image.hpp"
#include "../Sapphire.hpp"

namespace Sapphire{
namespace Galileo {

class Shader;
class ShaderProgram;

/////
// ContainerClass is intended to be a std::array, std::list, or std::vector.
//
namespace GL {
    
  class Drawable;
    
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
      std::shared_ptr<ShaderProgram> mShader;
      bool mDirty;
  public:
      
      virtual ~Operation();
      Operation();
    
      // This will leave any buffers, images, or arrays for this object bound.
      // This is important mostly for the OpenGL 2/1.3 renderer.
      virtual int Draw() = 0;
      virtual bool IsPersistent(void){
          return true;
      }

      virtual void SetShader(std::shared_ptr<ShaderProgram> aShader){}

      inline void MarkDirty(){mDirty = true;}
      inline bool IsDirty(){return mDirty;}
      virtual bool EndsScene(){return false;}
      
      virtual Drawable *AsDrawable() { return nullptr; }
      
  };

  class Drawable : public Operation {
  public:
    typedef std::vector<Vertex>::const_iterator const_iterator;
    typedef std::vector<Vertex>::const_reverse_iterator const_reverse_iterator;

  protected:
      int gl_mode;

      void InitGL(void); //Initializes the OpenGL components.
      void CloseGL(void);//Deletes the OpenGL components.
      
      std::vector<Vertex> mVertices;
      unsigned mVertexArray;
      unsigned mBuffer[3];

      virtual void FillGL(void) = 0; //Fills the OpenGL components given the vertices.

      std::shared_ptr<Image> mImage;
      
      unsigned last_texture_set;
      
  public:

      Drawable(){
          last_texture_set = 0;
          InitGL();
      }
      Drawable(std::vector<Vertex> &aVertices){
          last_texture_set = 0;

          mVertices.resize(aVertices.size());

          std::copy(aVertices.begin(), aVertices.end(), mVertices.begin());

          InitGL();

      }

      Drawable(std::vector<Vertex> &aVertices, std::shared_ptr<Image> aImage)
        : mImage(aImage) {
          last_texture_set = 0;

          mVertices.resize(aVertices.size());

          std::copy(aVertices.begin(), aVertices.end(), mVertices.begin());

          InitGL();

      }

      Drawable(int a){
          last_texture_set = 0;
          mVertices.reserve(a);
      }

      virtual ~Drawable(){CloseGL();}

      virtual int Draw() = 0;
      
      int GetGLMode() const { return gl_mode; }
      
      virtual void SetShader(std::shared_ptr<ShaderProgram> aShader) = 0;
      // Checks if the specified shader contains the attribtues and
      // uniforms this drawable expects.
      virtual bool CanUse(ShaderProgram *aShader) = 0;

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

      template<class T>
      void ReplaceImage(const std::shared_ptr<T> im){
          
          if(last_texture_set==im->DebugGetTexture())
              return;
          
          mImage = im;
          last_texture_set = mImage->DebugGetTexture();
          
      }

      virtual std::shared_ptr<Image> GetImage() const {
          return mImage;
      }
    
      virtual Drawable *AsDrawable() final { return this; }
    
  };
}

class Shape : public GL::Drawable {
protected:
    int vertex_size;
public:
    Shape(std::vector<Vertex> &aVertices, const std::shared_ptr<Image> aImage);

    virtual ~Shape() {}
    bool CanUse(ShaderProgram *aShader) override;
    void SetShader(std::shared_ptr<ShaderProgram> aShader) override;

    void FillGL() override;

    int Draw() override;

};

}
}
