/**<!-------------------------------------------------------------------->
   @class  GLState
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Summer 2008
   
   @brief
      Utility wrapper classes for accessing and synchronizing OpenGL state
   in a cleaner, more object-oriented manner, grouping similar state items 
   with each other.
   
   @note
      Where applicable, acceptable parameters are listed along with default 
   values marked between '<' and '>'. For example, the frontFace property of
   GLPrimitiveStateItem is preceded by the following comment:
      /// glFrontFace; GL_CW or <GL_CCW>
   Where GL_CCW is the OpenGL-defined default value for this property.
   <!-------------------------------------------------------------------->**/

#ifndef GL_STATE_H_
#define GL_STATE_H_

#include <utils/IFeatures.h>
#include <common/common.h>

#include <GL/gl.h>
#include <map>

#define GL_CHECK_ERROR() GLState::glCheckError(__FILE__, __LINE__)

namespace milton {

union MILTON_DLL_EXPORT GLcolor {
   GLclampf data[4];
   
   struct {
      GLclampf r;
      GLclampf g;
      GLclampf b;
      GLclampf a;
   };
};

class MILTON_DLL_EXPORT GLState {
   public:
      /**
       * @returns the next lowest light index which is not currently enabled, 
       *    or -1 if all light slots within the current GL context are taken
       */
      static int getFreeLight();
      
      static bool glCheckError(const char *file, int line);
      
   protected:
#if 0
      GLBlendStateItem     m_blendState;
      GLDepthStateItem     m_depthState;
      GLStencilStateItem   m_stencilState;
      GLPrimitiveStateItem m_primitiveState;
      GLAlphaStateItem     m_alphaState;
      GLColorStateItem     m_colorState;
      GLFogStateItem       m_fogState;
      GLPointStateItem     m_pointState;
#endif
};

class GLStateItem : public IFeatures {
   public:
      virtual ~GLStateItem()
      { }
      
      //template <typename T>
      //T get(const std::string &name) = 0;
      
      virtual bool sync()  = 0;
      virtual bool apply() = 0;
};

class GLBlendStateItem : public GLStateItem {
   public:
      virtual ~GLBlendStateItem()
      { }
      
      virtual bool sync();
      virtual bool apply();
      
      ///@name State variables
      //@{-----------------------------------------------------------------
      
      /// glEnable; GL_TRUE or <GL_FALSE>
      GLboolean enable;
      
      /// glBlendFunc; GL_ZERO, <GL_ONE>, 
      ///              GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, 
      ///              GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, 
      ///              GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 
      ///              GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, 
      ///              GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR, 
      ///              GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, 
      ///              GL_SRC_ALPHA_SATURATE
      GLenum    blendSrc;
      
      /// glBlendFunc; <GL_ZERO>, GL_ONE, 
      ///               GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, 
      ///               GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, 
      ///               GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, 
      ///               GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, 
      ///               GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR, 
      ///               GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA
      GLenum    blendDst;
      
      /// glBlendEquation; <GL_FUNC_ADD>, GL_FUNC_SUBTRACT, 
      ///                   GL_FUNC_REVERSE_SUBTRACT, GL_MIN, GL_MAX
      GLenum    blendEquation;
      
      /// glBlendColor; <0, 0, 0, 0>
      GLcolor   blendColor;
      
      //@}-----------------------------------------------------------------
};

class GLDepthStateItem : public GLStateItem {
   public:
      virtual ~GLDepthStateItem() 
      { }
      
      virtual bool sync();
      virtual bool apply();
      
      ///@name State variables
      //@{-----------------------------------------------------------------
      
      /// glEnable; GL_TRUE or <GL_FALSE>
      GLboolean enable;
      
      /// glDepthFunc; GL_NEVER, <GL_LESS>, GL_EQUAL, GL_GREATER, GL_NOTEQUAL, 
      ///              GL_GEQUAL, GL_ALWAYS
      GLenum    depthFunc;
      
      /// glDepthMask; <GL_TRUE> or GL_FALSE
      GLboolean depthMask;
      
      /// glDepthRange; <0, 1>
      GLclampf  depthRange[2];
      
      //@}-----------------------------------------------------------------
};

class GLStencilStateItem : public GLStateItem {
   public:
      virtual ~GLStencilStateItem() 
      { }
      
      virtual bool sync();
      virtual bool apply();
      
      ///@name State variables
      //@{-----------------------------------------------------------------
      
      /// glEnable; GL_TRUE or <GL_FALSE>
      GLboolean enable;
      
      /// glStencilFunc; GL_NEVER, <GL_LESS>, GL_EQUAL, GL_GREATER, 
      ///                GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS
      GLenum    stencilFunc;
      /// glStencilFunc; <0> to 2^n-1, where n is # of bitplanes in stencil buf
      GLint     stencilRef;
      /// glStencilFunc; <1>
      GLuint    stencilMask;
      
      /// glStencilOp; <GL_KEEP>, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP, 
      ///               GL_DECR, GL_DECR_WRAP, GL_INVERT
      GLenum    stencilFail;
      /// glStencilOp; <GL_KEEP>, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP, 
      ///               GL_DECR, GL_DECR_WRAP, GL_INVERT
      GLenum    stencilPassDepthFail;
      /// glStencilOp; <GL_KEEP>, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP, 
      ///               GL_DECR, GL_DECR_WRAP, GL_INVERT
      GLenum    stencilPassDepthPass;
      
      // TODO: incorporate glStencil{Op,Func,Mask}Separate?
      /*
      GLenum    stencilBackFail;
      GLenum    stencilBackFunc;
      GLenum    stencilBackPassDepthFail;
      GLenum    stencilBackPassDepthPass;
      
      GLuint    stencilBackRef;
      GLuint    stencilBackValueMask;
      GLuint    stencilBackWriteMask;
      
      GLenum    stencilPassDepthPass;
      GLuint    stencilValueMask;
      GLuint    stencilWriteMask;
      */
      
      //@}-----------------------------------------------------------------
};

class GLPrimitiveStateItem : public GLStateItem {
   public:
      virtual ~GLPrimitiveStateItem() 
      { }
      
      virtual bool sync();
      virtual bool apply();
      
      ///@name State variables
      //@{-----------------------------------------------------------------
      
      /// glPolygonMode; indexed by GL_POINT, GL_LINE, and GL_FILL
      std::map<GLenum, GLenum> polygonMode;
      
      /// glEnable; GL_TRUE or <GL_FALSE>
      GLboolean enableCullFace;
      /// glCullFace; GL_FRONT, <GL_BACK>, GL_FRONT_AND_BACK
      GLenum    cullFace;
      
      /// glFrontFace; GL_CW or <GL_CCW>
      GLenum    frontFace;
      
      /// glEnable; indexed by GL_POLYGON_OFFSET_{FILL|LINE|POINT}
      std::map<GLenum, GLboolean> enablePolygonOffset;
      
      /// glPolygonOffset
      GLfloat   polygonOffsetFactor;
      /// glPolygonOffset
      GLfloat   polygonOffsetUnits;
      
      //@}-----------------------------------------------------------------
};

class GLAlphaStateItem : public GLStateItem {
   public:
      virtual ~GLAlphaStateItem() 
      { }
      
      virtual bool sync();
      virtual bool apply();
      
      ///@name State variables
      //@{-----------------------------------------------------------------
      
      /// glEnable; GL_TRUE or <GL_FALSE>
      GLboolean enable;
      
      /// glAlphaFunc; GL_NEVER, GL_LESS, GL_EQUAL, GL_GREATER, GL_NOTEQUAL, 
      ///              GL_GEQUAL, <GL_ALWAYS>
      GLenum    alphaTestFunc;
      /// glAlphaFunc; <0> (lowest alpha) to 1 (highest alpha)
      GLfloat   alphaTestRef;
      
      //@}-----------------------------------------------------------------
};

class GLColorStateItem : public GLStateItem {
   public:
      virtual ~GLColorStateItem() 
      { }
      
      virtual bool sync();
      virtual bool apply();
      
      ///@name State variables
      //@{-----------------------------------------------------------------
      
      /// glEnable; <GL_TRUE> or GL_FALSE
      GLboolean enableDither;
      
      /// glColorMask; <GL_TRUE> or GL_FALSE
      GLboolean colorMask[4];
      
      //@}-----------------------------------------------------------------
};

class GLFogStateItem : public GLStateItem {
   public:
      virtual ~GLFogStateItem() 
      { }
      
      virtual bool sync();
      virtual bool apply();
      
      ///@name State variables
      //@{-----------------------------------------------------------------
      
      /// glEnable; GL_TRUE or <GL_FALSE>
      GLboolean enable;
      
      /// glFogi; GL_LINEAR, <GL_EXP>, GL_EXP2
      GLenum    fogMode;
      
      /// glFogf; <1> (must be non-negative)
      GLfloat   fogDensity;
      /// glFogf; <0>
      GLfloat   fogStart;
      /// glFogf; <1>
      GLfloat   fogEnd;
      
      /// glFogi; GL_FOG_COORD or <GL_FRAGMENT_DEPTH> -- TODO ? initial value
      GLenum    fogIndex;
      
      /// glFogfv; 
      GLcolor   fogColor;
      
      /// glHint; GL_FASTEST, GL_NICEST, or <GL_DONT_CARE>
      GLenum    fogHint;
      
      // TODO: add GL_FOG_COORD_ARRAY*
      
      //@}-----------------------------------------------------------------
};

class GLPointStateItem : public GLStateItem {
   public:
      virtual ~GLPointStateItem() 
      { }
      
      virtual bool sync();
      virtual bool apply();
      
      ///@name State variables
      //@{-----------------------------------------------------------------
      
      /// glPointSize; <1> (must be positive)
      GLfloat   pointSize;
      
      /// glPointParameterf; <0>
      GLfloat   pointSizeMin;
      /// glPointParameterf; <1>
      GLfloat   pointSizeMax;
      /// glPointParameterf; <1>
      GLfloat   pointFadeThresholdSize;
      
      /// glPointParameteri; <GL_LOWER_LEFT> or GL_UPPER_LEFT
      GLenum    pointSpriteCoordOrigin;
      
      /// glPointParameterfv; <1, 0, 0>
      GLenum    pointDistanceAttenuation[3];
      
      /// glEnable; GL_TRUE or <GL_FALSE>
      GLboolean pointSmooth;
      /// glHint; GL_FASTEST, GL_NICEST, or <GL_DONT_CARE>
      GLenum    pointSmoothHint;
      
      /// glEnable; GL_TRUE or <GL_FALSE>
      GLboolean pointSprite;
      
      /// glGetFloat; read-only
      GLfloat   pointSizeGranularity;
      GLfloat   pointSizeRange[2];
      
      //@}-----------------------------------------------------------------
};

}

#endif // GL_STATE_H_

