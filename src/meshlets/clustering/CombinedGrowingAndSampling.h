#pragma once

#include "../Meshlets.h"

namespace meshlets {
std::vector<Site> combined_growing_and_sampling(pmp::SurfaceMesh &mesh,
                                                int max_iterations = 1000);
} // namespace meshlets
