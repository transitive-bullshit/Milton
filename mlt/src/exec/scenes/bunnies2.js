{
   "scenefile" : {
      "version" : 0.1, 

         /**
          * Renderer
          */
         "renderer" : {
            "type" : "pathTracer", 
            "noRenderThreads" : 10, 
            "noDirectSamples" : 1, 
            "noSuperSamples"  : 1024, 
            "generator" : "super", 
         }, 

         /**
          * Camera 
          */
         "camera" : {
            "type" : "thinlens", 
            /* optional parameters */
            "eye"   : [.48, .35, 1.7],/*[ .5, .3, 1.8 ],*/
            "up"    : [ 0, 1, 0 ], 
            "heightAngle" : 45, 
            "aspectRatio" : 1.0, 
            "near"  : 1, 
            "far"   : 100, 
            
            "focalPoint" : [ 0.5, 0.45 ], 
            "aperture" : 2, 
            "fstop" : 50, 
         }, 

         /**
          * Scene (materials and shapes) 
          */
         "scene" : {
            "material" : { /* light */
               "emitter" : {
                  "type" : "oriented", 
                  "power" : 1000, 
               }, 
               "bsdf" : {
                  "type" : "diffuse", 
                  "kd"   : 1, 
               }, 

               "transform" : {
                  "translate" : [ 0, 10, 0 ], 
                  "scale"     : [ 5, 1, 5 ], 
                  "rotate" : [ 0, 0, 0, 1, 0, 0, 180 ], 
                  "plane"  : { },
               },
            }, 

            "material" : { /* light */
               "emitter" : {
                  "type" : "oriented", 
                  "power" : 30, 
               }, 
               "bsdf" : {
                  "type" : "diffuse", 
                  "kd"   : 1, 
               }, 

               "transform" : {
                  "translate" : [ .5, .3, 1.8 ], 
                  "scale"     : [ 2, 1, 2 ], 
                  "rotate" : [ 0, 0, 0, 1, 0, 0, -90 ], 
                  "plane"  : { },
               },
            }, 

            "material" : {  /* floor */
               "bsdf" : {
                  "type" : "diffuse", 
                  "kd"   : .75,
               }, 
               "transform" : {
                  "plane" : { 
                     "vertices" : [
                        -1, .0355,  2,
                     2, .0355,  2,
                     2, .0355, -1,
                     -1, .0355, -1,
                     ],
                  },
               },
            },

            "material" : {
               "bsdf" : {
                  "type" : "diffuse", 
                  "kd"   : [.5,.1,.1],
               }, 

               "plane" : { 
                  "vertices" : [
                     -1, 2.0355, 1,
                  -1, .0355, 1,
                  -1, 0.0355, -1,
                  -1, 2.0355, -1,
                  ],
               },
            },
            "material" : {
               "bsdf" : {
                  "type" : "diffuse", 
                  "kd"   : [.1,.5,.1],
               }, 
               "plane" : { 
                  "vertices" : [
                     -1, 0.0355, -1,
                  1, 0.0355, -1,
                  1, 2.0355, -1,
                  -1, 2.0355, -1,
                  ],
               },
            },

            "mesh" : {
               "name" : "bun",
               "path" : "data/meshes/bunny_scanned.ply", 
               /*"kdSplitPlaneType" : "splitPlaneMiddle", 
                 "kdMaxDepth" : 24, */
            },

            "material": {
               "transform": {
                  "shape": {
                     "instance": "bun"
                  },
                  "translate": [
                     -0.574005323786709,
                  0,
                  -0.543713566297618
                     ]
               },
               "bsdf": {
                  "kd": 0.7,
                  "type": "diffuse"
               }
            },
            "material": {
               "transform": {
                  "shape": {
                     "instance": "bun"
                  },
                  "translate": [
                     -0.305494645060295,
                  0,
                  -0.452525600505589
                     ]
               },
               "bsdf": {
                  "kd": 0.7,
                  "type": "diffuse"
               }
            }
            ,
               "material": {
                  "transform": {
                     "shape": {
                        "instance": "bun"
                     },
                     "translate": [
                        0.00849286493589205,
                     0,
                     -0.667271064924991
                        ]
                  },
                  "bsdf": {
                     "kd": 0.7,
                     "type": "diffuse"
                  }
               }
            ,
               "material": {
                  "transform": {
                     "shape": {
                        "instance": "bun"
                     },
                     "translate": [
                        0.774707949883034,
                     0,
                     -0.438126156491465
                        ]
                  },
                  "bsdf": {
                     "kd": 0.7,
                     "type": "diffuse"
                  }
               }
            ,
               "material": {
                  "transform": {
                     "shape": {
                        "instance": "bun"
                     },
                     "translate": [
                        -0.436921163215477,
                     0,
                     -0.127273583395916
                        ]
                  },
                  "bsdf": {
                     "kd": 0.7,
                     "type": "diffuse"
                  }
               }
            ,
               "material": {
                  "transform": {
                     "shape": {
                        "instance": "bun"
                     },
                     "translate": [
                        -0.240277714678247,
                     0,
                     -0.159540591213685
                        ]
                  },
                  "bsdf": {
                     "kd": 0.7,
                     "type": "diffuse"
                  }
               }
            ,
               "material": {
                  "transform": {
                     "shape": {
                        "instance": "bun"
                     },
                     "translate": [
                        0.144592076662898,
                     0,
                     -0.337805011437216
                        ]
                  },
                  "bsdf": {
                     "kd": 0.7,
                     "type": "diffuse"
                  }
               }
            ,
               "material": {
                  "transform": {
                     "shape": {
                        "instance": "bun"
                     },
                     "translate": [
                        0.470985138582459,
                     0,
                     -0.361202583954886
                        ]
                  },
                  "bsdf": {
                     "kd": 0.7,
                     "type": "diffuse"
                  }
               }
            ,
               "material": {
                  "transform": {
                     "shape": {
                        "instance": "bun"
                     },
                     "translate": [
                        -0.703119277620147,
                     0,
                     0.288386213880986
                        ]
                  },
                  "bsdf": {
                     "kd": 0.7,
                     "type": "diffuse"
                  }
               }
            ,
               "material": {
                  "transform": {
                     "shape": {
                        "instance": "bun"
                     },
                     "translate": [
                        -0.294949311043468,
                     0,
                     0.206118823020233
                        ]
                  },
                  "bsdf": {
                     "kd": 0.7,
                     "type": "diffuse"
                  }
               }
            ,
               "material": {
                  "transform": {
                     "shape": {
                        "instance": "bun"
                     },
                     "translate": [
                        0.173038254400296,
                     0,
                     0.253114967800112
                        ]
                  },
                  "bsdf": {
                     "kd": 0.7,
                     "type": "diffuse"
                  }
               }
            ,
               "material": {
                  "transform": {
                     "shape": {
                        "instance": "bun"
                     },
                     "translate": [
                        0.564735778091296,
                     0,
                     0.0424024334757015
                        ]
                  },
                  "bsdf": {
                     "kd": 0.7,
                     "type": "diffuse"
                  }
               }
            ,
               "material": {
                  "transform": {
                     "shape": {
                        "instance": "bun"
                     },
                     "translate": [
                        -0.543158377850528,
                     0,
                     0.488167168536698
                        ]
                  },
                  "bsdf": {
                     "kd": 0.7,
                     "type": "diffuse"
                  }
               }
            ,
               "material": {
                  "transform": {
                     "shape": {
                        "instance": "bun"
                     },
                     "translate": [
                        -0.115358413044501,
                     0,
                     0.556297232908486
                        ]
                  },
                  "bsdf": {
                     "kd": 0.7,
                     "type": "diffuse"
                  }
               }
            ,
               "material": {
                  "transform": {
                     "shape": {
                        "instance": "bun"
                     },
                     "translate": [
                        0.283850055279077,
                     0,
                     0.445356226797111
                        ]
                  },
                  "bsdf": {
                     "kd": 0.7,
                     "type": "diffuse"
                  }
               }
            ,
               "material": {
                  "transform": {
                     "shape": {
                        "instance": "bun"
                     },
                     "translate": [
                        0.489900197147211,
                     0,
                     0.510197000740227
                        ]
                  },
                  "bsdf": {
                     "kd": 0.7,
                     "type": "diffuse"
                  }
               }
            ,

               "transform" : {
                  "translate" : [ .1, .25, .5 ], 
                  "material": {
                     "transform": {
                        "shape": {
                           "instance": "bun"
                        },
                        "translate": [
                           -0.574005323786709,
                        0,
                        -0.543713566297618
                           ]
                     },
                     "bsdf": {
                        "kd": 0.7,
                        "type": "diffuse"
                     }
                  }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.305494645060295,
                           0,
                           -0.452525600505589
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.00849286493589205,
                           0,
                           -0.667271064924991
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.774707949883034,
                           0,
                           -0.438126156491465
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.436921163215477,
                           0,
                           -0.127273583395916
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.240277714678247,
                           0,
                           -0.159540591213685
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.144592076662898,
                           0,
                           -0.337805011437216
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.470985138582459,
                           0,
                           -0.361202583954886
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.703119277620147,
                           0,
                           0.288386213880986
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.294949311043468,
                           0,
                           0.206118823020233
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.173038254400296,
                           0,
                           0.253114967800112
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.564735778091296,
                           0,
                           0.0424024334757015
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.543158377850528,
                           0,
                           0.488167168536698
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.115358413044501,
                           0,
                           0.556297232908486
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.283850055279077,
                           0,
                           0.445356226797111
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.489900197147211,
                           0,
                           0.510197000740227
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
               },

               "transform" : {
                  "translate" : [ .8, .45, .9 ], 
                  "material": {
                     "transform": {
                        "shape": {
                           "instance": "bun"
                        },
                        "translate": [
                           -0.574005323786709,
                        0,
                        -0.543713566297618
                           ]
                     },
                     "bsdf": {
                        "kd": 0.7,
                        "type": "diffuse"
                     }
                  }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.305494645060295,
                           0,
                           -0.452525600505589
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.00849286493589205,
                           0,
                           -0.667271064924991
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.774707949883034,
                           0,
                           -0.438126156491465
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.436921163215477,
                           0,
                           -0.127273583395916
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.240277714678247,
                           0,
                           -0.159540591213685
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.144592076662898,
                           0,
                           -0.337805011437216
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.470985138582459,
                           0,
                           -0.361202583954886
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.703119277620147,
                           0,
                           0.288386213880986
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.294949311043468,
                           0,
                           0.206118823020233
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.173038254400296,
                           0,
                           0.253114967800112
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.564735778091296,
                           0,
                           0.0424024334757015
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.543158377850528,
                           0,
                           0.488167168536698
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.115358413044501,
                           0,
                           0.556297232908486
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.283850055279077,
                           0,
                           0.445356226797111
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.489900197147211,
                           0,
                           0.510197000740227
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
               },

               "transform" : {
                  "translate" : [ .12, 0, .8 ], 
                  "material": {
                     "transform": {
                        "shape": {
                           "instance": "bun"
                        },
                        "translate": [
                           -0.574005323786709,
                        0,
                        -0.543713566297618
                           ]
                     },
                     "bsdf": {
                        "kd": 0.7,
                        "type": "diffuse"
                     }
                  }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.305494645060295,
                           0,
                           -0.452525600505589
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.00849286493589205,
                           0,
                           -0.667271064924991
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.774707949883034,
                           0,
                           -0.438126156491465
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.436921163215477,
                           0,
                           -0.127273583395916
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.240277714678247,
                           0,
                           -0.159540591213685
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.144592076662898,
                           0,
                           -0.337805011437216
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.470985138582459,
                           0,
                           -0.361202583954886
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.703119277620147,
                           0,
                           0.288386213880986
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.294949311043468,
                           0,
                           0.206118823020233
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.173038254400296,
                           0,
                           0.253114967800112
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.564735778091296,
                           0,
                           0.0424024334757015
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.543158377850528,
                           0,
                           0.488167168536698
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.115358413044501,
                           0,
                           0.556297232908486
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.283850055279077,
                           0,
                           0.445356226797111
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.489900197147211,
                           0,
                           0.510197000740227
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
               },

               "transform" : {
                  "translate" : [ -.05, .6, .06 ], 
                  "material": {
                     "transform": {
                        "shape": {
                           "instance": "bun"
                        },
                        "translate": [
                           -0.574005323786709,
                        0,
                        -0.543713566297618
                           ]
                     },
                     "bsdf": {
                        "kd": 0.7,
                        "type": "diffuse"
                     }
                  }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.305494645060295,
                           0,
                           -0.452525600505589
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.00849286493589205,
                           0,
                           -0.667271064924991
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.774707949883034,
                           0,
                           -0.438126156491465
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.436921163215477,
                           0,
                           -0.127273583395916
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.240277714678247,
                           0,
                           -0.159540591213685
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.144592076662898,
                           0,
                           -0.337805011437216
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.470985138582459,
                           0,
                           -0.361202583954886
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.703119277620147,
                           0,
                           0.288386213880986
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.294949311043468,
                           0,
                           0.206118823020233
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.173038254400296,
                           0,
                           0.253114967800112
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.564735778091296,
                           0,
                           0.0424024334757015
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.543158377850528,
                           0,
                           0.488167168536698
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.115358413044501,
                           0,
                           0.556297232908486
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.283850055279077,
                           0,
                           0.445356226797111
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.489900197147211,
                           0,
                           0.510197000740227
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
               },

               "transform" : {
                  "translate" : [ 0, .35, 0.01 ], 
                  "material": {
                     "transform": {
                        "shape": {
                           "instance": "bun"
                        },
                        "translate": [
                           -0.574005323786709,
                        0,
                        -0.543713566297618
                           ]
                     },
                     "bsdf": {
                        "kd": 0.7,
                        "type": "diffuse"
                     }
                  }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.305494645060295,
                           0,
                           -0.452525600505589
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.00849286493589205,
                           0,
                           -0.667271064924991
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.774707949883034,
                           0,
                           -0.438126156491465
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.436921163215477,
                           0,
                           -0.127273583395916
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.240277714678247,
                           0,
                           -0.159540591213685
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.144592076662898,
                           0,
                           -0.337805011437216
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.470985138582459,
                           0,
                           -0.361202583954886
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.703119277620147,
                           0,
                           0.288386213880986
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.294949311043468,
                           0,
                           0.206118823020233
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.173038254400296,
                           0,
                           0.253114967800112
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.564735778091296,
                           0,
                           0.0424024334757015
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.543158377850528,
                           0,
                           0.488167168536698
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.115358413044501,
                           0,
                           0.556297232908486
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.283850055279077,
                           0,
                           0.445356226797111
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.489900197147211,
                           0,
                           0.510197000740227
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
               }, 


               "transform" : {
                  "translate" : [ -.1, .15, .1 ], 
                  "material": {
                     "transform": {
                        "shape": {
                           "instance": "bun"
                        },
                        "translate": [
                           -0.574005323786709,
                        0,
                        -0.543713566297618
                           ]
                     },
                     "bsdf": {
                        "kd": 0.7,
                        "type": "diffuse"
                     }
                  }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.305494645060295,
                           0,
                           -0.452525600505589
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.00849286493589205,
                           0,
                           -0.667271064924991
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.774707949883034,
                           0,
                           -0.438126156491465
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.436921163215477,
                           0,
                           -0.127273583395916
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.240277714678247,
                           0,
                           -0.159540591213685
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.144592076662898,
                           0,
                           -0.337805011437216
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.470985138582459,
                           0,
                           -0.361202583954886
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.703119277620147,
                           0,
                           0.288386213880986
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.294949311043468,
                           0,
                           0.206118823020233
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.173038254400296,
                           0,
                           0.253114967800112
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.564735778091296,
                           0,
                           0.0424024334757015
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.543158377850528,
                           0,
                           0.488167168536698
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              -0.115358413044501,
                           0,
                           0.556297232908486
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.283850055279077,
                           0,
                           0.445356226797111
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
                     "material": {
                        "transform": {
                           "shape": {
                              "instance": "bun"
                           },
                           "translate": [
                              0.489900197147211,
                           0,
                           0.510197000740227
                              ]
                        },
                        "bsdf": {
                           "kd": 0.7,
                           "type": "diffuse"
                        }
                     }
                  ,
               }, 

         }, 
   },
}

