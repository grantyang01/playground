#pragma once

#include <algorithm>
#include <bit>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace glo {

struct PlacementPiece {
    int id;
    int h;
    int w;
    bool can_rotate;
    char label;
    int count;
};

class BitmaskGridPacker {
public:
    explicit BitmaskGridPacker(const std::vector<std::string>& ascii_map)
        : H_(static_cast<int>(ascii_map.size())),
          W_(ascii_map.empty() ? 0 : static_cast<int>(ascii_map[0].size())),
          mask_(H_, 0),
          board_(ascii_map),
          full_row_(W_ == 32 ? 0xFFFFFFFFU : ((1U << W_) - 1U)) {
        for (int r = 0; r < H_; ++r) {
            for (int c = 0; c < W_; ++c) {
                if (ascii_map[r][c] == '#') {
                    mask_[r] |= (1U << c);
                }
            }
        }
    }

    int open_tiles() const {
        int total = 0;
        for (int r = 0; r < H_; ++r) {
            total += W_ - std::popcount(mask_[r]);
        }
        return total;
    }

    bool pack(std::vector<PlacementPiece> pieces, long long max_nodes = 400000) {
        int needed = 0;
        for (const auto& p : pieces) {
            needed += p.h * p.w * p.count;
        }
        int available = open_tiles();
        if (needed > available) return false;

        // Sort larger footprints first for faster pruning
        std::sort(pieces.begin(), pieces.end(), [](const PlacementPiece& a, const PlacementPiece& b) {
            return (a.h * a.w) > (b.h * b.w);
        });

        nodes_visited_ = 0;
        max_nodes_ = max_nodes;
        return dfs(0, pieces, available - needed);
    }

    void print_board(std::ostream& os = std::cout) const {
        for (const auto& row : board_) {
            os << "  " << row << '\n';
        }
    }

private:
    int H_ = 0;
    int W_ = 0;
    std::vector<uint32_t> mask_;
    std::vector<std::string> board_;
    uint32_t full_row_ = 0;
    long long nodes_visited_ = 0;
    long long max_nodes_ = 400000;

    inline bool can_place(int r, int c, int h, int w) const {
        if (r + h > H_ || c + w > W_) return false;
        uint32_t span = ((1U << w) - 1U) << c;
        for (int i = 0; i < h; ++i) {
            if (mask_[r + i] & span) return false;
        }
        return true;
    }

    inline void toggle_piece(int r, int c, int h, int w, char label) {
        uint32_t span = ((1U << w) - 1U) << c;
        for (int i = 0; i < h; ++i) {
            mask_[r + i] ^= span;
            for (int j = 0; j < w; ++j) {
                board_[r + i][c + j] = label;
            }
        }
    }

    bool dfs(int r, std::vector<PlacementPiece>& pieces, int slack_left) {
        if (++nodes_visited_ > max_nodes_) return false;

        while (r < H_ && mask_[r] == full_row_) {
            ++r;
        }
        if (r == H_) {
            for (const auto& p : pieces) {
                if (p.count > 0) return false;
            }
            return true;
        }

        int c = std::countr_zero(~mask_[r]);

        for (auto& p : pieces) {
            if (p.count <= 0) continue;

            // Primary orientation (h x w)
            if (can_place(r, c, p.h, p.w)) {
                --p.count;
                toggle_piece(r, c, p.h, p.w, p.label);
                if (dfs(r, pieces, slack_left)) return true;
                toggle_piece(r, c, p.h, p.w, '.');
                ++p.count;
            }

            // Rotated orientation (w x h)
            if (p.can_rotate && p.h != p.w && can_place(r, c, p.w, p.h)) {
                --p.count;
                toggle_piece(r, c, p.w, p.h, p.label);
                if (dfs(r, pieces, slack_left)) return true;
                toggle_piece(r, c, p.w, p.h, '.');
                ++p.count;
            }
        }

        if (slack_left > 0) {
            toggle_piece(r, c, 1, 1, '_');
            if (dfs(r, pieces, slack_left - 1)) return true;
            toggle_piece(r, c, 1, 1, '.');
        }

        return false;
    }
};

} // namespace glo
