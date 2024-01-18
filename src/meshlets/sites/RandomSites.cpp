#include "RandomSites.h"

namespace meshlets {
std::vector<Site> generate_random_sites(pmp::SurfaceMesh &mesh, int amount)
{
    std::vector<pmp::Face> faces_to_consider(mesh.faces_begin(),
                                             mesh.faces_end());
    return generate_random_sites(mesh, amount, faces_to_consider);
}

std::vector<Site> generate_random_sites(
    pmp::SurfaceMesh &mesh, int amount,
    std::vector<pmp::Face> &faces_to_consider)
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
        for (auto face : faces_to_consider)
        {
            is_site[face] = false;
        }
    }

    while (selected < amount)
    {
        auto face = helpers::pick_random_face(mesh, faces_to_consider);
        if (is_site[face])
        {
            continue;
        }
        is_site[face] = true;
        // calculate center of face
        pmp::vec3 centroid = pmp::centroid(mesh, face);
        // get normal of face
        pmp::vec3 normal = pmp::face_normal(mesh, face);
        sites[selected] = Site(selected, face, centroid, normal);
        selected++;
    }
    return sites;
}
} // namespace meshlets