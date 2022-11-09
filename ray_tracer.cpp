#include "collision_object.h"
#include "ray_tracer.h"

//TODO: convert to new near plane format
constfn vec4f plane_pixel_position(const Camera self, const size_t x, const size_t y) {
   cfloat x_ratio = (float)x / (float)self.image_width;
   cfloat y_ratio = (float)y / (float)self.image_height;
   //return add4f(self.near_plane, add4f(mul4fs(self.right, x_ratio), mul4fs(self.down, y_ratio)));
   return vec4f{ 1,1,1,1 };
}

vec4fc vec127 = {127.0f, 127.0f, 127.0f, 127.0f};
vec4fc vec128 = {128.0f, 128.0f, 128.0f, 128.0f};