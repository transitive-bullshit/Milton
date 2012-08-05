/**
 * Copyright (C) 2003 Billy Biggs <vektor@dumbterm.net>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifdef HAVE_OPENEXR

#ifndef EXRINPUT_H_INCLUDED
#define EXRINPUT_H_INCLUDED

#ifdef __cplusplus
namespace milton {
extern "C" {
#endif

/**
 * Example usage:
 *
 * exrinput_t *exrin = exrinput_new( "myimage.exr" );
 *
 * for( i = 0; i < exrinput_get_height( exrin ); i++ ) {
 *     double *scanline = exrinput_get_scanline( exrin, i );
 *
 *     for( x = 0; x < exrinput_get_width( exrin ); x++ ) {
 *         r = scanline[ (x * 4) + 0 ];
 *         g = scanline[ (x * 4) + 1 ];
 *         b = scanline[ (x * 4) + 2 ];
 *         a = scanline[ (x * 4) + 3 ];
 *     }
 * }
 *
 * exrinput_delete( exrin );
 */

typedef struct exrinput_s exrinput_t;

/**
 * Opens the filename as an exr file.  Returns 0 on error.
 */
exrinput_t *exrinput_new( const char *filename );

/**
 * Closes the exr file.
 */
void exrinput_delete( exrinput_t *exrinput );

/**
 * Returns the width of the exr file.
 */
unsigned int exrinput_get_width( exrinput_t *exrinput );

/**
 * Returns the height of the exr file.
 */
unsigned int exrinput_get_height( exrinput_t *exrinput );

/**
 * Returns a pointer to the given scanline data from 0 to height-1.
 */
double *exrinput_get_scanline( exrinput_t *exrinput, int num );

#ifdef __cplusplus
}
};
#endif

#endif /* EXRINPUT_H_INCLUDED */

#endif /* HAVE_OPENEXR */

