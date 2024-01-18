#include <map>
#include "LOD.h"
#include "../sites/RandomSites.h"
#include "../clustering/Lloyd.h"
#include "../../helpers/Random.h"

namespace meshlets {
// helper function to get all tree nodes of a certain level
std::vector<TreeNode> get_nodes(TreeNode &root, int level)
{
    std::vector<TreeNode> nodes;
    if (root.level == level)
    {
        nodes.push_back(root);
    }
    else
    {
        for (auto child : *root.children)
        {
            std::vector<TreeNode> child_nodes = get_nodes(child, level);
            nodes.insert(nodes.end(), child_nodes.begin(), child_nodes.end());
        }
    }
    return nodes;
}

TreeNode build_lod_tree(pmp::SurfaceMesh &mesh, int num_levels,
                        int num_level1_sites)
{
    TreeNode root = TreeNode({std::make_shared<std::vector<TreeNode>>(),
                              std::make_shared<std::vector<pmp::Face>>(
                                  mesh.faces_begin(), mesh.faces_end()),
                              0});

    int lloyd_max_iter = 20;
    int min_faces_per_meshlet = 10;
    float num_new_sites = 3;
    std::vector<TreeNode> last_added_nodes = {root};

    for (int i = 1; i < num_levels; i++)
    {
        std::vector<TreeNode> new_added_nodes;
        for (auto &parent_node : last_added_nodes)
        {
            std::vector<pmp::Face> faces = *parent_node.faces;
            // nodes become to small, stop and return an empty tree
            if (faces.size() < min_faces_per_meshlet)
            {
                return TreeNode({std::make_shared<std::vector<TreeNode>>(),
                                 std::make_shared<std::vector<pmp::Face>>(),
                                 0});
            }

            std::vector<Site> sites;
            if (i == 1)
            {
                sites = generate_random_sites(mesh, num_level1_sites, faces);
            }
            else
            {
                sites = generate_random_sites(mesh, num_new_sites, faces);
            }

            ClusterAndSites level_i = lloyd(mesh, sites, faces, lloyd_max_iter);
            validate_and_fix_meshlets(mesh, level_i.cluster, faces);

            for (auto meshlet : level_i.cluster)
            {
                auto faces = get_faces(*meshlet);
                if (faces.size() == 0)
                    continue;

                TreeNode child_node = TreeNode(
                    {std::make_shared<std::vector<TreeNode>>(),
                     std::make_shared<std::vector<pmp::Face>>(faces), i});
                parent_node.children->push_back(child_node);
                new_added_nodes.push_back(child_node);
            }
        }
        last_added_nodes = new_added_nodes;
    }

    return root;
}

void color_level(pmp::SurfaceMesh &mesh, TreeNode &root, int level)
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

    auto nodes = get_nodes(root, level);
    for (auto node : nodes)
    {
        auto node_color = helpers::generate_random_color();

        for (auto face : *node.faces)
        {
            color[face] = node_color;
        }
    }
}

void color_lod(pmp::SurfaceMesh &mesh, TreeNode &root,
               pmp::vec3 &camera_position)
{
    // TODO: implement
}
} // namespace meshlets
