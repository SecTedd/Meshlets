// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <pmp/visualization/mesh_viewer.h>
#include "meshlets/Meshlets.h"
#include "meshlets/sites/PoissonDiskRandom.h"

class MyViewer : public pmp::MeshViewer
{
public:
    // constructor
    MyViewer(const char* title, int width, int height)
        : MeshViewer(title, width, height)
    {
        set_draw_mode("Smooth Shading");
    }

    std::vector<meshlets::Site> sites_;
protected:
    // this function handles keyboard events
    void keyboard(int key, int code, int action, int mod) override;
    void process_imgui() override;
};
