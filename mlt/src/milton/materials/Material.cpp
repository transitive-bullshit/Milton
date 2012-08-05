/**<!-------------------------------------------------------------------->
   @file   Material.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Abstract representation of a surface Material, defined without respect 
   to the underlying surface (loose coupling between Shapes and Materials 
   from the point-of-view of a Material, but all Shapes know about their 
   surface Material).  Materials subclass PropertyMap, and it is through this 
   interface that Material properties may be set (ex: diffuse color, 
   texture/bump/color map(s), index of refraction of interior volume, etc.)
   
      Reflectivity, emittance, and sensor response (BSDFs, Emitters, and 
   Sensors respectively) are three properties of a material that are defined 
   at a single point on a surface.  A Material encapsulates BSDF, Emitter, 
   and Sensor properties defined over its surface, where specific instances of 
   BSDF, Emitter, and Sensor are allowed to have their inputs vary with 
   respect to position along the surface.  In this respect, Materials 
   represent a mapping from surface position to associated BSDF, Emitter, and 
   Sensor functions, where the underlying functions themselves remain constant 
   along the surface, and only the inputs vary among the different instances / 
   surface points.  For example, a Material may have a DiffuseBSDF over its 
   entire surface, but a specific DiffuseBSDF instance obtained by getBSDF or 
   implicitly in initSurfacePoint (which fills in the SurfacePoint's BSDF), is 
   allowed to have its 'kd' parameter (diffuse albedo) vary with respect to 
   the given surface point via lookup in an associated 'kd' texture map 
   defined over the UV coordinates of the surface.  
   <!-------------------------------------------------------------------->**/

#include "Material.h"
#include <materials.h>
#include <filters.h>

#include <ResourceManager.h>
#include <Camera.h>

#include <GL/gl.h>
#include <QtCore/QtCore>

namespace milton {

// initialize static members of Material
SurfacePoint Material::s_nullSurfacePoint;
Emitter *Material::s_nullEmitter = new NullEmitter(Material::s_nullSurfacePoint);
Sensor  *Material::s_nullSensor  = new NullSensor (Material::s_nullSurfacePoint);

Material::~Material() {
   safeDelete(m_filter);
}

void Material::init() {
   const std::string &filter = getValue<std::string>("filter", "triangle");
   
   if (NULL == m_filter)
      m_filter  = KernelFilter::create(filter, *this);
   
   m_bsdf    = getValue<const std::string>("bsdf", m_bsdf);
   m_emitter = getValue<const std::string>("emitter", m_emitter);
   m_bumpMap = getValue<const std::string>("bumpMap", m_bumpMap);
   
   m_repeatU       = getValue<real_t>("repeatU", m_repeatU);
   m_repeatV       = getValue<real_t>("repeatV", m_repeatV);
   m_bumpIntensity = getValue<real_t>("bumpIntensity", m_bumpIntensity); 
}

BSDF *Material::getBSDF(SurfacePoint &pt) {
   BSDF *bsdf = NULL;
   
   if (m_bsdf == "diffuse") {
      bsdf = new DiffuseBSDF(pt, this);
   } else if (m_bsdf == "dielectric") {
      bsdf = new DielectricBSDF(pt, this);
   } else if (m_bsdf == "modifiedPhong" || m_bsdf == "phong") {
      bsdf = new ModifiedPhongBSDF(pt, this);
   } else if (m_bsdf == "absorbent") {
      bsdf = new AbsorbentBSDF(pt, this);
   } else if (m_bsdf == "specular") {
      (*this)["transparency"] = 0.0;
      bsdf = new DielectricBSDF(pt, this);
   } else if (m_bsdf == "transmissive") {
      (*this)["transparency"] = 1.0;
      bsdf = new DielectricBSDF(pt, this);
   } else {
      cerr << "invalid material (BSDF type): " << m_bsdf << endl;
      ASSERT(0 && "Found Invalid Material (BSDF type)");
      return NULL;
   }
   
   bsdf->init();
   return bsdf;
}

bool Material::isEmitter() {
   return (m_emitter != "null");
}

Emitter *Material::getEmitter(SurfacePoint &pt) {
   Emitter *emitter = NULL;
   
   // standard area light; emittance distribution restricted by surface normal
   if (m_emitter == "oriented") {
      emitter = new OrientedEmitter(pt, this);
   } else if (m_emitter == "omni" || m_emitter == "point") {
      emitter = new OmniEmitter(pt, this);
   } else if (m_emitter == "environment") {
      emitter = new EnvironmentMap(this);
   } else {
      ASSERT(m_emitter == "null" && "Found Invalid Material (Emitter type)");
      
      return Material::s_nullEmitter;
   }
   
   emitter->init();
   return emitter;
}

Emitter *Material::getEmitter() {
   return getEmitter(s_nullSurfacePoint);
}

Sensor *Material::getSensor(SurfacePoint &pt) {
   Sensor *sensor = Material::s_nullSensor;
   Camera *camera = NULL;
   
   // TODO: make this not dependent on Camera...
   try {
      camera = dynamic_cast<Camera*>(pt.shape);
   } catch(std::bad_cast&) { }
   
   if (camera) {
      sensor = new Sensor(pt, this);
      sensor->init();
   }
   
   return sensor;
}

void Material::preview(Shape *shape) {
   SurfacePoint pt;
   pt.shape = shape;
   
   if (isEmitter()) {
      Emitter *emitter = getEmitter(pt);
      emitter->preview(shape);
      
      if (emitter != Material::s_nullEmitter)
         safeDelete(emitter);
   }
   
   BSDF *bsdf = getBSDF(pt);
   bsdf->preview(shape);
   
   safeDelete(bsdf);
}

void Material::initSurfacePoint(SurfacePoint &pt) {
   safeDelete(pt.bsdf);
   
   // TODO: abstract this out into SurfacePoint or Material
   if (pt.emitter != Material::s_nullEmitter)
      safeDelete(pt.emitter);
   if (pt.sensor != Material::s_nullSensor)
      safeDelete(pt.sensor);
   
   _initShadingNormal(pt);
   
   pt.bsdf    = getBSDF(pt);
   pt.emitter = getEmitter(pt);
   pt.sensor  = getSensor(pt);
   
   const SpectralSampleSet &ior = 
      getSpectralSampleSet("ior", IndexOfRefraction::AIR, pt);
   
   const unsigned index = Random::sampleInt(0, ior.getN());
   pt.ior2 = ior[index].value;
}

void Material::_initShadingNormal(SurfacePoint &pt) {
   // default to geometric normal if no bumpmap was specified
   if (m_bumpMap == "") {
      pt.normalS = pt.normalG;
      return;
   }
   
   // load or access cached version of bumpmap image
   const ImagePtr &image = ResourceManager::getImage(m_bumpMap);
   
   // if image failed to load successfully, default to geometric normal
   if (!image) {
      pt.normalS = pt.normalG;
      return;
   }
   
   // calculate bumpmap base coordinates
   const int width  = image->getWidth();
   const int height = image->getHeight();
   
   const real_t s = pt.uv.u * m_repeatU;
   const real_t t = pt.uv.v * m_repeatV;
   const int x = (int)(CLAMP((s - floor(s)) * width,  0, width  - 1));
   const int y = (int)(CLAMP((t - floor(t)) * height, 0, height - 1));
   
   // compute discrete partial derivatives
   const real_t xGrad = 
      image->getLuminance(y, x - (x > 0)) - 
      image->getLuminance(y, x + (x < width - 1));
   const real_t yGrad = 
      image->getLuminance(y - (y > 0), x) - 
      image->getLuminance(y + (y < height - 1), x);
   
   if (EQ(xGrad, 0.0) && EQ(yGrad, 0.0)) {
      pt.normalS = pt.normalG;
      return;
   }
   
   // construct orthonormal basis given geometric normal at samplepoint
   Vector3 N = pt.normalG;
   Vector3 U, V;
   
   N.getOrthonormalBasis(U, V);
   
   // scaleFactor modifies the intensity or magnitude of the bumps
   pt.normalS = (N + m_bumpIntensity * (xGrad * U + yGrad * V)).getNormalized();
}

SpectralSampleSet Material::getSpectralSampleSet(const std::string &key, 
                                                 const SpectralSampleSet &defaultValue, 
                                                 const SurfacePoint &pt)
{
   if (!contains(key))
      return boost::any_cast<const SpectralSampleSet&>((*this)[key] = defaultValue);
   
   return getSpectralSampleSet(key, pt);
}

SpectralSampleSet Material::getSpectralSampleSet(const std::string &key, 
                                                 const real_t &defaultValue, 
                                                 const SurfacePoint &pt)
{
   if (!contains(key)) {
      return boost::any_cast<const SpectralSampleSet&>((*this)[key] = 
                                                       SpectralSampleSet::fill(defaultValue));
   }
   
   return getSpectralSampleSet(key, pt);
}

SpectralSampleSet Material::getSpectralSampleSet(const std::string &key, 
                                                 const std::string &defaultValue, 
                                                 const SurfacePoint &pt)
{
   if (!contains(key))
      (*this)[key] = defaultValue;
   
   return getSpectralSampleSet(key, pt);
}

SpectralSampleSet Material::getSpectralSampleSet(const std::string &key, 
                                                 const SurfacePoint &pt)
{
   const boost::any &value = (*this)[key];
   
   if (value.type() == typeid(SpectralSampleSet)) {
      return boost::any_cast<SpectralSampleSet>(value);
   } else if (value.type() == typeid(std::string)) {
      const std::string &fileName = boost::any_cast<std::string>(value);
      const ImagePtr    &image    = ResourceManager::getImage(fileName);
      
      if (image)
         return SpectralSampleSet(getSample(image, pt.uv));
   } else if (value.type() == typeid(real_t)) {
      return SpectralSampleSet::fill(boost::any_cast<real_t>(value));
   } else {
      ASSERT(0 && "invalid spectrum type");
   }
   
   return SpectralSampleSet::fill(0.7);
}

RgbaHDR Material::getSample(const ImagePtr &image, const UV &uv)
{
   ASSERT(image);
   
   if (NULL == m_filter) {
      init();
      
      ASSERT(m_filter);
   }
   
   const unsigned width  = image->getWidth();
   const unsigned height = image->getHeight();
   
   const real_t s = uv.u * m_repeatU;
   const real_t t = uv.v * m_repeatV;
   const real_t x = CLAMP((s - floor(s)) * width,  0, width  - 1);
   const real_t y = CLAMP((t - floor(t)) * height, 0, height - 1);
   
   return m_filter->apply(image.get(), x, y);
}

void Material::setFilter(KernelFilter *filter) {
   if (filter != m_filter) {
      KernelFilter *oldFilter = m_filter;
      
      m_filter = filter;
      safeDelete(oldFilter);
   }
}

}

