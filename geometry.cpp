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

   cfloat t = ((-b) - sqrtf(discriminant)) / (2.0f * a);

   return t < 0.0f ? nan("aabb") : t;
}

pure vec4f sphere_get_normal(const Sphere* self, vec4fc point) {
   return (point - self->position) / self->radius;
}

pure bool aabb_get_collision(const Ray& ray, vec4fc bminv, vec4fc bmaxv)
{
	vec4fc dirfrac = 1.0 / ray.direction;
	arr4fc r1 = amake((bminv - ray.start) * dirfrac);
	arr4fc r2 = amake((bmaxv - ray.start) * dirfrac);

	float tmin = std::max(std::max(std::min(r1[0], r2[0]), std::min(r1[1], r2[1])), std::min(r1[2], r2[2]));
	float tmax = std::min(std::min(std::max(r1[0], r2[0]), std::max(r1[1], r2[1])), std::max(r1[2], r2[2]));

	if (tmax < 0)
	{
		return false;
	}

	if (tmin > tmax)
	{
		return false;
	}

	return true;
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

pure std::pair<float,const Triangle*> bvh_get_collision(const Scene& scene, const BVHNode* const nodes, const Ray& ray, const int idx)
{
    const BVHNode& node = nodes[idx];
	const Triangle* ret = NULL;
    if (!aabb_get_collision(ray, node.bbmin, node.bbmax))
		return std::pair<float,Triangle*>( nan("aabb"), NULL );
	
    if (node.tri_count > 0)
    {
		float tmin = INFINITY;
        for (int i = node.tri_start; i < node.tri_start + node.tri_count; i++) {
			const Face& indices = scene.triangles[i].indices;
            float cur_result = triangle_get_collision(scene.vertex_data, indices, ray);
			if (cur_result < tmin)
			{
				tmin = cur_result;
				ret = &scene.triangles[i];
			}
		}
		return std::pair<float, const Triangle*>(tmin, ret);
    }
    else
    {
		auto left_res = bvh_get_collision(scene, nodes, ray, node.left);
		auto right_res = bvh_get_collision(scene, nodes, ray, node.left + 1);
		if (left_res.first < right_res.first) return left_res;
		else return right_res;
    }
}



std::pair<float, CollisionObject> nearest_object(const Ray& ray, const Scene& scene) {
	float t_min = INFINITY;
	CollisionObject obj;

	for (const auto& sphere : scene.spheres)
	{
		float t = sphere_get_collision(sphere, ray);
		
		if (t_min > t)
		{
			t_min = t;
			obj.type = COLLISION_OBJECT_SPHERE;
			obj.data.sphere = &sphere;
		}
	}

	const auto [t, tri] = bvh_get_collision(scene, scene.bvh, ray);

	if (t_min > t)
	{
		t_min = t;
		obj.type = COLLISION_OBJECT_TRI;
		obj.data.tri = tri;
	}

	//for (const auto& tri : scene.triangles)
	//{
	//	// std::cout << ray.direction << '\t' << ray.start << '\t' << sphere.position << ' ' << sphere.radius << '\n';
	//	float t = triangle_get_collision(scene.vertex_data, tri.indices, ray);

	//	if (t_min > t)
	//	{
	//		t_min = t;
	//		obj.type = COLLISION_OBJECT_TRI;
	//		obj.data.tri = &tri;
	//	}
	//}

	return std::pair<float, CollisionObject>(t_min, obj);
}

bool do_geometry(const Scene& scene, const Ray& ray, float& t_min, CollisionObject& obj, Material& material, vec4f& hit_point, vec4f& norm) {
	auto nearest = nearest_object(ray, scene);
	t_min = nearest.first;
	obj = nearest.second;
	if (obj.type == COLLISION_OBJECT_INVALID)
		return false;

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

void set_node_bounds(const Scene& scene, BVHNode& node) {
	node.bbmin = vec4f{INFINITY, INFINITY, INFINITY, INFINITY};
	node.bbmax = vec4f{-INFINITY, -INFINITY, -INFINITY, -INFINITY};

	for (int i = node.tri_start; i < node.tri_start + node.tri_count; i++) {
		const Face& indices = scene.triangles[i].indices;

		node.bbmin = min4f( node.bbmin, scene.vertex_data[indices.v0_id] );
        node.bbmin = min4f( node.bbmin, scene.vertex_data[indices.v1_id] );
        node.bbmin = min4f( node.bbmin, scene.vertex_data[indices.v2_id] );
        node.bbmax = max4f( node.bbmax, scene.vertex_data[indices.v0_id] );
        node.bbmax = max4f( node.bbmax, scene.vertex_data[indices.v1_id] );
        node.bbmax = max4f( node.bbmax, scene.vertex_data[indices.v2_id] );
	}
}

std::pair<int, float> get_split_pos(const BVHNode& node) {
	arr4fc extent = amake(node.bbmax - node.bbmin);
	int axis = 0;
	if (extent[1] > extent[0]) axis = 1;
	if (extent[2] > extent[axis]) axis = 2;
	return {axis, node.bbmin[axis] + extent[axis]*0.5f};
}

void subdivide_node(Scene& scene, BVHNode* nodes, BVHNode& node, int& node_count) {
	if (node.tri_count <= 2) return;

	// Reorder triangles
	const auto [axis, split_point] = get_split_pos(node);
	int i = node.tri_start;
	int j = i + node.tri_count - 1;
	while (i <= j)
	{
		if (scene.triangles[i].indices.centroid[axis] < split_point)
		{
			i++;
		}
		else
		{
			std::swap(scene.triangles[i], scene.triangles[j]);
			j--;
		}
	}

	int left_count = i - node.tri_start;
	if (left_count == 0 || left_count == node.tri_count)
		return;

	int left_child = node_count++;
	int right_child = node_count++;
	node.left = left_child;

	nodes[left_child].tri_start = node.tri_start;
	nodes[left_child].tri_count = left_count;

	nodes[right_child].tri_start = i;
	nodes[right_child].tri_count = node.tri_count - left_count;

	// this is now an interior node
	node.tri_start = -1;
	node.tri_count = 0;

	set_node_bounds(scene, nodes[left_child]);
	set_node_bounds(scene, nodes[right_child]);

	subdivide_node(scene, nodes, nodes[left_child], node_count);
	subdivide_node(scene, nodes, nodes[right_child], node_count);
}

BVHNode* buildBVH(Scene& scene) {
	int tri_count = scene.triangles.size();
	BVHNode* nodes = new BVHNode[std::max(1, tri_count*2 - 1)];

	int node_count = 1;
	BVHNode& root = nodes[0];
	root.left = 0;
	root.tri_start = 0, root.tri_count = tri_count;

	set_node_bounds(scene, root);
	subdivide_node(scene, nodes, root, node_count);

	std::cout << "BVH success: node count = " << node_count << std::endl;

	int debug_tri_count = 0;
	for (int i = 0; i < node_count; i++)
	{
		debug_tri_count += nodes[i].tri_count;
	}
	std::cout << "BVH tri count: " << debug_tri_count << " Mesh tri count: " << tri_count << std::endl;

	return nodes;
}