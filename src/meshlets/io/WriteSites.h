#pragma once

#include "../Meshlets.h"
#include "pmp/io/write_off.h"

namespace meshlets {
/**
 * @brief Write the mesh to a file, storing face colors of faces which contain sites
 * 
 * @param mesh The mesh to write
 * @param filename The filename to write to
 */
void write_sites(pmp::SurfaceMesh &mesh, std::string filename);
} // namespace meshlets