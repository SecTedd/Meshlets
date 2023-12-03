#pragma once

#include "../Meshlets.h"

namespace meshlets {
/**
 * @brief perform a clustering using brute force (i.e. compare each face to each site) 
 * 
 * @param mesh the mesh to calculate the cluster on
 * @param sites the sites to use for the clustering
 * @return Cluster the resulting cluster
*/
Cluster brute_force_sites(pmp::SurfaceMesh &mesh, std::vector<Site> &sites);
} // namespace meshlets