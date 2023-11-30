#pragma once

#include "../Meshlets.h"
#include "./GrowSites.h"
#include "../sites/RandomSites.h"

namespace meshlets {
ClusterAndSites lloyd(pmp::SurfaceMesh &mesh, std::vector<Site> &init_sites,
                      int max_iterations = 100);
} // namespace meshlets