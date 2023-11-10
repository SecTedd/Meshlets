#include "Random.h"

namespace helpers {
pmp::Face pick_random_face(pmp::SurfaceMesh &mesh)
{
    std::mt19937 gen(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> dis(0, mesh.n_faces() - 1);
    int random_face_index = dis(gen);
    auto begin = mesh.faces_begin();
    std::advance(begin, random_face_index);
    return *begin;
}

pmp::Vertex pick_random_vertex(pmp::SurfaceMesh &mesh)
{
    std::mt19937 gen(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> dis(0, mesh.n_vertices() - 1);
    int random_vertex_index = dis(gen);
    auto begin = mesh.vertices_begin();
    std::advance(begin, random_vertex_index);
    return *begin;
}

pmp::Edge pick_random_edge(pmp::SurfaceMesh &mesh)
{
    std::mt19937 gen(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> dis(0, mesh.n_edges() - 1);
    int random_edge_index = dis(gen);
    auto begin = mesh.edges_begin();
    std::advance(begin, random_edge_index);
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
} // namespace helpers