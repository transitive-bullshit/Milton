/**<!-------------------------------------------------------------------->
   @file   BSDFVisualizer.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Visualizer for built-in BSDFs, displaying BSDF distributions, probability 
   density functions for sampling BSDFs, and the corresponding distribution of 
   samples taken from BSDFs.  This visualizer has been extremely helpful in 
   debugging BSDF implementations, allowing one to gain some insight visually 
   into what's happening with different BSDFs.
   <!-------------------------------------------------------------------->**/

#include "BSDFVisualizer.h"
#include <OpenGLCanvas.h>
#include <ImageCanvas.h>
#include <Gui.h>

#include <milton.h>
#include <QtGui>

BSDFVisualizer::BSDFVisualizer(OpenGLCanvas *parent)
   : InteractionListener(parent), m_material(new Material()), m_pt(NULL), 
     m_shape(NULL), m_quadric(NULL), m_index(0), m_display(1), 
     m_bsdf("")
{
   //m_shape = new Sphere();
   //m_shape = MeshLoader::load("test.ply");
   Mesh *mesh = MeshLoader::load("test.obj");
   ASSERT(mesh);
   //mesh->setTransToWorld(getRotMat(Point3(), Vector3(0, 0, 1), M_PI / 2));
   mesh->setTransToWorldInv(getInvTransMat(Vector3(0, 1, 0)));
   mesh->setTransToWorld(getTransMat(Vector3(0, 1, 0)));
   (*mesh)["spatialAccel"] = std::string("naive");
   m_shape = mesh;
   
   ASSERT(m_shape);
   Material *m = new Material();
   m_shape->setMaterial(m);
   m_shape->init();
}

BSDFVisualizer::~BSDFVisualizer() {
   safeDelete(m_pt);
   safeDelete(m_material);
   safeDelete(m_shape);
   
   if (m_quadric) {
      gluDeleteQuadric(m_quadric);
      m_quadric = NULL;
   }
}

void BSDFVisualizer::mousePressEvent  (InteractionInfo &info) {
   QMouseEvent *event = static_cast<QMouseEvent *>(info.event);
   const real_t width = m_parent->getWidth(), height = m_parent->getHeight();
   const Point2 &pt   = Point2(event->x() / width, event->y() / height);
   
   if (event->modifiers() & Qt::ShiftModifier) {
      const Ray &r    = m_parent->getCamera()->getWorldRay(pt);
      
      _init(r);
   } else if (!(event->modifiers() & Qt::ControlModifier)) {
      _regenerateSamples();
   }
}

void BSDFVisualizer::keyPressEvent(InteractionInfo &info) {
   QKeyEvent *event = static_cast<QKeyEvent *>(info.event);
   
   if (event->key() == Qt::Key_N) {
      ++m_index;
      
      _init(m_ray);
   } else if (event->key() == Qt::Key_Plus) {
      _sampleBSDF();
      m_parent->redraw();
   } else if (event->key() == Qt::Key_Minus) {
      if (m_wos.size() > 1) {
         m_wos.pop_back();
         m_parent->redraw();
      }
   } else if (event->key() == Qt::Key_1) { // show bsdf
      if (m_display != 1) {
         m_display = 1;
         _regenerateDisplay();
      }
   } else if (event->key() == Qt::Key_2) { // show pdf
      if (m_display != 2) {
         m_display = 2;
         _regenerateDisplay();
      }
   } else if (event->key() == Qt::Key_3) { // show sampled density for bsdf
      if (m_display != 3) {
         m_display = 3;
         _regenerateDisplay();
      }
   }
}

void BSDFVisualizer::render() {
   if (NULL == m_pt)
      return;
   
   if (NULL == m_quadric)
      m_quadric = gluNewQuadric();
   
   // save state
   /*glPushAttrib(GL_ENABLE_BIT);
   
   { // enable blending / transparency
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   }
   
   // restore state
   glPopAttrib(); // GL_BLEND
                  // GL_CULL_FACE
   */
   
   glColor4f(1, 1, 1, 1);
   glPushMatrix();
   m_shape->preview();
   glPopMatrix();
   
   glDisable(GL_LIGHTING);
   
   glBegin(GL_LINES);
   {
      glColor4f(1, 0, 1, 1);
      glVertex3real_tv((m_pt->position - m_ray.direction).data);
      glVertex3real_tv((m_pt->position).data);
      
      glColor4f(1, 1, 1, 1);
      for(unsigned i = m_wos.size(); i--;) {
         const Vector3 &wo = m_wos[i];
         
         glVertex3real_tv((m_pt->position).data);
         glVertex3real_tv((m_pt->position + wo).data);
      }
      
      glColor4f(0, 1, 0, 1);
      glVertex3real_tv((m_pt->position + m_pt->normalS).data);
      glVertex3real_tv((m_pt->position).data);
   }
   glEnd();
   
   glEnable(GL_LIGHTING);
   
   Matrix4x4 trans = 
      getTransMat(Vector3(m_pt->position.data));
      //getScaleMat(Vector3(M_PI, M_PI, M_PI));
   
   // no need to rotate cause we're drawing a sphere instead of a hemisphere
   /*const Vector3 &N = Vector3(0, 1, 0);
   if (m_pt->normalS != N) {
      trans = trans * getRotMat(Point3(), N.cross(m_pt->normalS), 
                                acos(m_pt->normalS.dot(N)));
   }*/
   
   real_t data[16];
   trans.fillGLMatrix(data);
   glPushMatrix();
   glMultMatrixreal_t(data);
   
   glColor3f(.4, .4, 1);
   //gluSphere(m_quadric, 0.5, 40, 40);
   
   if (!m_pt->bsdf->isSpecular() && m_vertices.size() > 0) {
      glBegin(GL_TRIANGLES);
      for(unsigned i = m_vertices.size() - 1; i--;) {
         Vector3List &lx0 = m_vertices[i];
         Vector3List &lx1 = m_vertices[i + 1];
         
         for(unsigned j = lx0.size() - 1; j--;) {
            const Vector3 &v0 = lx1[j + 1];
            glNormal3real_tv(v0.getNormalized().data);
            glVertex3real_tv(v0.data);
            
            const Vector3 &v2 = lx1[j];
            glNormal3real_tv(v2.getNormalized().data);
            glVertex3real_tv(v2.data);
            
            const Vector3 &v1 = lx0[j + 1];
            glNormal3real_tv(v1.getNormalized().data);
            glVertex3real_tv(v1.data);
            
            glNormal3real_tv(v1.getNormalized().data);
            glVertex3real_tv(v1.data);
            
            const Vector3 &v4 = lx0[j];
            glNormal3real_tv(v2.getNormalized().data);
            glVertex3real_tv(v2.data);
            
            glNormal3real_tv(v4.getNormalized().data);
            glVertex3real_tv(v4.data);
         }
      }
      glEnd();
   }
   
   glPopMatrix();
}

void BSDFVisualizer::init() {
   _init();
}

void BSDFVisualizer::_init() {
   const Point3 &eye = m_parent->getCamera()->getPosition(UV());
   const Ray &ray    = Ray(eye, (m_shape->getAABB().getCenter() - eye).getNormalized());
   
   _init(ray);
}

void BSDFVisualizer::_init(const Ray &ray) {
   const char *const bsdfs[] = {
      "diffuse", "transmissive", "phong", NULL
   };
   
   m_index = m_index % 3;
   const std::string &bsdf = bsdfs[m_index];
   
   Material *m = new Material();
   (*m)["bsdf"] = bsdf;
   (*m)["kd"]   = Spectrum::fill(.2);
   (*m)["ks"]   = Spectrum::fill(.8);
   (*m)["n"]    = Spectrum::fill(10);
   
   if (m_index == 1)
      (*m)["ior"] = 1.49;
   
   m_shape->setMaterial(m);
   
   SurfacePoint *pt = new SurfacePoint();
   pt = new SurfacePoint();
   pt->shape = m_shape;
   
   real_t t = m_shape->getIntersection(ray, *pt);
   if (!pt->init(ray, t)) {
      m_shape->setMaterial(m_material);
      safeDelete(m);
      safeDelete(pt);
      return;
   }
   
   m_bsdf = bsdf;
   
   safeDelete(m_pt);
   safeDelete(m_material);
   
   m_pt = pt;
   m_material = m;
   
   cerr << "BSDF: " << bsdf << endl
        << "-----------------------------------------------------------" 
        << endl << endl;
   
   m_ray = ray;
   _regenerate();
}

void BSDFVisualizer::_regenerate() {
   _regenerateSamples();
   _regenerateDisplay();
}

void BSDFVisualizer::_regenerateSamples() {
   unsigned n = m_wos.size();
   m_wos.clear();
   
   n += (n == 0);
   
   for(unsigned i = n; i--;)
      _sampleBSDF();
   
   m_parent->redraw();
}


void BSDFVisualizer::_regenerateDisplay() {
   unsigned nPhi = 50, nTheta = 50;
   const Vector3 &N = m_pt->normalS;
   
   m_vertices.clear();
   
   cerr << "Displaying ";
   if (m_display == 1)
      cerr << "BSDF (1)";
   else if (m_display == 2)
      cerr << "pdf (2)";
   else if (m_display == 3)
      cerr << "sampled density (3)";
   cerr << " for bsdf '" << m_bsdf << "'" << endl;
   
   m_pt->bsdf->setWi(m_ray.direction);
   if (!m_pt->bsdf->isSpecular()) {
      
      if (m_material->contains("kd"))
         cerr << "\t" << "kd = " << m_material->getValue<Spectrum>("kd") << endl;
      if (m_material->contains("ks"))
         cerr << "\t" << "ks = " << m_material->getValue<Spectrum>("ks") << endl;
      if (m_material->contains("n"))
         cerr << "\t" << "n  = " << m_material->getValue<Spectrum>("n") << endl;
      
      Vector3List v;
      if (m_display == 3) {
         unsigned k = 4096;
         
         for(unsigned i = k; i--;) {
            const Event &event = m_pt->bsdf->sample();
            const Vector3 &wo = event;
            
            v.push_back(wo);
         }
      }
      
      Event event = m_pt->bsdf->sample();
      real_t maxScale = 0;
      
      for(unsigned i = nPhi; i--;) {
         real_t phi = i * (2 * M_PI) / (nPhi - 1);
         m_vertices.push_back(Vector3List());
         Vector3List &vertices = m_vertices.back();
         
         for(unsigned j = nTheta; j--;) {
            real_t theta = j * (M_PI / 2) / (nTheta - 1);
            const Vector3 &wo = convertHemisphere(theta, phi, N);
            real_t scale = 1.0;
            
            if (m_display == 1) {
               const Spectrum &fs = m_pt->bsdf->getBSDF(wo);
               scale = fs[fs.getMaxFrequency()];
            } else if (m_display == 2) {
               scale = m_pt->bsdf->getPdf(Event(wo, event));
            } else if (m_display == 3) {
               scale = 0;
               
               FOREACH(Vector3ListConstIter, v, iter) {
                  scale += MAX(0, wo.dot(*iter));
               }
               
               scale /= v.size();
            }
            
            maxScale = MAX(scale, maxScale);
            vertices.push_back(wo * scale);
         }
      }
      
      /*if (m_display == 1 && maxScale > EPSILON) {
         for(unsigned i = m_vertices.size(); i--;) {
            Vector3List &vertices = m_vertices[i];
            
            for(unsigned j = vertices.size(); j--;) {
               vertices[j] /= maxScale;
            }
         }
      }*/
   }
   
   m_parent->redraw();
}

void BSDFVisualizer::_sampleBSDF() {
   if (NULL == m_pt)
      return;
   
   m_pt->bsdf->setWi(m_ray.direction);
   const Event &event = m_pt->bsdf->sample();
   const Vector3 &wo = event;
   
   cerr << m_ray.direction << " -> " << wo << endl;
   //const Vector3 &N = (m_pt->normal.dot(m_pt->bsdf->getWi()) > 0 ? -m_pt->normalS : m_pt->normalS);
   
   cerr << "\t" << "Fr  = " << 
      Fresnel::getFr(m_pt->bsdf->getWi(), m_pt->normalS, m_pt->ior1, m_pt->ior2) << endl;
   cerr << "\t" << "pdf = " << m_pt->bsdf->getPdf(event) << endl;
   cerr << "\t" << "fs  = " << m_pt->bsdf->getBSDF(wo) << endl;
   
   m_wos.push_back(1.2 * wo);
}

