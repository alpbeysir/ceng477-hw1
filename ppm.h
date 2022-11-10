#ifndef __ppm_h__
#define __ppm_h__
#include "vec.h"

void write_ppm(std::string filename, vec4f* data, int width, int height);

#endif // __ppm_h__
