#ifndef __VEC_H__
#define __VEC_H__

#include <stdio.h>
#include <array>
#include <xmmintrin.h>
#include <smmintrin.h>
#include <math.h>
#include <iostream>

#ifndef pure
   #ifdef __GNUC__
      #define pure __attribute__((pure))
   #else
      #define pure
   #endif
#endif

#ifndef aligned
   #ifdef __GNUC__
      #define aligned(N) __attribute__((aligned(N)))
   #else
      #define aligned(N) __declspec(align(N))
   #endif
#endif

#include <array>

typedef __m128 vec4f;
typedef const vec4f vec4fc;

typedef const float cfloat;

typedef std::array<float,4> arr4f;
typedef const arr4f arr4fc;

static inline vec4f add4f(vec4fc lhs, vec4fc rhs) {
    return _mm_add_ps(lhs, rhs);
}

static inline vec4f sub4f(vec4fc lhs, vec4fc rhs) {
    return _mm_sub_ps(lhs, rhs);
}

static inline vec4f mul4f(vec4fc lhs, vec4fc rhs) {
    return _mm_mul_ps(lhs, rhs);
}

static inline vec4f div4f(vec4fc lhs, vec4fc rhs) {
    return _mm_div_ps(lhs, rhs);
}

static inline vec4f mul4fs(vec4fc vec, cfloat num) {
    return mul4f(vec, _mm_set1_ps(num));
}

static inline vec4f mul4f(vec4fc vec, cfloat num) {
    return mul4f(vec, _mm_set1_ps(num));
}

static inline vec4f div4fs(vec4fc vec, cfloat num) {
   return div4f(vec, _mm_set1_ps(num));
}

static inline vec4f div4f(vec4fc vec, cfloat num) {
   return div4f(vec, _mm_set1_ps(num));
}

static inline vec4f mul4sf(cfloat num, vec4fc vec) {
   return mul4fs(vec,num);
}

static inline vec4f mul4s(cfloat num, vec4fc vec) {
   return mul4fs(vec,num);
}

static inline vec4f div4sf(cfloat num, vec4fc vec) {
   return div4fs(vec,num);
}

static inline vec4f div4s(cfloat num, vec4fc vec) {
   return div4fs(vec,num);
}

static inline float dot4f(vec4fc lhs, vec4fc rhs) {
   auto multiplied = _mm_mul_ps(lhs, rhs);
   auto lo_dual = multiplied;
   auto hi_dual = _mm_movehl_ps(multiplied, multiplied);
   auto sum_dual = _mm_add_ps(lo_dual, hi_dual);
   auto lo = sum_dual;
   auto hi = _mm_shuffle_ps(sum_dual, sum_dual, 0x1);
   auto sum = _mm_add_ps(lo, hi);
   return _mm_cvtss_f32(sum);
}

static inline vec4f cross4f(vec4fc lhs, vec4fc rhs) {
   auto tmp0 = _mm_shuffle_ps(lhs, lhs, _MM_SHUFFLE(3,0,2,1));
   auto tmp2 = _mm_shuffle_ps(lhs, lhs, _MM_SHUFFLE(3,1,0,2));
   auto tmp1 = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(3,1,0,2));
   auto tmp3 = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(3,0,2,1));
   return vec4f{ _mm_sub_ps(_mm_mul_ps(tmp0, tmp1), _mm_mul_ps(tmp2, tmp3)) };
}

static inline float len4f(vec4fc self) {
   return sqrtf(dot4f(self,self));
}

static inline vec4f normalize4f(vec4fc self) {
   return div4fs(self,len4f(self));
}

template<int i>
static inline float extract(vec4fc vec) {
   return _mm_cvtss_f32(_mm_shuffle_ps(vec, vec, i));
}

static inline vec4f vmake(arr4fc& arr) {
   return _mm_load_ps(arr.data());
}

static inline arr4f amake(vec4fc v) {
   arr4f a;
   _mm_store_ps(a.data(), v);
   return a;
}

#if !(defined(__GNUC__) || defined(__clang__))

static inline vec4f operator+(vec4fc lhs, vec4fc rhs) {
   return _mm_add_ps(lhs, rhs);
}

static inline vec4f operator-(vec4fc lhs, vec4fc rhs) {
   return _mm_sub_ps(lhs, rhs);
}

static inline vec4f operator*(vec4fc lhs, vec4fc rhs) {
   return _mm_mul_ps(lhs, rhs);
}

static inline vec4f operator*(vec4fc lhs, cfloat rhs) {
   return _mm_mul_ps(lhs, _mm_set1_ps(rhs));
}

static inline vec4f operator*(cfloat lhs, vec4fc rhs) {
   return _mm_mul_ps(_mm_set1_ps(lhs), rhs);
}

static inline vec4f operator/(vec4fc lhs, vec4fc rhs) {
   return _mm_div_ps(lhs, rhs);
}

static inline vec4f operator/(vec4f lhs, cfloat rhs) {
   return _mm_div_ps(lhs, _mm_set1_ps(rhs));
}

static inline vec4f operator/(cfloat lhs, vec4fc rhs) {
   return _mm_div_ps(_mm_set1_ps(lhs), rhs);
}

#endif

static inline std::ostream& operator<<(std::ostream& os, vec4fc v) {
   arr4fc arr = amake(v);
   std::printf("vec4f{%g, %g, %g, %g}", arr[0], arr[1], arr[2], arr[3]);
   return os;
}

static inline vec4f pow4f(vec4fc lhs, vec4fc rhs) {
   arr4fc lhsa = amake(lhs);
   arr4fc rhsa = amake(rhs);
   return vec4f{powf(lhsa[0], rhsa[0]), powf(lhsa[1], rhsa[1]), powf(lhsa[2], rhsa[2]), powf(lhsa[3], rhsa[3])};
}

static inline vec4f min4f(vec4fc lhs, vec4fc rhs) {
   arr4fc lhsa = amake(lhs);
   arr4fc rhsa = amake(rhs);
   return vec4f{fmin(lhsa[0], rhsa[0]), fmin(lhsa[1], rhsa[1]), fmin(lhsa[2], rhsa[2]), fmin(lhsa[3], rhsa[3])};
}

static inline vec4f max4f(vec4fc lhs, vec4fc rhs) {
   arr4fc lhsa = amake(lhs);
   arr4fc rhsa = amake(rhs);
   return vec4f{fmax(lhsa[0], rhsa[0]), fmax(lhsa[1], rhsa[1]), fmax(lhsa[2], rhsa[2]), fmax(lhsa[3], rhsa[3])};
}

#endif
