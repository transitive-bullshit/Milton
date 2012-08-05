/**<!-------------------------------------------------------------------->
   @file   materials.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Convenience header which includes all public Material headers, 
   including BSDFs, Emitters, and Sensors
   <!-------------------------------------------------------------------->**/

#ifndef MATERIALS_H_
#define MATERIALS_H_

// generic Material interface
#include <materials/Material.h>

// concrete BSDF implementations
#include <materials/bsdfs/AbsorbentBSDF.h>
#include <materials/bsdfs/AggregateBSDF.h>
#include <materials/bsdfs/DiffuseBSDF.h>
#include <materials/bsdfs/ModifiedPhongBSDF.h>
#include <materials/bsdfs/DielectricBSDF.h>

// emitters (lights)
#include <materials/emitters/EnvironmentMap.h>
#include <materials/emitters/NullEmitter.h>
#include <materials/emitters/OmniEmitter.h>
#include <materials/emitters/OrientedEmitter.h>

// sensors (cameras)
#include <materials/sensors/NullSensor.h>

// miscellaneous
#include <materials/IndexOfRefraction.h>
#include <materials/bsdfs/Fresnel.h>

#endif // MATERIALS_H_

