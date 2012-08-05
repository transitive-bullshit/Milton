/**<!-------------------------------------------------------------------->
   @file   milton.h
   @author Travis Fischer (fisch0920@gmail.com)
   @author Matthew Jacobs (jacobs.mh@gmail.com)
   @date   Fall 2008
   
   @brief
      Convenience header which includes all public headers used throughout 
   the Milton library
   <!-------------------------------------------------------------------->**/

#ifndef LIB_MILTON_H_
#define LIB_MILTON_H_

// include all main milton headers, most of which include their own subheaders
#include <accel/accel.h>
#include <cameras/cameras.h>
#include <common/common.h>
#include <common/image/miltonimage.h>
#include <core/core.h>
#include <dynamic/dynamic.h>
#include <filters/filters.h>
#include <loaders/loaders.h>
#include <materials/materials.h>
#include <renderers/renderers.h>
#include <shapes/shapes.h>
#include <stats/stats.h>
#include <third-party/third-party.h>
#include <utils/utils.h>

// below is the intro page for the Doxygen-generated documentation

/**
 * @mainpage Milton Documentation
 * 
 * @section intro Introduction
 * @htmlonly
 * <p class="indent">
 * @endhtmlonly
 *    Milton is open-source and aims to maintain a polished, well-documented code base.  Documentation for the source itself is available via <a href="/docs/doxygen/classes.php">Doxygen generated documentation</a>. Thorough documentation is also available for the Milton <a href="/docs/scenefile/index.php">scenefile format</a> (which is probably a good place to start getting a high-level overview of how Milton is structured, especially if you're not interested in the source itself). In addition to the thorough API docs, we also provide a collection of introductory tutorials which walkthrough %core Milton concepts in an attempt to familiarize the user with a high-level overview of an otherwise large and foreboding codebase.
 * </p>
 * <br />
 * <br />
 * 
 * @section tutorials Milton Tutorials
 * <p>
 *    <ul>
 *       <li><a href="/docs/scenefile/index.php">Scenefile documentation and tutorials</a></li>
 *       <li><a href="/tutorials/pathtracer.php">Path tracing example tutorial</a></li>
 *    </ul>
 * </p>
 * <br />
 * 
 * @section overview Source Layout
 * <div class="projDir" id="projDir1"><div>Core Milton directory structure</div></div>
 * 
 * @htmlonly
 * <p class="indent">
 * @endhtmlonly
 *    Milton is logically broken up into several distinct components. The <a href="/docs/doxygen/dirs.php">Doxygen generated documentation</a> does a pretty good job at allowing you to traverse the source in the way it was meant to be traversed. In addition, we provide here an overview of the source layout.
 * </p>
 * @htmlonly
 * <p class="indent">
 * @endhtmlonly
 *    At the top-level, there is <span class="docStyled">exec</span>, <span class="docStyled">gui</span>, <span class="docStyled">milton</span>, and <span class="docStyled">tests</span>, each with its own target (executable, library, etc.).  Here's a descriptive breakdown of each directory and its purpose:
 *    <ul>
 *       <li><span style="color: #FF8D00; font-weight: bold;">milton</span> contains all of the %core framework and is probably where you'll want to start getting familiar with the code. <span class="docStyled">milton</span>'s purpose is to build the %core <span class="docStyled">libmilton</span> library which external programs can link against. Note, for the purposes of compiling external programs against <span class="docStyled">milton</span>, you must have the top-level <span class="docStyled">milton</span> directory in your path (with g++, you'd need to invoke something like <span class="docStyled">g++ -Ipathtomilton -o myprogram main.cpp</span>). All #includes within <span class="docStyled">milton</span> are relative to the top-level directory for convenience. Also note the all-encompassing convenience header <span class="docStyled">milton/milton.h</span>, which indirectly #includes all public <span class="docStyled">milton</span> headers throughout the main <span class="docStyled">milton</span> library. This header is provided such that you only have to #include one file in order to use the <span class="docStyled">milton</span> library, without having to worry about #including all of the different files you have to use for a particular application.</li>
 *       <li><span style="color: #FF8D00; font-weight: bold;">gui</span> contains everything you'd need to get a quick graphical application built atop the %core <span class="docStyled">milton</span> library up-and-running. Specifically, it contains a multithreaded, OpenGL-capable <a href="http://doc.trolltech.com/4.3/">Qt</a>-based frontend. <span class="docStyled">gui</span> provides a hopefully easy-to-use wrapper, MiltonApp, which should be the only Gui interface that most Milton applications will need to use. <span class="docStyled">gui</span> compiles another library, <span class="docStyled">libgui</span>, which external applications may link against. Note that MiltonApp doesn't have to be graphical; it naturally supports a <span class="docStyled">nox</span> (aka non-graphical or no X-server) mode as well for rendering in batch or remotely over a network (where the execution environment might not be graphics-friendly). See MiltonApp or one of the applications in <span class="docStyled">exec</span> or <span class="docStyled">tests</span> which use MiltonApp for more details.</li>
 *       <li><span style="color: #FF8D00; font-weight: bold;">exec</span> contains the main executable that's packaged with Milton that'll work out-of-the box on supported platforms. <span class="docStyled">exec</span> compiles to an executable named <span class="docStyled">milton</span>; run <span class="docStyled">milton --help</span> from within <span class="docStyled">exec</span> for more details on how to invoke <span class="docStyled">milton</span>. <span class="docStyled">exec</span> also contains a plethora of built-in resources for <a href="/tutorials/scenefiles.php">scenefiles</a>, including testing scenefiles that we've used during the development of Milton, as well as a small repository of various textures and meshes. Note that <span class="docStyled">exec/main.cpp</span> uses MiltonApp to parse commandline parameters and get a default Gui up-and-running. Also note that the Makefile in <span class="docStyled">exec</span> links against <span class="docStyled">libgui</span> and <span class="docStyled">libmilton</span>.</li>
 *       <li><span style="color: #FF8D00; font-weight: bold;">tests</span> contains a hodgepodge of tests aimed at ensuring the stability and accuracy of the %core <span class="docStyled">milton</span> library. Note that <span class="docStyled">tests</span> also contains several fun, educational "applets" which specialize the notion of a Visualization to use Milton more as a framework and less as a standalone program. Examples include an OpenGL visualizer for the distributions of various built-in BSDFs and a visualizer for paths generated with the built-in BidirectionalPathTracer. Note that many of these test cases utilize MiltonApp (and thus both <span class="docStyled">libgui</span> and <span class="docStyled">libmilton</span>) as their main interface to Milton.</li>
 *    </ul>
 * </p>
 * 
 * <p>
 *    Within the %core <span class="docStyled">milton</span> directory, the framework is broken down logically into subdirectories, <span class="docStyled">accel</span>, <span class="docStyled">cameras</span>, <span class="docStyled">common</span>, etc, some of which contain their own subdirectories (ex, <span class="docStyled">common/math/simd</span>). Milton uses several third-party libraries (located in <span class="docStyled">milton/third-party</span>), namely:
 * </p>
 * <table>
 *    <tr><td><a href="http://doc.trolltech.com/4.3/">Qt</a></td><td>a cross-platform Gui and generic toolkit framework</td></tr>
 *    <tr><td><a href="http://www.boost.org/">boost</a></td><td>a set of advanced, portable C++ libraries (Milton makes heavy use of <a href="http://www.boost.org/doc/libs/1_37_0/doc/html/any.html">boost::any</a> in particular)</td></tr>
 *    <tr><td><a href="http://blog.beef.de/projects/tinyjson/">tinyjson*</a></td><td>a minimalistic JSON parser based on boost.spirit</td></tr>
 *    <tr><td><a href="http://www.cs.kuleuven.ac.be/~ares/libobj">libobj*</a></td><td>a modern C++ library for parsing the Wavefront OBJ file format (by <a href="http://www.cs.kuleuven.ac.be/~ares/">Ares Lagae</a>)</td></tr>
 *    <tr><td><a href="http://www.cs.kuleuven.ac.be/~ares/libply">libobj*</a></td><td>a modern C++ library for parsing the PLY file format (by <a href="http://www.cs.kuleuven.ac.be/~ares/">Ares Lagae</a>)</td></tr>
 * </table>
 * <p>
 *    Note: libraries denotd with a * are included in the standard Milton subversion (svn) repository. Qt and boost are not included due to their size.
 * </p>
 * @endhtmlonly
 */

#endif // LIB_MILTON_H_

