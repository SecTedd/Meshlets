#pragma once

#include "pmp/surface_mesh.h"
#include "../helpers/Random.h"

#include <iostream>
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
 * @brief helper function to get all the faces of a meshlet in a flat vector
 * 
 * @param meshlet the meshlet to get the faces from
*/
std::vector<pmp::Face> get_faces(Meshlet &meshlet);

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
 * @param cluster the cluster to check and fix
*/
void validate_and_fix_meshlets(pmp::SurfaceMesh &mesh, Cluster &cluster);

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
 * @brief check if the cluster data structure is consistent to the mesh face properties
 * 
 * @param mesh the mesh on which the cluster is located
 * @param cluster the cluster data structure
*/
bool check_consistency(pmp::SurfaceMesh &mesh, Cluster &cluster);
} // namespace meshlets
