#include <float.h>
#include "ppm.h"
#include "vec.h"
#include "parser.h"
#include "ray_tracer.h"
#include "collision_object.h"
#include "geometry.h"
#include "shading.h"

void trace(Scene& scene, Ray& ray, int depth, vec4f& color, bool& hit_info)
{
	if (depth <= 0)
		return;

	float t_min = FLT_MAX;
	CollisionObject obj;
	Material material;
	vec4f norm;
	vec4f hit_point;
	
	if(not do_geometry(scene, ray, hit_info, depth, t_min, obj, material, hit_point, norm, color)) {
		return;
	};

	vec4f diffuse_shade = vec4f{0,0,0,0};
	vec4f specular_shade = vec4f{0,0,0,0};

	do_shading(scene, hit_point, norm, material, ray, diffuse_shade, specular_shade);
	
	auto reverse_incoming = normalize4f(mul4fs(ray.direction, -1.0f));
	auto incoming_to_normal_projection = sub4f(reverse_incoming, norm);
	Ray bounced_ray(hit_point, add4f(reverse_incoming, mul4fs(incoming_to_normal_projection, 2.0f)));

	color += scene.ambient_light + diffuse_shade + specular_shade;
	// trace(scene, bounced_ray, depth - 1, color, hit_info);
}

void gamma_correction(vec4f* image, int width, int height) {
	for(int i = 0; i < width; i++) {
		for(int j = 0; j < height; j++) {
			vec4f& current_pixel = image[i*height+ j];
			arr4fc vValue = amake(current_pixel);
			cfloat gamma = 1.0f/2.2f;
			current_pixel = vec4f{powf(vValue[0], gamma), powf(vValue[1], gamma), powf(vValue[2], gamma), powf(vValue[3], gamma)};
		}
	}
}

void fix_tone(vec4f& pixel, const bool& hit_info, cfloat max_intensity) {
	if(hit_info)
		pixel /= max_intensity;
}

void render_camera(Scene& scene, int camIndex)
{
	auto& current_camera = scene.cameras[camIndex];

	auto image = new vec4f[current_camera.image_width * current_camera.image_height];
	auto image_hit_info = new bool[current_camera.image_width * current_camera.image_height];
	
	float max_intensity = -INFINITY;

	for (int i = 0; i < current_camera.image_width; i++)
	{
		for (int j = 0; j < current_camera.image_height; j++)
		{
			vec4f& current_pixel = image[i*current_camera.image_height + j];
			bool& current_hit_info = image_hit_info[i*current_camera.image_height + j];
			Ray ray = Ray::from_to(current_camera.position, plane_pixel_position(current_camera, i, j));
			current_pixel = vec4f{0,0,0,0};
			trace(scene, ray, scene.max_recursion_depth, current_pixel, current_hit_info);
			max_intensity = std::max(max_intensity, len4f(current_pixel));
		}
	}

	std::cout << "INTENSITY:\t" << max_intensity << '\n';

	cfloat gamma = 1.0f/2.2f;
	cfloat mrgamma = -1.0/gamma;
	cfloat A = powf(max_intensity, -1.0f/gamma);
	
	for (int i = 0; i < current_camera.image_width; i++)
	{
		for (int j = 0; j < current_camera.image_height; j++)
		{
			vec4f& current_pixel = image[i*current_camera.image_height + j];
			bool& current_hit_info = image_hit_info[i*current_camera.image_height + j];
			fix_tone(current_pixel, current_hit_info, max_intensity);
		}
	}
	// gamma_correction(image, current_camera.image_width, current_camera.image_height);
	write_ppm(current_camera.image_name, image, current_camera.image_width, current_camera.image_height);
}

int main()
{
	Scene scene;
	scene.loadFromXml("./inputs/simple_shading.xml");
	for (size_t i = 0; i < scene.cameras.size(); i++)
	{
		render_camera(scene, i);
	}
}