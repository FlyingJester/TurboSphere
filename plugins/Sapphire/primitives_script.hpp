#ifndef SAPPHIRE_PRIMITIVE_SCRIPT_HEAD
#define SAPPHIRE_PRIMITIVE_SCRIPT_HEAD

#ifndef __cplusplus
    #error This file is the bridge between V8 and the concurrent draw queue. Using it in C is a categorical mistake.
#endif

#include "sapphire.h"
#include "fjgl.h"
#include "drawqueue.hpp"
#include "../common/plugin.h"
#include "glextra/gl_extra.h"
#include "cmath"

namespace Sapphire {
    namespace Script {
        void Point(Turbo::JSArguments);
        void GradientLine(Turbo::JSArguments);
        void GradientTriangle(Turbo::JSArguments);
    }
/////
// DrawQueue wrapper types

    template<size_t n, GLenum mode=GL_POLYGON>
    class PrimitiveOp : public DrawingOp {
    public:
        PrimitiveOp(const PrimitiveOp&);
        PrimitiveOp(int x[n], int y[n], RGBA color[n], GLuint tex){
            glGenBuffers(3, buffers);
            glGenVertexArrays(1, &Array);

            float coords[n<<1];
            float texcoords[n<<1];
            GLfloat colors[n<<2];

            for(int i = 0; i<n; i++){
                coords[(i<<1)  ] = x[i];
                coords[(i<<1)+1] = y[i];

                colors[(i<<2)  ] = ( color[i] >> 24 ) & 0xFF;
                colors[(i<<2)+1] = ( color[i] >> 16 ) & 0xFF;
                colors[(i<<2)+2] = ( color[i] >> 8  ) & 0xFF;
                colors[(i<<2)+3] = ( color[i]       ) & 0xFF;

                colors[(i<<2)  ] /= 255.0f;
                colors[(i<<2)+1] /= 255.0f;
                colors[(i<<2)+2] /= 255.0f;
                colors[(i<<2)+3] /= 255.0f;

            }

            PrepareTexcoord(texcoords);

            glBindBuffer(GL_ARRAY_BUFFER, GetVertex());
            glBufferData(GL_ARRAY_BUFFER, n<<3, coords, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, GetTexcoord());
            glBufferData(GL_ARRAY_BUFFER, n<<3, texcoords, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, GetColor());
            glBufferData(GL_ARRAY_BUFFER, n<<4, colors, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

        }
        virtual ~PrimitiveOp(){
            glDeleteBuffers(3, buffers);
            glDeleteVertexArrays(1, &Array);
        }

        virtual void SetVertexAttribBinding(uint32_t vertex, uint32_t texcoord, uint32_t color){
            glBindVertexArray(Array);
            glBindBuffer(GL_ARRAY_BUFFER, GetVertex());
            glVertexAttribPointer(vertex,   2, GL_FLOAT, GL_FALSE, 0, NULL);
            glBindBuffer(GL_ARRAY_BUFFER, GetTexcoord());
            glVertexAttribPointer(texcoord, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glBindBuffer(GL_ARRAY_BUFFER, GetColor());
            glVertexAttribPointer(color,    4, GL_FLOAT, GL_FALSE, 0, NULL);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        //The vertex attributes for shaders must have been enabled by the owner group.
        virtual int call(void){
            glBindVertexArray(Array);
            glBindTexture(GL_TEXTURE_2D, texture);
            glDrawArrays(mode, 0, n);
            glBindVertexArray(0);
            return n;
        }

        virtual void PrepareTexcoord(float out[n<<1]){
                float fi = 0.0f;
                float fn = n;
                for(int i = 0; i<n; i++, fi++){
                    out[(i<<1)  ] = TexCoordU(fi/fn);
                    out[(i<<1)+1] = TexCoordV(fi/fn);
                }
        }

    private:

        inline float TexCoordU(float at){
            if(at<=0.25f)
                return at*4.0f;
            if((at>0.25f)&&(at<=0.5f))
                return 1.0f;
            if((at>0.5f)&&(at<=0.75f))
                return (0.25f-(at-0.5f))*4.0f;
            if(at>0.75f)
                return 0.0f;

            return 2.0f;
        }

        inline float TexCoordV(float at){
            if(at<=0.25f)
                return 0.0f;
            if((at>0.25f)&&(at<=0.5f))
                return (at-0.25f)*4.0f;
            if((at>0.5f)&&(at<=0.75f))
                return 1.0f;
            if(at>0.75f)
                return (0.25f-(at-0.75f))*4.0f;

            return 2.0f;
        }

    protected:

        GLuint texture;

        inline GLuint &GetVertex   (void){return buffers[0];}
        inline GLuint &GetTexcoord (void){return buffers[1];}
        inline GLuint &GetColor    (void){return buffers[2];}
        GLuint buffers[3];

        int x[n];
        int y[n];
        RGBA color[n];
        GLuint Array;
    };

    class PointOp : public PrimitiveOp<1, GL_POINTS> {
    public:
        PointOp(int x, int y, RGBA color, GLuint tex) : PrimitiveOp(&x, &y, &color, tex){}
        PointOp(int *x, int *y, RGBA *color, GLuint tex) : PrimitiveOp(x, y, color, tex){}
        virtual void PrepareTexcoord(float out[2]){
            out[0] = 0.0f;
            out[1] = 0.0f;
        }
        ~PointOp();
        virtual int call(void);
    };

    class LineOp : public PrimitiveOp<2, GL_LINE_LOOP> {
    public:
        LineOp(int x[2], int y[2], RGBA color[2], GLuint tex) : PrimitiveOp(x, y, color, tex){}
        virtual void PrepareTexcoord(float out[4]){
            out[0] = 0.0f;
            out[1] = 0.0f;
            out[2] = 1.0f;
            out[3] = 0.0f;
        }
        ~LineOp();
        virtual int call(void);
    };

    class TriangleOp : public PrimitiveOp<3, GL_TRIANGLE_FAN> {
    public:
        TriangleOp(int x[3], int y[3], RGBA color[3], GLuint tex) : PrimitiveOp(x, y, color, tex){}
        virtual void PrepareTexcoord(float out[6]){
            out[0] = 0.0f;
            out[1] = 0.0f;
            out[2] = 1.0f;
            out[3] = 0.0f;
            out[4] = 1.0f;
            out[5] = 1.0f;
        }
        ~TriangleOp();
        virtual int call(void);
    };

}



#endif
