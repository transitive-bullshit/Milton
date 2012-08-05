"renderer" : {
   // example of external renderer plugin dynamically loaded at run-time
   "type" : "dynamic", 
   
   // path to dynamic library conforming to milton's plugin format
   "path" : "photon/libphoton.so", 
   
   // arbitrary inputs to external renderer plugin, some of which may 
   // be shared by built-in renderers
   "noRenderThreads" : 10, 
   "noDirectSamples" : 1, 
   
   "noSuperSamples"  : 16, 
   "generator" : "super", 
   
   "diffuseGatherRadius" : 50, 
   "causticGatherRadius" : 0, 
},

