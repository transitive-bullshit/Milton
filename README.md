<pre>
 /$$      /$$ /$$$$$$ /$$    /$$$$$$$$ /$$$$$$  /$$   /$$
| $$$    /$$$|_  $$_/| $$   |__  $$__//$$__  $$| $$$ | $$
| $$$$  /$$$$  | $$  | $$      | $$  | $$  \ $$| $$$$| $$
| $$ $$/$$ $$  | $$  | $$      | $$  | $$  | $$| $$ $$ $$
| $$  $$$| $$  | $$  | $$      | $$  | $$  | $$| $$  $$$$
| $$\  $ | $$  | $$  | $$      | $$  | $$  | $$| $$\  $$$
| $$ \/  | $$ /$$$$$$| $$$$$$$$| $$  |  $$$$$$/| $$ \  $$
|__/     |__/|______/|________/|__/   \______/ |__/  \__/
</pre>

http://www.cs.brown.edu/research/pubs/theses/ugrad/2009/fischer.pdf

The goal of photorealistic rendering is to generate an image of a virtual scene that would match as closely as possible a photograph taken of the same scene in the real world.  The field of photorealistic rendering has drawn the interest of many researchers with a fascination for this unique opportunity to simulate nature in its infinite complexity and has consequently flourished over the past 25 years.  Today, this goal has been realized to the point where even experts are often unable to differentiate between a rendered image and a photograph, and it is this realism that has made rendering into an important tool that is used ubiquitously throughout such fields as entertainment, education, marketing, and academia.  Many of these applications benefit from some theoretical guarantee of the physical correctness of the renderings they use and therefore require rendering algorithms that are physically-based in that they attempt to model the way light functions in the real world.  The deeper understanding that is gained from studying the underlying physical processes may still benefit applications without these needs because more insight into these fundamentals can allow one to design better non-physically-based algorithms as well.  While the original goal of photorealistic rendering has certainly been accomplished, there are still practical problems that hinder many potential students from successfully entering the field.  The objective of this thesis is  to address several of these problems by introducing a new rendering framework named Milton.

Milton is a cross-platform, open-source rendering framework written in C++, with an emphasis on design, efficiency, and maintaining a very high quality codebase.  It is no secret that there are many rendering engines out there, but there are few, if any open source engines that fulfill all of the goals that Milton has set out to accomplish – all of the goals which are necessary for such an engine to be useful as an educational tool for others wishing to advance themselves in the field.  In addition to providing a clean, yet advanced reference engine, this thesis will discuss the major software engineering and design choices embodied by Milton.  We will also provide implementation details for a notoriously difficult rendering algorithm known as Metropolis Light Transport (MLT).

Milton is open-source and aims to maintain a polished, well-documented code base.  Documentation for the source itself is available via Doxygen generated documentation. Thorough documentation is also available for the Milton scenefile format (which is probably a good place to start getting a high-level overview of how Milton is structured, especially if you're not interested in the source itself). In addition to the thorough API docs, we also provide a collection of introductory tutorials which walkthrough core Milton concepts in an attempt to familiarize the user with a high-level overview of an otherwise large and foreboding codebase.

