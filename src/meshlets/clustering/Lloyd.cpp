#include "Lloyd.h"

#include "pmp/algorithms/normals.h"

namespace meshlets {
std::vector<std::unique_ptr<std::vector<pmp::Face>>> collect_meshlets(
    pmp::SurfaceMesh &mesh, std::vector<Site> &sites)
{
    std::vector<std::unique_ptr<std::vector<pmp::Face>>> meshlets(sites.size());

    for (auto &site : sites)
    {
        auto meshlet = std::make_unique<std::vector<pmp::Face>>();
        meshlet->push_back(site.face);
        meshlets[site.id] = std::move(meshlet);
    }

    pmp::FaceProperty<int> closest_site =
        mesh.get_face_property<int>("f:closest_site");

    for (auto face : mesh.faces())
    {
        if (closest_site[face] != -1)
        {
            meshlets[closest_site[face]]->push_back(face);
        }
    }

    return meshlets;
}

float median(std::vector<float> &values)
{
    std::sort(values.begin(), values.end());
    return values[values.size() / 2];
}

pmp::Face find_closest_triangle(pmp::SurfaceMesh &mesh,
                                std::vector<pmp::Face> &faces,
                                pmp::Point seed_point)
{
    float min_distance = std::numeric_limits<float>::max();
    pmp::Face closest_face;
    for (auto face : faces)
    {
        auto centroid = pmp::centroid(mesh, face);
        auto distance = pmp::distance(centroid, seed_point);
        if (distance < min_distance)
        {
            min_distance = distance;
            closest_face = face;
        }
    }
    return closest_face;
}

std::vector<Site> generate_new_sites(
    pmp::SurfaceMesh &mesh, std::vector<Site> &sites,
    std::vector<std::unique_ptr<std::vector<pmp::Face>>> &meshlets)
{
    std::vector<Site> new_sites(sites.size());
    pmp::FaceProperty<bool> is_site = mesh.get_face_property<bool>("f:is_site");

    // float to keep track of the mean move distance the sites made (relevant for the stopping criterion)
    float mean_move_distance = 0.0f;

    for (auto &site : sites)
    {
        auto meshlet = meshlets[site.id].get();
        is_site[site.face] = false;

        // create point cloud containing meshlet face centroids
        std::vector<float> points_x;
        std::vector<float> points_y;
        std::vector<float> points_z;
        points_x.reserve(meshlet->size());
        points_y.reserve(meshlet->size());
        points_z.reserve(meshlet->size());
        for (auto face : *meshlet)
        {
            auto centroid = pmp::centroid(mesh, face);
            points_x.push_back(centroid[0]);
            points_y.push_back(centroid[1]);
            points_z.push_back(centroid[2]);
        }

        // compute median of point cloud
        auto median_x = median(points_x);
        auto median_y = median(points_y);
        auto median_z = median(points_z);

        pmp::Point meshlet_centroid = pmp::Point(median_x, median_y, median_z);
        pmp::Face new_site_face =
            find_closest_triangle(mesh, *meshlet, meshlet_centroid);
        is_site[new_site_face] = true;

        auto normal = pmp::face_normal(mesh, new_site_face);
        auto position = pmp::centroid(mesh, new_site_face);

        new_sites[site.id] = Site(site.id, new_site_face, position, normal);

        // compute move distance
        mean_move_distance += pmp::distance(site.position, position);
    }
    mean_move_distance /= sites.size();

    if (mean_move_distance < 1.0e-5f)
    {
        return std::vector<Site>();
    }
    return new_sites;
}

std::vector<Site> lloyd(pmp::SurfaceMesh &mesh, std::vector<Site> &init_sites,
                        int max_iterations)
{
    std::vector<Site> sites = init_sites;
    int current_iteration = 0;

    while (current_iteration < max_iterations)
    {
        // grow sites
        grow_sites(mesh, sites);
        // collect sites
        std::vector<std::unique_ptr<std::vector<pmp::Face>>> meshlets =
            collect_meshlets(mesh, sites);
        // update sites and check for stopping criterion
        auto new_sites = generate_new_sites(mesh, sites, meshlets);
        if (new_sites.empty())
        {
            break;
        }
        else
        {
            sites = new_sites;
        }
        current_iteration++;
    }
    std::cout << "Lloyd made: " << current_iteration << " iterations."
              << std::endl;
    return sites;
}
} // namespace meshlets