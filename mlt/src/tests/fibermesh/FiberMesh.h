/**<!-------------------------------------------------------------------->
   @file   FiberMesh.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      FiberMesh engine
   <!-------------------------------------------------------------------->**/

#ifndef FIBERMESH_H_
#define FIBERMESH_H_

#include <DifferentialGeometry.h>

class Frontend;
class LeastSquaresSolver;
class QMutex;

struct DragData {
   LeastSquaresSolver  *solver;
   Vector3        handle;
   unsigned       curveIndex;
   unsigned       handleIndex;
   unsigned       roiSupport;
   real_t         roiSize;
   bool           isDragging;
   
   inline DragData()
      : solver(NULL), isDragging(false)
   { }
   
   ~DragData();
};

class FiberMesh {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      explicit FiberMesh(Frontend *parent);
      virtual ~FiberMesh();
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Curve Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      virtual bool addCurve(const Vector2List &vertices);
      
      inline const CurveList &getCurves() const {
         return m_curves;
      }
      
      //@}-----------------------------------------------------------------
      ///@name Surface Accessors
      ///@note No Mutators because meshes are defined completely in terms 
      ///    of the underlying control curves
      //@{-----------------------------------------------------------------
      
      inline SurfaceList &getSurfaces() {
         return m_surfaces;
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Drag Interaction
      //@{-----------------------------------------------------------------
      
      virtual void dragBegin(unsigned curveIndex, unsigned handleIndex);
      virtual void drag(const Vector3 &pt);
      virtual void dragEnd();
      
      inline bool isDragging() const {
         return m_dragData.isDragging;
      }
      
      inline const DragData &getDragData() const {
         return m_dragData;
      }
      
      inline Vector3 getWorldVertex(double x, double y) {
         return getWorldVertex(Vector2(x, y));
      }
      
      Vector3 getWorldVertex(const Vector2 &filmPt);
      
      
      //@}-----------------------------------------------------------------
      
      virtual bool optimizeSurface(unsigned i, unsigned noIterations);
      
   protected:
      virtual void    _updateConstraints(const real_t roiSize);
      virtual Vector3 _getNormal(DifferentialSurface *surface, unsigned i, unsigned m);
      virtual bool    _addSurface(const Vector2List &smooth, DifferentialCurve *curve);
      
   protected:
      CurveList   m_curves;
      SurfaceList m_surfaces;
      
      Frontend *m_parent;
      
      // data for dragging interaction
      DragData  m_dragData;
      QMutex   *m_mutex;
};

#endif // FIBERMESH_H_

