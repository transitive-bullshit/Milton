/**<!-------------------------------------------------------------------->
   @class  PropertyMap
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Bare-bones map used throughout Milton for holding custom initialization 
   parameters/options/information
   
   @TODO:  Try using QHash instead of std::map for efficiency reasons?
   <!-------------------------------------------------------------------->**/

#ifndef PROPERTY_MAP_H_
#define PROPERTY_MAP_H_

#include <common/common.h>
#include <boost/any.hpp>
#include <ostream>
#include <map>

namespace milton {

class MILTON_DLL_EXPORT PropertyMap {
   protected:
      DECLARE_STL_TYPEDEF2(std::map<std::string, boost::any>, STLPropertyMap);
      
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   PropertyMap()
      { }
      
      inline   PropertyMap(const PropertyMap &copy) {
         // Note: cannot just call 'inherit' which does the same thing 
         // because it is virtual and the subclass' vtables may not be 
         // initialized yet..
         FOREACH(STLPropertyMapConstIter, copy.m_propertyMap, iter) {
            insert(iter->first, iter->second);
         }
      }
      
      virtual ~PropertyMap()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * Resets all properties to their default values
       */
      void clear();
      
      virtual void inherit(const PropertyMap &m, bool overwrite = true) {
         FOREACH(STLPropertyMapConstIter, m.m_propertyMap, iter) {
            const std::string &key = iter->first;
            
            if (overwrite || !contains(key))
               (*this)[key] = iter->second;
         }
      }
      
      /**
       * @returns whether or not this map contains a value for the given key
       */
      inline bool contains(const std::string &key) const {
         STLPropertyMapConstIter iter = m_propertyMap.find(key);
         
         return ((iter != m_propertyMap.end()) && !iter->second.empty());
      }
       
      /**
       * Inserts the given key, value pair into this map
       */
      template<typename T>
      inline void insert(const std::string &key, const T &value) {
         // value will implicitly be converted to boost::any
         //m_propertyMap.insert(STLPropertyMap::value_type(key, boost::any(value)));
         m_propertyMap[key] = value;
      }
      
      /**
       * @returns a reference to the value associated with the given key
       */
      template<typename T>
      inline T &getValue(const std::string &key) {
         ASSERT(contains(key));
         
         try {
            T &ret = boost::any_cast<T&>((*this)[key]);
            
            return ret;
         } catch(boost::bad_any_cast &e) {
            std::cerr << "PropertyMap::getValue(" << key << ") " << std::endl;
            ASSERT(0);
            throw e;
         }
      }
      
      /**
       * @returns a reference to the value associated with the given key or 
       *    the given defaultValue if no custom value exists
       */
      template<typename T>
      inline T &getValue(const std::string &key, const T &defaultValue) {
         if (!contains(key))
            m_propertyMap[key] = defaultValue;
         
         return boost::any_cast<T&>((*this)[key]);
      }
      
      /**
       * @returns a reference to the variant wrapper around the value 
       *    associated with the given key
       */
      inline boost::any &operator[](const std::string &key) {
         return m_propertyMap[key];
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
#ifdef MILTON_ARCH_WINDOWS
#  pragma warning(push)
#  pragma warning(disable : 4251)
#endif

      /// Internal std::map
      STLPropertyMap m_propertyMap;

#ifdef MILTON_ARCH_WINDOWS
#  pragma warning(pop)
#endif
      
      friend std::ostream &operator<<(std::ostream &os, const PropertyMap &m);
};

template<>
MILTON_DLL_EXPORT
inline unsigned &PropertyMap::getValue(const std::string &key, const unsigned &defaultValue) {
   if (!contains(key))
      m_propertyMap[key] = defaultValue;
   
   boost::any &v = m_propertyMap[key];
   
   try {
      return boost::any_cast<unsigned&>(v);
   } catch(boost::bad_any_cast &) {
      v = static_cast<unsigned>(boost::any_cast<int>(v));
      
      return boost::any_cast<unsigned&>(v);
   }
}

template<>
MILTON_DLL_EXPORT
inline real_t &PropertyMap::getValue(const std::string &key, const real_t &defaultValue) {
   if (!contains(key))
      m_propertyMap[key] = defaultValue;
   
   boost::any &v = m_propertyMap[key];
   
   if (v.type() == typeid(real_t))
      return boost::any_cast<real_t&>(v);
   
   if (v.type() == typeid(int)) {
      v = static_cast<real_t>(boost::any_cast<int>(v));
      return boost::any_cast<real_t&>(v);
   }
   
   if (v.type() == typeid(unsigned)) {
      v = static_cast<real_t>(boost::any_cast<unsigned>(v));
      return boost::any_cast<real_t&>(v);
   }
   
   if (v.type() == typeid(float)) {
      v = static_cast<real_t>(boost::any_cast<float>(v));
      return boost::any_cast<real_t&>(v);
   }
   
   if (v.type() == typeid(double)) {
      v = static_cast<real_t>(boost::any_cast<double>(v));
      return boost::any_cast<real_t&>(v);
   }
   
   ASSERT(0 && "PropertyMap::getValue<real_t> found invalid value\n\n");
   return boost::any_cast<real_t&>(v);
}

}

#endif // PROPERTY_MAP_H_

