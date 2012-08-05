/**<!-------------------------------------------------------------------->
   @class  PointSample
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents the value of a generic function ({x,y} -> any) evaluated at a 
   particular 2D point
   <!-------------------------------------------------------------------->**/

#ifndef POINT_SAMPLE_H_
#define POINT_SAMPLE_H_

#include <common/math/Point.h>
#include <stats/Event.h>
#include <deque>

namespace milton {

struct MILTON_DLL_EXPORT PointSample {
   
   ///@name Public data
   //@{-----------------------------------------------------------------
   
   Point2 position;
   Event  value;
   bool   update;
   bool   save;
   
   
   //@}-----------------------------------------------------------------
   ///@name Constructors
   //@{-----------------------------------------------------------------
   
   inline PointSample(const Point2 &position_, const Event &value_, 
                      bool update_ = false, bool save_ = false)
      : position(position_), value(value_), update(update_), save(save_)
   { }
   
   inline PointSample(const Point2 &position_, 
                      bool update_ = false, bool save_ = false)
      : position(position_), value(), update(update_), save(save_)
   { }
   
   inline PointSample(const real_t x, const real_t y, const Event &value_, 
                      bool update_ = false, bool save_ = false)
      : position(x, y), value(value_), update(update_), save(save_)
   { }
   
   inline PointSample(const real_t x, const real_t y, 
                      bool update_ = false, bool save_ = false)
      : position(x, y), value(), update(update_), save(save_)
   { }
   
   inline PointSample()
      : value(), update(false), save(false)
   { }
   
   
   //@}-----------------------------------------------------------------
};

DECLARE_STL_TYPEDEF(std::vector<PointSample>, PointSampleList);
DECLARE_STL_TYPEDEF(std::deque<PointSample>,  PointSampleQueue);

}

#endif // POINT_SAMPLE_H_

