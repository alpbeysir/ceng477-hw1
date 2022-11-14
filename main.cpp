#include <float.h>
#include "ppm.h"
#include "vec.h"
#include "parser.h"
#include "ray_tracer.h"
#include "collision_object.h"
#include "geometry.h"
#include "shading.h"

std::pair<vec4f, bool> trace(const Scene& scene, Ray ray)
{
	float t_min = FLT_MAX;
	int depth = scene.max_recursion_depth;
	vec4f color = _mm_set1_ps(0);
	vec4f reflectance = _mm_set1_ps(1);
	bool hit_info = false;

	while(depth --> 0) { /* Hoho goes to operator goes to brr */
		CollisionObject obj;
		Material material;
		vec4f norm = vec4f{ 0,0,0,0 };
		vec4f hit_point = vec4f{ 0,0,0,0 };
		vec4f diffuse_shade = vec4f{0,0,0,0};
		vec4f specular_shade = vec4f{0,0,0,0};
		t_min = INFINITY;
		
		if (not do_geometry(scene, ray, t_min, obj, material, hit_point, norm)) {
			color += scene.background_color;
			break;
		}
		hit_info = true;

		do_shading(scene, hit_point, norm, material, ray, diffuse_shade, specular_shade);

		Ray bounced_ray(
			hit_point + norm*scene.shadow_ray_epsilon,
			-2.0f * dot4f(norm, ray.direction)*norm + ray.direction
		);

		color += (material.ambient*scene.ambient_light + diffuse_shade + specular_shade) * reflectance;

		if(material.is_mirror) {
			reflectance *= material.mirror;
			ray = bounced_ray;
		} else {
			break;
		}
	}
	return { color , hit_info };
}

void tone_map(vec4f& pixel, const bool& hit_info, vec4fc max_intensity) {
	if(hit_info) {
		pixel = (pixel /* - vec4f{65.0f, 65.0f, 65.0f, 0.0f} */) / max_intensity;
	}
}

void render_camera(Scene& scene, int camIndex)
{
	auto& current_camera = scene.cameras[camIndex];

	auto image = new vec4f[current_camera.image_width * current_camera.image_height];
	auto image_hit_info = new bool[current_camera.image_width * current_camera.image_height];

	#pragma omp parallel for
	for (int i = 0; i < current_camera.image_width; i++)
	{
		for (int j = 0; j < current_camera.image_height; j++)
		{
			Ray ray = Ray::from_to(current_camera.position, plane_pixel_position(current_camera, i, j));
			ray.direction = normalize4f(ray.direction);
			auto zort = trace(scene, ray);
			image[i*current_camera.image_height + j] = zort.first;
			image_hit_info[i * current_camera.image_height + j] = zort.second;
		}
	}
	
	for (int i = 0; i < current_camera.image_width; i++)
	{
		for (int j = 0; j < current_camera.image_height; j++)
		{
			vec4f& current_pixel = image[i*current_camera.image_height + j];
			bool& current_hit_info = image_hit_info[i*current_camera.image_height + j];
			tone_map(current_pixel, current_hit_info, vec4f{255.0, 255.0, 255.0, 255.0f});
		}
	}

	write_ppm(current_camera.image_name, image, current_camera.image_width, current_camera.image_height);
}

int main(int argc, char* argv[])
{
	Scene scene;
	scene.loadFromXml(argc < 2 ? "./inputs/horse_and_mug.xml" : argv[1]);
	scene.bvh = buildBVH(scene);
	for (size_t i = 0; i < scene.cameras.size(); i++)
	{
		render_camera(scene, i);
	}
}