// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "MyViewer.h"
#include "meshlets/sites/PoissonDiskRandom.h"
#include "meshlets/sites/RandomSites.h"
#include "meshlets/visualization/ShowSites.h"
#include "meshlets/io/WriteSites.h"
#include "meshlets/clustering/GrowSites.h"
#include "meshlets/clustering/BruteForceClustering.h"
#include "meshlets/clustering/Lloyd.h"
#include "meshlets/visualization/ShowMeshlets.h"
#include "meshlets/visualization/ShowSites.h"

#include <imgui.h>

void MyViewer::keyboard(int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS) // only react on key press events
        return;

    switch (key)
    {
        // add your own keyboard action here
        default:
        {
            pmp::MeshViewer::keyboard(key, scancode, action, mods);
            break;
        }
    }
}

void MyViewer::process_imgui()
{
    pmp::MeshViewer::process_imgui();

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Draw Mode", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Smooth Shading"))
        {
            set_draw_mode("Smooth Shading");
        }
        if (ImGui::Button("Show Triangles"))
        {
            set_draw_mode("Hidden Line");
        }
        if (ImGui::Button("Show Vertices"))
        {
            set_draw_mode("Points");
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Mesh Properties"))
    {
        if (ImGui::Button("Print Properties"))
        {
            mesh_.property_stats();
        }

        ImGui::Spacing();

        if (ImGui::Button("Check Clustering/Property Consistency"))
        {
            if (cluster_and_sites.cluster.empty())
            {
                std::cerr << "Clustering not computed yet" << std::endl;
                return;
            }
            bool consistent =
                meshlets::check_consistency(mesh_, cluster_and_sites.cluster);
            if (consistent)
            {
                std::cout << "Consistent" << std::endl;
            }
            else
            {
                std::cout << "Inconsistent" << std::endl;
            }
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Visualization and Statistics",
                                ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Show Sites"))
        {
            if (cluster_and_sites.sites.empty())
            {
                std::cerr << "No sites generated yet" << std::endl;
                return;
            }
            meshlets::show_faces_with_sites(mesh_);
            update_mesh();
            set_draw_mode("Smooth Shading");
            renderer_.set_diffuse(0.9);
            renderer_.set_specular(0.0);
            renderer_.set_shininess(1.0);
        }

        ImGui::Spacing();

        if (ImGui::Button("Show Meshlets"))
        {
            if (cluster_and_sites.cluster.empty())
            {
                std::cerr << "Clustering not computed yet" << std::endl;
                return;
            }
            meshlets::color_meshlets(mesh_, cluster_and_sites.cluster);
            update_mesh();
            set_draw_mode("Smooth Shading");
            renderer_.set_shininess(0);
            renderer_.set_specular(0);
            renderer_.set_diffuse(0);
        }

        ImGui::Spacing();

        if (ImGui::Button("Validate Meshlets"))
        {
            if (cluster_and_sites.cluster.empty())
            {
                std::cerr << "Clustering not computed yet" << std::endl;
                return;
            }

            // First check wheter all faces are assigned to a meshlet
            auto is_site = mesh_.get_face_property<bool>("f:is_site");
            auto closest_site = mesh_.get_face_property<int>("f:closest_site");
            assert(is_site);
            assert(closest_site);

            for (auto face : mesh_.faces())
            {
                if (is_site[face])
                {
                    continue;
                }
                if (closest_site[face] == -1)
                {
                    std::cerr << "Face " << face.idx()
                              << " is not assigned to a meshlet" << std::endl;
                    return;
                }
            }

            int num_valid = 0;
            int total = cluster_and_sites.cluster.size();

            meshlets::Cluster cluster_with_only_invalid_meshlets;
            cluster_with_only_invalid_meshlets.reserve(total);

            for (auto &meshlet : cluster_and_sites.cluster)
            {
                if (meshlets::is_valid(mesh_, *meshlet))
                {
                    num_valid++;
                }
                else
                {
                    cluster_with_only_invalid_meshlets.push_back(
                        std::make_shared<meshlets::Meshlet>(*meshlet));
                }
            }
            std::cout << "Valid Meshlets: " << num_valid << "/" << total << " ("
                      << ((float)num_valid / (float)total) * 100 << "%)"
                      << std::endl;

            meshlets::color_meshlets(mesh_, cluster_with_only_invalid_meshlets);
            update_mesh();
            set_draw_mode("Smooth Shading");
            renderer_.set_shininess(0);
            renderer_.set_specular(0);
            renderer_.set_diffuse(0);
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Algorithms", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static int num_sites = mesh_.n_faces() * 0.005;
        int min = mesh_.n_faces() * 0.001;
        int max = mesh_.n_faces() * 0.01;
        ImGui::SliderInt("Number of Sites", &num_sites, min, max, "%d");

        ImGui::Spacing();

        if (ImGui::Button("Generate PDS Sites"))
        {
            auto start = std::chrono::high_resolution_clock::now();
            cluster_and_sites.sites =
                meshlets::generate_pds_sites(mesh_, num_sites);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            std::cout << "Generating Sites took: " << elapsed.count() << " s\n";
        }

        ImGui::Spacing();

        if (ImGui::Button("Generate Random Sites"))
        {
            auto start = std::chrono::high_resolution_clock::now();
            cluster_and_sites.sites =
                meshlets::generate_random_sites(mesh_, num_sites);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            std::cout << "Generating Sites took: " << elapsed.count() << " s\n";
        }

        ImGui::Spacing();

        static int max_iterations = 1000;
        ImGui::InputInt("Max Iterations For Growing", &max_iterations);

        ImGui::Spacing();

        if (ImGui::Button("Grow Sites"))
        {
            if (cluster_and_sites.sites.empty())
            {
                std::cerr << "No sites generated. Please generate sites first."
                          << std::endl;
                return;
            }
            // measure time
            auto start = std::chrono::high_resolution_clock::now();
            cluster_and_sites.cluster = meshlets::grow_sites(
                mesh_, cluster_and_sites.sites, max_iterations);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            std::cout << "Growing Sites took: " << elapsed.count() << " s\n";
        }

        ImGui::Spacing();

        if (ImGui::Button("Brute Force Clustering"))
        {
            if (cluster_and_sites.sites.empty())
            {
                std::cerr << "No sites generated. Please generate sites first."
                          << std::endl;
                return;
            }
            // measure time
            auto start = std::chrono::high_resolution_clock::now();
            cluster_and_sites.cluster =
                meshlets::brute_force_sites(mesh_, cluster_and_sites.sites);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            std::cout << "Brute Force Clustering took: " << elapsed.count()
                      << " s\n";
        }

        ImGui::Spacing();

        static int max_lloyd_iterations = 100;
        ImGui::InputInt("Max Lloyd Iterations", &max_lloyd_iterations);

        ImGui::Spacing();

        if (ImGui::Button("Lloyd"))
        {
            if (cluster_and_sites.sites.empty())
            {
                std::cerr << "No sites generated. Please generate sites first."
                          << std::endl;
                return;
            }
            auto start = std::chrono::high_resolution_clock::now();
            cluster_and_sites = meshlets::lloyd(mesh_, cluster_and_sites.sites,
                                                max_lloyd_iterations);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = (end - start);
            std::cout << "and took: " << elapsed.count() << " s\n";
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Benchmarking"))
    {
        static int benchmark_iterations = 1000;
        ImGui::InputInt("Benchmark Iterations", &benchmark_iterations);

        if (ImGui::Button("Benchmark GS-Clustering"))
        {
            if (cluster_and_sites.sites.empty())
            {
                std::cerr << "No sites generated. Please generate sites first."
                          << std::endl;
                return;
            }
            // measure time
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < benchmark_iterations; i++)
            {
                meshlets::grow_sites(mesh_, cluster_and_sites.sites);
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed =
                (end - start) / benchmark_iterations;
            std::cout << "Mean GS-Clustering over " << benchmark_iterations
                      << " iterations: " << elapsed.count() << " s\n";
        }

        ImGui::Spacing();

        if (ImGui::Button("Benchmark BF-Clustering"))
        {
            if (cluster_and_sites.sites.empty())
            {
                std::cerr << "No sites generated. Please generate sites first."
                          << std::endl;
                return;
            }
            // measure time
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < benchmark_iterations; i++)
            {
                meshlets::brute_force_sites(mesh_, cluster_and_sites.sites);
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed =
                (end - start) / benchmark_iterations;
            std::cout << "Mean BF-Clustering over " << benchmark_iterations
                      << " iterations: " << elapsed.count() << " s\n";
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("IO"))
    {
        if (ImGui::Button("Save Mesh"))
        {
            std::string filename = "output.off";
            meshlets::write_sites(mesh_, filename);
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.04, 0.43, 0.3, 1), "%s", info_text.c_str());

    ImGui::Spacing();
    ImGui::Spacing();
}
