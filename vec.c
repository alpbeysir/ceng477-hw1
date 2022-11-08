#include <vec.h>

void print4f(vec4fc self) {
   array4fc arr = vec4f_to_array4f(self);
   printf("{%g %g %g %g}\n", arr.data[0], arr.data[1], arr.data[2], arr.data[3]);
}