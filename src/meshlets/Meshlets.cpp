#include <map>
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

pmp::Face get_site_face(Meshlet &meshlet)
{
    return meshlet.at(0)->at(0);
}

// helper function to get the 3 adjacent faces of a face
std::vector<pmp::Face> get_adjacent_faces(pmp::SurfaceMesh &mesh,
                                          pmp::Face &face)
{
    std::vector<pmp::Face> adjacent_faces;

    for (auto halfedge : mesh.halfedges(face))
    {
        auto opposite_face = mesh.face(halfedge);
        if (face == opposite_face)
        {
            auto opposite_halfedge = mesh.opposite_halfedge(halfedge);
            opposite_face = mesh.face(opposite_halfedge);
        }

        adjacent_faces.push_back(opposite_face);
    }

    return adjacent_faces;
}

// helper function to find out whether start is connected to end via a path
// only containing faaces of the same meshlet
bool is_connected_via_edges(pmp::SurfaceMesh &mesh, pmp::Face &start,
                            pmp::Face &end)
{
    if (start == end)
    {
        return true;
    }

    auto closest_site = mesh.get_face_property<int>("f:closest_site");
    assert(closest_site);

    int id_to_match = closest_site[start];

    std::map<pmp::Face, bool> visited_faces_map;
    std::vector<pmp::Face> faces_to_visit;
    faces_to_visit.push_back(start);

    bool end_found = false;
    while (!end_found && faces_to_visit.size() > 0)
    {
        std::vector<pmp::Face> faces_to_visit_next;
        std::vector<pmp::Face> visited_faces;
        for (auto &face : faces_to_visit)
        {
            for (auto &adjacent_face : get_adjacent_faces(mesh, face))
            {
                if (adjacent_face == end)
                {
                    end_found = true;
                    break;
                }
                // prune pahts that contain faces of other meshlets
                if (closest_site[adjacent_face] != id_to_match)
                {
                    continue;
                }
                if (visited_faces_map.find(adjacent_face) ==
                    visited_faces_map.end())
                {
                    visited_faces.push_back(adjacent_face);
                    faces_to_visit_next.push_back(adjacent_face);
                }
            }
        }
        for (auto &face : visited_faces)
        {
            visited_faces_map[face] = true;
        }
        faces_to_visit = faces_to_visit_next;
    }

    return end_found;
}

bool is_valid(pmp::SurfaceMesh &mesh, Meshlet &meshlet)
{
    auto is_site = mesh.get_face_property<bool>("f:is_site");
    auto closest_site = mesh.get_face_property<int>("f:closest_site");
    assert(is_site);
    assert(closest_site);

    bool rule_1_and_2 = true;

    auto faces = get_faces(meshlet);

    for (auto &face : faces)
    {
        if (is_site[face])
        {
            continue;
        }

        // rule 1 and 2
        auto site_face = get_site_face(meshlet);
        rule_1_and_2 = is_connected_via_edges(mesh, face, site_face);
        if (!rule_1_and_2)
        {
            break;
        }
    }

    // rule 3
    bool rule_3 = true;
    // skip 0 iteration, because it containes the legit meshlet's site
    for (int num_iteration = 1; num_iteration < meshlet.size(); num_iteration++)
    {
        auto iteration = meshlet.at(num_iteration);
        for (auto &face : *iteration)
        {
            if (is_site[face])
            {
                rule_3 = false;
                break;
            }
        }
    }

    // rule 4
    bool rule_4 = faces.size() > 0;

    return rule_1_and_2 && rule_3 && rule_4;
}

// TODO
void validate_and_fix_meshlets(Cluster &cluster) {}

// TODO
float evaluate_clustering(Cluster &cluster)
{
    return 100.0f;
}

bool check_consistency(pmp::SurfaceMesh &mesh, Cluster &cluster)
{
    auto is_site = mesh.get_face_property<bool>("f:is_site");
    auto closest_site = mesh.get_face_property<int>("f:closest_site");
    auto added_in_iteration =
        mesh.get_face_property<int>("f:added_in_iteration");
    assert(is_site);
    assert(closest_site);
    assert(added_in_iteration);

    bool consistent = true;

    for (int site_id = 0; site_id < cluster.size(); site_id++)
    {
        auto meshlet = cluster[site_id];
        for (int iteration_num = 0; iteration_num < meshlet->size();
             iteration_num++)
        {
            auto iteration = meshlet->at(iteration_num);
            for (auto &face : *iteration)
            {
                if (is_site[face])
                {
                    if (closest_site[face] != -1 ||
                        added_in_iteration[face] != -1)
                    {
                        consistent = false;
                    }
                }
                else
                {
                    if (closest_site[face] != site_id ||
                        added_in_iteration[face] != iteration_num)
                    {
                        consistent = false;
                    }
                }
            }
        }
    }

    return consistent;
}
} // namespace meshlets