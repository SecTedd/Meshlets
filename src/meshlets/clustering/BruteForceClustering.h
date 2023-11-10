#pragma once

#include "../Meshlets.h"

namespace meshlets
{
    /**
     * @brief assign faces to their closest site and store this info in the site and as a face property
    */
    void brute_force_sites(pmp::SurfaceMesh &mesh, std::vector<Site> &sites);
}