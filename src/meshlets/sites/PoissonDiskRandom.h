#pragma once

#include "../Meshlets.h"
#include "pmp/algorithms/normals.h"
#include "pmp/algorithms/differential_geometry.h"
#include "pmp/algorithms/utilities.h"
#include "../../helpers/Random.h"

namespace meshlets {
/**
 * @brief calculate whether a site is valid (based on the distance to other sites)
 * 
 * @param mesh The mesh to check the site on
 * @param sites The sites to check the site against
 * @param position The position of the site to check
*/
bool is_valid_site(pmp::SurfaceMesh &mesh, std::vector<Site> &sites,
                   pmp::vec3 &position);

/**
 * @brief generate sites using poisson disk sampling
 * 
 * @param mesh The mesh to generate the sites on
 * @param amount The amount of sites to generate
*/
std::vector<Site> generate_pds_sites(pmp::SurfaceMesh &mesh, int amount);
} // namespace meshlets