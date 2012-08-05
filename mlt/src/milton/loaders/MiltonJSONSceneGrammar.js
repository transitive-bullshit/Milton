root      = scenefile;
scenefile = { version?, renderer?, camera?, output?, scene };

variant   = type, property*;
property  = string : primitive;
primitive = null | boolean | int | double | string;

path      = string;  // could separate btwn relative and absolute paths
point3    = [ double, double, double ];
vector3   = [ double, double, double ];
color3    = [ double, double, double ]; // valid within [0, 1]^3
spectrum  = [ double* ]; // variable-length
clampedSpectrum = double | path | [ double* ]; // valid within [0, 1]*

version   = int | double;

// if no renderer is specified, it will default to an OpenGL preview
renderer  = { variant };
   renderer.type = "preview" | "rayCaster" | "rayTracer" | "pathTracer" | "bidirectionalPathTracer" | "bidirPathTracer" | "photonMapper" | "mlt" | "MLT";
   renderer.type["*"].noDirectSamples = uint;
   renderer.type["*"].noRenderThreads = uint;
   renderer.type["*"].noSuperSamples  = uint; // if directSampleGenerator == "super"
   renderer.type["*"].directSampleGenerator = string; // generates direct illum rays for area lights
   renderer.type["*"].generator = string; // generates primary rays
   
   renderer.type["rayCaster"].ambient   = spectrum;
   renderer.type["rayTracer"].maxDepth  = uint;
   renderer.type["rayTracer"].ambient   = spectrum;
   
   renderer.type["pathTracer"].efficientDirect = boolean;
   
   renderer.type["bidirPathTracer"] = renderer.type["bidirectionalPathTracer"];
   
   // TODO: specializations for mlt
   renderer.type["photonMapper"].diffuseNoGatherPhotons = uint;
   renderer.type["photonMapper"].causticNoGatherPhotons = uint;
   renderer.type["photonMapper"].diffuseGatherRadius    = double;
   renderer.type["photonMapper"].causticGatherRadius    = double;
   renderer.type["photonMapper"].diffuseNoPhotons       = uint;
   renderer.type["photonMapper"].causticNoPhotons       = uint;

camera    = { variant };
   camera.type = "pinhole" | "thinlens";
   camera.type["pinhole|thinlens"].eye   = point3;
   camera.type["pinhole|thinlens"].focus = point3;
   camera.type["pinhole|thinlens"].look  = vector3; // TODO: (look | focus)
   camera.type["pinhole|thinlens"].up    = vector3;
   camera.type["pinhole|thinlens"].heightAngle = double; // specified in degrees
   camera.type["pinhole|thinlens"].aspectRatio = double;
   camera.type["pinhole|thinlens"].near  = double;
   camera.type["pinhole|thinlens"].far   = double;
   
   camera.type["thinlens"].aperture      = double;  // specified in mm (eg. 35mm or 50mm)
   camera.type["thinlens"].fstop         = double;  // unitless
   camera.type["thinlens"].focalDistance = double;  // specified in m; optional
   camera.type["thinlens"].focalPoint    = vector2; // specified in NDC on [0, 1]^2
                                                    // optional for "auto-focus"

// if no output is specified, it will default to gui
output    = { variant };
   output.type   = "naive" | "reconstruction";
   output.width  = uint;
   output.height = uint;
   
   output.type["reconstruction"].filter = filter;

filter    = { variant };
   filter.type = "box" | "triangle" | "gaussian" | "mitchell" | "lanczosSinc";
   filter.type["*"].support = uint;
   filter.type["gaussian"].sigma  = double;
   filter.type["mitchell"].B      = double;
   filter.type["mitchell"].C      = double;
   filter.type["lanczosSinc"].tau = double;

scene     = { node*, background? };
node      = material | transform | shape;
   node.name = "string";

transform = { ((translate | rotate | scale)* | arbitraryTransform)?, node };
arbitraryTransform = [ double, double, double, double, double, double, double, double, 
                       double, double, double, double, double, double, double, double ];

translate = vector3;
rotate    = { point, axis, angle } | [ double, double, double, 
                                       double, double, double, 
                                       double ];
   rotate.point = point3;
   rotate.axis  = vector3;
   rotate.angle = double;  // specified in degrees
scale     = vector3;

shape     = { variant, material? };
   shape.type = "triangle" | "plane"  | "point" | "cube" | "cone" | "mesh" | "cylinder" | "sphere" | "blob" | "shapeSet";
   shape.type["plane"].vertices    = [ double, double, double, double, double, double, 
                                       double, double, double, double, double, double ];
   shape.type["triangle"].vertices = [ double, double, double, double, double, double, 
                                       double, double, double ]; // x, y, z
   shape.type["triangle"].normals  = [ double, double, double, double, double, double, 
                                       double, double, double ]; // x, y, z
   shape.type["triangle"].uvs      = [ double, double, double, double, double, double ];
   shape.type["triangle"].normal   = double; // constant over face of triangle
   shape.type["sphere"].position   = point3;
   shape.type["sphere"].radius     = double;
   shape.type["blob"]      = [ shape.type["blob"].meta*, material? ];
   shape.type["blob"].threshold  = double;  // valid within [0, 0.2)
   shape.type["blob"].resolution = vector3; // TODO!
   shape.type["blob"].metaObjects = [ variant ];
      shape.type["blob"].meta.type = "ball"; // TODO: possibly add tube, plane, cube
      shape.type["blob"].meta.type["*"].negative    = boolean;
      shape.type["blob"].meta.type["*"].strength    = double;
      shape.type["blob"].meta.type["ball"].position = vector3;
      shape.type["blob"].meta.type["ball"].radius   = double;
   
   shape.type["mesh"].path = path;
   
   shape.type["shapeSet"]  = node;
   shape.type["shapeSet"].spatialAccel = { variant };
      shape.type.shapeSet.spatialAccel.type = "naive" | "kdTree";
      shape.type.shapeSet.spatialAccel.type["kdTree"].kdSplitPlaneType = 
         "splitPlaneMiddle" | "splitPlaneMedian" | "splitPlaneSAH";
      shape.type.shapeSet.spatialAccel.type["kdTree"].kdSplitAxisType = 
         "splitAxisRoundRobin" | "splitAxisLongestExtent";
      shape.type.shapeSet.spatialAccel.type["kdTree"].kdMinPrimitives = uint;
      shape.type.shapeSet.spatialAccel.type["kdTree"].kdMaxDepth      = uint;
      shape.type.shapeSet.spatialAccel.type["kdTree"].kdNoThreads     = uint;
      shape.type.shapeSet.spatialAccel.type["kdTree"].kdPostCompress  = boolean;
      shape.type.shapeSet.spatialAccel.type["kdTree"].kdCostTraversal = double;
      shape.type.shapeSet.spatialAccel.type["kdTree"].kdCostIntersect = double;
      shape.type.shapeSet.spatialAccel.type["kdTree"].kdEmptyBias     = double;

material  = { bsdf, emitter?, node? };
   material.bumpMap        = path;
   material.bumpIntensity  = double; // (0, inf), default 5.0
   material.medium         = medium;
   material.filter         = string;
   material.repeatU        = double;
   material.repeatV        = double;

bsdf      = { variant };
   bsdf.type = "absorbent" | "diffuse" | "dielectric" | "specular" | "transmissive" | "modifiedPhong" | "phong";
   bsdf.type["diffuse"].kd             = clampedSpectrum;
   bsdf.type["dielectric"].ks          = clampedSpectrum;
   bsdf.type["dieletric"].transparency = clampedSpectrum;
   bsdf.type["phong"].kd               = clampedSpectrum;
   bsdf.type["phong"].ks               = clampedSpectrum;
   bsdf.type["phong"].n                = spectrum; // [0, inf)*
   // convenience aliases
   bsdf.type["specular"]     = bsdf.type["dielectric"] with transparency=white;
   bsdf.type["transmissive"] = bsdf.type["dielectric"] with transparency=black;

emitter   = { variant };
   emitter.type = "null" | "omni" | "oriented";
   emitter.type["*"].power = spectrum | double; // if single double, fill3(double)
   emitter.type["*"].glDiffuseColor  = color3;
   emitter.type["*"].glSpecularColor = color3;
   emitter.type["*"].glAttenuation   = vector3;

medium    = { variant };
   medium.type = "homogenous";
   medium.type["*"].ior = spectrum;

generator = { variant };
   generator.type = "uniform" | "stochastic" | "jittered" | "super" | "dissolve" | "hilbert";
   generator["*"].binWidth  = double;
   generator["*"].binHeight = double;
   generator["*"].binSize   = double;

background = { emitter? };
   background.emitter.type += | "environment";
   emitter.type["environment"].coords = "spherical" | "latLong";
   emitter.type["environment"].path   = path;

// allow all variant types to be specified as dynamic plugins at runtime
*.type += | "dynamic";
*.type["dynamic"].path = path;

