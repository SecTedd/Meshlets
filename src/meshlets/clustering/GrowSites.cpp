#include "./GrowSites.h"
#include "pmp/algorithms/differential_geometry.h"
#include "pmp/algorithms/normals.h"
#include "../../helpers/CubicBezier.h"

namespace meshlets {

void grow_sites(pmp::SurfaceMesh &mesh, std::vector<Site> &sites,
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
    // get face property indicating whether a face is a site (this is set in the site generation)
    pmp::FaceProperty<bool> is_site = mesh.get_face_property<bool>("f:is_site");
    int current_iteration = 0;
    int mean_faces_added_per_iteration = 100;

    // create a vector to store a data structure for all sites:
    // datastructure is a  vector of vectors to store the faces added in each iteration
    std::vector<
        std::unique_ptr<std::vector<std::unique_ptr<std::vector<pmp::Face>>>>>
        data_base(sites.size());
    for (auto &site : sites)
    {
        data_base[site.id] = std::make_unique<
            std::vector<std::unique_ptr<std::vector<pmp::Face>>>>();
    }

    // normal penalty for the angle between the normal of the site and the normal of the face
    float max_penalty = 2.0f;
    float min_penalty = 0.7f;
    // control points for the cubic bezier curve of the angle between the normal of the site and the normal of the face
    helpers::Point p0(0.0f, max_penalty);
    helpers::Point p1(1.0f, max_penalty);
    helpers::Point p2(0.75f, max_penalty);
    helpers::Point p3(1.0f, min_penalty);

    int changed = 1;
    while (changed > 0 && current_iteration <= max_iterations)
    {
        changed = 0;
        for (auto &site : sites)
        {
            // get the data structure for the current site
            auto &faces_added_per_iteration = data_base[site.id];
            // create a vector to store the faces added in the current iteration
            auto faces_added_in_current_iteration =
                std::make_unique<std::vector<pmp::Face>>();
            if (current_iteration == 0)
            {
                faces_added_in_current_iteration->push_back(site.face);
                changed++;
                faces_added_per_iteration->push_back(
                    std::move(faces_added_in_current_iteration));
            }
            else
            {
                // get the faces added in the previous iteration
                auto &faces_added_in_previous_iteration =
                    faces_added_per_iteration->at(current_iteration - 1);

                for (int i = 0; i < faces_added_in_previous_iteration->size();
                     i++)
                {
                    pmp::Face face = faces_added_in_previous_iteration->at(i);
                    // get the vertecies
                    for (auto v : mesh.vertices(face))
                    {
                        if (visited_by[v] != site.id)
                        {
                            visited_by[v] = site.id;
                            // get the faces
                            for (auto f : mesh.faces(v))
                            {
                                if (!is_site[f] && closest_site[f] != site.id)
                                {
                                    if (closest_site[f] == -1)
                                    {
                                        closest_site[f] = site.id;
                                        faces_added_in_current_iteration
                                            ->push_back(f);
                                        changed++;
                                    }
                                    // face belongs to another site
                                    else
                                    {
                                        auto other_site =
                                            sites[closest_site[f]];
                                        auto site_normal = site.normal;
                                        site_normal.normalize();
                                        auto other_site_normal =
                                            other_site.normal;
                                        other_site_normal.normalize();
                                        auto face_normal =
                                            pmp::face_normal(mesh, f);
                                        face_normal.normalize();

                                        float penalty_site =
                                            helpers::cubicBezier(
                                                (pmp::dot(site_normal,
                                                          face_normal) +
                                                 1) /
                                                    2,
                                                p0, p1, p2, p3)
                                                .y;

                                        float penalty_other_site =
                                            helpers::cubicBezier(
                                                (pmp::dot(other_site_normal,
                                                          face_normal) +
                                                 1) /
                                                    2,
                                                p0, p1, p2, p3)
                                                .y;

                                        if (penalty_site *
                                                pmp::distance(
                                                    site.position,
                                                    pmp::centroid(mesh, f)) <
                                            penalty_other_site *
                                                pmp::distance(
                                                    other_site.position,
                                                    pmp::centroid(mesh, f)))
                                        {
                                            closest_site[f] = site.id;
                                            faces_added_in_current_iteration
                                                ->push_back(f);
                                            changed++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                faces_added_per_iteration->push_back(
                    std::move(faces_added_in_current_iteration));
            }
        }
        current_iteration++;
    }
}
} // namespace meshlets