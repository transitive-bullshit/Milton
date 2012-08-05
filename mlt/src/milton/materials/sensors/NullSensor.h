/**<!-------------------------------------------------------------------->
   @class  NullSensor
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Null sensor which doesn't emit any importance
   <!-------------------------------------------------------------------->**/

#ifndef NULL_SENSOR_H_
#define NULL_SENSOR_H_

#include <materials/Sensor.h>

namespace milton {

class MILTON_DLL_EXPORT NullSensor : public Sensor {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   NullSensor(SurfacePoint &pt, Material *parent = NULL)
         : Sensor(pt, parent)
      { }
      
      virtual ~NullSensor() 
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual bool isSensor();
      
      virtual SpectralSampleSet getWe(const Vector3 &wo);
      
      virtual SpectralSampleSet getImportance();
      
      
      //@}-----------------------------------------------------------------
      ///@name Sampling interface
      //@{-----------------------------------------------------------------
      
      virtual Event sample(const Event &);
      
      virtual real_t getPd(const Event &event);
      
      virtual SpectralSampleSet evaluate(const Vector3 &wi, const Vector3 &wo);
      
      
      //@}-----------------------------------------------------------------
};

}

#endif // NULL_SENSOR_H_

