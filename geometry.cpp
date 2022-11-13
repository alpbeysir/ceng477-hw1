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

pure bool aabb_get_collision(const Ray& ray, vec4fc bminv, vec4fc bmaxv)
{
	auto bmin = amake(bminv), bmax = amake(bmaxv);
	auto rayo = amake(ray.start), rayd = amake(ray.direction);

    float tx1 = (bmin[0] - rayo[0]) / rayd[0], tx2 = (bmax[0] - rayo[0]) / rayd[0];
    float tmin = std::min(tx1, tx2), tmax = std::max(tx1, tx2);
    float ty1 = (bmin[1] - rayo[1]) / rayd[1], ty2 = (bmax[1] - rayo[1]) / rayd[1];
    tmin = std::max(tmin, std::min( ty1, ty2 ) ), tmax = std::min( tmax, std::max( ty1, ty2 ) );
    float tz1 = (bmin[2] - rayo[2]) / rayd[2], tz2 = (bmax[2] - rayo[2]) / rayd[2];
    tmin = std::max( tmin, std::min( tz1, tz2 ) ), tmax =  std::min( tmax, std::max( tz1, tz2 ) );

	// assume 1 for ray.t
    return tmax >= tmin && tmin < /*ray.t*/1 && tmax > 0;
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

pure float bvh_get_collision(const Scene& scene, const BVHNode* const nodes, const Ray& ray, const int idx) {
    const BVHNode& node = nodes[idx];
    if (!aabb_get_collision(ray, node.bbmin, node.bbmax))
		return nan("aabb");
	
    if (node.tri_count > 0)
    {
		float tmin = INFINITY;
        for (int i = 0; i < node.tri_count; i++) {
			const Face& indices = scene.triangles[i].indices;
            float cur_result = triangle_get_collision(scene.vertex_data, indices, ray);
			if (cur_result < tmin) tmin = cur_result;
		}
		return tmin;
    }
    else
    {
        return std::min(bvh_get_collision(scene, nodes, ray, node.left), bvh_get_collision(scene, nodes, ray, node.left + 1));
    }
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
		// if(len4f(ray.direction - vec4f{0.45, 0.4, -1, 0}) < 0.0000006f)
		// 	std::cout << "ZORT:\t" << '\t' << vertices[tri.indices.v0_id] << '\t' << vertices[tri.indices.v1_id] << '\t' << vertices[tri.indices.v2_id] << "\n";
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

bool do_geometry(const Scene& scene, const Ray& ray, bool& hit_info, int depth, vec4fc reflectance, float& t_min, CollisionObject& obj, Material& material, vec4f& hit_point, vec4f& norm) {
	auto nearest = nearest_object(ray, scene.spheres,  scene.triangles, scene.vertex_data);
	t_min = nearest.first;
	obj = nearest.second;
	

	if (obj.type == COLLISION_OBJECT_INVALID) {
		if(depth == scene.max_recursion_depth) {
			hit_info = false;
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

void set_node_bounds(const Scene& scene, BVHNode& node) {
	node.bbmin = vec4f{INFINITY, INFINITY, INFINITY, INFINITY};
	node.bbmax = vec4f{-INFINITY, -INFINITY, -INFINITY, -INFINITY};

	for (int first = node.tri_start, i = 0; i < node.tri_count; i++) {
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
	vec4f extent = node.bbmax - node.bbmin;
	int axis = 0;
	arr4f extenta = amake(extent);
	if (extenta[1] > extenta[0]) axis = 1;
	if (extenta[2] > extent[axis]) axis = 2;
	return {axis, node.bbmin[axis] + extent[axis] * 0.5f};
}

void subdivide_node(Scene& scene, BVHNode* nodes, vec4f* centroids, BVHNode& node, int& node_count) {
	if (node.tri_count <= 2) return;

	// Reorder triangles
	auto split_pos = get_split_pos(node);	
	int i = node.tri_start;
	int j = i + node.tri_count - 1;
	while (i <= j)
	{
		arr4fc centroid = amake(centroids[i]);
		if (centroid[split_pos.first] < split_pos.second)
			i++;
		else
			std::swap(scene.triangles[i], scene.triangles[j--]);
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
	node.tri_count = 0;

	set_node_bounds(scene, nodes[left_child]);
	set_node_bounds(scene, nodes[right_child]);

	subdivide_node(scene, nodes, centroids, nodes[left_child], node_count);
	subdivide_node(scene, nodes, centroids, nodes[right_child], node_count);
}

BVHNode* buildBVH(Scene& scene) {
	int tri_count = scene.triangles.size();
    vec4f centroids[tri_count];
	BVHNode* nodes = new BVHNode[std::max(1, tri_count*2 - 1)];

	int node_count = 1;
	BVHNode& root = nodes[0];
	root.left = 0;
	root.tri_start = 0, root.tri_count = tri_count;

	// Calculate triangle centers
    for (int i = 0; i < tri_count; i++) {
		const Face& indices =  scene.triangles[i].indices;
        centroids[i] = scene.vertex_data[indices.v0_id] + scene.vertex_data[indices.v1_id] + scene.vertex_data[indices.v2_id];
		centroids[i] /= 3.0f;
    }

	set_node_bounds(scene, root);
	subdivide_node(scene, nodes, centroids, root, node_count);

	std::cout << "BVH success: node count = " << node_count << std::endl;
	return nodes;
}