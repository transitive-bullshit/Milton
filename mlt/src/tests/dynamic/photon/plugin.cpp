/**<!-------------------------------------------------------------------->
   @file   plugin.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Tests dynamic plugin loading within Milton via an external Photon 
   Mapping plugin
   <!-------------------------------------------------------------------->**/

#include <milton.h>
#include <PhotonMapper.h>
#include <QtCore>

#ifdef __cplusplus
   extern "C" {
#endif

MILTON_PLUGIN_TYPE plugin_type() {
   return MILTON_PLUGIN_TYPE_RENDERER;
}

void *plugin_new() {
   return new PhotonMapper();
}

void  plugin_delete(void *plugin) {
   ASSERT(plugin);
   
   PhotonMapper *p = (PhotonMapper *)plugin;
   safeDelete(p);
}

#ifdef __cplusplus
   }
#endif

