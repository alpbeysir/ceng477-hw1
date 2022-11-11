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
   if(amake(ray.direction) == amake(vec4f{-0.4375, 0.5, -1, 0})) {
      std::cout << ray.start << self.position << ray.direction << '\n';
   }

   return ((-b) - sqrtf(discriminant)) / (2.0f * a);
}

pure vec4f sphere_get_normal(const Sphere* self, vec4fc point) {
   return div4f(sub4f(point, self->position), self->radius);
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
