#pragma once

#include "../Meshlets.h"

namespace meshlets {
/**
 * @brief assigns a black color to each face that has a site on it (and a constant greyish color to the other faces)
 * 
 * @param mesh The mesh to show the sites on
 */
void show_faces_with_sites(pmp::SurfaceMesh &mesh);
} // namespace meshlets