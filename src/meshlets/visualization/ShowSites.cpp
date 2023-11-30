#include "ShowSites.h"

namespace meshlets {
void show_faces_with_sites(pmp::SurfaceMesh &mesh)
{
    pmp::FaceProperty<pmp::Color> color;
    if (!mesh.has_face_property("f:color"))
    {
        color = mesh.add_face_property<pmp::Color>("f:color",
                                                   pmp::Color(0.7, 0.7, 0.7));
    }
    else
    {
        color = mesh.get_face_property<pmp::Color>("f:color");
    }
    pmp::FaceProperty<bool> is_site = mesh.get_face_property<bool>("f:is_site");
    assert(is_site);

    for (auto face : mesh.faces())
    {
        if (is_site[face])
        {
            color[face] = pmp::Color(0, 0, 0);
        } else 
        {
            color[face] = pmp::Color(0.7, 0.7, 0.7);
        }
    }
}
} // namespace meshlets