"noRenderThreads" : {
   "_brief" : "Number of core rendering threads to use during rendering", 
   "_desc"  : "Rendering in Milton is highly concurrent and is designed from the ground up to scale well with an increasing number of processors on the underlying machine. Choosing too few threads will drastically affect the performance of Milton by not taking advantage of all available resources. On the other hand, choosing too many threads may bottleneck the system and cause cache / memory thrashing from frequent time sharing between the threads across an insufficient number of available processors. <br />A good rule of thumb is to choose noRenderThreads to be about 2 times the number of available processors.  i.e., if you're on a quad-core machine, setting noRenderThreads to 8 or 10 will yield optional usage of the 4 available processors.", 
   "_info" : { "type" : "uint", "optional" : true, "default" : 8 }
}, 
"noDirectSamples" : {
   "_desc" : "Number of direct \"shadow rays\" to sample from area light sources during evaluation of direct illumination", 
   "_info" : { "type" : "uint", "optional" : true, "default" : 1 }
}, 
"directSampleGenerator" : {
   "_info" : { "type" : "generator", "optional" : true, "default" : "jittered" }, 
}, 
"generator" : {
   "_brief" : "generator to use to generate samples over the film plane", 
   "_info" : { "type" : "generator", "optional" : true, "default" : "super" }
}, 

