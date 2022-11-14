#ifndef __RAY_TRACER_H__
#define __RAY_TRACER_H__

#include "vec.h"
#include <stdint.h>
#include <stddef.h>
#include "parser.h"

#ifndef constfn
   #ifdef __GNUC__
      #define constfn __attribute__((const))
   #else
      #define constfn
   #endif
#endif

union Point2
{
    struct
    {
        size_t x, y;
    };
    struct
    {
        size_t width, height;
    };
};

union Color
{
    struct
    {
        uint8_t r, g, b, a;
    };
    uint8_t as_array[4];
    uint32_t as_integer;
};

struct Ray
{
    vec4f start;
    vec4f direction;

    Ray(vec4fc s, vec4fc d) {
        start = s;
        direction = d;
    }

    static inline Ray from_to(vec4fc f, vec4fc e)
    {
        return Ray(f, sub4f(e, f));
    }
};

typedef vec4f (*pixel_position_function)(const Camera self, const size_t x, const size_t y);

extern pixel_position_function pixel_positon;

constfn static vec4f cast_ray(const Ray self, cfloat t) {
    return add4f(self.start, mul4sf(t, normalize4f(self.direction)));
}

constfn vec4f plane_pixel_position(const Camera self, const size_t x, const size_t y);

#endif
