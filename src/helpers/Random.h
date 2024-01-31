#pragma once

#include "pmp/surface_mesh.h"

#include <random>

namespace helpers {

/**
 * @brief Pick a random face from the mesh
 *
 * @param mesh The mesh to pick a face from
*/
pmp::Face pick_random_face(pmp::SurfaceMesh &mesh);

/**
 * @brief Pick a random face from the mesh
 *
 * @param mesh The mesh to pick a face from
 * @param faces_to_consider Faces to consider when picking a random face
*/
pmp::Face pick_random_face(pmp::SurfaceMesh &mesh,
                           std::vector<pmp::Face> &faces_to_consider);

/**
 * @brief generate a random color
*/
pmp::Color generate_random_color();

/**
 * @brief generate a random id
*/
int generate_random_id();
} // namespace helpers