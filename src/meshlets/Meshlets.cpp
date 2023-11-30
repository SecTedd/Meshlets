#include "Meshlets.h"
#include "./sites/PoissonDiskRandom.h"
#include "./clustering/BruteForceClustering.h"
#include "./clustering/GrowSites.h"
#include "./visualization/ShowMeshlets.h"

#include <iostream>

namespace meshlets {
std::vector<pmp::Face> get_faces(Meshlet &meshlet)
{
    std::vector<pmp::Face> faces;
    for (auto &iteration : meshlet)
    {
        for (auto &face : *iteration)
        {
            faces.push_back(face);
        }
    }
    return faces;
}

bool is_valid(pmp::SurfaceMesh &mesh, Meshlet &meshlet)
{
    // TODO think of a way to check if a meshlet violates the rules:
    // 1. Meshlet is not split into multiple connected components
    //// 2. Meshlet has no triangles that are only connected by a single vertex
    //// 3. Meshlet does not contain any other sites
    //// 4. Meshlet has > 0 faces

    auto is_site = mesh.get_face_property<bool>("f:is_site");
    auto closest_site = mesh.get_face_property<int>("f:closest_site");
    assert(is_site);
    assert(closest_site);

    bool rule_1 = true;
    bool rule_2 = true;
    bool rule_3 = true;

    auto faces = get_faces(meshlet);

    for (auto &face : faces)
    {
        // rule 1

        // rule 2
        bool is_connected_via_edge = false;
        for (auto halfedge : mesh.halfedges(face))
        {
            auto opposite_face = mesh.face(halfedge);
            if (face == opposite_face)
            {
                auto opposite_halfedge = mesh.opposite_halfedge(halfedge);
                opposite_face = mesh.face(opposite_halfedge);
            }

            if (closest_site[opposite_face] == closest_site[face])
            {
                is_connected_via_edge = true;
                break;
            }
        }
        rule_2 = is_connected_via_edge;

        // rule 3
    }

    bool rule_4 = faces.size() > 0;

    return rule_1 && rule_2 && rule_3 && rule_4;
}

// TODO
void validate_and_fix_meshlets(Cluster &cluster) {}

// TODO
float evaluate_clustering(Cluster &cluster)
{
    return 100.0f;
}
} // namespace meshlets