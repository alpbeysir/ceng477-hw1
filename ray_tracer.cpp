#include "collision_object.h"
#include "ray_tracer.h"

//TODO: convert to new near plane format
constfn vec4f plane_pixel_position(const Camera self, const size_t x, const size_t y) {
   cfloat x_ratio = (float)x / (float)self.image_width;
   cfloat y_ratio = (float)y / (float)self.image_height;
   arr4f arr = amake(self.near_plane);
   vec4fc side = cross4f(self.up, self.gaze);
   vec4fc right = (arr[0] + arr[1]) * side;
   vec4fc down = (arr[2] + arr[3]) * (-self.up);
   return add4f(self.position, add4f(mul4fs(right, x_ratio), mul4fs(down, y_ratio)));
}

