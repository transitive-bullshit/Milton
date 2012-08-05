/**<!-------------------------------------------------------------------->
   @file   Scene.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Contains all primitive and material data representing a given 3D scene
   <!-------------------------------------------------------------------->**/

#include <Scene.h>
#include <shapes.h>

#include <Material.h>
#include <Ray.h>

#include <GL/gl.h>

namespace milton {

Scene::~Scene() {
   FOREACH(MaterialListIter, m_materials, iter) {
      Material *material = *iter;
      
      safeDelete(material);
   }
   
   safeDelete(m_shapes);
   safeDelete(m_lights);
}

void Scene::init() {
   ASSERT(m_shapes);
   
   if (m_initted)
      return;
   
   m_initted = true;
   if (m_materials.size() == 0) {
      Material *m = new Material();
      
      m->init();
      m_materials.push_back(m);
   }
   
   if (m_background == NULL)
      m_background = Material::s_nullEmitter;
   
   if (m_shapes->getMaterial() == NULL)
      m_shapes->setMaterial(m_materials[0]);
   
   // initialize spatial acceleration and materials for shapes
   m_shapes->init();
   
   m_lights = new ShapeSet(false);
   m_lights->setMaterial(m_materials[0]);
   SurfacePoint pt;
   
   // record lights separately in addition to normal geometry
   FOREACH(PrimitiveListIter, m_shapes->getPrimitives(), iter) {
      Shape *shape = *iter;
      Material *material = shape->getMaterial();
      
      if (material && material->isEmitter())
         m_lights->push_back(shape);
   }
   
   // initialize spatial acceleration for lights
   m_lights->init(false);
   
   m_emitterSampler.init(m_lights);
}

void Scene::preview() {
   ASSERT(m_shapes);
   
   // Draw unit coordinate system
   glPushAttrib(GL_LIGHTING_BIT);
   glDisable(GL_LIGHTING);
   
   glBegin(GL_LINES);
   
   glColor3f(1, 0, 0);
   glVertex3f(0, 0, 0);
   glVertex3f(1, 0, 0);
   
   glColor3f(0, 1, 0);
   glVertex3f(0, 0, 0);
   glVertex3f(0, 1, 0);
   
   glColor3f(0, 0, 1);
   glVertex3f(0, 0, 0);
   glVertex3f(0, 0, 1);
   
   glEnd();
   
   glPopAttrib(); // GL_LIGHTING_BIT
   glColor3f(1, 1, 1);
   
   glPushMatrix();
   m_shapes->preview();
   glPopMatrix();
}

real_t Scene::getIntersection(const Ray &ray, SurfacePoint &pt) {
   ASSERT(m_shapes);
   
   return m_shapes->getIntersection(ray, pt);
}

bool Scene::intersects(const Ray &ray, real_t tMax) {
   ASSERT(m_shapes);
   
   return m_shapes->intersects(ray, tMax - EPSILON);
}

SpectralSampleSet Scene::getBackgroundRadiance(const Vector3 &w) {
   return m_background->getLe(-w);
}

}

