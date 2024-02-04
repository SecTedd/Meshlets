#pragma once

#include "../Meshlets.h"

namespace meshlets {
/**
 * @brief Returns all nodes of a certain level of the tree.
 * 
 * @param root The root of the tree
 * @param level The level to get the nodes from
*/
std::vector<TreeNode> get_nodes(TreeNode &root, int level);
/**
 * @brief Generates a tree of meshlets (needed for LOD).
 * 
 * @param mesh The mesh to generate the tree on
 * @param num_levels Number of levels of the tree
 * @param num_level1_sites Number of sites in the first level of the tree
 * @return The root of the tree
*/
TreeNode build_lod_tree(pmp::SurfaceMesh &mesh, int num_levels,
                        int num_level1_sites);

/**
 * @brief Colors a certain level of the tree on the mesh.
 * 
 * @param mesh The mesh to color
 * @param root The root of the tree
 * @param level The level to color
*/
void color_level(pmp::SurfaceMesh &mesh, TreeNode &root, int level);

/**
 * @brief Visualizes the level of detail on the mesh.
 * 
 * @param mesh The mesh to visualize the lod on
 * @param root The root of the lod tree
 * @param camera_position The position of the camera
 * @param currently_visible_nodes The currently visible nodes
*/
void color_lod(pmp::SurfaceMesh &mesh, TreeNode &root,
               pmp::vec3 &camera_position,
               std::vector<meshlets::TreeNode> &currently_visible_nodes);
} // namespace meshlets
