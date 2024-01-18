#include <future>
#include <mutex>
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

int get_meshlet_id(pmp::SurfaceMesh &mesh, pmp::Face &site_face)
{
    auto closest_site = mesh.get_face_property<int>("f:closest_site");
    assert(closest_site);

    int meshlet_id = -1;
    for (auto &face : get_adjacent_faces(mesh, site_face))
    {
        if (meshlet_id == -1)
        {
            meshlet_id = closest_site[face];
        }
        else if (meshlet_id != closest_site[face])
        {
            return -1;
        }
    }
    return meshlet_id;
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

    std::unordered_map<pmp::IndexType, bool> visited_faces_map;
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
                // prune paths that contain faces of other meshlets
                if (closest_site[adjacent_face] != id_to_match)
                {
                    continue;
                }
                if (visited_faces_map.find(adjacent_face.idx()) ==
                    visited_faces_map.end())
                {
                    visited_faces.push_back(adjacent_face);
                    faces_to_visit_next.push_back(adjacent_face);
                }
            }
        }
        for (auto &face : visited_faces)
        {
            visited_faces_map[face.idx()] = true;
        }
        faces_to_visit = faces_to_visit_next;
    }

    return end_found;
}

// helper function that returns all faces that are connected to the site face via edges
std::unordered_map<pmp::IndexType, bool> get_connected_faces(
    pmp::SurfaceMesh &mesh, pmp::Face &site_face)
{
    auto closest_site = mesh.get_face_property<int>("f:closest_site");
    assert(closest_site);

    std::unordered_map<pmp::IndexType, bool> connected_faces_map;
    std::vector<pmp::Face> faces_to_visit;
    faces_to_visit.push_back(site_face);

    int site_face_id = get_meshlet_id(mesh, site_face);

    while (faces_to_visit.size() > 0)
    {
        std::vector<pmp::Face> faces_to_visit_next;

        for (auto &face : faces_to_visit)
        {
            for (auto &adjacent_face : get_adjacent_faces(mesh, face))
            {
                // prune paths that contain faces of other meshlets
                if (closest_site[adjacent_face] != site_face_id)
                {
                    continue;
                }
                if (connected_faces_map.find(adjacent_face.idx()) ==
                    connected_faces_map.end())
                {
                    connected_faces_map[adjacent_face.idx()] = true;
                    faces_to_visit_next.push_back(adjacent_face);
                }
            }
        }
        faces_to_visit = faces_to_visit_next;
    }

    return connected_faces_map;
}

bool is_valid(pmp::SurfaceMesh &mesh, Meshlet &meshlet)
{
    auto is_site = mesh.get_face_property<bool>("f:is_site");
    auto closest_site = mesh.get_face_property<int>("f:closest_site");
    assert(is_site);
    assert(closest_site);

    auto faces = get_faces(meshlet);
    auto site_face = get_site_face(meshlet);
    auto connected_faces = get_connected_faces(mesh, site_face);

    // if meshlet only consists of the site face, it is valid
    if (faces.size() == 1 && faces[0] == site_face && is_site[site_face])
    {
        return true;
    }

    // rule 1 and 2
    bool rule_1_and_2 =
        connected_faces.size() ==
        faces.size() - 1; // -1 because site face is not connected to itself

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

void validate_and_fix_meshlets(pmp::SurfaceMesh &mesh, Cluster &cluster)
{
    std::vector<pmp::Face> faces_to_consider(mesh.faces_begin(),
                                             mesh.faces_end());
    validate_and_fix_meshlets(mesh, cluster, faces_to_consider);
}

void validate_and_fix_meshlets(pmp::SurfaceMesh &mesh, Cluster &cluster,
                               std::vector<pmp::Face> &faces_to_consider)
{
    auto is_site = mesh.get_face_property<bool>("f:is_site");
    auto closest_site = mesh.get_face_property<int>("f:closest_site");
    auto added_in_iteration =
        mesh.get_face_property<int>("f:added_in_iteration");
    assert(is_site);
    assert(closest_site);
    assert(added_in_iteration);

    // convert faces_to_consider to a hashmap
    std::unordered_map<pmp::IndexType, bool> faces_to_consider_map;
    for (auto face : faces_to_consider)
    {
        faces_to_consider_map[face.idx()] = true;
    }
    int max_num_dryruns = 5;
    int current_num_dryruns = 0;
    int unchanged_faces = 1;

    while (unchanged_faces > 0 && current_num_dryruns < max_num_dryruns)
    {
        unchanged_faces = 0;

        for (auto &meshlet : cluster)
        {
            auto faces = get_faces(*meshlet);
            auto site_face = get_site_face(*meshlet);
            auto connected_faces = get_connected_faces(mesh, site_face);

            if (connected_faces.size() != faces.size() - 1)
            {
                // meshlet is invalid
                for (auto &face : faces)
                {
                    if (is_site[face])
                    {
                        continue;
                    }

                    if (connected_faces.find(face.idx()) ==
                        connected_faces.end())
                    {
                        // Majority vote
                        std::unordered_map<int, int> closest_site_count;
                        for (auto &adjacent_face :
                             get_adjacent_faces(mesh, face))
                        {
                            if (is_site[adjacent_face] ||
                                closest_site[adjacent_face] ==
                                    closest_site[face] ||
                                faces_to_consider_map.find(
                                    adjacent_face.idx()) ==
                                    faces_to_consider_map.end())
                            {
                                continue;
                            }
                            if (closest_site_count.find(
                                    closest_site[adjacent_face]) ==
                                closest_site_count.end())
                            {
                                closest_site_count
                                    [closest_site[adjacent_face]] = 1;
                            }
                            else
                            {
                                closest_site_count
                                    [closest_site[adjacent_face]]++;
                            }
                        }
                        // if face is not surrounded by faces of the same meshlet
                        if (closest_site_count.size() > 0)
                        {
                            int max_count = 0;
                            int max_count_site_id = -1;
                            for (auto &pair : closest_site_count)
                            {
                                if (pair.second > max_count)
                                {
                                    max_count = pair.second;
                                    max_count_site_id = pair.first;
                                }
                            }
                            auto new_meshlet = cluster[max_count_site_id];
                            auto old_meshlet = cluster[closest_site[face]];
                            // remove face from old site
                            old_meshlet->at(added_in_iteration[face])
                                ->erase(
                                    std::remove(
                                        meshlet->at(added_in_iteration[face])
                                            ->begin(),
                                        meshlet->at(added_in_iteration[face])
                                            ->end(),
                                        face),
                                    meshlet->at(added_in_iteration[face])
                                        ->end());
                            // add face to new site (for now it keeps the iteration number)

                            new_meshlet->at(added_in_iteration[face])
                                ->push_back(face);
                            closest_site[face] = max_count_site_id;
                            current_num_dryruns = 0;
                        }
                        else
                        {
                            unchanged_faces++;
                            current_num_dryruns++;
                        }
                    }
                }
            }
        }
    }
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

    std::unordered_map<pmp::IndexType, bool> seen_faces;

    for (int site_id = 0; site_id < cluster.size(); site_id++)
    {
        auto meshlet = cluster[site_id];
        for (int iteration_num = 0; iteration_num < meshlet->size();
             iteration_num++)
        {
            auto iteration = meshlet->at(iteration_num);
            for (auto &face : *iteration)
            {
                // check if faces occur multiple times, which means they are part of multiple meshlets
                if (seen_faces.find(face.idx()) == seen_faces.end())
                {
                    seen_faces[face.idx()] = true;
                }
                else
                {
                    return false;
                }
                if (is_site[face])
                {
                    if (closest_site[face] != -1 ||
                        added_in_iteration[face] != -1)
                    {
                        return false;
                    }
                }
                else
                {
                    if (closest_site[face] != site_id ||
                        added_in_iteration[face] != iteration_num)
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}
} // namespace meshlets