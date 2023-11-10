#include "pmp/algorithms/differential_geometry.h"
#include "BruteForceClustering.h"

namespace meshlets {
void brute_force_sites(pmp::SurfaceMesh &mesh, std::vector<Site> &sites)
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
            }
        }
    }
}
} // namespace meshlets