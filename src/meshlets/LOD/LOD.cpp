#include <map>
#include "LOD.h"
#include "../sites/RandomSites.h"
#include "../clustering/Lloyd.h"
#include "../../helpers/Random.h"

namespace meshlets {

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

int generate_node_id(std::unordered_map<int, bool> &generated_ids)
{
    int id = helpers::generate_random_id();
    while (generated_ids.find(id) != generated_ids.end())
    {
        id = helpers::generate_random_id();
    }
    generated_ids[id] = true;
    return id;
}

TreeNode build_lod_tree(pmp::SurfaceMesh &mesh, int num_levels,
                        int num_level1_sites)
{
    std::unordered_map<int, bool> generated_ids;
    // root has no parent
    TreeNode root = TreeNode({generate_node_id(generated_ids),
                              helpers::generate_random_color(),
                              std::make_shared<TreeNode>(),
                              std::make_shared<std::vector<TreeNode>>(),
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
                TreeNode root =
                    TreeNode({generate_node_id(generated_ids),
                              helpers::generate_random_color(),
                              std::make_shared<TreeNode>(),
                              std::make_shared<std::vector<TreeNode>>(),
                              std::make_shared<std::vector<pmp::Face>>(
                                  mesh.faces_begin(), mesh.faces_end()),
                              0});
                return root;
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
                    {generate_node_id(generated_ids),
                     helpers::generate_random_color(),
                     std::make_shared<TreeNode>(parent_node),
                     std::make_shared<std::vector<TreeNode>>(),
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
               pmp::vec3 &camera_position,
               std::vector<meshlets::TreeNode> &currently_visible_nodes)
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
    }
    // ! maybe replace this with 0 0 0 since mesh is not moving
    auto distance_to_mesh_center =
        pmp::distance(camera_position, pmp::centroid(mesh));

    std::vector<TreeNode> nodes_to_add;
    std::vector<TreeNode> nodes_to_remove;

    std::unordered_map<int, bool> invalid_nodes;

    for (auto node : currently_visible_nodes)
    {
        // if node is invalid, skip it
        if (invalid_nodes.find(node.id) != invalid_nodes.end())
            continue;

        auto distance_to_camera = pmp::distance(
            camera_position, pmp::centroid(mesh, node.faces->at(0)));

        if (distance_to_camera >= distance_to_mesh_center)
        {
            // this is the coarsest level since its parent is the root (i.e. the whole mesh)
            if (node.level == 1)
                continue;
            // go up one level
            auto parent = node.parent;

            // invalidate all nodes that are not visible anymore
            for (auto child : *parent->children)
            {
                invalid_nodes[child.id] = true;
                nodes_to_remove.push_back(child);
            }

            // add the parent node
            nodes_to_add.push_back(*parent);
        }
        else
        {
            auto children = node.children;
            // this is the finest level since it has no children
            if (children->size() == 0)
                continue;

            // invalidate the parent node
            invalid_nodes[node.id] = true;
            nodes_to_remove.push_back(node);
            
            // add the children nodes
            nodes_to_add.insert(nodes_to_add.end(), children->begin(),
                                children->end());
        }
    }

    // std::clog << "currently visible nodes: " << currently_visible_nodes.size()
    //           << std::endl;
    // std::clog << "nodes to remove: " << nodes_to_remove.size() << std::endl;
    // std::clog << "nodes to add: " << nodes_to_add.size() << std::endl;
    // update currently visible nodes
    for (auto node : nodes_to_remove)
    {
        currently_visible_nodes.erase(
            std::remove(currently_visible_nodes.begin(),
                        currently_visible_nodes.end(), node),
            currently_visible_nodes.end());
    }

    for (auto node : nodes_to_add)
    {
        currently_visible_nodes.push_back(node);
    }
    // std::clog << "currently visible nodes: " << currently_visible_nodes.size()
    //           << std::endl;
    // std::clog << "==========================================" << std::endl;

    // color all nodes
    for (auto node : currently_visible_nodes)
    {
        for (auto face : *node.faces)
        {
            color[face] = node.color;
        }
    }
}
} // namespace meshlets
