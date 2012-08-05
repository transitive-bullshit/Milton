/**<!-------------------------------------------------------------------->
   @file   PropertyMap.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Bare-bones map used throughout Milton for holding custom initialization 
   parameters/options/information
   <!-------------------------------------------------------------------->**/

#include "PropertyMap.h"
#include <typeinfo>

using namespace std;

namespace milton {

void PropertyMap::clear() {
   // TODO: ensure no memory leak here
   m_propertyMap.clear();
}

std::ostream &operator<<(std::ostream &os, const PropertyMap &m) {
   os << "{ " << endl;
   
   FOREACH(PropertyMap::STLPropertyMapConstIter, m.m_propertyMap, iter) {
      os << "  " << iter->first;
      
      if (iter->second.type() == typeid(std::string))
         os << " = " << boost::any_cast<std::string>(iter->second);
      else if (iter->second.type() == typeid(unsigned))
         os << " = " << boost::any_cast<unsigned>(iter->second);
      else if (iter->second.type() == typeid(int))
         os << " = " << boost::any_cast<int>(iter->second);
      else if (iter->second.type() == typeid(double))
         os << " = " << boost::any_cast<double>(iter->second);
      else if (iter->second.type() == typeid(float))
         os << " = " << boost::any_cast<float>(iter->second);
      else if (iter->second.type() == typeid(char))
         os << " = " << boost::any_cast<char>(iter->second);
      else 
         os << " = ?";
      
      os << endl;
   }
   
   os << "}";
   return os;
}

}

