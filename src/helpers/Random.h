#pragma once

#include "pmp/surface_mesh.h"

#include <random>

namespace helpers {

/**
   * @brief Pick a random face from the mesh
   *
   * @param mesh The mesh to pick a face from
  */
pmp::Face pick_random_face(pmp::SurfaceMesh &mesh);

/**
   * @brief Pick a random vertex from the mesh
   *
   * @param mesh The mesh to pick a vertex from
  */
pmp::Vertex pick_random_vertex(pmp::SurfaceMesh &mesh);

/**
   * @brief Pick a random edge from the mesh
   *
   * @param mesh The mesh to pick a edge from
  */
pmp::Edge pick_random_edge(pmp::SurfaceMesh &mesh);

/**
 * @brief generate a random color
 * 
 * @param seed The seed to use for the random number generator
*/
pmp::Color generate_random_color();
} // namespace helpers