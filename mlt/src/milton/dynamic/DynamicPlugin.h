/**<!-------------------------------------------------------------------->
   @file   DynamicPlugin.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Wrapper around a dynamic, externally-loaded plugin for Milton
   
      Every external Milton plugin must conform to the following interface, 
   comprised of three functions: plugin_type, plugin_new, and plugin_delete.
   When a 'dynamic' type is found in a scenefile for a renderer, camera, 
   output, etc.. (see MILTON_PLUGIN_TYPE for a complete list of available 
   dynamic plugin types), Milton will attempt to load the dynamic library 
   specified by the 'path' attribute of that dynamic object in the scenefile, 
   which is expected to contain a single plugin for which plugin_type should 
   return the expected object type required by the reference location in the 
   scenefile. plugin_new will then be called to instantiate an object which 
   is assumed to adhere to the interface specified by plugin_type() 
   (returning a 'new'ed object subclassing the corresponding Milton 
   superclass).  As of writing this (Spring 2009), plugin_delete will never 
   actually be called, but plugin authors are encouraged to conform to this 
   simple interface and include a sensible cleanup handler for plugin_delete 
   anyway, assuming the pointer passed in was previously allocated via a call 
   to plugin_new.
   <!-------------------------------------------------------------------->**/

#ifndef MILTON_DYNAMIC_PLUGIN_H_
#define MILTON_DYNAMIC_PLUGIN_H_

#include <common/common.h>

namespace milton {

#ifdef __cplusplus
   extern "C" {  // turn off name mangling
#endif
      
      // available dynamic plugin types
      enum MILTON_PLUGIN_TYPE {
         MILTON_PLUGIN_TYPE_RENDERER = 0, 
         MILTON_PLUGIN_TYPE_CAMERA, 
         MILTON_PLUGIN_TYPE_OUTPUT, 
         MILTON_PLUGIN_TYPE_FILTER, 
         MILTON_PLUGIN_TYPE_SHAPE, 
         MILTON_PLUGIN_TYPE_BSDF, 
         MILTON_PLUGIN_TYPE_EMITTER, 
         MILTON_PLUGIN_TYPE_MEDIUM, 
         MILTON_PLUGIN_TYPE_GENERATOR, 
         MILTON_PLUGIN_TYPE_METAOBJECT, 
      };
      
      extern MILTON_DLL_EXPORT MILTON_PLUGIN_TYPE plugin_type();
      extern MILTON_DLL_EXPORT void *plugin_new();
      extern MILTON_DLL_EXPORT void  plugin_delete(void *plugin);
      
      typedef MILTON_PLUGIN_TYPE (*plugin_type_func_t)();
      typedef void * (*plugin_new_func_t)();
      typedef void * (*plugin_delete_func_t)();
      
#ifdef __cplusplus
   }  // end of extern "C"
#endif

}

#endif // MILTON_DYNAMIC_PLUGIN_H_

