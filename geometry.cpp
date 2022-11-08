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

pure vec4f sphere_get_normal(const Sphere& self, vec4fc point) {
   return normalize4f(sub4f(point, self.position));
}

pure float triangle_get_collision(const Face& self, const Ray& ray)
{
	return float();
}

pure vec4f triangle_get_normal(const Face& self)
{
	return vec4f();
}
