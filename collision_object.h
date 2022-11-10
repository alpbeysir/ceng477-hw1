#ifndef __COLLISION_OBJECT_H__
#define __COLLISION_OBJECT_H__

#include <memory>
#include <stdbool.h>
#include "ray_tracer.h"
#include "vec.h"

enum CollisionObjectType
{
    COLLISION_OBJECT_INVALID,
    COLLISION_OBJECT_SPHERE,
    COLLISION_OBJECT_TRI,
};

struct CollisionObject
{
    CollisionObjectType type;
    union CollisionObjectData
    {
        const Sphere* sphere;
        const Triangle* tri;

        CollisionObjectData()
        {

        }

        ~CollisionObjectData() {

        }

    } data;

    CollisionObject()
    {
        type = COLLISION_OBJECT_INVALID;
    }

    CollisionObject(const Sphere* sphere)
    {
        type = COLLISION_OBJECT_SPHERE;
        data.sphere = sphere;
    }

    CollisionObject(const Triangle* tri)
    {
        type = COLLISION_OBJECT_TRI;
        data.tri = tri;
    }
};

#endif
