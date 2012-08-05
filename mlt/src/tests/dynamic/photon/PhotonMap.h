/**<!-------------------------------------------------------------------->
   @class  PhotonMap
   @author Travis Fischer (fisch0920@gmail.com)
   @author Matthew Jacobs (jacobs.mh@gmail.com)
   @date   Fall 2008
   
   @brief
      Slightly modified version of Henrik Wann Jensen's open source kd-Tree 
   implementation used for photon mapping.  The PhotonMap is a balanced, 
   axis-aligned binary tree which supports efficient nearest neighbor (KNN) 
   searches over a three-dimensional point set of photons.
   
   @note
      This version implements thread-safe KNN (which is necessary for 
   multithreaded photon mapping) via thread-local storage
   
   @see Photon
   @see PhotonMapper
   @see Jensen's book, "Realistic Image Synthesis Using Photon Mapping" (2001)
      for further details
   <!-------------------------------------------------------------------->**/

#ifndef PHOTON_MAP_H_
#define PHOTON_MAP_H_

#include <Photon.h>
#include <accel/AABB.h>

class  SurfacePoint;
class  Camera;
struct NearestPhotons;

DECLARE_STL_TYPEDEF(std::vector<Photon>, PhotonList);

class PhotonMap {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      /**
       * @param gatherPhotons specifies the maximum number of photons to be 
       *    gathered during KNN (the maximum value of K)
       * @param gatherRadius specifies the maximum KNN search radius
       * 
       * @note
       *    As gatherPhotons -> infinity and gatherRadius -> zero, the 
       * value returned by getIrradianceEstimate converges to the true 
       * irradiance value being estimated (which depends on the types of 
       * photons in question). this is called a consistent estimator, in 
       * that the estimated value only converges to the true value in the 
       * limit as the number of samples -> infinity
       */
      PhotonMap(unsigned gatherPhotons, real_t gatherRadius = INFINITY);
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Adds the given photon to this map
       * 
       * @note
       *    For performance reasons, the underlying kd-Tree is built lazily, 
       * so the effects of adding photons won't be realized until 'init' is 
       * called to build the tree
       */
      inline void addPhoton(const Photon &photon) {
         m_photons.push_back(photon);
         
         ++m_size;
      }
      
      /**
       * @brief
       *    Builds the underlying kd-Tree of photons
       * 
       * @note
       *    All photons should be added via 'addPhoton' before calling init, 
       * and 'addPhoton' should not be called again after initializing the 
       * kd-Tree
       */
      void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs a KNN search starting from the given point @p pos
       * (KNN stands for K-Nearest-Neighbors)
       * 
       * @returns K, the number of neighbors found
       * @returns an array of pointers to photons in @p neighbors
       * @returns the distance from the point given farthest of the K neighbors
       * 
       * @note
       *    getKNN is thread-safe
       * @note 
       *    the returned array should *not* be deallocated by the caller as it 
       *    will be reused by future calls to getKNN from the calling thread
       * 
       * Example usage:
       *    const Photon **neighbors = NULL;
       *    real_t radius = 0;
       *    
       *    unsigned noNeighbors = getKNN(Point3(x, y, z), &neighbors, &radius);
       *    for(unsigned i = 0; i < noNeighbors; ++i) {
       *       const Photon *photon = neighbors[i];
       *       ...
       *    }
       */
      unsigned getKNN(const Point3 &pos, const Photon ***neighbors, 
                      real_t *radius) const;
      
      /**
       * @returns an estimate of the irradiance at the given SurfacePoint with 
       *    respect to the type of illumination represented by the underlying 
       *    photons
       */
      SpectralSampleSet getIrradiance(const SurfacePoint &pt) const;
      
      /**
       * @brief
       *    Multitples the power of all photons in this map by the given scale 
       * factor
       */
      void scalePhotonPower(real_t scaleFactor);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the number of photons stored in this map
       */
      inline unsigned size() const {
         return m_size - 1;
      }
      
      /**
       * @returns the maximum number of photons to search for during KNN
       */
      inline unsigned getGatherPhotons() const {
         return m_gatherPhotons;
      }
      
      /**
       * @returns the maximum radius during KNN gathering
       */
      inline real_t getGatherRadius() const {
         return m_gatherRadius;
      }
      
      /**
       * @returns a list containing the underlying photons stored in this map
       */
      inline const PhotonList &getPhotons() const {
         return m_photons;
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Miscellaneous functionality
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Clears and resets this photon map
       */
      void clear();
      
      /**
       * @brief
       *    Projects all photons stored in this map onto an image and saves 
       * the resulting image to the given file. The projection and image 
       * dimensions are determined from the given Camera
       * 
       * @returns whether or not the image was successfully saved (it may 
       *    fail, for instance, if the given fileName was invalid)
       */
      //bool save(const std::string &fileName, Camera *camera) const;
      
      
      //@}-----------------------------------------------------------------
      
   private:
      ///@name Internal kd-Tree construction and KNN
      //@{-----------------------------------------------------------------
      
      void _balance(Photon **pbal, Photon **porg, unsigned index, 
                   unsigned start, unsigned end, AABB &bb);
      void _median_split(Photon **p, unsigned start, unsigned end, 
                        unsigned median, unsigned axis);
      void _k_nearest_photons(unsigned index, NearestPhotons *np) const;
      
      
      //@}-----------------------------------------------------------------
      
   private:
      PhotonList m_photons;
      
      unsigned   m_gatherPhotons;
      real_t     m_gatherRadius;
      unsigned   m_halfPhotons;
      unsigned   m_size;
};

#endif // PHOTON_MAP_H_

