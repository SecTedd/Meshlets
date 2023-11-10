// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "MyViewer.h"
#include "meshlets/sites/PoissonDiskRandom.h"
#include "meshlets/visualization/ShowSites.h"
#include "meshlets/io/WriteSites.h"
#include "meshlets/clustering/GrowSites.h"
#include "meshlets/clustering/BruteForceClustering.h"
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
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Meshlets", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static int num_sites = mesh_.n_faces() * 0.002;
        int min = mesh_.n_faces() * 0.001;
        int max = mesh_.n_faces() * 0.005;
        ImGui::SliderInt("Number of Sites", &num_sites, min, max, "%d");

        ImGui::Spacing();

        if (ImGui::Button("Generate Sites"))
        {
            sites_ = meshlets::generate_pds_sites(mesh_, num_sites);
            meshlets::show_faces_with_sites(mesh_, sites_);
            update_mesh();
            set_draw_mode("Smooth Shading");
            renderer_.set_diffuse(0.9);
            renderer_.set_specular(0.0);
            renderer_.set_shininess(1.0);
        }

        ImGui::Spacing();

        static int max_iterations = 1000;
        ImGui::InputInt("Max Iterations", &max_iterations);

        ImGui::Spacing();

        if (ImGui::Button("Grow Sites"))
        {
            if (sites_.empty())
            {
                sites_ = meshlets::generate_pds_sites(mesh_, num_sites);
            }
            // measure time
            auto start = std::chrono::high_resolution_clock::now();
            meshlets::grow_sites(mesh_, sites_, max_iterations);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            std::cout << "Growing Sites took: " << elapsed.count() << " s\n";
            meshlets::color_meshlets(mesh_, sites_);
            update_mesh();
            if (max_iterations != 1000)
            {
                set_draw_mode("Hidden Line");
                max_iterations++;
            }
            else
            {
                set_draw_mode("Smooth Shading");
            }
            renderer_.set_shininess(0);
            renderer_.set_specular(0);
            renderer_.set_diffuse(0);
        }

        ImGui::Spacing();

        if (ImGui::Button("Brute Force Clustering"))
        {
            if (sites_.empty())
            {
                sites_ = meshlets::generate_pds_sites(mesh_, num_sites);
            }
            // measure time
            auto start = std::chrono::high_resolution_clock::now();
            meshlets::brute_force_sites(mesh_, sites_);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            std::cout << "Brute Force Clustering took: " << elapsed.count()
                      << " s\n";
            meshlets::color_meshlets(mesh_, sites_);
            update_mesh();
            set_draw_mode("Smooth Shading");
            renderer_.set_shininess(0);
            renderer_.set_specular(0);
            renderer_.set_diffuse(0);
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        static int benchmark_iterations = 1000;
        ImGui::InputInt("Benchmark Iterations", &benchmark_iterations);

        if (ImGui::Button("Benchmark GS-Clustering"))
        {
            if (sites_.empty())
            {
                sites_ = meshlets::generate_pds_sites(mesh_, num_sites);
            }
            // measure time
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < benchmark_iterations; i++)
            {
                meshlets::grow_sites(mesh_, sites_);
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed =
                (end - start) / benchmark_iterations;

            std::cout << "Mean GS-Clustering over " << benchmark_iterations
                      << " iterations: " << elapsed.count() << " s\n";
            meshlets::color_meshlets(mesh_, sites_);
            update_mesh();
            set_draw_mode("Smooth Shading");
            renderer_.set_shininess(0);
            renderer_.set_specular(0);
            renderer_.set_diffuse(0);
        }

        ImGui::Spacing();

        if (ImGui::Button("Benchmark BF-Clustering"))
        {
            if (sites_.empty())
            {
                sites_ = meshlets::generate_pds_sites(mesh_, num_sites);
            }
            // measure time
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < benchmark_iterations; i++)
            {
                meshlets::brute_force_sites(mesh_, sites_);
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed =
                (end - start) / benchmark_iterations;

            std::cout << "Mean BF-Clustering over " << benchmark_iterations
                      << " iterations: " << elapsed.count() << " s\n";
            meshlets::color_meshlets(mesh_, sites_);
            update_mesh();
            set_draw_mode("Smooth Shading");
            renderer_.set_shininess(0);
            renderer_.set_specular(0);
            renderer_.set_diffuse(0);
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
}
