#include "PoissonDiskRandom.h"

namespace meshlets {
bool is_valid_site(pmp::SurfaceMesh &mesh, std::vector<Site> &sites,
                   pmp::vec3 &position)
{
    auto size = pmp::bounds(mesh).size() * 0.01;
    for (auto &site : sites)
    {
        if (site.position == position ||
            pmp::distance(site.position, position) < size)
        {
            return false;
        }
    }
    return true;
}

std::vector<Site> generate_pds_sites(pmp::SurfaceMesh &mesh, int amount)
{
    std::vector<Site> sites(amount);
    int selected = 0;

    // add face property to mesh indicating whether a face is a site
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

    while (selected < amount)
    {
        auto face = helpers::pick_random_face(mesh);
        // calculate center of face
        pmp::vec3 centroid = pmp::centroid(mesh, face);

        if (is_valid_site(mesh, sites, centroid))
        {
            is_site[face] = true;
            // get normal of face
            pmp::vec3 normal = pmp::face_normal(mesh, face);
            sites[selected] = Site(selected, face, centroid, normal);
            selected++;
        }
    }
    return sites;
}
} // namespace meshlets