#pragma once

#include "../Meshlets.h"
#include "pmp/algorithms/normals.h"
#include "pmp/algorithms/differential_geometry.h"
#include "pmp/algorithms/utilities.h"
#include "../../helpers/Random.h"

namespace meshlets {
/**
 * @brief generate random sites on a mesh (without any checks on distance or anything)
 * 
 * @param mesh The mesh to generate the sites on
 * @param amount The amount of sites to generate
*/
std::vector<Site> generate_random_sites(pmp::SurfaceMesh &mesh, int amount);

/**
 * @brief generate random sites on a mesh (without any checks on distance or anything)
 * 
 * @param mesh The mesh to generate the sites on
 * @param amount The amount of sites to generate
 * @param faces_to_consider The faces to consider when generating sites
*/
std::vector<Site> generate_random_sites(
    pmp::SurfaceMesh &mesh, int amount,
    std::vector<pmp::Face> &faces_to_consider);
} // namespace meshlets