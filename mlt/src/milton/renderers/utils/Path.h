/**<!-------------------------------------------------------------------->
   @class  Path
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Core data structure for manipulating a sequence x0,x1,...,xk of points on 
   scene surfaces. Paths are the central unit in the path integral formulation
   of light transport, upon which path tracing, bidirectional path tracing, and
   MLT are all founded.
   
   Each Path is assumed to either start at an emitter (light subpath), end at a 
   sensor (eye subpath), or both start at an emitter and end at a sensor 
   (complete, valid path).
   
   @note individual vertices (see PathVertex) are stored internally in an 
      std::deque
   <!-------------------------------------------------------------------->**/

#ifndef PATH_H_
#define PATH_H_

#include <renderers/utils/PathVertex.h>

namespace milton {

class Renderer;

class MILTON_DLL_EXPORT Path {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   Path(const PathVertexList &vertices, Renderer *renderer)
         : m_vertices(vertices), m_renderer(renderer)
      { }
      
      inline   Path(Renderer *renderer)
         : m_renderer(renderer)
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Functionality related to path as a whole
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the radiance propagated along this path in the direction 
       *    of lightflow (beginning with an emitter)
       */
      inline const SpectralSampleSet &getRadiance() const {
         return m_radiance;
      }
      
      /**
       * @returns the unweighted contribution of this path broken up into 
       *    a light subpath prefix of length @p s and an eye subpath suffix 
       *    of length @p t
       * 
       * @note this method contains an implicit visibility check between the 
       *    connecting edge which will not be performed if (s + t) is equal 
       *    to the length of this path (since this path is already assumed 
       *    to be 'valid', visibility is therefore guaranteed in this case)
       * @note if @p tentative is true, the aforementioned visibility check 
       *    will not be evaluated (visibility is assumed)
       * @note either this light subpath or the given eye subpath may be 
       *    empty, but not both (an empty path is considered invalid), 
       *    which implies that (s + t) >= 2
       * 
       * @see section 10.2 (pgs 302-305) of Veach's thesis for more details
       */
      SpectralSampleSet getContribution(unsigned s, unsigned t, 
                                        bool tentative = false);
      
      real_t getPd(unsigned s, unsigned t, bool tentative = false);
      
      /**
       * @returns an array of @p k + 1 probability densities in @p pdfs, 
       *    corresponding to all of the @p k + 1 ways in which this path with
       *    @p k vertices could be broken up into different length light and 
       *    eye subpaths
       * 
       * @param k denotes the path length of interest (k <= the length of this
       *    path)
       * @param actualS denotes the length of the actual light path which this 
       *    path was generated from (actualS <= the length of this path)
       * @param pdfs is an out-array which is assumed to be preallocated to 
       *    hold at least @p k + 1 real_ts
       * 
       * @note probability densities are with respect to a surface area measure
       *    on the space of all paths
       * 
       * @see section 10.2 (pgs 302-305) of Veach's thesis for more details
       */
      void getPds(unsigned k, unsigned actualS, real_t *pdfs);
      
      
      //@}-----------------------------------------------------------------
      ///@name Path deletion operations
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Clears this Path of all vertices, resulting in an empty path
       */
      inline void clear() {
         m_vertices.clear();
      }
      
      /**
       * @returns a copy of the path formed by the left @p n vertices of the 
       *    form:  x0,x1,...,xn-1
       * @note @p n is inclusive here, so if @p n is zero, the returned path 
       *    will be empty, and if @p n is equal to the length of this path, 
       *    the returned path will equal this path
       */
      inline Path left(unsigned n) const {
         ASSERT(n <= length());
         
         return Path(PathVertexList(m_vertices.begin(), m_vertices.begin() + n),
                     m_renderer);
      }
      
      /**
       * @returns a copy of the path formed by the @p n right-most vertices 
       *    of the form:  x(k-n),x(k-n+1),...,x(k-1)  (where k represents the 
       *    length of this path)
       * @note @p n is inclusive here, so if @p n is zero, the returned path 
       *    will be empty, and if @p n is equal to the length of this path, 
       *    the returned path will equal this path
       */
      inline Path right(unsigned n) const {
         ASSERT(n <= length());
         
         return Path(PathVertexList(m_vertices.end() - n, m_vertices.end()),
                     m_renderer);
      }
      
      /**
       * @brief
       *    Removes the back vertex (last light vertex) in this Path
       */
      inline void pop_back() {
         ASSERT(length() > 0);
         m_vertices.pop_back();
      }
      
      /**
       * @brief
       *    Removes the front vertex (last eye vertex) in this Path
       */
      inline void pop_front() {
         ASSERT(length() > 0);
         m_vertices.pop_front();
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Path addition operations
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Samples the BSDF at the end of this light path, and attempts to 
       * add a new PathVertex corresponding to the first surface intersected 
       * in the sampled direction, updating all internal data as necessary.
       * 
       * @param roulette denotes whether or not to apply standard russian 
       *    roulette to the decision of whether or not to add a vertex
       * 
       * @returns whether or not a vertex was successfully added
       * @note may return false if no surface was found in the sampled direction
       *    or if @p roulette is true and the random walk is terminated via 
       *    russian roulette
       */
      bool append(bool roulette = false, bool sampleBSDF = true);
      
      /**
       * @brief
       *    Samples the BSDF at the end of this eye path, and attempts to 
       * add a new PathVertex corresponding to the first surface intersected 
       * in the sampled direction, updating all internal data as necessary.
       * 
       * @param roulette denotes whether or not to apply standard russian 
       *    roulette to the decision of whether or not to add a vertex
       * 
       * @returns whether or not a vertex was successfully added
       * @note may return false if no surface was found in the sampled direction
       *    or if @p roulette is true and the random walk is terminated via 
       *    russian roulette
       */
      bool prepend(bool roulette = false, bool sampleBSDF = true);
      
      /**
       * @brief
       *    Adds the given PathVertex at the end of this eye path
       * 
       * @returns true if the point was successfully added or false if the 
       *    operation would invalidate this path
       */
      bool prepend(const PathVertex &v1);
      
      /**
       * @brief
       *    Appends the given eye subpath onto this light subpath, updating 
       * both the cumulative eye and light contributions along the way
       * 
       * @note either there are no restrictions or edge cases with respect 
       *    to path lengths; either or both paths may be empty
       * @returns true if the resulting path is valid, false otherwise (the 
       *    resulting path will be invalid -- zero contribution -- if the 
       *    last light subpath vertex first eye subpath vertex are not 
       *    mutually visible)
       */
      bool append (const Path &path);
      
      /**
       * @brief
       *    Adds the given PathVertex at the end of this light path
       * 
       * @returns true if the point was successfully added or false if the 
       *    operation would invalidate this path
       */
      bool append (const PathVertex &v1);
      
      
      //@}-----------------------------------------------------------------
      ///@name PathVertex accessors
      //@{-----------------------------------------------------------------
      
      inline PathVertex &operator[](unsigned index) {
         ASSERT(index < length());
         
         return m_vertices[index];
      }
      
      inline const PathVertex &operator[](unsigned index) const {
         ASSERT(index < length());
         
         return m_vertices[index];
      }
      
      inline PathVertex &front() {
         ASSERT(length() > 0);
         
         return m_vertices[0];
      }
      
      inline const PathVertex &front() const {
         ASSERT(length() > 0);
         
         return m_vertices[0];
      }

      inline PathVertex &back() {
         ASSERT(length() > 0);
         
         return m_vertices[length() - 1];
      }
      
      inline const PathVertex &back() const {
         ASSERT(length() > 0);
         
         return m_vertices[length() - 1];
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Size accessors
      //@{-----------------------------------------------------------------
      
      inline unsigned length() const {
         return m_vertices.size();
      }
      
      inline unsigned getNoVertices() const {
         return m_vertices.size();
      }
      
      inline unsigned getNoEdges() const {
         const unsigned noVertices = getNoVertices();
         
         return (noVertices < 2 ? 0 : noVertices - 1);
      }
      
      inline bool empty() const {
         return (length() == 0);
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Miscellaneous functionality
      //@{-----------------------------------------------------------------
      
      inline Renderer *getRenderer() {
         return m_renderer;
      }
      
      inline void setRenderer(Renderer *renderer) {
         m_renderer = renderer;
      }
      
      std::string toHeckbertNotation() const;
      
      bool operator==(const Path &p) const;
      
      inline bool operator!=(const Path &p) const {
         return !(*this == p);
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      /**
       * @brief
       *    Samples the BSDF at the end of the path (which end is determined 
       * implicitly by the adjoint parameter), and attempts to add a new 
       * PathVertex corresponding to the first surface intersected in the 
       * sampled direction, updating all internal data as necessary.
       * 
       * @param roulette denotes whether or not to apply standard russian 
       *    roulette to the decision of whether or not to add a vertex
       * 
       * @returns whether or not a vertex was successfully added
       * @note may return false if no surface was found in the sampled direction
       *    or if @p roulette is true and the random walk is terminated via 
       *    russian roulette
       */
      bool _samplePathVertex(bool roulette, bool adjoint, bool sampleBSDF);
      
      /**
       * @brief
       *    Initializes light-path vertex @p y with respect to the point given
       * 
       * @param roulette denotes whether or not to apply standard russian 
       *    roulette to the decision of whether or not to add a vertex
       * 
       * @note @p y and @p pt are assumed to be mutually visible to each other
       * @returns the updated cumulative light subpath contribution (@p alphaL)
       *    and the updated cumulative light subpath probability density (@p pL)
       *    and whether or not to add the vertex (if russian roulette is 
       *    disabled or succeeded)
       */
      bool _initL(PathVertex &y, const Vector3 &wi, const Vector3 &wo, real_t t,
                  const SurfacePoint *pt, SpectralSampleSet &alphaL, real_t &pL,
                  bool roulette = false) const;
      
      /**
       * @brief
       *    Initializes eye-path vertex @p z with respect to the point given
       * 
       * @param roulette denotes whether or not to apply standard russian 
       *    roulette to the decision of whether or not to add a vertex
       * 
       * @note @p z and @p pt are assumed to be mutually visible to each other
       * @returns the updated cumulative eye subpath contribution (@p alphaE)
       *    and the updated cumulative eye subpath probability density (@p pE)
       *    and whether or not to add the vertex (if russian roulette is 
       *    disabled or succeeded)
       */
      bool _initE(PathVertex &z, const Vector3 &wi, const Vector3 &wo, real_t t,
                  const SurfacePoint *pt, SpectralSampleSet &alphaE, real_t &pE,
                  bool roulette = false) const;
      
      /**
       * @returns the radiance propagated along this path in the direction 
       *    of lightflow (beginning with an emitter)
       */
      void _computeRadiance();
      
   protected:
      PathVertexList    m_vertices;
      Renderer         *m_renderer;
      SpectralSampleSet m_radiance;
};

/// Prints a Path to an output stream
MILTON_DLL_EXPORT std::ostream &operator<<(std::ostream &os, const Path &path);

}

#endif // PATH_H_

