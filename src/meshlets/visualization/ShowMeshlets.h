#pragma once

#include "../Meshlets.h"

namespace meshlets
{
    /**
     * @brief assign the color of the site to all faces assigned to it
    */
    void color_meshlets(pmp::SurfaceMesh &mesh, std::vector<Site> &sites);
}