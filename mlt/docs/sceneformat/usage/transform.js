"transform" : {
   // scale a unit cube uniformly by 1/2, then rotate about the x-axis by 90 degrees, 
   // then translate up three units about the new x axis
   // note: if multiple transformations are listed in a single transform block,
   // all rotations will be applied first, then all scales, then all translations
   "translate" : [ 3, 0, 0 ], 
   "rotate" : [ 0, 0, 0, 1, 0, 0, 90 ], 
   "scale" : [ .5, .5, .5 ], 
   
   // cube in unit box from [ -.5, -.5, -.5 ] to [ .5, .5, .5 ]
   "cube" : { }
   
   // ... (elided) other nodes which this transform should be applied to
}, 

