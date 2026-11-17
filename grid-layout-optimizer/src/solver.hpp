#pragma once

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace glo {

struct EntityDef {
    std::string name;
    char label = 'B';
    int h = 2;
    int w = 2;
    bool can_rotate = false;
    int min_count = 0;
    int max_count = 20;

    // Capacity & support balances
    int cap_delta = 0;      // Positive = provides capacity, negative = consumes capacity
    int support_delta = 0;  // Positive = provides support/morale, negative = consumes

    // Bilinear / quadratic output parameters
    double base_res_a = 0.0;
    double mult_res_a = 0.0;
    double base_res_b = 0.0;
    double mult_res_b = 0.0;
    double flat_throughput = 0.0;
};

struct SystemRules {
    int base_capacity = 200;
    double base_mult_a = 1.5;
    double base_mult_b = 1.5;
    double base_throughput = 0.0;
    double support_ratio = 2.0;      // Required support per unit of positive capacity
    double balance_penalty = 0.25;   // Penalty weight on |ResA - ResB|
    int max_wasted_tiles = 8;
    bool maximize_throughput = false;
};

struct EvaluatedPlan {
    std::vector<int> counts;
    double total_res_a = 0.0;
    double total_res_b = 0.0;
    double total_throughput = 0.0;
    int used_tiles = 0;
    int wasted_tiles = 0;
    double score = 0.0;
};

class LayoutSolver {
public:
    LayoutSolver(std::vector<EntityDef> entities, SystemRules rules, int total_tiles)
        : entities_(std::move(entities)), rules_(rules), total_tiles_(total_tiles) {}

    std::vector<EvaluatedPlan> generate_candidates() const {
        std::vector<EvaluatedPlan> results;
        std::vector<int> current(entities_.size(), 0);
        search(0, 0, current, results);

        std::sort(results.begin(), results.end(), [](const EvaluatedPlan& a, const EvaluatedPlan& b) {
            return a.score > b.score;
        });
        return results;
    }

    const std::vector<EntityDef>& entities() const { return entities_; }

private:
    std::vector<EntityDef> entities_;
    SystemRules rules_;
    int total_tiles_ = 0;

    void search(size_t idx, int used_tiles, std::vector<int>& current,
                std::vector<EvaluatedPlan>& results) const {
        if (used_tiles > total_tiles_) return;

        if (idx == entities_.size()) {
            int wasted = total_tiles_ - used_tiles;
            if (wasted > rules_.max_wasted_tiles) return;

            int cap_balance = rules_.base_capacity;
            int positive_cap = 0;
            int support_balance = 0;

            double sum_base_a = 0.0, sum_mult_a = rules_.base_mult_a;
            double sum_base_b = 0.0, sum_mult_b = rules_.base_mult_b;
            double throughput = rules_.base_throughput;

            for (size_t i = 0; i < entities_.size(); ++i) {
                int n = current[i];
                if (n == 0) continue;
                const auto& e = entities_[i];

                cap_balance += n * e.cap_delta;
                if (e.cap_delta > 0) positive_cap += n * e.cap_delta;
                support_balance += n * e.support_delta;

                sum_base_a += n * e.base_res_a;
                sum_mult_a += n * e.mult_res_a;
                sum_base_b += n * e.base_res_b;
                sum_mult_b += n * e.mult_res_b;
                throughput += n * e.flat_throughput;
            }

            if (cap_balance < 0) return;
            if (support_balance < static_cast<int>(std::ceil(positive_cap * rules_.support_ratio))) {
                return;
            }

            double res_a = sum_base_a * sum_mult_a;
            double res_b = sum_base_b * sum_mult_b;
            double score = rules_.maximize_throughput
                ? throughput
                : (res_a + res_b) - rules_.balance_penalty * std::abs(res_a - res_b);

            results.push_back({current, res_a, res_b, throughput, used_tiles, wasted, score});
            return;
        }

        const auto& e = entities_[idx];
        int area = e.h * e.w;
        int max_fit = (total_tiles_ - used_tiles) / area;
        int upper = std::min(e.max_count, max_fit);

        for (int c = e.min_count; c <= upper; ++c) {
            current[idx] = c;
            search(idx + 1, used_tiles + c * area, current, results);
        }
        current[idx] = 0;
    }
};

} // namespace glo
