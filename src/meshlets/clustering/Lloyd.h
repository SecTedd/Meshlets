#pragma once

#include "../Meshlets.h"
#include "./GrowSites.h"
#include "../sites/RandomSites.h"

namespace meshlets {
/**
 * @brief perform a clustering using the lloyd algorithm (i.e. perform repeated clustering while moving the sites to the center of their meshlet between each iteration)
 * 
 * @param mesh the mesh to calculate the cluster on
 * @param init_sites the initial sites to use for the first clustering iteration
 * @param max_iterations the maximum number of iterations the algorithm will perform (default: 100). The algorithm stops if almost nothing changes anymore before the maximum number of iterations is reached.
 * @return ClusterAndSites the resulting cluster and sites
*/
ClusterAndSites lloyd(pmp::SurfaceMesh &mesh, std::vector<Site> &init_sites,
                      int max_iterations = 100);

/**
 * @brief perform a clustering using the lloyd algorithm (i.e. perform repeated clustering while moving the sites to the center of their meshlet between each iteration)
 * 
 * @param mesh the mesh to calculate the cluster on
 * @param init_sites the initial sites to use for the first clustering iteration
 * @param faces_to_consider the faces to consider for the clustering
 * @param max_iterations the maximum number of iterations the algorithm will perform (default: 100). The algorithm stops if almost nothing changes anymore before the maximum number of iterations is reached.
 * @return ClusterAndSites the resulting cluster and sites
*/
ClusterAndSites lloyd(pmp::SurfaceMesh &mesh, std::vector<Site> &init_sites,
                      std::vector<pmp::Face> &faces_to_consider,
                      int max_iterations = 100);
} // namespace meshlets