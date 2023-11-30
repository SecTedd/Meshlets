#include "Lloyd.h"

#include "pmp/algorithms/normals.h"

namespace meshlets {
float median(std::vector<float> &values)
{
    std::sort(values.begin(), values.end());
    return values[values.size() / 2];
}

pmp::Face find_closest_triangle(pmp::SurfaceMesh &mesh, Meshlet &meshlet,
                                pmp::Point seed_point)
{
    auto faces = get_faces(meshlet);
    float min_distance = std::numeric_limits<float>::max();
    pmp::Face closest_face;
    for (auto &face : faces)
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

std::vector<Site> generate_new_sites(pmp::SurfaceMesh &mesh,
                                     std::vector<Site> &old_sites,
                                     Cluster &cluster)
{
    std::vector<Site> new_sites(old_sites.size());
    pmp::FaceProperty<bool> is_site = mesh.get_face_property<bool>("f:is_site");
    assert(is_site);

    // count how many meshlets changed their center triangle (relevant for stopping criterion)
    int center_triangle_changed_count = 0;

    for (auto &site : old_sites)
    {
        auto meshlet = cluster[site.id].get();
        is_site[site.face] = false;

        // create point cloud containing meshlet face centroids
        std::vector<float> points_x;
        std::vector<float> points_y;
        std::vector<float> points_z;

        for (auto face : get_faces(*meshlet))
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

        if (new_site_face != site.face)
        {
            center_triangle_changed_count++;
        }
    }

    if (center_triangle_changed_count < old_sites.size() * 0.01)
    {
        return std::vector<Site>();
    }
    return new_sites;
}

ClusterAndSites lloyd(pmp::SurfaceMesh &mesh, std::vector<Site> &init_sites,
                      int max_iterations)
{
    ClusterAndSites cluster_and_sites;
    cluster_and_sites.sites = init_sites;
    int current_iteration = 0;

    while (current_iteration < max_iterations)
    {
        // grow sites
        cluster_and_sites.cluster = grow_sites(mesh, cluster_and_sites.sites);
        // update sites and check for stopping criterion
        auto new_sites = generate_new_sites(mesh, cluster_and_sites.sites,
                                            cluster_and_sites.cluster);
        if (new_sites.empty())
        {
            break;
        }
        else
        {
            cluster_and_sites.sites = new_sites;
        }
        current_iteration++;
    }
    std::cout << "Lloyd made " << current_iteration << " iterations ";
    return cluster_and_sites;
}
} // namespace meshlets