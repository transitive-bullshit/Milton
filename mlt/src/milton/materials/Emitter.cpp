/**<!-------------------------------------------------------------------->
   @file   Emitter.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Abstract representation of emittance defined at a single point on a 
   surface in 3-space (describing a light).
   
      The reason Emitter subclasses BSDF is for convenience during generation 
   and evaluation of paths in bidirectional path tracing.  Emittance at a 
   point in a given direction, Le(x,wo), is broken into two parts, 
   Le0(x), and Le1(x), as originally detailed by Veach and Guibas
   (see Eric Veach's thesis, section 8.3.2).  Scattering at a vertex can then 
   be uniformly viewed as evaluation / sampling of a BSDF, including initial, 
   fake 'scattering' at a light source, which can be viewed as a special case 
   of scattering with no incident vector (the incident vector is thus 
   disregarded).
   
   @see Emitter::evaluate() or the Path class for more details.
   <!-------------------------------------------------------------------->**/

#include "Emitter.h"
#include "Material.h"

#include <GLState.h>
#include <Shape.h>

#include <GL/gl.h>
#include <GL/glu.h>

namespace milton {

void Emitter::init() {
   ASSERT(m_parent);
   
   m_power = m_parent->getValue<SpectralSampleSet>("power", 
                                                   SpectralSampleSet::black());
   
   const real_t surfaceArea = 
      (m_pt.shape ? m_pt.shape->getSurfaceArea() : 0);
   
   if (NEQ(surfaceArea, 0.0)) {
      m_radiantExitance = m_power / surfaceArea;
   } else {
      m_radiantExitance = m_power;
   }
   
   BSDF::init();
}

bool Emitter::isEmitter() {
   return (getPower() != SpectralSampleSet::black());
}

SpectralSampleSet Emitter::getPower() {
   return m_power;
}

SpectralSampleSet Emitter::getRadiantExitance() {
   return m_radiantExitance;
}

SpectralSampleSet Emitter::getLe0() {
   return m_radiantExitance;
}

SpectralSampleSet Emitter::getLe1(const Vector3 &wo) {
   return getLe(wo) / getLe0();
}

void Emitter::preview(Shape *shape) {
   ASSERT(m_parent);
   
   //unsigned lightIndex = m_parent->getValue<unsigned int>("glLightIndex", 0);
   //ASSERT(lightIndex >= 0 && lightIndex <= 8);
   //lightIndex += GL_LIGHT0;
   
   GLint lightIndex = GLState::getFreeLight();
   //cerr << "LightIndex: " << (lightIndex - GL_LIGHT0) << endl;
   
   if (lightIndex < GL_LIGHT0)
      return;
   
   // enable light
   glEnable(lightIndex);
   GL_CHECK_ERROR();
   
   // light position
   glPushMatrix();
   glLoadIdentity();
   
   const Point3 &center    = shape->getAABB().getCenter();
   const GLfloat centerf[] = {
      center[0], center[1], center[2], center[3]
   };
   
   glLightfv(lightIndex, GL_POSITION, centerf);
   GL_CHECK_ERROR();
   
   
   // light color
   const Vector3 &diffuseColor  = m_parent->getValue<Vector3>(
      "glDiffuseColor",  Vector3(1, 1, 1));
   const Vector3 &specularColor = m_parent->getValue<Vector3>(
      "glSpecularColor", Vector3(1, 1, 1));
   
   const GLfloat diffuse[] = {
      diffuseColor[0], diffuseColor[1], diffuseColor[2], 1
   };
   
   const GLfloat specular[] = {
      specularColor[0], specularColor[1], specularColor[2], 1
   };
   
   glLightfv(lightIndex, GL_DIFFUSE,  diffuse);
   glLightfv(lightIndex, GL_SPECULAR, specular);
   
   GL_CHECK_ERROR();
   
   // light function (attenuation)
   const Vector3 &attenuation = m_parent->getValue<Vector3>(
      "glAttenuation", Vector3(1, 0, 0));
   
   glLightf(lightIndex, GL_CONSTANT_ATTENUATION, attenuation[0]);
   glLightf(lightIndex, GL_LINEAR_ATTENUATION, attenuation[1]);
   glLightf(lightIndex, GL_QUADRATIC_ATTENUATION, attenuation[2]);
   
   glTranslatef(centerf[0], centerf[1], centerf[2]);
   
   glPushAttrib(GL_POLYGON_BIT | GL_LIGHTING_BIT);
   //GLint params[2];
   //glGetInteverv(GL_POLYGON_MODE, params);
   
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glDisable(GL_LIGHTING);
   
   glColor3f(204 / 255.0f, 51 / 255.0f, 1);
   GLUquadric *quadric = gluNewQuadric();
   gluSphere(quadric, 0.25, 20, 20);
   gluDeleteQuadric(quadric);
   
   //glPolygonMode(params[0], params[1]);
   
   glPopAttrib(); // GL_POLYGON_BIT | GL_LIGHTING_BIT
   
   glPopMatrix();
   
   GL_CHECK_ERROR();
}

Event Emitter::sample(const Event &) {
   ASSERT(m_sampler);
   
   Event e = m_sampler->sample();
   e.setParent(this);
   
   return e;
}

real_t Emitter::getPd(const Event &event) {
   ASSERT(m_sampler);
   //const Vector3 &wo = event.getValue<const Vector3&>();
   
   return m_sampler->getPd(event);
}

SpectralSampleSet Emitter::evaluate(const Vector3 &/* wi unused */, 
                                   const Vector3 &wo)
{
   return getLe1(wo);
}

}

