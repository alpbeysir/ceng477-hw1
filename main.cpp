#include <float.h>
#include "ppm.h"
#include "vec.h"
#include "parser.h"
#include "ray_tracer.h"
#include "collision_object.h"
#include "geometry.h"

std::pair<float, CollisionObject> nearest_object(const Ray& ray, const std::vector<Sphere>& spheres, const std::vector<Mesh>& meshes, const std::vector<Triangle>& tris, const std::vector<vec4f>& vertices) {
	float t_min = FLT_MAX;
	CollisionObject obj;

	for (auto& sphere : spheres)
	{
		float t = sphere_get_collision(sphere, ray);
		if (t_min > t)
		{
			t_min = t;
			obj.type = COLLISION_OBJECT_SPHERE;
			obj.data.sphere = sphere;
		}
	}

	for (auto& mesh : meshes)
	{
		for (auto& face : mesh.faces)
		{
			float t = triangle_get_collision(vertices, face, ray);
			if (t_min > t)
			{
				t_min = t;
				obj.type = COLLISION_OBJECT_TRI;
				obj.data.tri = { mesh.material_id, face };
			}
		}
	}

	for (auto& tri : tris)
	{
		float t = triangle_get_collision(vertices, tri.indices, ray);
		if (t_min > t)
		{
			t_min = t;
			obj.type = COLLISION_OBJECT_TRI;
			obj.data.tri = tri;
		}
	}

	return { t_min, obj };
}
bool is_shadowed(const Ray& ray, const std::vector<Sphere>& spheres, const std::vector<Mesh>& meshes, const std::vector<Triangle>& tris, const std::vector<vec4f>& vertices)
{
	for (auto& sphere : spheres)
	{
		float t = sphere_get_collision(sphere, ray);
		if (t < 1.0f)
			return true;
	}

	for (auto& mesh : meshes)
	{
		for (auto& face : mesh.faces)
		{
			float t = triangle_get_collision(vertices, face, ray);
			if (t < 1.0f)
				return true;
		}
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
	if (depth == 0)
	{
		return;
	}

	// vec4f color = scene.ambient_light;

	float t_min = FLT_MAX;
	CollisionObject obj;

	auto nearest = nearest_object(ray, scene.spheres, scene.meshes, scene.triangles, scene.vertex_data);
	t_min = nearest.first;
	obj = nearest.second;

	if (obj.type == COLLISION_OBJECT_INVALID)
	{
		return;
	}

	vec4fc hit_point = cast_ray(ray, t_min);
	vec4f norm;
	Material material;
	switch (obj.type)
	{
		case COLLISION_OBJECT_SPHERE:
			material = scene.materials[obj.data.sphere.material_id];
			norm = sphere_get_normal(obj.data.sphere, hit_point);
			break;
		case COLLISION_OBJECT_TRI:
			material = scene.materials[obj.data.tri.material_id];
			norm = triangle_get_normal(scene.vertex_data, obj.data.tri.indices);
			break;
	}

	for (auto& light : scene.point_lights)
	{
		Ray shadow_ray = Ray::from_to(hit_point, light.position);
		if (is_shadowed(shadow_ray, scene.spheres, scene.meshes, scene.triangles, scene.vertex_data))
			continue;
		
		// Diffuse
		color = add4f(color, mul4f(material.diffuse, div4fs(mul4sf((-dot4f(normalize4f(shadow_ray.direction), norm)), light.intensity), dot4f(shadow_ray.direction, shadow_ray.direction))));
	}

	auto reverse_incoming = normalize4f(mul4fs(ray.direction, -1.0f));
	auto incoming_to_normal_projection = sub4f(reverse_incoming, norm);
	Ray bounced_ray(hit_point, add4f(reverse_incoming, mul4fs(incoming_to_normal_projection, 2.0f)));
	trace(scene, bounced_ray, depth - 1, color);
}

void render_camera(Scene& scene, int camIndex)
{
	auto& current_camera = scene.cameras[camIndex];

	auto image = new vec4f[current_camera.image_width * current_camera.image_height];
	
	for (int i = 0; i < current_camera.image_width; i++)
	{
		for (int j = 0; j < current_camera.image_height; j++)
		{
			Ray ray(current_camera.position, plane_pixel_position(current_camera, i, j));
			trace(scene, ray, scene.max_recursion_depth, image[i * current_camera.image_height + j]);
		}
	}

}

int main()
{
	Scene scene;
	scene.loadFromXml("./inputs/simple.xml");

	for (size_t i = 0; i < scene.cameras.size(); i++)
	{
		render_camera(scene, i);
	}
}