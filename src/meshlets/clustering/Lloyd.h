#pragma once

#include "../Meshlets.h"
#include "./GrowSites.h"
#include "../sites/RandomSites.h"

namespace meshlets {
std::vector<std::unique_ptr<std::vector<pmp::Face>>> collect_meshlets(
    pmp::SurfaceMesh &mesh, std::vector<Site> &sites);

std::vector<Site> generate_new_sites(
    pmp::SurfaceMesh &mesh, std::vector<Site> &sites,
    std::vector<std::unique_ptr<std::vector<pmp::Face>>> &meshlets);

std::vector<Site> lloyd(pmp::SurfaceMesh &mesh, std::vector<Site> &sites,
                        int max_iterations = 20);
} // namespace meshlets