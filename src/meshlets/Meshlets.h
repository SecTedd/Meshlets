#pragma once

#include "pmp/surface_mesh.h"
#include "../helpers/Random.h"

#include <iostream>
#include <vector>

namespace meshlets {

typedef std::vector<std::shared_ptr<std::vector<pmp::Face>>> Meshlet;
typedef std::vector<std::shared_ptr<Meshlet>> Cluster;

typedef struct Site
{
    int id;
    pmp::Face face;
    pmp::vec3 position;
    pmp::vec3 normal;

    Site()
    {
        id = -1;
        face = pmp::Face();
        position = pmp::vec3();
        normal = pmp::vec3();
    }

    Site(int id, pmp::Face face, pmp::vec3 position, pmp::vec3 normal)
        : id(id), face(face), position(position), normal(normal){};
} Site;

typedef struct ClusterAndSites
{
    Cluster cluster;
    std::vector<Site> sites;
} ClusterAndSites;

std::vector<pmp::Face> get_faces(Meshlet &meshlet);

/**
 * @brief check if the meshlet is valid according to the following rules:
 * 1. Meshlet is not split into multiple connected components
 * 2. Meshlet has no triangles that are only connected by a single vertex
 * 3. Meshlet does not contain any other sites
 * 4. Meshlet has > 0 faces
 * 
 * @param mesh the mesh
 * @param meshlet the meshlet
*/
bool is_valid(pmp::SurfaceMesh &mesh, Meshlet &meshlet);

void validate_and_fix_meshlets(Cluster &cluster);

float evaluate_clustering(Cluster &cluster);

pmp::Face get_site_face(Meshlet &meshlet);

/**
 * @brief check if the cluster data structure is consistent to the mesh face properties
 * 
 * @param mesh the mesh
 * @param cluster the cluster data structure
*/
bool check_consistency(pmp::SurfaceMesh &mesh, Cluster &cluster);
} // namespace meshlets
