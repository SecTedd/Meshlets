#pragma once

#include "../Meshlets.h"

namespace meshlets {
/**
 * @brief perform a clustering using the grow sites algorithm (i.e. grow the sites until they converge)
 * 
 * @param mesh the mesh to calculate the cluster on
 * @param sites the sites to use for the clustering
 * @param max_iterations the maximum number of iterations the algorithm will perform (default: 1000). The algorithm stops if the sites converge before the maximum number of iterations is reached.
 * @return Cluster the resulting cluster
*/
Cluster grow_sites(pmp::SurfaceMesh &mesh, std::vector<Site> &sites,
                   int max_iterations = 1000);

/**
 * @brief perform a clustering using the grow sites algorithm (i.e. grow the sites until they converge)
 * 
 * @param mesh the mesh to calculate the cluster on
 * @param sites the sites to use for the clustering
 * @param faces_to_consider The faces to consider when performing the clustering
 * @param max_iterations the maximum number of iterations the algorithm will perform (default: 1000). The algorithm stops if the sites converge before the maximum number of iterations is reached.
 * @return Cluster the resulting cluster
*/
Cluster grow_sites(pmp::SurfaceMesh &mesh, std::vector<Site> &sites,
                   std::vector<pmp::Face> &faces_to_consider,
                   int max_iterations = 1000);
} // namespace meshlets