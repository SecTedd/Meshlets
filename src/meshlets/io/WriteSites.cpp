#include "WriteSites.h"

namespace meshlets {
void write_sites(pmp::SurfaceMesh &mesh, std::string filename)
{
    std::cout << "Writing mesh to " << filename << std::endl;
    auto flags = new pmp::IOFlags();
    flags->use_binary = false;
    flags->use_face_colors = true;
    pmp::write_off(mesh, filename, *flags);
}
} // namespace meshlets