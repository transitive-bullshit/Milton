/**<!-------------------------------------------------------------------->
   @file   LSystem.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   December 2006
   
   @brief
      Models a Lindenmayer System, which may be used to simulate a varient of 
   effects in Nature, such as plant growth and fractals.
   <!-------------------------------------------------------------------->**/

#include "LSystem.h"

namespace milton {

std::string LSystem::_process(const std::string &state, unsigned depth) {
   if (depth <= 0)
      return state;
   
   std::string next = "";
   
   for(unsigned i = 0; i < state.length(); ++i) {
      const char cur = state[i];
      
      if (m_rules.find(cur) != std::string::npos)
         next += _processRule(cur);
      else next += cur;
   }
   
   return _process(next, depth - 1);
}

}

