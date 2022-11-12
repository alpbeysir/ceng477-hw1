#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "vec.h"
#include "ray_tracer.h"
#include "collision_object.h"

pure float sphere_get_collision(const Sphere& self, const Ray& Ray);
pure vec4f sphere_get_normal(const Sphere* self, vec4fc point);
pure float triangle_get_collision(const std::vector<vec4f>& vertices, const Face& self, const Ray& ray);
pure vec4f triangle_get_normal(const std::vector<vec4f>& vertices, const Face& self);

bool do_geometry(const Scene& scene, const Ray& ray, bool& hit_info, int depth, float& t_min, CollisionObject& obj, Material& material, vec4f& hit_point, vec4f& norm, vec4f& color);

#endif