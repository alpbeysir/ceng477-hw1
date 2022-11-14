#ifndef __SHADING_H__
#define __SHADING_H__
#include "vec.h"
#include "parser.h"
#include "geometry.h"

void do_shading(const Scene& scene, vec4fc hit_point, vec4fc norm, const Material& material, const Ray& incoming_ray, vec4f& diffuse_shade, vec4f& specular_shade);

#endif