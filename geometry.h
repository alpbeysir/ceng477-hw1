#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "vec.h"
#include "ray_tracer.h"

pure float sphere_get_collision(const Sphere& self, const Ray& Ray);
pure vec4f sphere_get_normal(const Sphere& self, vec4fc point);
pure float triangle_get_collision(const Face& self, const Ray& ray);
pure vec4f triangle_get_normal(const Face& self);

#endif