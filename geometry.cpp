#include "geometry.h"
#include "ray_tracer.h"

pure float sphere_get_collision(const Sphere& self, const Ray& ray) {
   vec4fc C = self.position;
   cfloat r = self.radius;
   vec4fc o = ray.start;
   vec4fc u = ray.direction;

   cfloat a = dot4f(u,u);
   vec4fc oc = sub4f(o,C);
   cfloat b = 2.0f * dot4f(u, oc);
   cfloat c = dot4f(oc,oc) - (r*r);

   cfloat discriminant = (b*b) - (4.0f*a*c);
   // if(amake(ray.direction) == amake(vec4f{-0.4375, 0.5, -1, 0})) {
   //    std::cout << ray.start << self.position << ray.direction << '\n';
   // }

   cfloat sqrtDiscriminant = sqrtf(discriminant);

   return ((-b) - sqrtDiscriminant) / (2.0f * a);
}

pure vec4f sphere_get_normal(const Sphere* self, vec4fc point) {
   return (point - self->position) / self->radius;
}

pure float triangle_get_collision(const std::vector<vec4f>& vertices, const Face& self, const Ray& ray)
{
	cfloat epsilon = 0.0000001;
   vec4fc vertex0 = vertices[self.v0_id-1];
   vec4fc vertex1 = vertices[self.v1_id-1];
   vec4fc vertex2 = vertices[self.v2_id-1];
   vec4fc edge1 = self.edge0;
   vec4fc edge2 = self.edge1;
   vec4fc h = cross4f(ray.direction, edge2);
   cfloat a = dot4f(edge1, h);
   if (a > -epsilon && a < epsilon)
      return nan("aabb");
   cfloat f = 1.0/a;
   vec4fc s = ray.start - vertex0;
   cfloat u = f * dot4f(s,h);
   if (u < 0.0 || u > 1.0)
      return nan("aabb");
   vec4fc q = cross4f(s, edge1);
   cfloat v = f * dot4f(ray.direction, q);
   if (v < 0.0 || u + v > 1.0)
      return nan("aabb");
   float t = f * dot4f(edge2, q);
   if (t > epsilon)
      return t;
   else
      return nan("aabb");
}

pure vec4f triangle_get_normal(const std::vector<vec4f>& vertices, const Face& self)
{
	vec4fc edge1 = self.edge0;
   vec4fc edge2 = self.edge1;
   return normalize4f(cross4f(edge1, edge2));
}

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

bool do_geometry(const Scene& scene, const Ray& ray, bool& hit_info, int depth, float& t_min, CollisionObject& obj, Material& material, vec4f& hit_point, vec4f& norm, vec4f& color) {
	auto nearest = nearest_object(ray, scene.spheres,  scene.triangles, scene.vertex_data);
	t_min = nearest.first;
	obj = nearest.second;
	

	if (obj.type == COLLISION_OBJECT_INVALID) {
		if(depth == scene.max_recursion_depth) {
			hit_info = false;
			color = scene.background_color;
		}
		return false;
	}
	hit_info = true;

	hit_point = cast_ray(ray, t_min);
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
	return true;
}
