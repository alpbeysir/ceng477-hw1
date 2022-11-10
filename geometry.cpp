#include "geometry.h"
#include "ray_tracer.h"

pure float sphere_get_collision(const Sphere& self, const Ray& Ray) {
   vec4fc C = self.position;
   cfloat r = self.radius;
   vec4fc o = Ray.start;
   vec4fc u = Ray.direction;

   cfloat a = dot4f(u,u);
   vec4fc oc = sub4f(o,C);
   cfloat b = 2.0f * dot4f(u, oc);
   cfloat c = dot4f(oc,oc) - (r*r);

   cfloat discriminant = (b*b) - (4.0f*a*c);

   return ((-b) - sqrtf(discriminant)) / (2.0f * a);
}

pure vec4f sphere_get_normal(const Sphere* self, vec4fc point) {
   return div4f(sub4f(point, self->position), self->radius);
}

pure float triangle_get_collision(const std::vector<vec4f>& vertices, const Face& self, const Ray& ray)
{
	vec4fc vertex0 = vertices[self.v0_id];
   vec4fc vertex1 = vertices[self.v1_id];
   vec4fc vertex2 = vertices[self.v2_id];
   vec4fc edge1 = sub4f(vertex1, vertex0);
   vec4fc edge2 = sub4f(vertex2, vertex0);
   vec4fc pvec = cross4f(ray.direction, edge2);
   float det = dot4f(edge1, pvec);
   if(det < 0.005f) {
      return nan("aabb");
   }

   float inv_det = 1.0f/det;

   vec4fc tvec = sub4f(ray.start, vertex0);
   cfloat u = dot4f(tvec, pvec) * inv_det;
   if(u < 0.0f or u > 1.0f) {
      return nan("aabb");
   }

   vec4fc qvec = cross4f(tvec, edge1);
   cfloat v = dot4f(ray.start, qvec) * inv_det;
   if(v < 0.0f or v > 1.0f) {
      return nan("aabb");
   }

   return dot4f(edge2, qvec) * inv_det;
}

pure vec4f triangle_get_normal(const std::vector<vec4f>& vertices, const Face& self)
{
	vec4fc edge1 = sub4f(vertices[self.v0_id], vertices[self.v1_id]);
   vec4fc edge2 = sub4f(vertices[self.v0_id], vertices[self.v2_id]);
   return cross4f(edge1, edge2);
}
