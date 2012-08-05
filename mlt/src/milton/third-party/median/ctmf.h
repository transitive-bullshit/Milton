/* see http://nomis80.org/ctmf.html */

#ifndef CTMF_H
#define CTMF_H

namespace milton {

void ctmf(
   const unsigned char* src, unsigned char* dst,
   int width, int height,
   int src_step_row, int dst_step_row,
   int r, int channels, unsigned long memsize
);

}

#endif

