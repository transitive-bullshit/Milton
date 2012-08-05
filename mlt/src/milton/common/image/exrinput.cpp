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

#include <stdio.h>
#include <OpenEXR/ImfCRgbaFile.h>
#include "exrinput.h"

using namespace Imf;

namespace milton {

struct exrinput_s {
    ImfInputFile *file;
    ImfRgba *base;
    int width;
    int height;
    double *scanline;
};

exrinput_t *exrinput_new( const char *filename )
{
    exrinput_t *exrinput = malloc( sizeof( exrinput_t ) );
    int xmin, xmax, ymin, ymax;
    const ImfHeader *header;

    if( !exrinput ) {
        fprintf( stderr, "exrinput: Cannot allocate memory.\n" );
        return 0;
    }

    exrinput->file = ImfOpenInputFile( filename );
    if( !exrinput->file ) {
        fprintf( stderr, "exrinput: %s\n", ImfErrorMessage() );
        free( exrinput );
        return 0;
    }

    header = ImfInputHeader( exrinput->file );
    ImfHeaderDataWindow( header, &xmin, &ymin, &xmax, &ymax );
    exrinput->width  = xmax - xmin + 1;
    exrinput->height = ymax - ymin + 1;

    exrinput->base = malloc( exrinput->width * sizeof( ImfRgba ) );
    exrinput->scanline = malloc( exrinput->width * sizeof( double ) * 4 );
    if( !exrinput->base || !exrinput->scanline ) {
        fprintf( stderr, "exrinput: Cannot allocate memory.\n" );
        if( exrinput->scanline ) free( exrinput->scanline );
        if( exrinput->base ) free( exrinput->base );
        ImfCloseInputFile( exrinput->file );
        free( exrinput );
        return 0;
    }

    return exrinput;
}

void exrinput_delete( exrinput_t *exrinput )
{
    ImfCloseInputFile( exrinput->file );
    free( exrinput->scanline );
    free( exrinput->base );
    free( exrinput );
}

unsigned int exrinput_get_width( exrinput_t *exrinput )
{
    return exrinput->width;
}

unsigned int exrinput_get_height( exrinput_t *exrinput )
{
    return exrinput->height;
}

double *exrinput_get_scanline( exrinput_t *exrinput, int num )
{
    ImfRgba *inscanline = exrinput->base;
    double *outscanline = exrinput->scanline;
    int i;

    ImfInputSetFrameBuffer( exrinput->file, exrinput->base - (num * exrinput->width), 1, exrinput->width );
    ImfInputReadPixels( exrinput->file, num, num );

    for( i = 0; i < exrinput->width; i++ ) {
        float temp;
        temp = ImfHalfToFloat( inscanline[ i ].r );
        *outscanline++ = temp;
        temp = ImfHalfToFloat( inscanline[ i ].g );
        *outscanline++ = temp;
        temp = ImfHalfToFloat( inscanline[ i ].b );
        *outscanline++ = temp;
        temp = ImfHalfToFloat( inscanline[ i ].a );
        *outscanline++ = temp;
    }

    return exrinput->scanline;
}

}

#endif /* HAVE_OPENEXR */

