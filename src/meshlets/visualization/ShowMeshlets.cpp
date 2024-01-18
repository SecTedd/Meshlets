#include <map>
#include "ShowMeshlets.h"
#include "../../helpers/Random.h"

namespace meshlets {
void color_meshlets(pmp::SurfaceMesh &mesh, Cluster &cluster)
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

    std::map<int, pmp::Color> colors;

    for (auto &meshlet : cluster)
    {
        for (auto face : get_faces(*meshlet))
        {
            int site_id = closest_site[face];

            if (is_site[face])
            {
                color[face] = pmp::Color(0, 0, 0);
                continue;
            }
            if (site_id == -1)
            {
                std::cerr << "WARNING: Face " << face.idx()
                          << " has no closest site" << std::endl;
                continue;
            }
            if (colors.find(site_id) == colors.end())
            {
                colors[site_id] = helpers::generate_random_color();
            }
            color[face] = colors[site_id];
        }
    }
}
} // namespace meshlets