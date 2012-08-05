#include <Stats.h>
#include <Materials.h>
#include <Shapes.h>
#include <iostream>
#include <QImage>
using namespace std;

int main(int argc, char** argv) {
   Random::init();
   
   // TODO: energy-conserving phong
   const char *const bsdfs[] = { 
      "diffuse", "absorbent", "specular", NULL, 
      //"modifiedPhong", NULL
   };
   
   unsigned int i = 0;
   const char *bsdfStr = NULL;
   Material material;
   
   // Initialize a fictitious SamplePoint
   SamplePoint p, p2;
   
   p.shape   = new Sphere(&material);
   p.shape->init();
   
   p2.shape  = new Sphere(&material);
   p2.shape->init();
   
   while(NULL != (bsdfStr = bsdfs[i++])) {
      material["bsdf"] = std::string(bsdfStr);
      
      cerr << "Testing BSDF: " << bsdfStr << endl;
      Spectrum reflected = Spectrum::black();
      unsigned long long noSamples = 0;
      
      // sample hemisphere of incident directions
      for(double theta = 0; theta < M_PI / 2; theta += M_PI / 2 / 100)
      {
         for(double phi = 0; phi < 2 * M_PI; phi += 2 * M_PI / 100, ++noSamples)
         {
            Vector3 v(Vector3::fromSpherical(theta, phi));
            Point3  origin(v[0], v[1], v[2], 1);
            
            p.ray = Ray(origin, (Point3() - origin).getNormalized());
            p.ior = 1;
            p.t   = p.shape->getIntersection(p.ray, p);
            
            ASSERT(p.init());
            
            BSDFPtr bsdf = material.getBSDF(p);
            ASSERT(bsdf.get());
            bsdf->init();
            
            const Event &x    = bsdf->sample();
            const Vector3 &wo = x;
            const double pdf  = bsdf->getPdf(x);
            const Spectrum &fraction = bsdf->getBSDF(wo);
            
            ASSERT(pdf >= -EPSILON);
            if (EQ(pdf, 0))
               continue;
            
            ASSERT(wo.isUnit());
            ASSERT(p.wi.isUnit());
            
            reflected += fraction * (MAX(0, -p.wi.dot(p.normalG))) / pdf;
            
            // test symmetry
            p2.ray = Ray(Point3() + wo, -wo);
            p2.ior = 1;
            p2.t   = p2.shape->getIntersection(p2.ray, p2);
            
            p2.init();
            
            BSDFPtr symmetric = material.getBSDF(p2);
            const Spectrum &symmetricFraction = symmetric->getBSDF(-p.wi);
            
            ASSERT(fraction == symmetricFraction); // fuzzy equality
         }
      }
      
      reflected /= noSamples;
      
      cerr << "E[total reflected] = " << reflected << endl;
      // test conservation of energy
      ASSERT(reflected[reflected.getMaxFrequency()] <= 1 + EPSILON);
      
      cerr << endl;
   }
   
   return 0;
}

