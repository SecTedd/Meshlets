#pragma once

#include "../Meshlets.h"

namespace meshlets {
/**
 * @brief assigns a color to each meshlet in the cluster
 * 
 * @param mesh the mesh the cluster is located on
 * @param cluster the cluster to color
*/
void color_meshlets(pmp::SurfaceMesh &mesh, Cluster &cluster);
} // namespace meshlets