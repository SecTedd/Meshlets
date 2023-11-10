#pragma once

#include "../Meshlets.h"

namespace meshlets {
/**
 * @brief create texture containing sites
 * 
 * @param mesh The mesh to color the faces of
 * @param sites The sites to color
 */
void show_faces_with_sites(pmp::SurfaceMesh &mesh, std::vector<Site> &sites);
} // namespace meshlets