#ifndef __VEC_H__
#define __VEC_H__

#include <stdio.h>
#include <xmmintrin.h>
#include <math.h>

#ifndef pure
   #ifdef __GNUC__
      #define pure __attribute__((pure))
   #else
      #define pure
   #endif
#endif

#ifndef constfn
   #ifdef __GNUC__
      #define constfn __attribute__((const))
   #else
      #define constfn
   #endif
#endif

#ifdef __GNUC__
   // #define __always_inline __attribute__((always_inline))
#else
   #define __always_inline inline
#endif

#include <array>

union vec4f
{
    __m128 v;
    struct
    {
        float x;
        float y;
        float z;
        float w;
    };
    struct
    {
        float r;
        float g;
        float b;
        float a;
    };
    std::array<float, 4> arr;
};

typedef const vec4f vec4fc;

typedef const float cfloat;

constfn static inline vec4f add4f(vec4fc lhs, vec4fc rhs) {
    return { _mm_add_ps(lhs.v, rhs.v) };
}

constfn static inline vec4f sub4f(vec4fc lhs, vec4fc rhs) {
    return { _mm_sub_ps(lhs.v, rhs.v) };
}

constfn static inline vec4f mul4f(vec4fc lhs, vec4fc rhs) {
    return { _mm_mul_ps(lhs.v, rhs.v) };
}

constfn static inline vec4f div4f(vec4fc lhs, vec4fc rhs) {
    return { _mm_div_ps(lhs.v, rhs.v) };
}

constfn static inline vec4f mul4fs(vec4fc vec, cfloat num) {
    return mul4f(vec, vec4f{ num, num, num, num });
}

constfn static inline vec4f div4fs(vec4fc vec, cfloat num) {
   return div4f(vec, {num, num, num, num});
}

constfn static inline vec4f mul4sf(cfloat num, vec4fc vec) {
   return mul4fs(vec,num);
}

constfn static inline vec4f div4sf(cfloat num, vec4fc vec) {
   return div4fs(vec,num);
}

constfn static inline float dot4f(vec4fc lhs, vec4fc rhs) {
   auto multiplied = _mm_mul_ps(lhs.v, rhs.v);
   auto lo_dual = multiplied;
   auto hi_dual = _mm_movehl_ps(multiplied, multiplied);
   auto sum_dual = _mm_add_ps(lo_dual, hi_dual);
   auto lo = sum_dual;
   auto hi = _mm_shuffle_ps(sum_dual, sum_dual, 0x1);
   auto sum = _mm_add_ps(lo, hi);
   return _mm_cvtss_f32(sum);
}

constfn static inline vec4f cross4f(vec4fc lhs, vec4fc rhs) {
   auto tmp0 = _mm_shuffle_ps(lhs.v, lhs.v, _MM_SHUFFLE(3,0,2,1));
   auto tmp2 = _mm_shuffle_ps(lhs.v, lhs.v, _MM_SHUFFLE(3,1,0,2));
   auto tmp1 = _mm_shuffle_ps(rhs.v, rhs.v, _MM_SHUFFLE(3,1,0,2));
   auto tmp3 = _mm_shuffle_ps(rhs.v, rhs.v, _MM_SHUFFLE(3,0,2,1));
   return { _mm_sub_ps(_mm_mul_ps(tmp0, tmp1), _mm_mul_ps(tmp2, tmp3)) };
}

constfn static inline float len4f(vec4fc self) {
   return sqrtf(dot4f(self,self));
}

constfn static inline vec4f normalize4f(vec4fc self) {
   return div4fs(self,len4f(self));
}

#endif
