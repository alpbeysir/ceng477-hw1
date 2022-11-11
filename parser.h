#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>
#include "vec.h"

struct Camera
{
    vec4f position;
    vec4f gaze;
    vec4f up;
    vec4f near_plane;
    float near_distance;
    int image_width, image_height;
    std::string image_name;
};

struct PointLight
{
    vec4f position;
    vec4f intensity;
};

struct Material
{
    bool is_mirror;
    vec4f ambient;
    vec4f diffuse;
    vec4f specular;
    vec4f mirror;
    float phong_exponent;
};

struct Face
{
    int v0_id;
    int v1_id;
    int v2_id;
    vec4f edge0;
    vec4f edge1;
};

struct Mesh
{
    int material_id;
    std::vector<Face> faces;
};

struct Triangle
{
    int material_id;
    Face indices;
};

struct Sphere
{
    vec4f position;
    float radius;
    int material_id;
};

struct Scene
{
    //Data
    vec4f background_color;
    float shadow_ray_epsilon;
    int max_recursion_depth;
    std::vector<Camera> cameras;
    vec4f ambient_light;
    std::vector<PointLight> point_lights;
    std::vector<Material> materials;
    std::vector<vec4f> vertex_data;
    std::vector<Triangle> triangles;

    std::vector<Sphere> spheres;

    //Functions
    void loadFromXml(const std::string &filepath);
};


#endif
