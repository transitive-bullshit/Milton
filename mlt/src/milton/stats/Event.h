/**<!-------------------------------------------------------------------->
   @class  Event
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Represents a single event which was sampled from a Sampler. The 
   internal value of this event is stored as a variant using boost::any, 
   where the concrete value of type T can be extracted using 
   Event<T>::getValue()
   
      This makes it easy to have all Samplers conform to the same sampling 
   interface, which facilitates the implementation of more abstract sampling 
   algorithms without respect to specific Sampler implementations.  One 
   example of this generic Sampling interface being useful is generic 
   multiple importance sampling (see MultipleImportanceSampler)
   <!-------------------------------------------------------------------->**/

#ifndef EVENT_H_
#define EVENT_H_

#include <common/common.h>
#include <common/math/Vector.h>
#include <utils/SpectralSampleSet.h>
#include <boost/any.hpp>

namespace milton {

class Sampler;

class MILTON_DLL_EXPORT Event {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      /**
       * @param x is the value of this random event
       * @param randomVar is the sampler from which this random event was 
       *    generated
       * @param metadata is optional sampler-specific metadata associated 
       *    with this event
       * @param p is the probability with which this event occurred
       */
      inline Event(const boost::any &x, Sampler *randomVar = NULL, 
                   const boost::any &metadata = boost::any(), 
                   real_t p = -1)
         : m_x(x), m_metadata(metadata), m_sampler(randomVar), m_p(p)
      { }
      
      inline Event(const boost::any &x, const Event &event)
         : m_x(x), m_metadata(event.m_metadata), m_sampler(event.m_sampler), 
           m_p(-1)
      { }
      
      inline Event(Sampler *randomVar = NULL, 
                   const boost::any &metadata = boost::any())
         : m_x(), m_metadata(metadata), m_sampler(randomVar), m_p(-1)
      { }
      
      virtual ~Event()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      template<typename T>
      inline T getValue() const {
         ASSERT(hasValue());
         
         return boost::any_cast<T>(m_x);
      }
      
      template<typename T>
      inline T getMetadata() const {
         ASSERT(hasMetadata());
         
         return boost::any_cast<T>(m_metadata);
      }
      
      /**
       * @returns a cache of the probability with which this event occurred
       *    (initializes the cache of it hasn't been computed yet)
       */
      real_t getPd();
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors/Mutators
      //@{-----------------------------------------------------------------
      
      inline boost::any getValue() const {
         return m_x;
      }
      
      inline void setValue(const boost::any &x) {
         m_x = x;
      }
      
      inline bool hasValue() const {
         return (!m_x.empty());
      }
      
      inline Sampler *getParent() {
         return m_sampler;
      }
      
      inline void setParent(Sampler *randomVar) {
         m_sampler = randomVar;
      }
      
      inline boost::any getMetadata() const {
         return m_metadata;
      }
      
      inline void setMetadata(const boost::any &metadata) {
         m_metadata = metadata;
      }
      
      inline bool hasMetadata() const {
         return (!m_metadata.empty());
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Convenience implicit conversions for common Event types
      //@{-----------------------------------------------------------------
      
      /// @returns this Event as a real_t (implicit conversion) for 
      ///    convenience purposes
      inline operator real_t() const {
         return getValue<real_t>();
      }
      
      /// @returns this Event as an int (implicit conversion) for 
      ///    convenience purposes
      inline operator int() const {
         return getValue<int>();
      }
      
      /// @returns this Event as a Vector3 (implicit conversion) for 
      ///    convenience purposes
      inline operator Vector3() const {
         return getValue<Vector3>();
      }
      
      /// @returns this Event as a SpectralSampleSet (implicit conversion) for 
      ///    convenience purposes
      inline operator SpectralSampleSet() const {
         return getValue<SpectralSampleSet>();
      }
      
      //@}-----------------------------------------------------------------
      
   protected:
      /// internal value variant
      boost::any m_x;
      
      /// internal sampler-specific metadata associated with this event
      /// note: may store information about how this event was generated 
      /// for determining the probability of the event
      boost::any m_metadata;
      
      /// parent random distribution from which this event was sampled
      Sampler   *m_sampler;
      
      /// probability with which this event occurred with respect to parent 
      /// sampling distribution
      real_t     m_p;
};

}

#endif // EVENT_H_

