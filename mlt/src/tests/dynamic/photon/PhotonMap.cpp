/**<!-------------------------------------------------------------------->
   @file   PhotonMap.cpp
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

#include "PhotonMap.h"
#include <milton.h>
#include <QtCore>

/// thread-local utility struct which is used internally during KNN search
struct NearestPhotons {
   unsigned       max_photons;
   unsigned       num_found;
   unsigned       heap;
   Point3         pos;
   real_t        *dist2;
   const Photon **photons;
};

PhotonMap::PhotonMap(unsigned gatherPhotons, real_t gatherRadius)
   : m_gatherPhotons(gatherPhotons), m_gatherRadius(gatherRadius)
{
   clear();
}

void PhotonMap::clear() {
   m_photons.clear();
   
   // push on a dummy photon to make the vector 1-indexed for heap
   Photon p;
   m_photons.push_back(p);
   m_size = 1;
}

void PhotonMap::init() {
   // build the underlying balanced kd-Tree
   if (size() > 1) {
      Photon **orig = (Photon **) malloc(sizeof(Photon*) * (size() + 1));
      Photon **tree = (Photon **) malloc(sizeof(Photon*) * (size() + 1));
      AABB aabb;
      
      // add all photons
      for (unsigned i = 1; i <= size(); ++i) {
         orig[i] = &m_photons[i];
         m_photons[i].plane = 0;
         
         aabb.add(m_photons[i].position);
         tree[i] = NULL;
      }
      
      // recursively balance tree
      _balance(tree, orig, 1, 1, size(), aabb);
      
      free(orig);
      
      unsigned d, j = 1, temp = 1;
      Photon temp_photon = m_photons[j];
      
      // update internal bookkeeping
      for (unsigned i = 1; i <= size(); ++i) {
         d = tree[j] - &m_photons[0];
         tree[j] = NULL;
         
         if (d != temp) {
            m_photons[j] = m_photons[d];
         } else {
            m_photons[j] = temp_photon;

            if (i < size()) {
               for (; temp <= size(); ++temp) 
                  if (tree[temp] != NULL)
                     break;
               
               temp_photon = m_photons[temp];
               j = temp;
            }
            
            continue;
         }
         
         j = d;
      }
      
      free(tree);
   }
   
   m_halfPhotons = m_size / 2 - 1;
}

void PhotonMap::_balance(Photon **pbal, Photon **porg, unsigned index, 
                         unsigned start, unsigned end, AABB &aabb)
{
   const unsigned last = end - start + 1;
   unsigned median = 1;
   
   while ((median * 4) <= last)
      median += median;
   
   if ((3 * median)  <= last) {
      median += median;
      median += start - 1;
   } else {
      median  = end - median + 1;
   }
   
   const unsigned axis = aabb.getMaxExtent();
   assert(axis >= 0 && axis <3);
   _median_split(porg, start, end, median, axis);
   
   pbal[index] = porg[median];
   pbal[index]->plane = axis;
   
   if (median > start) {
      if (start < median - 1) {
         const real_t tmp = aabb.max[axis];
         aabb.max[axis]   = pbal[index]->position[axis];
         _balance(pbal, porg, 2 * index, start, median - 1, aabb);
         aabb.max[axis]   = tmp;
      } else {
         pbal[2 * index]  = porg[start];
      }
   }
   
   if (median < end) {
      if (median + 1 < end) {
         const real_t tmp = aabb.min[axis];
         aabb.min[axis]   = pbal[index]->position[axis];
         _balance(pbal, porg, 2 * index + 1, median + 1, end, aabb);
         aabb.min[axis]   = tmp;
      } else {
         pbal[2 * index + 1] = porg[end];
      }
   }
}

void PhotonMap::_median_split(Photon **p, unsigned start, unsigned end, 
                              unsigned median, unsigned axis)
{
   unsigned left  = start;
   unsigned right = end;
   
   while (right > left) {
      const real_t v = p[right]->position[axis];
      unsigned i = left - 1;
      unsigned j = right;
      
      for (;;) {
         while (p[++i]->position[axis] < v);
         while (p[--j]->position[axis] > v && j > left);
         if (i >= j)
            break;
         
         Photon *temp = p[i];
         p[i] = p[j];
         p[j] = temp;
      }
      
      Photon *temp = p[i];
      p[i]     = p[right];
      p[right] = temp;
      
      if (i >= median)
         right = i - 1;
      if (i <= median)
         left  = i + 1;		
   }
}

void PhotonMap::_k_nearest_photons(unsigned index, NearestPhotons *np) const {
   if (index > size())
      return;
   
   const Photon *p = &m_photons[index];
   real_t dist = np->pos[p->plane] - p->position[p->plane];
   
   if (dist > 0) {
      _k_nearest_photons(2 * index + 1, np);
      
      if (dist * dist < np->dist2[0])
         _k_nearest_photons(2 * index, np);
   } else {
      _k_nearest_photons(2 * index, np);
      
      if (dist * dist < np->dist2[0])
         _k_nearest_photons(2 * index + 1, np);
   }
   
   const real_t dist2 = (np->pos - p->position).getMagnitude2();
   
   if (dist2 < np->dist2[0]) {
      // there was enough room to just add the photon to the end
      if (np->num_found < np->max_photons) {
         np->num_found++;
         np->dist2[np->num_found] = dist2;
         np->photons[np->num_found] = p;
      } else {
         // we need to kick out a photon and build a heap if we don't have one
         unsigned j, parent;
         
         if (!np->heap) {
            real_t temp;
            const Photon *photon;
            unsigned half_found = np->num_found >> 1;
            
            for (unsigned k = half_found; k >= 1; --k) {
               parent = k;
               photon = np->photons[k];
               temp = np->dist2[k];
               while (parent <= half_found) {
                  j = parent + parent;
                  if (j < np->num_found && np->dist2[j] < np->dist2[j+1])
                     j++;
                  if (temp >= np->dist2[j])
                     break;
                  
                  np->dist2[parent] = np->dist2[j];
                  np->photons[parent] = np->photons[j];
                  parent = j;
               }
               
               np->photons[parent] = photon;
               np->dist2[parent] = temp;
            }
            
            np->heap = 1;
         }

         // insert photon into the heap
         parent = 1;
         j = 2;
         while (j <= np->num_found) {
            if (j < np->num_found && np->dist2[j] < np->dist2[j+1])
               j++;
            if (dist2 > np->dist2[j])
               break;
            
            np->dist2[parent] = np->dist2[j];
            np->photons[parent] = np->photons[j];
            parent = j;
            j += j;
         }
         
         np->photons[parent] = p;
         np->dist2[parent] = dist2;
         np->dist2[0] = np->dist2[1];
      }
   }
}

unsigned PhotonMap::getKNN(const Point3 &pos, const Photon ***neighbors, 
                           real_t *radius) const
{
   static const std::string &key = "photonKdTreeNearestPhotons";
   NearestPhotons *np = 
      ResourceManager::getValueThreadLocal<NearestPhotons*>(key, NULL);
   
   if (NULL == np) {
      np = new NearestPhotons();
      np->dist2       = (real_t*) malloc(sizeof(real_t) * (m_gatherPhotons + 1));
      np->photons     = (const Photon**) malloc(sizeof(Photon*) * (m_gatherPhotons + 1));
      np->max_photons = m_gatherPhotons;
      
      // TODO: need some way of cleaning up thread-local storage
      ResourceManager::insertThreadLocal<NearestPhotons*>(key, np);
   }
   
   // initialize search structure
   np->pos       = pos.data;
   np->num_found = 0;
   np->heap      = 0;
   np->dist2[0]  = m_gatherRadius * m_gatherRadius;
   
   // perform KNN search
   _k_nearest_photons(1, np);
   
   *neighbors = &np->photons[1];
   *radius    = sqrt(np->dist2[0]);
   
   return np->num_found;
}

SpectralSampleSet PhotonMap::getIrradiance(const SurfacePoint &pt) const {
   const Photon **neighbors = NULL;
   real_t radius = 0;
   
   const unsigned noPhotons  = getKNN(pt.position, &neighbors, &radius);
   const real_t filterK      = 1.0; // cone filter; see Jensen, pg 82
   const real_t weightFactor = 1.0 / (filterK * radius);
   const real_t filterNorm   = 1.0 / (1.0 - 2.0 / (3 * filterK));
   SpectralSampleSet irradiance;
   
   if (noPhotons < 8)
      return irradiance;
   
   // sum irradiance from all photons
   for(unsigned i = 0; i < noPhotons; ++i) {
      const Photon *p     = neighbors[i];
      
      // normalized cone filter:  1 - dist / (k * radius)
      const real_t dist   = (p->position - pt.position).getMagnitude();
      const real_t weight = MAX(0, 1 - dist * weightFactor);
      
      irradiance += pt.bsdf->getBSDF(p->w) * p->Li * weight;
   }
   
   return irradiance * (filterNorm / (M_PI * radius * radius));
}

void PhotonMap::scalePhotonPower(real_t scaleFactor) {
   FOREACH(PhotonListIter, m_photons, iter) {
      iter->Li *= scaleFactor;
   }
}

#if 0
bool PhotonMap::save(const std::string &fileName, Camera *camera) const {
   ASSERT(camera);
   
   const unsigned width  = camera->getWidth();
   const unsigned height = camera->getHeight();
   Image image(width, height);
   
   FOREACH(PhotonListConstIter, m_photons, iter) {
      const Photon &photon = *iter;
      
      const Point2 &pt = camera->getProjection(photon.position);
      
      const unsigned x = floor(pt[0]);
      const unsigned y = floor(pt[1]);
      
      if (x < width && y < height) {
         const Pixel &p = Pixel::fromVector(photon.Li.getRGB());
         
         image.setPixel(y, x, &p);
      }
   }
   
   return image.save(fileName.c_str());
}
#endif

