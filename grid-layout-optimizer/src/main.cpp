#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "packer.hpp"
#include "solver.hpp"

namespace {

std::vector<std::string> load_map_file(const std::string& path) {
    std::ifstream file(path);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty() && line[0] != ';') lines.push_back(line);
    }
    return lines;
}

bool load_config_file(const std::string& path, std::vector<glo::EntityDef>& entities, glo::SystemRules& rules) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string tag;
        iss >> tag;

        if (tag == "RULE") {
            std::string key;
            iss >> key;
            if (key == "base_capacity") iss >> rules.base_capacity;
            else if (key == "base_mult_a") iss >> rules.base_mult_a;
            else if (key == "base_mult_b") iss >> rules.base_mult_b;
            else if (key == "base_throughput") iss >> rules.base_throughput;
            else if (key == "support_ratio") iss >> rules.support_ratio;
            else if (key == "balance_penalty") iss >> rules.balance_penalty;
            else if (key == "max_wasted_tiles") iss >> rules.max_wasted_tiles;
            else if (key == "maximize_throughput") {
                int flag = 0;
                iss >> flag;
                rules.maximize_throughput = (flag != 0);
            }
        } else if (tag == "ENTITY") {
            glo::EntityDef e;
            int rot = 0;
            iss >> e.name >> e.label >> e.h >> e.w >> rot
                >> e.min_count >> e.max_count
                >> e.cap_delta >> e.support_delta
                >> e.base_res_a >> e.mult_res_a
                >> e.base_res_b >> e.mult_res_b
                >> e.flat_throughput;
            e.can_rotate = (rot != 0);
            entities.push_back(e);
        }
    }
    return !entities.empty();
}

} // namespace

int main(int argc, char* argv[]) {
    std::string map_path;
    std::string cfg_path;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--map" && i + 1 < argc) map_path = argv[++i];
        else if (arg == "--config" && i + 1 < argc) cfg_path = argv[++i];
    }

    std::vector<std::string> grid_map = map_path.empty() ? std::vector<std::string>{
        "#######...............",
        "#######...............",
        "#######...............",
        "#######...............",
        "#######...............",
        "#######...............",
        "......................",
        "......................",
        "......................",
        "......................",
        "......................",
        "......................",
        "................######",
        "................######",
        "................######",
        "................######"
    } : load_map_file(map_path);

    if (grid_map.empty() || grid_map[0].size() > 32) {
        std::cerr << "Error: Map must be non-empty and <= 32 columns wide.\n";
        return 1;
    }

    glo::SystemRules rules;
    std::vector<glo::EntityDef> entities;

    if (cfg_path.empty() || !load_config_file(cfg_path, entities, rules)) {
        // Built-in default scenario
        entities = {
            {"House_Std",   'F', 2, 2, false, 0, 14,  110,    0,  25000.0, 0.00,     0.0, 0.00,   0.0},
            {"House_Boost", 'B', 2, 2, false, 0, 14,   80,    0,   7500.0, 0.20,     0.0, 0.00,   0.0},
            {"House_High",  'C', 2, 2, false, 0,  3,  150,    0, 130000.0, 0.00,     0.0, 0.00,   0.0},
            {"Producer_B",  'P', 2, 3, true, 15, 36,  -60,    0,      0.0, 0.00, 14400.0, 0.10,   0.0},
            {"Support_L",   'S', 2, 3, true,  0,  8,    0,  900,      0.0, 0.00,     0.0, 0.00, 180.0},
            {"Support_S",   's', 2, 2, false, 0, 10,    0,  750,      0.0, 0.00,     0.0, 0.00, 120.0},
            {"Modifier",    'T', 2, 2, false, 4,  4,    0, -150,      0.0, 0.00,     0.0, 0.00,   0.0}
        };
    }

    glo::BitmaskGridPacker base_grid(grid_map);
    const int total_tiles = base_grid.open_tiles();

    auto t0 = std::chrono::high_resolution_clock::now();
    glo::LayoutSolver solver(entities, rules, total_tiles);
    std::vector<glo::EvaluatedPlan> candidates = solver.generate_candidates();

    for (const auto& plan : candidates) {
        std::vector<glo::PlacementPiece> pieces;
        for (size_t i = 0; i < entities.size(); ++i) {
            if (plan.counts[i] > 0) {
                pieces.push_back({
                    static_cast<int>(i),
                    entities[i].h,
                    entities[i].w,
                    entities[i].can_rotate,
                    entities[i].label,
                    plan.counts[i]
                });
            }
        }

        glo::BitmaskGridPacker packer(grid_map);
        if (packer.pack(pieces)) {
            auto t1 = std::chrono::high_resolution_clock::now();
            double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

            std::cout << "=== grid-layout-optimizer (" << ms << " ms) ===\n"
                      << "Tiles Used: " << plan.used_tiles << " / " << total_tiles
                      << " (Open: " << plan.wasted_tiles << ")\n\n"
                      << "Selected Entities:\n";
            for (size_t i = 0; i < entities.size(); ++i) {
                if (plan.counts[i] > 0) {
                    std::cout << "  [" << entities[i].label << "] " << entities[i].name
                              << " (" << entities[i].h << "x" << entities[i].w << "): "
                              << plan.counts[i] << "\n";
                }
            }
            std::cout << "\nOutputs:\n"
                      << "  Resource A: " << static_cast<long long>(plan.total_res_a) << "\n"
                      << "  Resource B: " << static_cast<long long>(plan.total_res_b) << "\n"
                      << "  Throughput: " << static_cast<long long>(plan.total_throughput) << "\n\n"
                      << "Packed 2D Grid (# = Obstacle, _ = Open Slack):\n";
            packer.print_board();
            return 0;
        }
    }

    std::cout << "No valid 2D packing found for the evaluated candidates.\n";
    return 1;
}
