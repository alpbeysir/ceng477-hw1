#include "collision_object.h"
#include "ray_tracer.h"

//TODO: convert to new near plane format
constfn vec4f plane_pixel_position(const Camera self, const size_t x, const size_t y) {
   cfloat x_ratio = ((float)x / (float)self.image_width);
   cfloat y_ratio = ((float)y / (float)self.image_height);
   arr4f arr = amake(self.near_plane);
   cfloat left = arr[0];
   cfloat right = arr[1];
   cfloat down = arr[2];
   cfloat up = arr[3];

   cfloat horizontal = abs(right-left);
   cfloat vertical = abs(down-up);

   vec4fc right_vector = cross4f(self.gaze, self.up);
   vec4fc down_vector = -self.up;


   vec4fc up_left_corner = self.position + right_vector*left - down_vector*up;

   return up_left_corner + self.gaze*self.near_distance + (x_ratio*horizontal*right_vector) + (y_ratio*vertical*down_vector);
}

