#include <Stats.h>
#include <Materials.h>
#include <Spectrum.h>
#include <Shapes.h>
#include <iostream>
#include <QImage>
using namespace std;

int main(int argc, char** argv) {
   Random::init();
   
   Shape *s = new Plane();
   //Triangle *s = new Triangle(Vertex(0,0,0), Vertex(0,1,0), Vertex(0,1,1));
   Material *m = new Material();
   m->insert<std::string>("bsdf", std::string("diffuse"));
   m->insert<Spectrum>   ("kd", Spectrum::fill(1.0));
   s->setMaterial(m);
   
   // Initialize a fictitious SamplePoint
   SamplePoint pt;
   pt.position = Point3(0, 0, 0);
   pt.shape    = s;
   pt.init();
   BSDF *bsdf = pt.bsdf;
   
   cout << pt << endl;
   
   for(int i = 10000; i--;) {
      const Event &event = bsdf->sample();
      const Vector3 &wo  = event;
      const double pdf   = bsdf->getPdf(event);
      const Spectrum &fs = bsdf->getBSDF(wo);
      
      assert(wo.isUnit());
      assert(wo.dot(pt.normalS) > 0);
      cout << wo.dot(pt.normalS) << endl;
      //cout << wo << " \t" << pdf << " \t" << fs << endl;
   }
   
   return 0;
}

