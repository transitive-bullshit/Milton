var format = {
   /**
    * Conventions used throughout this document:
    *    _brief : brief description of this element
    *    _desc  : more in-depth description of this element
    *    _note  : notes relating to this element
    *    _info  : [ "type" : string, "optional" : boolean, "default" : value, "units" : string, "cond" : string describing restrictions on the valid range of values for this element ]
    *    _usage : string depicting example usage of this element in a valid scenefile
    *    _class : string naming a related C++ class in Milton library / docs to link to
    *    _image : { ("_path" : string)*, "_caption" : path }
    *    #include : string path to file containing js to #include verbatim as a preprocess
    */
   "%scenefile" : {
      "_brief" : "Root element which contains the %scene file description; All valid %scene files must contain a single scenefile element.", 
      "_desc"  : "The main component of the scenefile is the scene element, which specifies all geometry and materials (including light sources) in the scene. You may optionally also specify camera, output, and/or renderer elements, and for each that's left unspecified, a sensible default will be used.", 
      "_info"  : { "optional" : false }, 
      "_usage1" : "usage/scenefile.js", 
      "_usage2" : "usage/scenefile2.js", 
      
      "version" : {
         "_desc"  : "Version of the Milton %scenefile format that this %scenefile conforms to", 
         "_info"  : { "type" : "real", "optional" : true, "default" : 1.0 }, 
         "_usage" : "usage/version.js"
      }, 
      
      "renderer" : {
         "_brief" : "Suggested renderer and relating settings to use for the scene specified in this %scenefile", 
         "_usage" : "usage/renderer.js", 
         "_desc"  : "Many renderer-specific parameters need to be fine-tuned on a scene-by-scene basis. The accuracy and efficiency of photonMapper, for example, is greatly dependent on scene dimensions and geometry. Scene-specific renderer parameters may be specified in the renderer block, and any extraneous parameters unrelated to a specific renderer will be ignored at render-time.", 
         "_info"  : { "type" : "renderer", "optional" : "true", "default" : "if no renderer is specified and Milton is run in 'nox' mode, pathTracer will be used as the default; if Milton is run in gui mode, preview (OpenGLRenderer) will be used as the default." }
      }, 
      
      "camera" : {
         "_brief" : "Virtual camera to be used to render the scene specified in this %scenefile", 
         "_usage" : "usage/camera.js", 
         "_info"  : { "type" : "camera", "optional" : "true", "default" : "if no camera is specified, a default pinhole camera will be used looking at the origin." }
      }, 
      
      "output" : {
         "_brief" : "Specifies an output format for rendering the scene in this %scenefile", 
         "_usage" : "usage/output.js", 
         "_info"  : { "optional" : "true", "default" : "If no output is specified, the output size default to 480x480, and the output format depends on Milton's configuration and environment. If running on 'nox' mode, output will default to a file \"out.png\" unless otherwise specified via the \"-output\" commandline flag. If running in gui mode, rendering output will default to an ImageCanvas which allows the user to view the progress of the progressive renderer. Note that OpenGLRenderer does not support 'nox' mode as it is inherently interactive." }
      }, 
      
      "scene" : {
         "_brief" : "Contains all shape and material data representing the virtual scene", 
         /*"_usage" : "usage/scene.js", */
         "_info"  : { "type" : "scene", "optional" : "false", "default" : "Defaults to a rather boring, empty scene -- spice it up!!!" }, 
      }
   }, 
   
   /* ------------------------------------------ *
    *                  Renderer                  *
    * ------------------------------------------ */
   "#renderer" : {
      "_brief" : "Specifies a renderer to use for rendering or previewing a scene", 
      "_usage" : "usage/renderer.js", 
      "_class" : "Renderer", 
      
      "_type"  : {
         /*#include "include/dynamic.js"*/
         
         "preview" : {
            "_desc"  : "OpenGL scene preview renderer which can also be specified via the commandline flag --preview", 
            "_class" : "OpenGLRenderer"
         }, 
         "rayCaster" : {
            "_desc"  : "Simple, non-recursive raycaster", 
            "_class" : "RayCaster", 
            /*#include "include/pointSampleRenderer.js"*/
            /*#include "include/ambient.js"*/
         }, 
         "rayTracer" : {
            "_desc"  : "Classic Whitted-style recursive raytracer", 
            "_note"  : "For a concise introduction to raytracing, see http://www.raytracing.co.uk/study/ray_intro.htm", 
            "_class" : "WhittedRayTracer", 
            /*#include "include/pointSampleRenderer.js"*/
            /*#include "include/ambient.js"*/
            
            "noIndirectSamples" : {
               "_desc" : "Number of secondary samples to generate at an intersection point to approximate indirect illumination", 
               "_note" : "The number of rays generated during Whitted-style raytracing grows exponentially with depth, so it's generallly a good idea to keep noIndirectSamples very small (1 or 2).", 
               "_info" : { "type" : "uint", "optional" : true, "default" : 1 }
            }, 
            "maxDepth" : {
               "_desc" : "Maximum recursion depth", 
               "_note" : "Whitted-style raytracing is biased as an estimator of the rendering equation precisely because it is a finite estimator of an infinite series.", 
               "_info" : { "type" : "uint", "optional" : true, "default" : 5 }
            }
         }, 
         "pathTracer" : {
            "_brief" : "Unbiased path tracer with support for efficient direct illumination", 
            "_desc"  : "Path tracing provides for a brute-force estimator of Kajiya's rendering equation by averaging the contributions from a large number of random \"light\" walks throughout the space of all paths through a scene. Certain paths end up being more important to the resulting image that our camera or eye sees, and for the most part, path tracing does a poor job of concentrating effort on these important paths. The path tracing implementation in Milton optionally allows for efficient evaluation of direct illumination (visually the most important paths contributing to the final image).", 
            "_class" : "PathTracer", 
            /*#include "include/pointSampleRenderer.js"*/
            
            "efficientDirect" : {
               "_desc" : "Whether or not to enable efficient sampling of direct illumination", 
               "_note" : "Path tracing converges much, much quicker in practice with efficient direct illumination enabled", 
               "_info" : { "type" : "boolean", "optional" : true, "default" : true }
            }
         }, 
         "bidirectionalPathTracer" : {
            "_brief" : "Unbiased bidirectional path tracer with support for efficient direct illumination", 
            "_desc"  : "Bidirectional path tracing is a fairly complex, unbiased estimator Kajiya's rendering equation, which averages the contributions from a large number of \"familes\" of random walks (paths) between light sources and the camera. Bidirectional path tracing makes use of multiple importance sampling to combine paths of different lengths generated by combining paths starting from light sources and the camera. Certain paths end up being more important to the resulting image that our camera or eye sees, and for the most part, path tracing does a poor job of concentrating effort on these important paths. The path tracing implementation in Milton optionally allows for efficient evaluation of direct illumination (visually the most important paths contributing to the final image).", 
            "_note"  : "The bidirectional path tracing implementation in Milton closely follows that of Veach & Guibas (1997).", 
            "_class" : "BidirectionalPathTracer", 
            /*#include "include/pointSampleRenderer.js"*/
         },
         "photonMapper" : {
            "_brief" : "Implementation of Henrik Wann Jensen's photon mapping algorithm.", 
            "_desc"  : "Photon Mapping is a two-pass global illumination algorithm in which photons are first traced from light sources and stored in a global kd-tree called the photon map. Final gathering is then performed by tracing paths from the eye and using photons from the photon map to approximate irradiance at points.", 
            "_note"  : [ "Photon mapping parameters are generally especially scene-dependent.", "Photon mapping treats diffuse and caustic paths separately, so many photonMapper-specific parameters have separate diffuse and caustic versions.", "Photon mapping is a consistent estimator of Kajiya's rendering equation, in that its expected value converges to the true value only in the limit as the number of samples -> infinity.", "See Jensen's book, \"Realistic Image Synthesis Using Photon Mapping\" (2001) for further details." ], 
            "_class" : "PhotonMapper", 
            /*#include "include/pointSampleRenderer.js"*/
            
            "diffuseNoGatherPhotons" : {
               "_desc" : "Maximum number of diffuse photons to be gathered during KNN for irradiance estimates from the diffuse photon map", 
               "_info" : { "type" : "uint", "optional" : true, "default" : 500 }
            }, 
            "causticNoGatherPhotons" : {
               "_desc" : "Maximum number of caustic photons to be gathered during KNN for irradiance estimates from the caustic photon map", 
               "_note" : "Because caustics are by their very nature a focusing event, you can generally set causticNoGatherPhotons to be much lower than diffuseNoGatherPhotons without compromising the quality of your render.", 
               "_info" : { "type" : "uint", "optional" : true, "default" : 300 }
            }, 
            "diffuseGatherRadius" : {
               "_desc" : "Maximum KNN search radius during irradiance estimates from the diffuse photon map", 
               "_note" : "This parameter depends <i>greatly</i> on the scale of your scene.", 
               "_info" : { "type" : "real+", "optional" : true, "default" : 1 }
            }, 
            "causticGatherRadius" : {
               "_desc" : "Maximum KNN search radius during irradiance estimates from the caustic photon map", 
               "_note" : "This parameter depends <i>greatly</i> on the scale of your scene.", 
               "_info" : { "type" : "real+", "optional" : true, "default" : 1 }
            }, 
            "diffuseNoPhotons" : {
               "_desc" : "Maximum number of (diffuse photons to shoot from emitters) and (photons to store in the diffuse photon map)", 
               "_info" : { "type" : "uint", "optional" : true, "default" : 60000 }
            },
            "causticNoPhotons" : {
               "_desc" : "Maximum number of (caustic photons to shoot from emitters) and (photons to store in the caustic photon map)", 
               "_info" : { "type" : "uint", "optional" : true, "default" : 60000 }
            }
         }
      }
   },
   
   /* ------------------------------------------ *
    *                   Camera                   *
    * ------------------------------------------ */
   "#camera" : {
      "_brief" : "Virtual camera to be used to render the scene specified in this scenefile", 
      "_usage" : "usage/camera.js", 
      "_class" : "Camera", 
      
      "_type"  : {
         /*#include "include/dynamic.js"*/
         
         "pinhole" : {
            "_brief" : "Basic pinhole camera using perspective projection (aka Camera Obscura)", 
            "_desc"  : "A pinhole camera is a special case of the thin lens model, in which the camera's aperture is infinitesmally small such that all rays which pass through the aperture focus at one unique location on the film plane.  PinholeCamera, therefore, subclasses ThinLensCamera.", 
            "_class" : "PinholeCamera", 
            /*#include "include/pinhole.js"*/
         }, 
         "thinlens" : {
            "_brief" : "Thin lens camera approximation supporting common camera inputs and allowing for depth of field", 
            "_class" : "ThinLensCamera", 
            "_usage" : "usage/camera.js", 
            
            "_image" : {
               "_path" : [ "images/dof/dofFront.png", "images/dof/dofCenter.png", "images/dof/dofFar.png" ], 
               "_caption" : "Demonstration of Depth of Field effect with thin lens camera by varying the focalPoint parameter to focus on each of the three balls respectively. For each render, a 35mm diameter lens aperture and an fstop of 8 were used."
            }, 
            
            /*#include "include/pinhole.js"*/
            "aperture" : {
               "_desc" : "diameter of the aperature of the lens in mm (eg. 35mm, 50mm)", 
               "_info" : { "type" : "real+", "optional" : true, "default" : 35, "units" : "mm" }
            }, 
            "fstop" : {
               "_desc" : "(aka F number), is defined as the unitless ratio of the focal length to the diameter of the aperture of the lens", 
               "_info" : { "type" : "real+", "optional" : true, "default" : 14, "units" : "unitless" }
            }, 
            "focalDistance" : {
               "_desc" : "the distance in meters between the lens and the focal plane (objects lying on the focal plane will be in perfect focus)", 
               "_note" : "if a valid focalPoint is specified, it will override focalDistance; focalPoint will often be easier to specify if you're trying to focus on a particular object in the scene", 
               "_info" : { "type" : "real+", "optional" : true, "units" : "meters" }
            }, 
            "focalPoint" : {
               "_desc" : "user may specify a focalPoint in NDC on the film-plane instead of specifying a focalDistance to \"auto-focus\" the object intersected by a ray traced through the corresponding pixel, whose distance away from the lens will be used as the focalDistance.", 
               "_note" : [ "focalPoint is expressed as a vector2 in the unit square [0,1]^2, where [0,0] is the upper left corner of the image, and [1,1] is the lower right corner", "if a valid focalPoint is specified, it will override focalDistance" ], 
               "_info" : { "type" : "vector2", "optional" : true, "default" : [ 0.5, 0.5 ], "units" : "unitless" }
            }
         }
      }
   },
   
   /* ------------------------------------------ *
    *                   Output                   *
    * ------------------------------------------ */
   "#output" : {
      "_brief" : "Records point samples as output from a renderer which may be used to construct an output image on a local or distributed machine", 
      "_note"  : "Tonemapping as a post-process is currently not supported by Milton.", 
      "_usage" : "usage/output.js", 
      "_class" : "RendererOutput", 
      
      "_type"  : {
         /*#include "include/dynamic.js"*/
         
         "naive" : {
           "_desc" : "Records point samples from a renderer and naively reconstructs the underlying image by storing samples into the bin/pixel which they fall into, overwriting previous samples, and not using any reconstruction filter.", 
           "_class" : "RenderOutput"
         }, 
         "reconstruction" : {
            "_desc" : "Records point samples from a renderer and attempts to reconstruct the underlying image by filtering samples with a reconstruction filter.", 
            "_class" : "ReconstructionRenderOutput", 
            "_usage" : "usage/output.js", 
            
            "filter" : {
               "_desc" : "2D, symmetric filter to use for reconstructing an approximation of the underlying image from the given point samples", 
               "_info" : { "type" : "filter", "optional" : true, "default" : "lanczosSinc" }
            }
         }
      }
   },
   
   /* ------------------------------------------ *
    *                   Scene                    *
    * ------------------------------------------ */
   "#scene" : {
      "_brief" : "Contains all shape and material data representing the virtual scene", 
      "_desc"  : "The scene element is composed of zero or more 'nodes' and possibly a background element, where a 'node' is either a material, transform, or shape element.", 
      /*"_usage" : "usage/scene.js", */
      "_class" : "Scene", 
      
      "background" : {
         "_desc"  : "Specifies a background emitter conceptually surrounding a scene.", 
         "_usage" : "usage/background.js", 
         "_info"  : { "type" : "node", "optional" : "true", "default" : "black background (ie, no background emission", "cond" : "background element must contain exactly one child emitter element" }, 
         
         "emitter" : {
            "_info"  : { "type" : "emitter", "optional" : "false" }
         }
      }, 
      
      /*#include "include/node.js"*/
   },
   
   /* ------------------------------------------ *
    *                  Material                  *
    * ------------------------------------------ */
   "#material" : {
      "_brief" : "Abstract representation of a surface Material, defined without respect to the underlying surface", 
      "_desc"  : "There is a loose coupling between Shapes and Materials from the point-of-view of a Material, but all Shapes know about their surface Material.  Materials subclass PropertyMap, and it is through this interface that Material properties may be set (ex: diffuse color, texture/bump/color map(s), index of refraction of interior volume, etc.).<br /><br /> Reflectivity, emittance, and sensor response (BSDFs, Emitters, and Sensors respectively) are three properties of a material that are defined at a single point on a surface.  A Material encapsulates BSDF, Emitter, and Sensor properties defined over its surface, where specific instances of BSDF, Emitter, and Sensor are allowed to have their inputs vary with respect to position along the surface.  In this respect, Materials represent a mapping from surface position to associated BSDF, Emitter, and Sensor functions, where the underlying functions themselves remain constant along the surface, and only the inputs vary among the different instances / surface points.  For example, a Material may have a DiffuseBSDF over its entire surface, but a specific DiffuseBSDF instance obtained by getBSDF or implicitly in initSurfacePoint (which fills in the SurfacePoint's BSDF), is allowed to have its kd parameter (diffuse albedo) vary with respect to the given surface point via lookup in an associated kd texture map defined over the UV coordinates of the surface.", 
      "_usage" : "usage/material.js", 
      "_class" : "Material", 
      
      /* properties common to all materials */
      "bumpMap" : {
         "_desc" : "Specifies a path to an image to be applied as a bump map for this material", 
         "_info" : { "type" : "string", "optional" : true, "default" : "no bump map" }
      }, 
      "bumpIntensity" : {
         "_desc" : "Scaling factor for the intensity of the bump map effect", 
         "_info" : { "type" : "real+", "optional" : true, "default" : 5.0 }
      }, 
      "medium" : {
         "_desc" : "Internal medium of shapes to which this material is applied", 
         "_info" : { "type" : "medium", "optional" : true, "default" : "air" }
      }, 
      "filter" : {
         "_desc" : "Filter to use for filtering texture map lookups", 
         "_info" : { "type" : "filter", "optional" : true, "default" : "triangle" }
      }, 
      "repeatU" : {
         "_desc" : "Number of times to repeat texture coordinates across this Material in the horizontal direction, u", 
         "_info" : { "type" : "real+", "optional" : true, "default" : 1.0 }
      }, 
      "repeatV" : {
         "_desc" : "Number of times to repeat texture coordinates across this Material in the vertical direction, v", 
         "_info" : { "type" : "real+", "optional" : true, "default" : 1.0 }
      }, 
      
      "bsdf" : {
         "_desc" : "Constant reflectivity model of this material, whose model-specific inputs are allowed to vary across the surface of this material", 
         "_info" : { "type" : "bsdf", "optional" : true, "default" : "gray, diffuse" }
      }, 
      "emitter" : {
         "_desc" : "Constant emittance model / distribution for this material, whose model-specific inputs are allowed to vary across the surface of this material", 
         "_info" : { "type" : "emitter", "optional" : true, "default" : "null (no emittance)" }
      }
   }, 
   
   /* ------------------------------------------ *
    *                    BSDF                    *
    * ------------------------------------------ */
   "#bsdf" : {
      "_desc" : "Constant reflectivity model for a material, whose model-specific inputs are allowed to vary across the surface of the material", 
      "_class" : "BSDF", 
      
      "_type" : {
         /*#include "include/dynamic.js"*/
         
         "absorbent" : {
            "_desc"  : "Ideal, cool, black-body absorbent BSDF (absorbs all incoming light)", 
            "_class" : "AbsorbentBSDF"
         }, 
         "diffuse" : {
            "_desc"  : "Ideal diffuse BSDF (aka lambertian), with variable albedo", 
            "_class" : "DiffuseBSDF", 
            "_usage" : "usage/diffuse.js", 
            
            /*#include "include/kd.js"*/
         }, 
         "dielectric" : {
            "_desc"  : "Wavelength-dependent, thin dielectric BSDF. The opacity input controls the transmittance versus reflectance of different wavelengths. A pure specular mirror may be obtained by creating a dielectric BSDF with zero opacity. Conversely, a purely transparent surface may be obtained by creating a dielectric with full opacity.  Note that a completely transparent surface will still reflect some light (in proportion to Fresnel's Laws) because refraction is undefined past a critical angle when light is traveling between a lighter and denser medium. When this happens, some light is reflected regardless of the opacity parameter, and this is typically known as total internal reflection.", 
            "_note"  : "bsdf::specular and bsdf::transmissive are special cases of bsdf::dielectric with opacity set to 1 and 0 respectively.", 
            "_class" : "DielectricBSDF", 
            
            "opacity" : {
               "_desc" : "Wavelength-dependent parameter which specifies the transmittance of the material", 
               "_info" : { "type" : "clampedSpectrum", "optional" : true, "default" : "1 (completely opaque)", "units" : "unitless" }
            }, 
            /*#include "include/ks.js"*/
         }, 
         "phong" : {
            "_brief" : "Physically-correct modified Phong model for glossy/specular surfaces.", 
            "_desc"  : "The modified Phong model has two inputs: kd, and ks in [0, 1] subject to kd + ks <= 1, where Kd and Ks represent the diffuse and specular reflectivity of the surface respectively (fraction of incoming energy that is reflected diffusely/specularly). A third input, n, represents the specular shininess of the surface, where higher values of n correspond to tighter / sharper specular highlights, and lower values of n correspond to wider / glossier highlights. The Phong bsdf creates a lobe-like distribution with a cos(alpha)^a fallof, where alpha is the angle between an exitant vector and the perfect specular direction and n is the user-specified \"shininess\" property of the material.", 
            "_note"  : "For more information, please see: <br />E. Lafortune and Y. Willems. Using the modified Phong reflectance model for physically based rendering. Technical Report CW197, Dept of Computer Science, K.U. Leuven, 1994.", 
            "_class" : "ModifiedPhongBSDF", 
            "_image" : {
               "_path" : "images/bsdfs/phong.png", 
               "_caption" : "Visualization of Phong BSDF, with kd=.2, ks=.8, and n=10."
            }, 
            
            /*#include "include/kd.js"*/
            /*#include "include/ks.js"*/
            
            "n" : {
               "_desc" : "Wavelength-dependent shininess exponent of material; specifies how quickly the Phong lobe falls off from the perfect specular direction which has the effect of changing how \"glossy\" the resulting surface appears", 
               "_note" : "Higher values of n denote a shinier, more specular surface", 
               "_info" : { "type" : "real+", "optional" : true, "default" : "1", "units" : "unitless" }
            }
         },
         "specular" : {
            "_desc" : "Perfect specular (mirror) reflector.", 
            "_note" : "bsdf::specular is just a special case of bsdf::dielectric with opacity set to 1.", 
            "_class" : "DielectricBSDF", 
            
            /*#include "include/ks.js"*/
         }, 
         "transmissive" : {
            "_desc" : "Perfect transmissive (transparent) material.", 
            "_note" : "bsdf::transmissive is just a special case of bsdf::dielectric with opacity set to 0. Note that even a perfectly transparent surface may reflect some light due to total internal reflection.", 
            "_class" : "DielectricBSDF", 
            
            /*#include "include/ks.js"*/
         }, 
         
         "aggregate" : {
            "_desc" : "Linear combination of different BSDF implementations, where the coefficients are determined either a priori (inherent property described in scenefile) or via a texture map lookup.", 
            "_class" : "AggregateBSDF", 
            
            "bsdfs"  : {
               "_desc" : "Array of sub-bsdfs of length <i>n</i> which this aggregate bsdf is composed of.", 
               "_info" : { "type" : "array[bsdf]", "optional" : false, "cond" : "must be non-empty and have the same length as weights" }
            }, 
            "weights"  : {
               "_desc" : "Array of weighting coefficients for sub-bsdfs of length <i>n</i>, specifying what fraction of each sub-bsdf this aggregate bsdf is composed of.", 
               "_note" : "Array of weights does not need to be normalized, and since individual weights are clampedSpectrum elements themselves, they may be mapped over the surface of the material (eg, via a \"weight\" texture map)",  
               "_info" : { "type" : "array[clampedSpectrum]", "optional" : false, "cond" : "must be non-empty" }
            }
         }
      }, 
      
      "node" : { }
   }, 
   
   /* ------------------------------------------ *
    *                  Emitter                   *
    * ------------------------------------------ */
   "#emitter" : {
      "_desc" : "Constant emittance model / distribution for a material, whose model-specific inputs are allowed to vary across the surface of the material", 
      "_class" : "Emitter", 
      
      "_type" : {
         /*#include "include/dynamic.js"*/
         
         "null" : {
            "_desc"  : "Null emitter which doesn't emit any light", 
            "_class" : "NullEmitter"
         }, 
         "omni" : {
            "_desc"  : "Point emitter / light which emits light uniformly in every direction (omnidirectional).", 
            "_class" : "OmniEmitter", 
            
            /*#include "include/power.js"*/
         }, 
         "oriented"  : {
            "_desc"  : "Area emitter / light which emits light uniformly across a 2D domain, restricted to the local positive hemisphere (oriented along hemisphere about local surface normal).", 
            "_class" : "OrientedEmitter", 
            
            /*#include "include/power.js"*/
         }, 
         "environment"  : {
            "_desc"  : "An environment map roughly models distant illumination surrounding a scene by conceptually surrounding the scene with a spherical emitter that emits light inwards according to a distribution defined by a 2D 'environment' texture map that is projected onto the surface of the sphere. Note the term HDR environment map is thrown around a lot; the only difference is that the underlying environment texture is stored in some type of high dynamic range format, such as OpenEXR or HDR.", 
            "_class" : "EnvironmentMap", 
            "_usage" : "usage/background.js", 
            
            /*#include "include/power.js"*/
            "path" : {
               "_desc"  : "Path to environment map image", 
               "_info" : { "type" : "string", "optional" : false }
            }, 
            "coords" : {
               "_desc"  : "Type of coordinates this environment map is stored in; specifies the type of 3D -> 2D projection (and vice-versa) to be used for sampling the environment map image.", 
               "_note"  : "Projections supported are \"sphereical\" or \"latLong\".", 
               "_info" : { "type" : "string", "optional" : true, "default" : "spherical" }
            }
         }
      }
   },
   
   /* ------------------------------------------ *
    *                   Medium                   *
    * ------------------------------------------ */
   "#medium" : {
      "_desc" : "Medium describing the properties of a volume", 
      "_note" : "As of right now, the only medium available is specifying the internal medium of a shape to which a material is applied.", 
      
      "_type" : {
         /*#include "include/dynamic.js"*/
         
         "homogenous" : {
            "_desc" : "Constant medium throughout volume", 
            
            "ior" : {
               "_desc" : "Index of refraction of medium", 
               "_info" : { "type" : "spectrum", "optional" : true, "default" : "1.0003 (air)" }
            }
         }
      }
   }, 
   
   /* ------------------------------------------ *
    *                   Filter                   *
    * ------------------------------------------ */
   "#filter" : {
      "_brief" : "2D discrete, symmetric filter which can be compactly / efficiently stored / applied using a 2D kernel (array)", 
      "_usage" : "usage/filter.js", 
      "_class" : "KernelFilter", 
      
      "_type"  : {
         /*#include "include/dynamic.js"*/
         
         "box" : {
            "_desc"  : "2D symmetric box filter (unweighted area filtering)", 
            "_class" : "BoxFilter", 
            /*#include "include/support.js"*/
         }, 
         "triangle" : {
            "_desc"  : "2D symmetric triangle filter (aka linear / tent filter)", 
            "_class" : "TriangleFilter", 
            /*#include "include/support.js"*/
         }, 
         "gaussian" : {
            "_brief" : "2D discrete, symmetric gaussian filter, centered at the origin", 
            "_note"  : "Separable like most of the other filters, but we're not taking advantage of this to make the filter framework cleaner and more cohesive.", 
            "_class" : "GaussianFilter", 
            /*#include "include/support.js"*/
            
            "sigma" : {
               "_desc" : "Sigma parameter of this gaussian distribution (standard deviation)", 
               "_info" : { "type" : "real+", "optional" : true, "default" : 1.0, "units" : "pixels" }
            }
         }, 
         "mitchell" : {
            "_brief" : "2D symmetric Mitchell filter which is parameterized to tradeoff between 'ringing' and 'blurring' that other filters tend towards in difficult reconstruction cases", 
            "_note" : "Mitchell Filter taken from PBRT (Pharr, Humphreys)", 
            "_class" : "MitchellFilter", 
            /*#include "include/support.js"*/
            
            "B" : {
               "_desc" : "B parameter", 
               "_info" : { "type" : "real", "optional" : true, "default" : "1/3" }, 
            }, 
            "C" : {
               "_desc" : "C parameter", 
               "_info" : { "type" : "real", "optional" : true, "default" : "1/3" }
            }
         }, 
         "lanzcosSinc" : {
            "_brief" : "2D symmetric Lanzcos filter whose aim is to approximate a truncated sinc (the ideal rerconstruction filter) while minimizing the amount of visible ringing resulting from the truncation", 
            "_note" : "LanczosSinc Filter taken from PBRT (Pharr, Humphreys)", 
            "_class" : "LanczosSincFilter", 
            /*#include "include/support.js"*/
            
            "B" : {
               "_desc" : "B parameter", 
               "_info" : { "type" : "real", "optional" : true, "default" : "1/3" }
            }, 
            "C" : {
               "_desc" : "C parameter", 
               "_info" : { "type" : "real", "optional" : true, "default" : "1/3" }
            }
         }
      }
   }, 
   
   /* ------------------------------------------ *
    *                  Spectrum                  *
    * ------------------------------------------ */
   "#spectrum" : {
      "_brief" : "<i>n</i>-length Vector of radiance values sampled at <i>n</i> distinct frequencies", 
      "_note"  : "Currently, only RGB spectra are handled, but this abstraction is in place and used ubiquitously throughout Milton s.t. we can eventually come back and implement spectrally-aware rendering with the majority of the changes taking place in this one class.", 
      "_class" : "SpectralSampleSet", 
      
      "_info"  : { "type" : "vector[real+]", "units" : "W/(m^2*sr*sec)" }
   }, 
   
   /* ------------------------------------------ *
    *               ClampedSpectrum              *
    * ------------------------------------------ */
   "#clampedSpectrum" : {
      "_brief" : "<i>n</i>-length Vector of real scalars values corresponding to unitless coefficients sampled at <i>n</i> distinct frequencies", 
      "_note"  : "Currently, only RGB spectra are handled, but this abstraction is in place and used ubiquitously throughout Milton s.t. we can eventually come back and implement spectrally-aware rendering with the majority of the changes taking place in this one class.", 
      "_class" : "SpectralSampleSet", 
      "_desc"  : "A clampedSpectrum is an input to a bsdf, emitter, or sensor. As such, it is allowed to vary across the surface of the material via lookup in a texture map defined over the unit (u,v) coordinates of the surface. <b>Note</b>: for convenience, a clampedSpectrum may be specified as either a single real scalar value, an <i>n</i>-length array of scalar values (one per active frequency being simulated), or as a path to a texture map image.", 
      
      "_info"  : { "type" : "vector[real+] or real or string", "units" : "unitless" }
   }, 
   
   /* ------------------------------------------ *
    *                 Generator                  *
    * ------------------------------------------ */
   "#generator" : {
      "_brief" : "Generates point samples over the 2D unit interval [0,1]^2.", 
      "_desc"  : "Used for choosing sample points on the film plane and within individual pixels for super-sampling. Also used for choosing the distribution of sample points on area lights sources when evaluating direct illumination.", 
      "_usage" : "usage/generator.js", 
      "_info"  : { "default" : "jittered" }, 
      "_class" : "SampleGenerator", 
      
      "_type"  : {
         /*#include "include/dynamic.js"*/
         
         "uniform" : {
            "_brief" : "Uniform point sample generation over the 2D unit interval, where samples are generated uniformly in a grid over the domain", 
            "_note"  : "The regular spacing of samples can manifest itself as coherent aliasing during rendering. That is, grid-like structure of uniformly-generated samples discretizes the domain s.t. if the function being evaluated is not very constant, the grid pattern manifests as familiar stair-casing or \"jaggies\" in the estimation of how the function looks over the domain. The human eye is very sensitive to patterns and much less sensitive to noise, so by adding some randomness to our placement of sample points (see generator::stochastic or generator::jittered), we can reduce the regular nature of our samples which can lead to deterministic aliasing. Note that stochastic (random) sampling doesn't necessarily diminish aliasing in and of itself; it's just a safety measure against certain functions for which uniform sampling is <i>guaranteed</i> to alias.", 
            "_image" : {
               "_path" : "images/generators/sampleGenerator_8_uniform.png", 
               "_caption" : "64 samples generated uniformly over an 8x8 grid"
            }, 
            "_class" : "UniformSampleGenerator"
         }, 
         "stochastic" : {
            "_brief" : "Stochastic point sample generation over the 2D unit interval, where samples are generated completely randomly over the domain", 
            "_note" : "Note the clumping and relatively large sparse areas characteristic of straight-up stochastic sampling, leading to higher variance in our estimate overall since we're very likely to completely miss sampling some areas of the domain -- which is bad, especially if those areas end up being \"important\"", 
            "_image" : {
               "_path" : "images/generators/sampleGenerator_8_stochastic.png", 
               "_caption" : "64 samples generated (uniformly) randomly over an 8x8 grid"
            }, 
            "_class" : "StochasticSampleGenerator"
         }, 
         "jittered" : {
            "_brief" : "Jittered point sample generation over the 2D unit interval, where M*N samples are generated randomly within M*N uniformly-spaced subdomains", 
            "_image" : {
               "_path" : "images/generators/sampleGenerator_8_jittered.png", 
               "_caption" : "64 samples generated in a jittered pattern over an 8x8 grid"
            }, 
            "_class" : "JitteredSampleGenerator"
         }, 
         "super" : {
            "_brief" : "Brute force super sampling over the 2D unit interval, where sub-bin (sub-pixel) sampling is done via an auxillary generator", 
            "_usage" : "usage/generator.js", 
            "noSuperSamples" : {
               "_desc" : "Number of samples to generate per-bin (per-pixel)", 
               "_info" : { "type" : "uint", "optional" : true, "default" : 4 }, 
               "_note" : "If noSuperSamples is set to zero, an infinite number of samples will be generated per-pixel (ie, sample-generation will never terminate)"
            }, 
            "subGeneratorType" : {
               "_desc" : "Type of generator which should be used to generate all sub-bin (sub-pixel) sample points", 
               "_note" : "should be one of uniform | stochastic | jittered", 
               "_info" : { "type" : "string", "optional" : true, "default" : "jittered" }
            }, 
            "_class" : "SuperSampleGenerator"
         }, 
         "dissolve" : {
            "_brief" : "Generates samples over the discretized 2D unit interval in a deterministic, pseudo-random order that looks like a dissolve effect.", 
            "_usage" : "usage/dissolve.js", 
            "noSuperSamples" : {
               "_desc" : "Number of samples to generate per-bin (per-pixel)", 
               "_info" : { "type" : "uint", "optional" : true, "default" : 0 }, 
               "_note" : "If noSuperSamples is set to zero, an infinite number of samples will be generated per-pixel (ie, sample-generation will never terminate)"
            }
         }
      }
   }, 
   
   /* ------------------------------------------ *
    *                   Shape                    *
    * ------------------------------------------ */
   "#shape" : {
      "_brief" : "Declares a shape node", 
      "_desc"  : "Shapes may either be specified via the common \"shape\" : { \"type\" : \"sphere\" ... } syntax or, for convenience, via \"sphere\" : { ... }. See the example usage for a concrete example.", 
      "_note"  : "Aggregate shapes such as shapeSet may contain child materials and transforms.", 
      "_usage" : "usage/shape.js", 
      "_class" : "Shape", 
      
      "_type"  : {
         /*#include "include/dynamic.js"*/
         
         "triangle" : {
            "_brief" : "Creates a single triangle", 
            "_note"  : "If you do not specify vertices for the triangle, it will lie in the y=0 plane, with normal [ 0, 1, 0 ].", 
            "_class" : "Triangle", 
            "_usage" : "usage/triangle.js", 
            
            "vertices" : {
               "_brief" : "Triangle vertices", 
               "_desc"  : "Expects 9 scalar values: [ vAx, vAy, vAz, vBx, vBy, vBz, vCx, vCy, vCz ], where A, B, and C denote the first, second and third vertices respectively and vAx, vAy, vAz denotes their respective x, y, and z components.", 
               "_info" : { "type" : "vector9", "optional" : true, "default" : "[ 0, 0, 0, 0, 0, 0.5, 0.5, 0, 0 ]" }
            }, 
            "normals" : {
               "_brief" : "Per-vertex triangle normals", 
               "_desc"  : "Expects 9 scalar values: [ nAx, nAy, nAz, nBx, nBy, nBz, nCx, nCy, nCz ], where A, B, and C denote the first, second and third vertices respectively and nAx, nAy, nAz denotes their respective x, y, and z components.", 
               "_note"  : "If no normals are specified, the geometric normal will be used as the counter-clockwise cross-product of the triangle vertices.", 
               "_info" : { "type" : "vector9", "optional" : true }
            }, 
            "uvs" : {
               "_brief" : "Per-vertex UV texture coordinates", 
               "_desc"  : "Expects 6 scalar values: [ Au, Av, Bu, Bv, Cu, Cv ], where A, B, and C denote the first, second and third vertices respectively and Au, Av denotes their respective u, v texture coordinates.", 
               "_note"  : "If no uvs are specified, sensible defaults will be used.", 
               "_info" : { "type" : "vector6", "optional" : true }
            }
         }, 
         "plane" : {
            "_desc"  : "Creates a finite plane (quad) shape", 
            "_note"  : "If you do not specify vertices for the plane, it will default to a unit plane with normal [ 0, 1, 0 ] (ranging from [ -.5, 0, -.5 ] to [ .5, 0, .5 ]).", 
            "_class" : "Plane", 
            "_usage1" : "usage/plane1.js", 
            "_usage2" : "usage/plane2.js", 
            
            "vertices" : {
               "_brief" : "Plane vertices", 
               "_desc"  : "Expects 12 scalar values: [ vAx, vAy, vAz, vBx, vBy, vBz, vCx, vCy, vCz, vDx, vDy, vDz ], where A, B, C, and D denote the four vertices respectively and vAx, vAy, vAz denotes their respective x, y, and z components.", 
               "_info" : { "type" : "vector12", "optional" : true, "default" : "[ -.5, 0, 0.5, 0.5, 0, 0.5, 0.5, 0, -0.5, -0.5, 0, -0.5 ]" }
            }
         }, 
         "sphere" : {
            "_desc"  : "Creates a sphere", 
            "_note"  : "If left unspecified, the center of the sphere will default to the origin, and the radius of the sphere will default to 0.5 (contained in the unit box ranging from [ -.5, -.5, -.5 ] to [ .5, .5, .5 ]).", 
            "_class" : "Sphere", 
            "_usage" : "usage/sphere.js", 
            
            "position" : {
               "_brief" : "Center of sphere", 
               "_desc"  : "Expects 3 scalar values: [ px, py, pz ].", 
               "_info" : { "type" : "vector3", "optional" : true, "default" : "[ 0, 0, 0 ]  (origin)" }
            }, 
            "radius" : {
               "_brief" : "Radius of sphere", 
               "_info" : { "type" : "real+", "optional" : true, "default" : "0.5" }
            }
         }, 
         "blob" : {
            "_desc"  : "Blobby shape comopsed of a set of metaobject elements", 
            "_note"  : "Blobby isosurface composed of a set of MetaObjects, all of which combine to form a scalar field whose contour at this blob's threshold value defines the surface. Upon initialization (Blob::init), the implicit surface is polygonized into triangles using a variant of the well-known Marching Cubes algorithm.<br> A MetaObject exerts a positive or negative 'charge' in a scalar field, whose isocontours (level sets) define blobby surfaces with different threshold values corresponding to the contour level.  MetaObjects positively or negatively affect their neighboring MetaObjects depending on their  'strength' and 'negative' attributes.", 
            "_class" : "Blob", 
            "_usage" : "usage/blob.js", 
            "_image" : {
               "_path" : [ "images/shapes/metaBalls_threshold=0.1.png", "images/shapes/metaBalls_threshold=0.2.png", "images/shapes/metaBalls_threshold=0.202.png", "images/shapes/octree.png" ], 
               "_caption" : "Effect of varying the threshold parameter with 3 metaballs; also shown is an example octree spatial acceleration data structure being used for the resulting mesh. Thresholds of 0.1 (left), 0.18 (center), and 0.19999 (right) were used to create these blobs.", 
            }, 
            
            "threshold" : {
               "_brief" : "specifies the particular isocontour boundary which will define the surface of this blobby shape", 
               "_info"  : { "type" : "real", "optional" : true, "default" : "0.17", "cond" : "valid within [0, 0.2)" }
            }, 
            "resolution" : {
               "_desc" : "Grid resolution for marching cubes to generate a mesh from the metaobjects", 
               "_note" : "TODO: make this customizable; currently hard-coded", 
               "_info" : { "type" : "vector3", "optional" : true, "default" : "[ 128, 128, 128 ]" }
            }, 
            "metaObjects" : {
               "_desc" : "Array of metaobject elements comprising this blobby shape", 
               "_info" : { "type" : "vector[metaobject]", "optional" : false, "cond" : "must be non-empty" }, 
               "_note" : "See also metaobject"
            }
         }, 
         "point" : {
            "_desc"  : "Creates a point shape at the origin", 
            "_class" : "PointShape"
         }, 
         "cube" : {
            "_desc"  : "Creates a unit cube, centered at the origin (ranging from [ -0.5, -0.5, -0.5 ] to [ 0.5, 0.5, 0.5 ])", 
            "_class" : "Cube"
         }, 
         "cone" : {
            "_desc"  : "Creates a unit cone, centered at the origin (ranging from [ -0.5, -0.5, -0.5 ] to [ 0.5, 0.5, 0.5 ]), with its tip at [ 0, 0.5, 0 ], and its base in the y=-.5 plane", 
            "_class" : "Cone"
         }, 
         "cylinder" : {
            "_desc"  : "Creates a unit cylinder, centered at the origin (ranging from [ -0.5, -0.5, -0.5 ] to [ 0.5, 0.5, 0.5 ]), with the caps in the y=.5 and y=-.5 planes", 
            "_class" : "Cylinder"
         }, 
         "mesh" : {
            "_brief" : "Loads a mesh from an external file", 
            "_desc"  : "Milton supports loading meshes in obj and ply formats. Each distinct mesh will have its own, internal acceleration data structure, and as such, when specifying a mesh in the scenefile, you may add hints to the acceleration data structure to customize its performance towards a particular mesh.", 
            "_note"  : "Meshes are great candidates for <i>instancing</i>, where you load a 'master' mesh once, give it a name, and then have multiple, lightweight copies of that one master mesh called instances by referencing the master mesh's name. Any scene node may be instanced in Milton, but in practice, it is generally only useful (and tested..) to instance meshes because each instance would otherwise cost so much in terms of memory and speed during rendering.", 
            "_class" : "Mesh", 
            "_usage" : "usage/shape.js", 
            
            "path" : {
               "_desc" : "Path to obj or ply mesh to load", 
               "_note" : "If path specified is relative, it must be with respect to the executable's current working directory (generally this will be the directory where you invoke your executable from).", 
               "_info" : { "type" : "string", "optional" : false }
            }, 
            "normalize" : {
               "_brief" : "Whether or not to normalize this mesh's AABB", 
               "_desc"  : "If enabled, the mesh's vertices will be linearly scaled to lie within the unit cube (ranging from [ -0.5, -0.5, -0.5 ] to [ 0.5, 0.5, 0.5 ]). This is useful if you're not sure of the original mesh's scale / location and would like to add transforms to your mesh as though it were any other primitive (which all lie within the same unit box).", 
               "_info"  : { "type" : "bool", "optional" : true }
            }, 
            /*#include "include/spatialAccel.js"*/
         }, 
         "shapeSet" : {
            "_brief" : "Groups a set of related shapes together within a common spatial acceleration data structure", 
            "_desc"  : "Any shape defined as a child of this shapeSet will be implicitly added to its acceleration data structure.", 
            "_note"  : "ShapeSets should not contain any emitters since as of writing this, they will not be handled properly by Milton; that is, all emitters must lie within the top-level scene element and not within sub shapeSet elements.", 
            "_class" : "ShapeSet", 
            
            /*#include "include/spatialAccel.js"*/
         }
      }
   }, 
   
   /* ------------------------------------------ *
    *                Metaobject                  *
    * ------------------------------------------ */
   "#metaobject" : {
      "_brief" : "Blobby shapes are composed of one or more metaobjects, which each exert a positive or negative 'charge' over a scalar field, whose isocontours (level sets) define blobby surfaces with different threshold values corresponding to the contour level.", 
      "_desc"  : "MetaObjects positively or negatively affect their neighboring MetaObjects depending on their 'strength' and 'negative' attributes. 'strength' defines the initial charge of the object, and 'negative' is a boolean which defaults to false, denoting whether or not this MetaObject has a positive or a negative impact on surrounding MetaObjects. MetaObject is an abstract class, and specific implementations define how its initial charge dissipates throughout the field.", 
      "_note"  : "Currently, the only support type of metaobject is a metaball.", 
      "_class" : "MetaObject", 
      "_image" : {
         "_path" : [ "images/shapes/metaBalls_threshold=0.1.png", "images/shapes/metaBalls_threshold=0.2.png", "images/shapes/metaBalls_threshold=0.202.png", "images/shapes/octree.png" ], 
         "_caption" : "Example of metaballs aggregating to create a blobby shape. Effect of varying the threshold parameter with 3 metaballs; also shown is an example octree spatial acceleration data structure being used for the resulting mesh. Thresholds of 0.1 (left), 0.18 (center), and 0.19999 (right) were used to create these blobs.", 
      },
      "_usage" : "usage/blob.js", 
      
      "_type"  : {
         "ball" : {
            "_desc" : "Spherical metaobject which radiates influence uniformly in all directions outwards from its centerpoint", 
            "_class" : "MetaBall", 
            
            "position" : {
               "_brief" : "Specifies the center of this metaball", 
               "_info"  : { "type" : "vector3", "optional" : true, "default" : "[ 0, 0, 0 ]  (origin)" }
            }, 
            "radius" : {
               "_brief" : "Specifies the radius of this metaball", 
               "_info"  : { "type" : "real+", "optional" : true, "default" : 1.0 }
            }, 
            "strength" : {
               "_brief" : "Specifies the intensity of the initial charge of this metaobject", 
               "_info"  : { "type" : "real", "optional" : true, "default" : 1.0 }
            }, 
            "negative" : {
               "_brief" : "whether or not this metaobject has a positive or negative impact on surrounding metaobjects", 
               "_info"  : { "type" : "boolean", "optional" : true, "default" : false }
            }, 
         }
      }
   }, 
   
   /* ------------------------------------------ *
    *                 Transform                  *
    * ------------------------------------------ */
   "#transform" : {
      "_brief" : "Declares a transformation block which affects all subnodes", 
      "_desc"  : "Specifies a 3D homogenous transformation which should be applied to all subnodes. This transform can be comprised of any number of individual translate, rotate, scale, and/or arbitraryTransform elements. If multiple translates/scales/rotates/arbitraryTransforms are specified in a single transform block, scales will always be applied first to subnodes, then rotations, then translations, then arbitrary transforms, regardless of the order specified in the scenefile. transform blocks are hierarchical, and transform blocks closer to the definitions of shape nodes will always be applied to those nodes first.", 
      "_note"  : "Milton assumes that world-space distances are specified in <b>meters</b>.", 
      "_usage" : "usage/transform.js", 
      
      "translate" : {
         "_desc" : "Translate origin by the specified vector", 
         "_note" : "Expects 3 scalar values: \"translate\" : [ 3, 4, 1 ]", 
         "_info" : { "type" : "vector3", "optional" : true }
      }, 
      "rotate" : {
         "_brief" : "Rotation about specified point along specified vector by the given number of degrees", 
         "_desc"  : "Expects 9 scalar values: \"rotate\" : [ px, py, pz, vx, vy, vz, d ] where (px, py, pz) is the origin of the rotation, (vx, vy, vz) is the axis of rotation, and d specifies the amount to rotate in degrees. Ex. \"rotate\" : [ 0, 0, 0, 1, 0, 0, 90 ] will rotate the world 90 degrees about the x-axis.", 
         "_info"  : { "type" : "vector9", "optional" : true }
      }, 
      "scale" : {
         "_brief" : "Scale transformation", 
         "_desc"  : "Expects 3 scalar values: \"scale\" : [ dx, dy, dz ] where dx, dy, and dz are the amounts to scale the x, y, and z axes respectively.", 
         "_usage" : "usage/scale.js", 
         "_info"  : { "type" : "vector3", "optional" : true }
      },
      "arbitraryTransform" : {
         "_brief" : "Applies an arbitrary homogenous 3D transformation matrix (4x4)", 
         "_desc"  : "Expects 16 scalar values; the elements of the transformation matrix in row-major order: \"arbitraryTransform\" : [ t00, t01, t02, t03, t10, t11, t12, t13, t20, t21, t22, t23, t30, t31, t32, t33 ] where t(i,j) denotes the element of the transformation matrix at row i, column j.", 
         "_usage" : "usage/arbitraryTransform.js", 
         "_info"  : { "type" : "vector9", "optional" : true }
      }
   }
};

