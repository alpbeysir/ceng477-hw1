#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include "vec.h"
#include "ray_tracer.h"
#include "collision_object.h"



pure float sphere_get_collision(const Sphere& self, const Ray& Ray);
pure vec4f sphere_get_normal(const Sphere* self, vec4fc point);
pure float triangle_get_collision(const std::vector<vec4f>& vertices, const Face& self, const Ray& ray);
pure std::pair<float, const Triangle*> bvh_get_collision(const Scene& scene, const BVHNode* const nodes, const Ray& ray, const int idx = 0);
pure static inline vec4f triangle_get_normal(const std::vector<vec4f>& vertices, const Face& self)
{
	return self.norm;
}

bool do_geometry(const Scene& scene, const Ray& ray, float& t_min, CollisionObject& obj, Material& material, vec4f& hit_point, vec4f& norm);

BVHNode* buildBVH(Scene& scene);

#endif