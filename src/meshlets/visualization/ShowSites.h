#pragma once

#include "../Meshlets.h"

namespace meshlets {
/**
 * @brief create texture containing sites
 * 
 * @param mesh The mesh to color the faces of
 */
void show_faces_with_sites(pmp::SurfaceMesh &mesh);
} // namespace meshlets