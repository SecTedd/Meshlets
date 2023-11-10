#pragma once

#include "../Meshlets.h"

namespace meshlets {
/**
     * @brief grow sites to their closest faces and store this info in a face property
    */
void grow_sites(pmp::SurfaceMesh &mesh, std::vector<Site> &sites,
                int max_iterations = 1000);
} // namespace meshlets