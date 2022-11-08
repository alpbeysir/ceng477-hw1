#ifndef __COLLISION_OBJECT_H__
#define __COLLISION_OBJECT_H__

#include <stdbool.h>
#include "ray_tracer.h"
#include "vec.h"

enum CollisionObjectType
{
    COLLISION_OBJECT_INVALID,
    COLLISION_OBJECT_SPHERE,
    COLLISION_OBJECT_TRI,
} type;

struct CollisionObject
{
    CollisionObjectType type;
    union CollisionObjectData
    {
        Sphere& sphere;
        Triangle& tri;

        CollisionObjectData()
        {

        }

    } data;

    CollisionObject()
    {
        type = COLLISION_OBJECT_INVALID;
    }

    CollisionObject(const Sphere& sphere)
    {
        type = COLLISION_OBJECT_SPHERE;
        data.sphere = sphere;
    }

    CollisionObject(const Triangle& tri)
    {
        type = COLLISION_OBJECT_TRI;
        data.tri = tri;
    }


    CollisionObject& operator=(const CollisionObject& rhs)
    {
        type = rhs.type;
        switch (type)
        {
        case COLLISION_OBJECT_TRI:
            data.tri = rhs.data.tri;
            break;
        case COLLISION_OBJECT_SPHERE:
            data.sphere = rhs.data.sphere;
            break;
        }
        return *this;
    }
};

#endif
