"eye" : {
   "_desc" : "Camera location (position of eye)", 
   "_info" : { "type" : "vector3", "optional" : true, "default" : [ 2, 2, 1 ] }
}, 
"focus" : {
   "_desc" : "Point where the camera should point towards (implicitly specifying look vector)", 
   "_info" : { "type" : "vector3", "optional" : true, "default" : "origin" }
}, 
"look" : {
   "_desc" : "Camera look direction", 
   "_note" : "If \"focus\" is specified, it will override the value of \"look\"", 
   "_info" : { "type" : "vector3", "optional" : true, "default" : "towards origin" }
}, 
"up" : {
   "_brief" : "Camera up direction (orients camera)", 
   "_info"  : { "type" : "vector3", "optional" : true, "default" : [ 0, 1, 0 ], "cond" : "Must not be collinear to look vector" }
}, 
"heightAngle" : {
   "_desc" : "Vertical height angle of camera frustum", 
   "_info" : { "type" : "real", "optional" : true, "default" : 45, "units" : "degrees" }
}, 
"aspectRatio" : {
   "_desc" : "Aspect ratio of camera frustum", 
   "_info" : { "type" : "real", "optional" : true, "default" : 1.0 }
}, 
"near" : {
   "_desc" : "Near clipping plane for OpenGL preview", 
   "_info" : { "type" : "real", "optional" : true, "default" : 1.0 }
}, 
"far" : {
   "_desc" : "Far clipping plane for OpenGL preview", 
   "_info" : { "type" : "real", "optional" : true, "default" : 100.0 }
}, 
