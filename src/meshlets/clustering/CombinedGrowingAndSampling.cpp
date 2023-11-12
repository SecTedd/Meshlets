#include "CombinedGrowingAndSampling.h"
#include "../../helpers/Random.h"
#include "pmp/algorithms/normals.h"
#include "pmp/algorithms/differential_geometry.h"

namespace meshlets {
Site create_site(pmp::SurfaceMesh &mesh, int id, pmp::Face &face)
{
    pmp::vec3 centroid = pmp::centroid(mesh, face);
    pmp::vec3 normal = pmp::face_normal(mesh, face);

    return Site(id, face, centroid, normal);
}

std::unique_ptr<std::vector<pmp::Face>> grow_site(
    pmp::SurfaceMesh &mesh, std::vector<Site> &sites, Site &site,
    int iterations, pmp::FaceProperty<int> &closest_site,
    pmp::VertexProperty<int> &visited_by, pmp::FaceProperty<bool> &is_site)
{
    int current_iteration = 0;
    std::vector<std::unique_ptr<std::vector<pmp::Face>>>
        faces_added_per_iteration;

    while (current_iteration <= iterations)
    {
        auto faces_added = std::make_unique<std::vector<pmp::Face>>();
        if (current_iteration == 0)
        {
            faces_added->push_back(site.face);
            faces_added_per_iteration.push_back(std::move(faces_added));
        }
        else
        {
            auto &faces_added_in_previous_iteration =
                faces_added_per_iteration[current_iteration - 1];

            for (int i = 0; i < faces_added_in_previous_iteration->size(); i++)
            {
                pmp::Face face = faces_added_in_previous_iteration->at(i);

                for (auto v : mesh.vertices(face))
                {
                    if (visited_by[v] == -1)
                    {
                        visited_by[v] = site.id;
                        for (auto f : mesh.faces(v))
                        {
                            if (!is_site[f] && closest_site[f] != site.id)
                            {
                                if (closest_site[f] == -1)
                                {
                                    closest_site[f] = site.id;
                                    faces_added->push_back(f);
                                }
                                else if (pmp::distance(site.position,
                                                       pmp::centroid(mesh, f)) <
                                         pmp::distance(
                                             sites[closest_site[f]].position,
                                             pmp::centroid(mesh, f)))
                                {
                                    closest_site[f] = site.id;
                                    faces_added->push_back(f);
                                }
                            }
                        }
                    }
                }
            }
            faces_added_per_iteration.push_back(std::move(faces_added));
        }
        current_iteration++;
    }
    return std::move(faces_added_per_iteration.back());
}

std::vector<Site> combined_growing_and_sampling(pmp::SurfaceMesh &mesh,
                                                int max_iterations)
{
    // create a face property to store the closest site
    pmp::FaceProperty<int> closest_site;
    if (!mesh.has_face_property("f:closest_site"))
    {
        closest_site = mesh.add_face_property<int>("f:closest_site", -1);
    }
    else
    {
        closest_site = mesh.get_face_property<int>("f:closest_site");
        for (auto face : mesh.faces())
        {
            closest_site[face] = -1;
        }
    }
    // create a vertex property to store the visited state of that vertex
    pmp::VertexProperty<int> visited_by;
    if (!mesh.has_vertex_property("v:visited_by"))
    {
        visited_by = mesh.add_vertex_property<int>("v:visited_by", -1);
    }
    else
    {
        visited_by = mesh.get_vertex_property<int>("v:visited_by");
        for (auto vertex : mesh.vertices())
        {
            visited_by[vertex] = -1;
        }
    }
    // create a face property to store whether a face is a site
    pmp::FaceProperty<bool> is_site;
    if (!mesh.has_face_property("f:is_site"))
    {
        is_site = mesh.add_face_property<bool>("f:is_site", false);
    }
    else
    {
        is_site = mesh.get_face_property<bool>("f:is_site");
        for (auto face : mesh.faces())
        {
            is_site[face] = false;
        }
    }

    int grow_iterations = 5;
    int new_sites = 3;
    int num_sites = 0;

    std::vector<Site> sites; // maybe reserve some space in advance

    auto start_face = helpers::pick_random_face(mesh);
    sites.push_back(create_site(mesh, num_sites, start_face));
    is_site[start_face] = true;
    num_sites++;

    int current_iteration = 0;
    int changed = 1;

    while (changed > 0 && current_iteration <= max_iterations)
    {
        std::vector<Site> sites_to_add;
        // grow sites
        for (auto &site : sites)
        {
            auto faces_added = grow_site(mesh, sites, site, grow_iterations,
                                         closest_site, visited_by, is_site);
            // pick new sites from the faces added
            // make sure they are good distributed
            int picked = 0;
            std::vector<pmp::vec3> connections;
            connections.reserve(new_sites);
            for (int i = 0; i < faces_added->size(); i++)
            {
                if (picked > new_sites)
                {
                    break;
                }
                pmp::Face face = faces_added->at(i);
                if (!is_site[face])
                {
                    if (connections.size() == 0)
                    {
                        is_site[face] = true;
                        sites_to_add.push_back(
                            create_site(mesh, num_sites, face));
                        pmp::vec3 centroid = pmp::centroid(mesh, face);
                        connections.push_back(centroid - site.position);
                        num_sites++;
                        picked++;
                    }
                    else
                    {
                        pmp::vec3 centroid = pmp::centroid(mesh, face);
                        bool valid = true;
                        for (auto &connection : connections)
                        {
                            if (pmp::angle(connection,
                                           centroid - site.position) <
                                2 * M_PI / new_sites)
                            {
                                valid = false;
                                break;
                            }
                        }
                        if (valid)
                        {
                            is_site[face] = true;
                            sites_to_add.push_back(
                                create_site(mesh, num_sites, face));
                            connections.push_back(centroid - site.position);
                            num_sites++;
                            picked++;
                        }
                    }
                }
            }
        }
        sites.clear();
        sites.insert(sites.end(), sites_to_add.begin(), sites_to_add.end());
        changed = sites_to_add.size();
        current_iteration++;
    }

    return sites;
}
} // namespace meshlets