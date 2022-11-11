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
		if (t < 1.0f)
			return true;
	}

	for (auto& tri : tris)
	{
		float t = triangle_get_collision(vertices, tri.indices, ray);
		if (t < 1.0f)
			return true;
	}
	return false;
}

void trace(Scene& scene, Ray& ray, size_t depth, vec4f& color)
{
	if (depth < 0)
		return;

	vec4f temp_color = vec4f{0,0,0,0};

	float t_min = FLT_MAX;
	CollisionObject obj;

	auto nearest = nearest_object(ray, scene.spheres,  scene.triangles, scene.vertex_data);
	t_min = nearest.first;
	obj = nearest.second;
	

	if (obj.type == COLLISION_OBJECT_INVALID) {
		if(depth == scene.max_recursion_depth) {
			color = scene.background_color;
		}
		return;
	}

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

	bool not_in_shadow = false;
	for (auto& light : scene.point_lights)
	{
		Ray shadow_ray = Ray::from_to(hit_point, light.position);
		if (is_shadowed(shadow_ray, scene.spheres, scene.triangles, scene.vertex_data))
			continue;
		
		not_in_shadow = true;

		vec4fc lh = light.position - hit_point;
		cfloat r2 = dot4f(lh, lh);
		vec4fc zor = light.intensity / r2;
		temp_color += material.diffuse * std::max(0.0f, dot4f(norm,lh)) * zor;
	}
	if(not not_in_shadow) {
		temp_color = scene.ambient_light;
	}

	// temp_color = (norm+_mm_set1_ps(1.0f))*0.5f;
	
	auto reverse_incoming = normalize4f(mul4fs(ray.direction, -1.0f));
	auto incoming_to_normal_projection = sub4f(reverse_incoming, norm);
	Ray bounced_ray(hit_point, add4f(reverse_incoming, mul4fs(incoming_to_normal_projection, 2.0f)));

	color *= temp_color;
	// trace(scene, bounced_ray, depth - 1, color);
}

void render_camera(Scene& scene, int camIndex)
{
	auto& current_camera = scene.cameras[camIndex];

	auto image = new vec4f[current_camera.image_width * current_camera.image_height];
	//TODO: Gamma Correction
	
	for (int i = 0; i < current_camera.image_width; i++)
	{
		for (int j = 0; j < current_camera.image_height; j++)
		{
			Ray ray = Ray::from_to(current_camera.position, plane_pixel_position(current_camera, i, j));
			image[i * current_camera.image_height + j] = vec4f{1,1,1,1};
			trace(scene, ray, scene.max_recursion_depth, image[i * current_camera.image_height + j]);
		}
	}
	write_ppm(current_camera.image_name, image, current_camera.image_width, current_camera.image_height);
}

int main()
{
	Scene scene;
	scene.loadFromXml("./inputs/monkey.xml");
	for (size_t i = 0; i < scene.cameras.size(); i++)
	{
		render_camera(scene, i);
	}
}