// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <pmp/visualization/mesh_viewer.h>
#include "meshlets/Meshlets.h"
#include "meshlets/sites/PoissonDiskRandom.h"

// =======================================================================
// =========== Code generated by Github Copilot on 30.11.2023 ============
class ImGuiStreamBuffer : public std::streambuf
{
public:
    ImGuiStreamBuffer(std::string& str) : str_(str) {}

protected:
    virtual int_type overflow(int_type c)
    {
        if (c != EOF)
        {
            if (c == '\n')
            {
                str_.clear();
                str_ += buffer_;
                buffer_.clear();
            }
            else
            {
                buffer_ += static_cast<char>(c);
            }
        }
        return c;
    }

private:
    std::string& str_;
    std::string buffer_;
};
// =======================================================================

class MyViewer : public pmp::MeshViewer
{
public:
    // constructor
    MyViewer(const char* title, int width, int height)
        : MeshViewer(title, width, height), imguiBuffer(info_text)
    {
        set_draw_mode("Smooth Shading");
        std::cout.rdbuf(&imguiBuffer);
        std::cerr.rdbuf(&imguiBuffer);
    }

protected:
    // this function handles keyboard events
    void keyboard(int key, int code, int action, int mod) override;
    void process_imgui() override;

private:
    meshlets::ClusterAndSites cluster_and_sites;
    std::string info_text = "Welcome to the Meshlet Viewer!";
    ImGuiStreamBuffer imguiBuffer;
};