/**<!-------------------------------------------------------------------->
   @class  LSystem
   @author Travis Fischer (fisch0920@gmail.com)
   @date   December 2006
   
   @brief
      Models a Lindenmayer System, which may be used to simulate a varient of 
   effects in Nature, such as plant growth and fractals.
   <!-------------------------------------------------------------------->**/

#ifndef LSYSTEM_H_
#define LSYSTEM_H_

#include <common/common.h>

namespace milton {

class MILTON_DLL_EXPORT LSystem {
   
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline LSystem(const std::string &rules, const std::string &axiom)
         : m_rules(rules), m_axiom(axiom)
      { }
      
      virtual ~LSystem()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      inline std::string process(unsigned maxDepth) {
         return _process(m_axiom, maxDepth);
      }
      
      
      //@}-----------------------------------------------------------------
   protected:
      std::string _process(const std::string &state, unsigned depth);
      virtual std::string _processRule(char c) = 0;
      
   protected:
      std::string m_rules;
      std::string m_axiom;
};

}

#endif // LSYSTEM_H_

