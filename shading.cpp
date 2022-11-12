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

void do_diffuse_shading(const PointLight& light, const Scene& scene, vec4fc hit_point, vec4fc norm, const Material& material, vec4f& diffuse_shade) {
	Ray shadow_ray = Ray::from_to(hit_point + norm*scene.shadow_ray_epsilon, light.position);
	if (is_shadowed(shadow_ray, scene.spheres, scene.triangles, scene.vertex_data))
		return;

	vec4fc lh = light.position - hit_point;
	cfloat r2 = dot4f(lh, lh);
	vec4fc zor = light.intensity / r2;
	diffuse_shade += material.diffuse * std::max(0.0f, dot4f(norm,lh)) * zor;
}

void do_specular_shading(const PointLight& light, const Scene& scene, vec4fc hit_point, vec4fc norm, const Material& material, const Ray& incoming_ray, vec4f& specular_shade) {
	Ray shadow_ray = Ray::from_to(hit_point + norm*scene.shadow_ray_epsilon, light.position);
	if(is_shadowed(shadow_ray, scene.spheres, scene.triangles, scene.vertex_data))
		return;
	vec4fc h = normalize4f(-incoming_ray.direction + shadow_ray.direction);

	vec4fc lh = light.position - hit_point;
	cfloat r2 = dot4f(lh,lh);
	vec4fc zor = light.intensity / r2;

	specular_shade += material.specular * powf(std::max(dot4f(norm,h), 0.0f), material.phong_exponent) * zor;
}

void do_shading(const Scene& scene, vec4fc hit_point, vec4fc norm, const Material& material, const Ray& incoming_ray, vec4f& diffuse_shade, vec4f& specular_shade) {
	for(const auto& light : scene.point_lights) {
		do_diffuse_shading(light, scene, hit_point, norm, material, diffuse_shade);
		do_specular_shading(light, scene, hit_point, norm, material, incoming_ray, specular_shade);
	}
}