/**<!-------------------------------------------------------------------->
   @file   Frontend.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      Frontend for FiberMesh project
   <!-------------------------------------------------------------------->**/

#include "Frontend.h"
#include "FiberMesh.h"
#include "MeshUtils.h"
#include <milton.h>

using namespace std;

Frontend::Frontend(OpenGLCanvas *parent)
   : InteractionListener(parent), m_engine(new FiberMesh(this)), 
     m_mode(INTERACTION_MODE_DRAW), m_noIterations(100), m_delay(0)
{ }

Frontend::~Frontend() {
   safeDelete(m_engine);
}

void Frontend::mousePressEvent  (InteractionInfo &info) {
   QMouseEvent *event = static_cast<QMouseEvent *>(info.event);
   
   if (event->modifiers() & Qt::ControlModifier) {
      _handleCameraMousePressEvent(event);
      return;
   }
   
   const Vector2 pt(event->x(), event->y());
   
   switch(m_mode) {
      case INTERACTION_MODE_DRAW:
         m_active.clear();
         m_active.push_back(pt);
         
         m_parent->redraw();
         break;
      case INTERACTION_MODE_DRAG:
         {
            const CurveList &curves = m_engine->getCurves(); 
            Camera *camera = m_parent->getCamera();
            
            const unsigned width  = m_parent->getWidth();
            const unsigned height = m_parent->getHeight();
            const Point2 filmPt(pt[0] / width, pt[1] / height);
            
            unsigned minCurveIndex  = -1;
            unsigned minVertexIndex = -1;
            real_t minDist = INFINITY;
            
            // find closest vertex in existing curves to click point
            for(unsigned c = curves.size(); c--;) {
               const Vector3List &curve = curves[c]->vertices;
               const unsigned n = curve.size();
               
               for(unsigned i = n; i--;) {
                  const Point3 &pWorld = Point3(curve[i].data);
                  Point2 pFilm = camera->getProjection(pWorld);
                  
                  const real_t diff = (filmPt - pFilm).getMagnitude2();
                  
                  if (diff < minDist) {
                     minDist = diff;
                     minCurveIndex  = c;
                     minVertexIndex = i;
                  }
               }
            }
            
            // maximum percent distance in screen-space
            const real_t maxDist = .05;
            
            // if user clicked close enough to a curve, begin dragging it
            if (minDist < maxDist) {
               m_handle = 
                  Point3(curves[minCurveIndex]->vertices[minVertexIndex].data);
               m_engine->dragBegin(minCurveIndex, minVertexIndex);
            }
            
            m_delay = 0;
         }
         break;
      default:
         break;
   }
}

void Frontend::mouseMoveEvent  (InteractionInfo &info) {
   QMouseEvent *event = static_cast<QMouseEvent *>(info.event);
   
   if (event->modifiers() & Qt::ControlModifier) {
      _handleCameraMouseMoveEvent(event);
      return;
   }
   
   const Vector2 pt(event->x(), event->y());
   
   switch(m_mode) {
      case INTERACTION_MODE_DRAW:
         if (m_active.size() > 0 && m_active.back() != pt) {
            m_active.push_back(pt);
            m_parent->redraw();
         }
         break;
      case INTERACTION_MODE_DRAG:
         if (m_engine->isDragging() && ++m_delay > 5) {
            m_delay = 0;
            
            const unsigned width   = m_parent->getWidth();
            const unsigned height  = m_parent->getHeight();
            ThinLensCamera *camera = (ThinLensCamera *)m_parent->getCamera();
            
            const Point2 filmPt(pt[0] / width, pt[1] / height);
            Ray ray(camera->getWorldRay(filmPt));
            
            const Point3  &E = ray.origin;
            const Vector3 &l = camera->getLook();
            
            const real_t t   = (m_handle - E).dot(l) / (ray.direction.dot(l));
            const Vector3 &F = Vector3((E + t * ray.direction).data);
            
            // drag handle index on selected curve to point F and update curve 
            // coords and surface accordingly
            m_engine->drag(F);
            m_parent->redraw();
         }
         break;
      default:
         break;
   }
}

void Frontend::mouseReleaseEvent(InteractionInfo &info) {
   switch(m_mode) {
      case INTERACTION_MODE_DRAW:
         {
            const unsigned n = m_active.size();
            
            if (n > 0) {
               m_engine->addCurve(m_active);
               
               m_active.clear();
               m_parent->redraw();
            }
         }
      case INTERACTION_MODE_DRAG:
         if (m_engine->isDragging()) {
            // finalize a drag interaction
            m_engine->dragEnd();
            m_delay = 0;
            
            m_parent->redraw();
         }
         break;
      default:
         break;
   }
}

void Frontend::keyPressEvent(InteractionInfo &info) {
   QKeyEvent *event = static_cast<QKeyEvent *>(info.event);
   
   if (event->modifiers() & Qt::ControlModifier)
      return;
   
   // change input mode via key presses
   if (event->key() == Qt::Key_1) {
      m_mode = INTERACTION_MODE_DRAW;
      
      cerr << "draw mode" << endl;
   } else if (event->key() == Qt::Key_2) {
      m_mode = INTERACTION_MODE_DRAG;
      
      cerr << "drag mode" << endl;
   } else if (event->key() == Qt::Key_Plus) {
      cerr << "noIterations: " << ++m_noIterations << endl;
   } else if (event->key() == Qt::Key_Minus) {
      if (m_noIterations > 0)
         cerr << "noIterations: " << --m_noIterations << endl;
   } else if (event->key() == Qt::Key_P) {
      double r = 160;
      
      Vector2List v;
      for(unsigned slice = 0; slice < 100; ++slice) {
         double theta1 = slice * (2 * M_PI) / 100;
         const unsigned width  = m_parent->getWidth();
         const unsigned height = m_parent->getHeight();
         
         v.push_back(Vector2(r*cos(theta1),r*sin(theta1)) + Vector2(width / 2, height / 2));
      }
      
      m_engine->addCurve(v);
   } else if (event->key() == Qt::Key_Space) {
      unsigned noSurfaces = m_engine->getSurfaces().size();
      
      if (noSurfaces > 0)
         m_engine->optimizeSurface(noSurfaces - 1, m_noIterations);
   } else {
      return;
   }
   
   m_parent->redraw();
}

void Frontend::paintGL() {
   const SurfaceList &surfaces = m_engine->getSurfaces();
   
   for(unsigned i = surfaces.size(); i--;) {
      Mesh *mesh = surfaces[i]->mesh;
      ASSERT(mesh);
      
      mesh->preview();
      
#if 0 // preview hacky normals around border
      const MeshTriangle *triangles = mesh->getTriangles();
      Vector3 *vertices = mesh->getVertices();
      const CurveList &curves  = m_engine->getCurves();
      const unsigned m  = curves[i].vertices.size();
      
      glColor3f(1, 1, 1);
      glDisable(GL_LIGHTING);
      glBegin(GL_LINES);
      for(unsigned j = m; j--;) {
         //const Vector3 &normal = mesh->getNormals()[j] / 10;
         
         Neighborhood &nei = meshes[i]->vertexNeighborhood[j];
         ASSERT(nei.noPolygons > 0);
         unsigned k = m - 1;
         for(unsigned p = 0; p < nei.noPolygons; ++p) {
            const MeshTriangle &t = triangles[nei.polygons[p]];
            for(unsigned l = 3; l--;) {
               if (t.data[l] >= m) {
                  k = t.data[l];
                  break;
               }
            }
            
            if (k >= m)
               break;
         }
         Vector3 v3;
         if (k < m) {
            v3 = mesh->getNormals()[j] / 10;
         } else {
            const Vector3 &v0 = (vertices[j + 1 - m * (j == m - 1)] - 
                                 vertices[j]).getNormalized();
            const Vector3 &v1 = (vertices[k] - vertices[j]);
            const Vector3 &v2 = v0.cross(v1).getNormalized();
            v3 = v0.cross(v2);
         }
         
         glVertex3dv(vertices[j].data);
         glVertex3dv((vertices[j] + v3 / 10).data);
      }
      glEnd();
      glEnable(GL_LIGHTING);
      //MeshLoader::save(mesh, "fiber.obj");
#endif
   }
   
   const CurveList &curves  = m_engine->getCurves();
   const DragData &dragData = m_engine->getDragData();
   
   glDisable(GL_LIGHTING);
   glLineWidth(3);
   glPolygonOffset(2, 1);
   glPointSize(3);
   
   // paint old curves that have previously been drawn
   for(unsigned c = curves.size(); c--;) {
      const Vector3List &curve = curves[c]->vertices;
      const unsigned n  = curve.size();
      const bool isDrag = (dragData.isDragging && dragData.curveIndex == c);
      
      if (isDrag)
         glColor3f(0, 1, 0);
      else
         glColor3f(0, 0, 1);
      
      glBegin(curves[c]->closed ? GL_LINE_LOOP : GL_LINE_STRIP);
      for(unsigned i = n; i--;)
         glVertex3dv(curve[i].data);
      glEnd();
      
      glColor3f(0, 1, 0);
      glBegin(GL_POINTS);
      for(unsigned i = n; i--;)
         glVertex3dv(curve[i].data);
      glEnd();
   }
   
   glEnable(GL_LIGHTING);
   glLineWidth(1);
   glPolygonOffset(0, 0);
   glPointSize(1);
}

void Frontend::paint(QPainter *p) {
   QPen pen(Qt::red);
   pen.setWidth(3);
   
#if 0
   const unsigned width  = m_parent->getWidth();
   const unsigned height = m_parent->getHeight();
   Camera *camera = m_parent->getCamera();
   
   const CurveList &curves  = m_engine->getCurves();
   const DragData &dragData = m_engine->getDragData();
   
   // paint old curves that have previously been drawn
   for(unsigned c = curves.size(); c--;) {
      const Vector3List &curve = curves[c]->vertices;
      const unsigned n  = curve.size();
      const bool isDrag = (dragData.isDragging && dragData.curveIndex == c);
      
      if (isDrag)
         pen.setColor(Qt::green);
      else
         pen.setColor(Qt::blue);
      
      p->setPen(pen);
      
      for(unsigned i = n - (!curves[c]->closed); i--;) {
         const Point3 &pAWorld = Point3(curve[i].data);
         const Point3 &pBWorld = Point3(curve[(i == n - 1 ? 0 : i + 1)].data);
         const Point2 &pAFilm  = camera->getProjection(pAWorld);
         const Point2 &pBFilm  = camera->getProjection(pBWorld);
         
         p->drawLine(QPointF(pAFilm[0] * width, pAFilm[1] * height), 
                     QPointF(pBFilm[0] * width, pBFilm[1] * height));
      }
      
      pen.setColor(Qt::green);
      p->setPen(pen);
      
      for(unsigned i = n; i--;) {
         const Point3 &pAWorld = Point3(curve[i].data);
         const Point2 &pAFilm  = camera->getProjection(pAWorld);
         p->drawPoint(QPointF(pAFilm[0] * width, pAFilm[1] * height));
      }
   }
#endif
   
   const unsigned n = m_active.size();
   
   if (n > 0) {
      // paint current, active curve
      pen.setColor(Qt::red);
      p->setPen(pen);
      
      Vector2 min = Vector2::infinity();
      Vector2 max = Vector2::negativeInfinity();
      
      for(unsigned i = n - 1; i--;) {
         const Vector2 &vA = m_active[i];
         const Vector2 &vB = m_active[(i == n - 1 ? 0 : i + 1)];
         
         p->drawLine(QPointF(vA[0], vA[1]), QPointF(vB[0], vB[1]));
         
         min = Vector2::min(vA, min);
         max = Vector2::max(vA, max);
      }
      
      pen.setColor(Qt::green);
      p->setPen(pen);
      for(unsigned i = n; i--;) {
         const Vector2 &vA = m_active[i];
         p->drawPoint(QPointF(vA[0], vA[1]));
      }
      
      /*pen.setColor(Qt::blue);
      pen.setWidth(1);
      p->setPen(pen);
      for(unsigned i = min[1]; i < max[1]; ++i) {
         for(unsigned j = min[0]; j < max[0]; ++j) {
            if (MeshUtils::isPointInPolygon(Vector2(j, i), m_active)) {
               p->drawPoint(QPointF(j, i));
            }
         }
      }*/
   }
}

void Frontend::init() {
   m_engine->init();
   m_active.clear();
}

void Frontend::_handleCameraMousePressEvent(QMouseEvent *event) {
   const unsigned width  = m_parent->getWidth();
   const unsigned height = m_parent->getHeight();
   const Vector2 pt(event->x(), event->y());
   const Point2 filmPt(pt[0] / width, pt[1] / height);
   
   if (event->modifiers() & Qt::ControlModifier) {
      m_lastMousePos = m_mouseDownPos = Point2(event->x(), event->y());
      
      ThinLensCamera *camera = dynamic_cast<ThinLensCamera *>(m_parent->getCamera());
      ASSERT(camera);
      
      m_origEye  = camera->getEye();
      m_origUp   = camera->getUp();
      m_origLook = camera->getLook();
      m_origU    = camera->getU();
      
      m_origin = Point3();
      
      // if the user clicked a point on a mesh, use that point as the origin 
      // for the camera trackball rotation
      if (event->buttons() & Qt::LeftButton) {
         const SurfaceList &surfaces = m_engine->getSurfaces();
         Ray ray(camera->getWorldRay(filmPt));
         SurfacePoint sp;
         
         for(unsigned i = surfaces.size(); i--;) {
            const real_t t = surfaces[i]->mesh->getIntersection(ray, sp);
            
            if (Ray::isValid(t)) {
               m_origin = ray.origin + t * ray.direction;
               break;
            }
         }
      }
   }
}

void Frontend::_handleCameraMouseMoveEvent(QMouseEvent *event) {
   Qt::MouseButtons buttons = event->buttons();
   const Vector2 &diff = Point2(event->x(), event->y()) - m_lastMousePos;
   
   if (m_active.size() > 0)
      m_active.clear();
   
   ThinLensCamera *camera = dynamic_cast<ThinLensCamera *>(m_parent->getCamera());
   if (camera != NULL) {
      if (buttons & Qt::LeftButton) {
         // Camera trackball rotation
         // --------------------------------------------
         const Vector2 &diff = 
            Point2(event->x(), event->y()) - m_mouseDownPos;
         
         const real_t scaleFactor   = -0.015f;
         const Matrix4x4 &leftRight = getRotMat(m_origin, m_origUp, 
                                                scaleFactor * diff[0]);
         const Matrix4x4 &upDown    = getRotMat(m_origin, m_origU, 
                                                scaleFactor * diff[1]);
         
         // Calculate new camera parameters
         const Point3 &eyeNew   = upDown * leftRight * m_origEye;
         const Vector3 &upNew   = upDown * leftRight * m_origUp;
         const Vector3 &lookNew = upDown * leftRight * m_origLook;
         
         camera->setOrientation(eyeNew, lookNew, upNew);
      } else if (buttons & Qt::RightButton) {
         // Camera dolly (translation along look vector)
         // --------------------------------------------
         const real_t   amount = diff[0] / 200.0;
         const Vector3 &offset = camera->getLook() * amount;
         
         const Point3 &eyeNew  = camera->getEye() + offset;
         camera->setOrientation(eyeNew, camera->getLook(), camera->getUp());
      } else if (buttons & Qt::MidButton) {
         // Camera pan (translation along uv film plane)
         // --------------------------------------------
         const Point3 &eyeNew  = camera->getEye() + 
            (camera->getU() * (-diff[0] / 64))    + 
            (camera->getV() * (diff[1] / 64));
         
         camera->setOrientation(eyeNew, camera->getLook(), camera->getUp());
      }
      
      m_parent->redraw();
   } 
   
   m_lastMousePos = Point2(event->x(), event->y());
}

