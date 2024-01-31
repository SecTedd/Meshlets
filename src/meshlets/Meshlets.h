#pragma once

#include "pmp/surface_mesh.h"
#include "../helpers/Random.h"

#include <iostream>
#include <memory>
#include <vector>

namespace meshlets {

/**
 * @brief The Meshlet data structure is a vector of iterations, where each iteration is a vector of faces. The first (0) iteration only holds the site_face.
*/
typedef std::vector<std::shared_ptr<std::vector<pmp::Face>>> Meshlet;
/**
 * @brief The Cluster data structure is a vector of Meshlets.
*/
typedef std::vector<std::shared_ptr<Meshlet>> Cluster;

/**
 * @brief The Site data structure holds the information of a site.
*/
typedef struct Site
{
    // id of the site
    int id;
    // face where the site is located
    pmp::Face face;
    // actual position of the site (centroid of the face)
    pmp::vec3 position;
    // normal of the face
    pmp::vec3 normal;

    // default constructor
    Site()
    {
        id = -1;
        face = pmp::Face();
        position = pmp::vec3();
        normal = pmp::vec3();
    }
    // constructor
    Site(int id, pmp::Face face, pmp::vec3 position, pmp::vec3 normal)
        : id(id), face(face), position(position), normal(normal){};
} Site;

/**
 * @brief The ClusterAndSites data structure holds the entire information of a cluster, including the sites.
*/
typedef struct ClusterAndSites
{
    Cluster cluster;
    std::vector<Site> sites;
} ClusterAndSites;

/**
 * @brief The TreeNode data structure is used to build a tree of meshlets (needed for LOD).
*/
typedef struct TreeNode
{
    int id;
    pmp::Color color;
    std::shared_ptr<TreeNode> parent;
    std::shared_ptr<std::vector<TreeNode>> children;
    std::shared_ptr<std::vector<pmp::Face>> faces;
    int level;
} TreeNode;
bool operator==(const TreeNode &lhs, const TreeNode &rhs);

/**
 * @brief helper function to get all the faces of a meshlet in a flat vector
 * 
 * @param meshlet the meshlet to get the faces from
*/
std::vector<pmp::Face> get_faces(Meshlet &meshlet);

/**
 * @brief helper function to get all 3 adjacent faces of a face
 * 
 * @param mesh the mesh on which the face is located
 * @param face the face to get the adjacent faces from
*/
std::vector<pmp::Face> get_adjacent_faces(pmp::SurfaceMesh &mesh,
                                          pmp::Face &face);

/**
 * @brief check if the meshlet is valid according to the following rules:
 * 1. Meshlet is not split into multiple connected components
 * 2. Meshlet has no triangles that are only connected by a single vertex
 * 3. Meshlet does not contain any other sites
 * 4. Meshlet has > 0 faces
 * 
 * @param mesh the mesh on which the meshlet is located
 * @param meshlet the meshlet to check
*/
bool is_valid(pmp::SurfaceMesh &mesh, Meshlet &meshlet);

/**
 * @brief checks for each meshlet in the cluster if it's valid and performs a fix if not
 * 
 * @param mesh the mesh on which the cluster is located
 * @param cluster the cluster to check and fix
*/
void validate_and_fix_meshlets(pmp::SurfaceMesh &mesh, Cluster &cluster);

/**
 * @brief checks for each meshlet in the cluster if it's valid and performs a fix if not
 * 
 * @param mesh the mesh on which the cluster is located
 * @param cluster the cluster to check and fix
 * @param faces_to_consider the faces that were considered during clustering
*/
void validate_and_fix_meshlets(pmp::SurfaceMesh &mesh, Cluster &cluster,
                               std::vector<pmp::Face> &faces_to_consider);

/**
 * @brief evaluates a clustering based on certain criteria and returns a score (score is not bounded)
 * 
 * @param cluster the cluster to evaluate
*/
float evaluate_clustering(Cluster &cluster);

/**
 * @brief helper function to get the site_face of a meshlet
 * 
 * @param meshlet the meshlet to get the site_face from
*/
pmp::Face get_site_face(Meshlet &meshlet);

/**
 * @brief helper function to get the meshlet id of a meshlet given the site_face
 * 
 * @param mesh the mesh on which the meshlet is located
 * @param site_face the site_face of the meshlet
*/
int get_meshlet_id(pmp::SurfaceMesh &mesh, pmp::Face &site_face);

/**
 * @brief check if the cluster data structure is consistent to the mesh face properties
 * 
 * @param mesh the mesh on which the cluster is located
 * @param cluster the cluster data structure
*/
bool check_consistency(pmp::SurfaceMesh &mesh, Cluster &cluster);
} // namespace meshlets
