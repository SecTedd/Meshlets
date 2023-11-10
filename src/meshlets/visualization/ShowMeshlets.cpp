#include "ShowMeshlets.h"
#include "../../helpers/Random.h"

namespace meshlets {
void color_meshlets(pmp::SurfaceMesh &mesh, std::vector<Site> &sites)
{
    // create color face property
    pmp::FaceProperty<pmp::Color> color;
    if (!mesh.has_face_property("f:color"))
    {
        color =
            mesh.add_face_property<pmp::Color>("f:color", pmp::Color(0, 0, 0));
    }
    else
    {
        color = mesh.get_face_property<pmp::Color>("f:color");
        for (auto face : mesh.faces())
            color[face] = pmp::Color(0, 0, 0);
    }

    pmp::FaceProperty<bool> is_site = mesh.get_face_property<bool>("f:is_site");
    pmp::FaceProperty<int> closest_site =
        mesh.get_face_property<int>("f:closest_site");
    assert(is_site);
    assert(closest_site);

    for (auto face : mesh.faces())
    {
        if (is_site[face])
        {
            color[face] = pmp::Color(0, 0, 0);
            continue;
        }
        if (closest_site[face] == -1)
        {
            color[face] = pmp::Color(0.7, 0.7, 0.7);
            continue;
        }
        auto site = sites[closest_site[face]];
        color[face] = site.color;
    }
}
} // namespace meshlets