#include "shading.h"

bool is_shadowed(const Ray& ray, const std::vector<Sphere>& spheres, const std::vector<Triangle>& tris, const std::vector<vec4f>& vertices)
{
	for (auto& sphere : spheres)
	{
		float t = sphere_get_collision(sphere, ray);
		if (0.0f < t and t < 1.0f)
			return true;
	}

	for (auto& tri : tris)
	{
		float t = triangle_get_collision(vertices, tri.indices, ray);
		if (0.0f < t and t < 1.0f)
			return true;
	}
	return false;
}

void do_diffuse_shading(const Ray& shadow_ray, vec4fc lh, vec4fc zor, vec4fc norm, cfloat r2, const Material& material, vec4f& diffuse_shade) {
	diffuse_shade += material.diffuse * std::max(0.0f, dot4f(norm,normalize4f(lh))) * zor;
}

void do_specular_shading(const Ray& shadow_ray, vec4fc lh, vec4fc zor, vec4fc norm, cfloat r2, const Material& material, const Ray& incoming_ray, vec4f& specular_shade) {
	vec4fc h = normalize4f(normalize4f(shadow_ray.direction) - normalize4f(incoming_ray.direction));

	specular_shade += material.specular * powf(std::max(0.0f, dot4f(norm,h)), material.phong_exponent) * zor;
}

void do_shading(const Scene& scene, vec4fc hit_point, vec4fc norm, const Material& material, const Ray& incoming_ray, vec4f& diffuse_shade, vec4f& specular_shade) {
	for(const auto& light : scene.point_lights) {
		Ray shadow_ray = Ray::from_to(hit_point + norm*scene.shadow_ray_epsilon, light.position);
		if (is_shadowed(shadow_ray, scene.spheres, scene.triangles, scene.vertex_data))
			continue;;
		vec4fc lh = shadow_ray.direction;
		cfloat r2 = dot4f(lh, lh);
		vec4fc zor = light.intensity / r2;
		do_diffuse_shading(shadow_ray, lh, zor, norm, r2, material, diffuse_shade);
		do_specular_shading(shadow_ray, lh, zor, norm, r2, material, incoming_ray, specular_shade);
	}
}