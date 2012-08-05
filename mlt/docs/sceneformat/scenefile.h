/**<!-------------------------------------------------------------------->
   @file   scenefile.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Auto-generated documentation of Milton scenefile format
   
   @note
      This header is a stub which gets prefixed to the auto-generated docs, 
   providing an intro page for the Doxygen-generated documentation.
   <!-------------------------------------------------------------------->**/

#pragma once

/**
 * @mainpage Milton Scenefile Documentation
 * 
 * @section intro Introduction
 * @htmlonly
 * <p class="indent">
 * @endhtmlonly
 *    Milton scenefiles are specified in the JSON (<a href="http://www.json.org/">json.org</a>) file format. Documentation for the format itself is available via <a href="/docs/scenefile/namespacescenefile.php">auto-generated documentation</a>. In addition to the thorough element-by-element docs, we also provide an introductory tutorial which walks through core concepts in an attempt to familiarize the user with a high-level overview of everything.
 * </p>
 * 
 * <p>
 * Here is a concise, valid scenefile that should give you a pretty good overview of what's going on:
 * </p>
 * <br />
 * 
 * @htmlonly
 * <div class='usage'>
 * <p>Example scenefile:</p>
 * <div class='example'>
 * {
 * <div class=indent>// typical scenefile example</div>
 * <div class=indent><a href="namespacescenefile.php">"scenefile"</a> : {</div>
 * <div class=indent><div class=indent><a href="structrenderer.php">"renderer"</a> : {</div></div>
 * <div class=indent><div class=indent><div class=indent>"type" : <a href="structpathTracer.php">"pathTracer"</a>, </div></div></div>
 * <div class=indent><div class=indent><div class=indent>"noRenderThreads" : 10, </div></div></div>
 * <div class=indent><div class=indent><div class=indent>"noSuperSamples" : 1000, </div></div></div>
 * <div class=indent><div class=indent>}, </div></div>
 * <br>
 * <div class=indent><div class=indent><a href="structcamera.php">"camera"</a> : {</div></div>
 * <div class=indent><div class=indent><div class=indent>"type" : <a href="structthinlens.php">"thinlens"</a>, </div></div></div>
 * <div class=indent><div class=indent><div class=indent>"eye" : "0, 10, 0", </div></div></div>
 * <br>
 * <div class=indent><div class=indent><div class=indent>"aperture" : 50, </div></div></div>
 * <div class=indent><div class=indent><div class=indent>"fstop" : 18, </div></div></div>
 * <div class=indent><div class=indent>}, </div></div>
 * <br>
 * <div class=indent><div class=indent><a href="structscene.php">"scene"</a> : {</div></div>
 * <div class=indent><div class=indent><div class=indent><a href="structmaterial.php">"material"</a> : {</div></div></div>
 * <div class=indent><div class=indent><div class=indent><div class=indent><a href="structemitter.php">"emitter"</a> : {</div></div></div></div>
 * <div class=indent><div class=indent><div class=indent><div class=indent><div class=indent>"type" : <a href="structoriented.php">"oriented"</a>, </div></div></div></div></div>
 * <div class=indent><div class=indent><div class=indent><div class=indent><div class=indent>"power" : [ 80, 80, 80 ], </div></div></div></div></div>
 * <div class=indent><div class=indent><div class=indent><div class=indent>}, </div></div></div></div>
 * <br>
 * <div class=indent><div class=indent><div class=indent><div class=indent><a href="structplane.php">"plane"</a> : { }, </div></div></div></div>
 * <div class=indent><div class=indent><div class=indent>}, </div></div></div>
 * <div class=indent><div class=indent>}, </div></div>
 * <div class=indent>},</div>
 * }
 * <br>
 * </div></div>
 * @endhtmlonly
 * 
 * <p>
 *    At the top-level is the scenefile element, which every valid %scene file must contain. The scenefile element may contain several main child elements (renderer, camera, output, and scene), all of which are optional except for the required scene element. The scene element contains all of the geometry and materials comprising the given %scene (shape and material elements respectively).
 * </p>
 * <br />
 * <br />
 * 
 * @section tutorials Scenefile Tutorials
 * <p>
 *    <ul>
 *       <li><a href="/tutorials/scenefiles.php">Introductory tutorial</a></li>
 *    </ul>
 * </p>
 * <br />
 * 
 * @section overview Scenefile Design
 * @htmlonly
 * <p class="indent">
 * @endhtmlonly
 *    Many classes in Milton are customizable via a number of input parameters, 
 * and it would both be awkward and decrease readability to have those classes 
 * take in all possible combinations of inputs as parameters either via 
 * constructors or accessors / mutators. We chose to instead parameterize these 
 * classes at run-time via a common base class called a PropertyMap, which 
 * represents a key-value mapping between std::string keys and a set of well-
 * defined primitive variant-value types (int, double, string, etc.). By taking 
 * advantage of boost::any, a template variant holder utility class provided by 
 * the boost library, all code dealing with parameters became both more 
 * extensible and more readable at the expense of static type-checking in several 
 * places. This allows parameters for most core Milton classes to be specified 
 * via the commandline or more commonly in the scenefiles themselves, and loose 
 * type-checking is instead done at run-time during scenefile parsing. This 
 * setup allows for a lot of flexibility, both within Milton and in terms of 
 * allowing external extensions. One could, for example, use the same built-in 
 * loading code and scenefile format but add his/her own BSDF type with its own
 * arbitrary key-value inputs, and as long as the value-types in question are 
 * supported variants, without having to change anything within the Milton 
 * library itself.
 * </p>
 * 
 * @htmlonly
 * <p class="indent">
 * @endhtmlonly
 *    Closely related to the loose nature of PropertyMaps is the Milton scenefile
 * format. In order to fulfill our goals of having scenefiles that were going 
 * to be both concise, well-defined, and highly extensible via arbitrary key-
 * value pairs, we ended up defining an implementation-independent context free
 * grammar to capture the structure and types that would be used in Milton 
 * scenefiles as well as a concrete implementation of that grammar via the 
 * popular JSON format. Many aspects of the scenefile, including the camera, 
 * output, and renderer are all optional, and in the case that they aren't 
 * provided, reasonable defaults will be used. What's great about JSON is that 
 * its own design (and Javascript in general) was guided by the recursive 
 * definition of objects as lists of key-value pairs similar in spirit to our 
 * extensible PropertyMap paradigm.
 * </p>
 */

