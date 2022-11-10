#include "parser.h"
#include "tinyxml2.h"
#include <sstream>
#include <stdexcept>

#define TO_TEMP temp[0] >> temp[1] >> temp[2]

void Scene::loadFromXml(const std::string &filepath)
{
    tinyxml2::XMLDocument file;
    std::stringstream stream;
    arr4f temp;

    auto res = file.LoadFile(filepath.c_str());
    if (res)
    {
        throw std::runtime_error("Error: The xml file cannot be loaded.");
    }

    auto root = file.FirstChild();
    if (!root)
    {
        throw std::runtime_error("Error: Root is not found.");
    }

    //Get BackgroundColor
    auto element = root->FirstChildElement("BackgroundColor");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0 0 0" << std::endl;
    }
    //
    stream >> TO_TEMP;
    background_color = vmake(temp);
    // stream >> background_color.x >> background_color.y >> background_color.z;

    //Get ShadowRayEpsilon
    element = root->FirstChildElement("ShadowRayEpsilon");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0.001" << std::endl;
    }
    stream >> shadow_ray_epsilon;

    //Get MaxRecursionDepth
    element = root->FirstChildElement("MaxRecursionDepth");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0" << std::endl;
    }
    stream >> max_recursion_depth;

    //Get Cameras
    element = root->FirstChildElement("Cameras");
    element = element->FirstChildElement("Camera");
    Camera Camera;
    while (element)
    {
        auto child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Gaze");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Up");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearPlane");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearDistance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageResolution");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageName");
        stream << child->GetText() << std::endl;

        //
        stream >> TO_TEMP;
        Camera.position = vmake(temp);
        stream >> TO_TEMP;
        Camera.gaze = vmake(temp);
        stream >> TO_TEMP;
        Camera.up = vmake(temp);
        stream >> TO_TEMP;
        stream >> temp[3];
        Camera.near_plane = vmake(temp);
        // stream >> Camera.position.x >> Camera.position.y >> Camera.position.z;
        // stream >> Camera.gaze.x >> Camera.gaze.y >> Camera.gaze.z;
        // stream >> Camera.up.x >> Camera.up.y >> Camera.up.z;
        // stream >> Camera.near_plane.x >> Camera.near_plane.y >> Camera.near_plane.z >> Camera.near_plane.w;
        stream >> Camera.near_distance;
        stream >> Camera.image_width >> Camera.image_height;
        stream >> Camera.image_name;

        cameras.push_back(Camera);
        element = element->NextSiblingElement("Camera");
    }

    //Get Lights
    element = root->FirstChildElement("Lights");
    auto child = element->FirstChildElement("AmbientLight");
    stream << child->GetText() << std::endl;
    //
    stream >> TO_TEMP;
    ambient_light = vmake(temp);
    // stream >> ambient_light.x >> ambient_light.y >> ambient_light.z;
    element = element->FirstChildElement("PointLight");
    PointLight point_light;
    while (element)
    {
        child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Intensity");
        stream << child->GetText() << std::endl;

        //
        stream >> TO_TEMP;
        point_light.position = vmake(temp);
        stream >> TO_TEMP;
        point_light.intensity = vmake(temp);
        // stream >> point_light.position.x >> point_light.position.y >> point_light.position.z;
        // stream >> point_light.intensity.x >> point_light.intensity.y >> point_light.intensity.z;

        point_lights.push_back(point_light);
        element = element->NextSiblingElement("PointLight");
    }

    //Get Materials
    element = root->FirstChildElement("Materials");
    element = element->FirstChildElement("Material");
    Material material;
    while (element)
    {
        material.is_mirror = (element->Attribute("type", "mirror") != NULL);

        child = element->FirstChildElement("AmbientReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("DiffuseReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("SpecularReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("MirrorReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("PhongExponent");
        stream << child->GetText() << std::endl;

        //
        stream >> TO_TEMP;
        material.ambient = vmake(temp);
        stream >> TO_TEMP;
        material.diffuse = vmake(temp);
        stream >> TO_TEMP;
        material.specular = vmake(temp);
        stream >> TO_TEMP;
        material.mirror = vmake(temp);
        // stream >> material.ambient.x >> material.ambient.y >> material.ambient.z;
        // stream >> material.diffuse.x >> material.diffuse.y >> material.diffuse.z;
        // stream >> material.specular.x >> material.specular.y >> material.specular.z;
        // stream >> material.mirror.x >> material.mirror.y >> material.mirror.z;
        stream >> material.phong_exponent;

        materials.push_back(material);
        element = element->NextSiblingElement("Material");
    }

    //Get VertexData
    element = root->FirstChildElement("VertexData");
    stream << element->GetText() << std::endl;
    vec4f vertex;
    while (!(stream >> temp[0]).eof())
    {
        stream >> temp[1] >> temp[2];
        vertex = vmake(temp);
        vertex_data.push_back(vertex);
    }
    stream.clear();

    //Get Meshes
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Mesh");
    //Mesh tri;
    int material_id_temp;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> material_id_temp;

        child = element->FirstChildElement("Faces");
        stream << child->GetText() << std::endl;
        Face face;
        while (!(stream >> face.v0_id).eof())
        {
            stream >> face.v1_id >> face.v2_id;
            triangles.push_back(Triangle {material_id_temp, face});
            //tri.faces.push_back(face);
        }
        stream.clear();

        element = element->NextSiblingElement("Mesh");
    }
    stream.clear();

    //Get Triangles
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Triangle");
    Triangle triangle;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> triangle.material_id;

        child = element->FirstChildElement("Indices");
        stream << child->GetText() << std::endl;
        stream >> triangle.indices.v0_id >> triangle.indices.v1_id >> triangle.indices.v2_id;

        triangles.push_back(triangle);
        element = element->NextSiblingElement("Triangle");
    }

    //Get Spheres
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Sphere");
    Sphere sphere;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> sphere.material_id;

        child = element->FirstChildElement("Center");
        stream << child->GetText() << std::endl;

        int zort;
        stream >> zort;
        sphere.position = vertex_data[zort - 1];

        child = element->FirstChildElement("Radius");
        stream << child->GetText() << std::endl;
        stream >> sphere.radius;

        spheres.push_back(sphere);
        element = element->NextSiblingElement("Sphere");
    }
}
