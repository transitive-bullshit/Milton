/**<!-------------------------------------------------------------------->
   @file   rgbe.h
   @author Bruce Walter (http://www.graphics.cornell.edu/~bjw/)
   @author Travis Fischer (fisch0920@gmail.com)
   @date   January 2009
   
   @brief
      Utilities for reading and writing Ward's rgbe image format.
   
   @note
      This code is from http://www.graphics.cornell.edu/~bjw/rgbe/
   
   @ee also http://www.graphics.cornell.edu/online/formats/rgbe/
   <!-------------------------------------------------------------------->**/

#ifndef RGBE_H_
#define RGBE_H_

/* THIS CODE CARRIES NO GUARANTEE OF USABILITY OR FITNESS FOR ANY PURPOSE.
 * WHILE THE AUTHORS HAVE TRIED TO ENSURE THE PROGRAM WORKS CORRECTLY,
 * IT IS STRICTLY USE AT YOUR OWN RISK. */

#include <stdio.h>

namespace milton {

typedef struct {
  int valid;            /* indicate which fields are valid */
  char programtype[16]; /* listed at beginning of file to identify it 
                         * after "#?".  defaults to "RGBE" */ 
  float gamma;          /* image has already been gamma corrected with 
                         * given gamma.  defaults to 1.0 (no correction) */
  float exposure;       /* a value of 1.0 in an image corresponds to
                         * <exposure> watts/steradian/m^2. 
                         * defaults to 1.0 */
} rgbe_header_info;

/* flags indicating which fields in an rgbe_header_info are valid */
#define RGBE_VALID_PROGRAMTYPE 0x01
#define RGBE_VALID_GAMMA       0x02
#define RGBE_VALID_EXPOSURE    0x04

/* return codes for rgbe routines */
#define RGBE_RETURN_SUCCESS 0
#define RGBE_RETURN_FAILURE -1

/**
 * @brief
 *    writes headers
 * 
 * @note
 *    you may set rgbe_header_info to null if you want to
 */
int RGBE_WriteHeader(FILE *fp, int width, int height, rgbe_header_info *info);

/**
 * @brief
 *    reads headers
 * 
 * @note
 *    you may set rgbe_header_info to null if you want to
 */
int RGBE_ReadHeader(FILE *fp, int *width, int *height, rgbe_header_info *info);


/** 
 * @brief
 *    writes pixels
 * 
 * @note
 *    can write pixels in chunks of any size including single pixels
 */
int RGBE_WritePixels(FILE *fp, const float *data, int numpixels);

/** 
 * @brief
 *    reads pixels
 * 
 * @note
 *    can read pixels in chunks of any size including single pixels
 */
int RGBE_ReadPixels(FILE *fp, float *data, int numpixels);


/** 
 * @brief
 *    writes run length encoded files
 * 
 * @note
 *    must be called to write whole scanlines
 */
int RGBE_WritePixels_RLE(FILE *fp, float *data, int scanline_width,
                         int num_scanlines);

/** 
 * @brief
 *    reads run length encoded files
 * 
 * @note
 *    must be called to read whole scanlines
 */
int RGBE_ReadPixels_RLE(FILE *fp, float *data, int scanline_width,
                        int num_scanlines);

}

#endif /* RGBE_H_ */

