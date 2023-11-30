#pragma once

#include "pmp/surface_mesh.h"
#include "../helpers/Random.h"

#include <iostream>
#include <vector>

namespace meshlets {

typedef std::vector<std::unique_ptr<std::vector<pmp::Face>>> Meshlet;
typedef std::vector<std::unique_ptr<Meshlet>> Cluster;

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

bool is_valid(pmp::SurfaceMesh &mesh, Meshlet &meshlet);

void validate_and_fix_meshlets(Cluster &cluster);

float evaluate_clustering(Cluster &cluster);
} // namespace meshlets
