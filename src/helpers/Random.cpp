#include "Random.h"

namespace helpers {
pmp::Face pick_random_face(pmp::SurfaceMesh &mesh)
{
    std::vector<pmp::Face> faces_to_consider(mesh.faces_begin(),
                                             mesh.faces_end());
    return pick_random_face(mesh, faces_to_consider);
}

pmp::Face pick_random_face(pmp::SurfaceMesh &mesh,
                           std::vector<pmp::Face> &faces_to_consider)
{
    std::mt19937 gen(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> dis(0, faces_to_consider.size() - 1);
    int random_face_index = dis(gen);
    auto begin = faces_to_consider.begin();
    std::advance(begin, random_face_index);
    return *begin;
}

pmp::Color generate_random_color()
{
    std::mt19937 gen(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> dis(0, 255);
    pmp::Color color;
    while (true)
    {
        int r = dis(gen);
        int g = dis(gen);
        int b = dis(gen);
        color = pmp::Color(r, g, b);
        // calculate luminance of generated color
        // https://stackoverflow.com/questions/596216/formula-to-determine-brightness-of-rgb-color
        float luminance = (0.299 * r + 0.587 * g + 0.114 * b) / 255;
        if (luminance < 0.5)
        {
            break;
        }
    }
    return color;
}

int generate_random_id()
{
    std::mt19937 gen(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> dis(0, INT32_MAX);

    return dis(gen);
}
} // namespace helpers