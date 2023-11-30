#include "pmp/algorithms/differential_geometry.h"
#include "BruteForceClustering.h"

namespace meshlets {
Cluster brute_force_sites(pmp::SurfaceMesh &mesh, std::vector<Site> &sites)
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
    // create a face property to store the iteration in which the face was added
    pmp::FaceProperty<int> added_in_iteration;
    if (!mesh.has_face_property("f:added_in_iteration"))
    {
        added_in_iteration =
            mesh.add_face_property<int>("f:added_in_iteration", -1);
    }
    else
    {
        added_in_iteration =
            mesh.get_face_property<int>("f:added_in_iteration");
        for (auto face : mesh.faces())
        {
            added_in_iteration[face] = -1;
        }
    }

    // create a vector to store a data structure for all sites:
    // datastructure is a  vector of vectors to store the faces added in each iteration
    Cluster cluster(sites.size());
    for (auto &site : sites)
    {
        auto meshlet = std::make_unique<Meshlet>(1);
        // create single vector for the only iteration there will be
        meshlet->at(0) = std::make_unique<std::vector<pmp::Face>>();
        cluster[site.id] = std::move(meshlet);
    }

    for (auto face : mesh.faces())
    {
        auto centroid = pmp::centroid(mesh, face);
        float min_distance = std::numeric_limits<float>::max();
        for (auto &site : sites)
        {
            if (site.face == face)
                continue;

            float distance = pmp::distance(centroid, site.position);
            if (distance < min_distance)
            {
                min_distance = distance;
                closest_site[face] = site.id;
                added_in_iteration[face] = 0;
            }
        }
        // add face to the meshlet of the closest site
        cluster[closest_site[face]]->at(0)->push_back(face);
    }

    return cluster;
}
} // namespace meshlets