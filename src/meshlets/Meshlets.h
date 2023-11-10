#pragma once

#include "pmp/surface_mesh.h"
#include "../helpers/Random.h"

#include <iostream>
#include <vector>

namespace meshlets {

typedef struct Site
{
    int id;
    pmp::Face face;
    pmp::vec3 position;
    pmp::vec3 normal;
    pmp::Color color = helpers::generate_random_color();

    Site()
    {
        id = -1;
        face = pmp::Face();
        position = pmp::vec3();
        normal = pmp::vec3();
        color = pmp::Color(1.0, 1.0, 1.0);
    }

    Site(int id, pmp::Face face, pmp::vec3 position, pmp::vec3 normal)
        : id(id), face(face), position(position), normal(normal)
    {
        color = helpers::generate_random_color();
    }
} Site;
} // namespace meshlets
