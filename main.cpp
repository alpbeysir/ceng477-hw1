#include <float.h>
#include "ppm.h"
#include "vec.h"
#include "parser.h"
#include "ray_tracer.h"
#include "collision_object.h"
#include "geometry.h"

std::pair<float, CollisionObject> nearest_object(const Ray& ray, const std::vector<Sphere>& spheres, const std::vector<Triangle>& tris, const std::vector<vec4f>& vertices) {
	float t_min = INFINITY;
	CollisionObject obj;

	for (auto& sphere : spheres)
	{
		// std::cout << ray.direction << '\t' << ray.start << '\t' << sphere.position << ' ' << sphere.radius << '\n';
		float t = sphere_get_collision(sphere, ray);

		if(amake(ray.direction) == amake(vec4f{-0.45, 1.5, -1, 0})) {
			std::cout << t << '\n';
		}
		
		if (t_min > t)
		{
			t_min = t;
			obj.type = COLLISION_OBJECT_SPHERE;
			obj.data.sphere = &sphere;
		}
	}

	for (auto& tri : tris)
	{
		if(len4f(ray.direction - vec4f{0.45, 0.4, -1, 0}) < 0.0000006f)
			std::cout << "ZORT:\t" << '\t' << vertices[tri.indices.v0_id] << '\t' << vertices[tri.indices.v1_id] << '\t' << vertices[tri.indices.v2_id] << "\n";
		float t = triangle_get_collision(vertices, tri.indices, ray);
		if (t_min > t)
		{
			t_min = t;
			obj.type = COLLISION_OBJECT_TRI;
			obj.data.tri = &tri;
		}
	}

	return std::pair<float, CollisionObject>(t_min, obj);
}

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

void trace(Scene& scene, Ray& ray, int depth, vec4f& color, bool& hit_info)
{
	if (depth <= 0)
		return;

	float t_min = FLT_MAX;
	CollisionObject obj;

	auto nearest = nearest_object(ray, scene.spheres,  scene.triangles, scene.vertex_data);
	t_min = nearest.first;
	obj = nearest.second;
	

	if (obj.type == COLLISION_OBJECT_INVALID) {
		if(depth == scene.max_recursion_depth) {
			hit_info = false;
			color = scene.background_color;
		}
		return;
	}
	hit_info = true;

	vec4fc hit_point = cast_ray(ray, t_min);
	vec4f norm;
	Material material;
	switch (obj.type)
	{
		case COLLISION_OBJECT_SPHERE:
			material = scene.materials[obj.data.sphere->material_id-1];
			norm = sphere_get_normal(obj.data.sphere, hit_point);
			break;
		case COLLISION_OBJECT_TRI:
			material = scene.materials[obj.data.tri->material_id-1];
			norm = triangle_get_normal(scene.vertex_data, obj.data.tri->indices);
			break;
		default:
			break;
	}

	vec4f diffuse_shade = vec4f{0,0,0,0};
	vec4f specular_shade = vec4f{0,0,0,0};

	for (auto& light : scene.point_lights)
	{
		Ray shadow_ray = Ray::from_to(hit_point + norm*scene.shadow_ray_epsilon, light.position);
		if (is_shadowed(shadow_ray, scene.spheres, scene.triangles, scene.vertex_data))
			continue;

		// Diffuse Shading
		vec4fc lh = light.position - hit_point;
		cfloat r2 = dot4f(lh, lh);
		vec4fc zor = light.intensity / r2;
		diffuse_shade += material.diffuse * std::max(0.0f, dot4f(norm,lh)) * zor;
	}
	
	auto reverse_incoming = normalize4f(mul4fs(ray.direction, -1.0f));
	auto incoming_to_normal_projection = sub4f(reverse_incoming, norm);
	Ray bounced_ray(hit_point, add4f(reverse_incoming, mul4fs(incoming_to_normal_projection, 2.0f)));

	color += scene.ambient_light + diffuse_shade + specular_shade;
	if(obj.type == COLLISION_OBJECT_SPHERE) {
		// std::cout << color << '\t' << shadow_ray.direction << '\n';
	}
	// trace(scene, bounced_ray, depth - 1, color);
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
	scene.loadFromXml("./inputs/cornellbox.xml");
	for (size_t i = 0; i < scene.cameras.size(); i++)
	{
		render_camera(scene, i);
	}
}