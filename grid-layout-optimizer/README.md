# grid-layout-optimizer

A lightweight, zero-dependency **C++20** solver for 2D irregular grid packing and nonlinear (quadratic/bilinear) resource synergy optimization.

Designed for orthogonal rectangle packing (`2x2`, `3x2`, `2x3`, `3x3`, and arbitrary `WxH` dimensions) inside irregular polygons or bounding boxes with fixed obstacles, while simultaneously optimizing coupled resource equations (`Output = Base * Multiplier`).

## Features

- **Zero External Dependencies:** Pure C++20 using `<bit>` hardware bitmask intrinsics (`std::countr_zero`, `std::popcount`). Compiles into a standalone ~150 KB executable.
- **Exact-Cover 2D Bitmask Packer (`packer.hpp`):** Packs arbitrary rotatable and fixed rectangles around fixed obstacles (`#`) with configurable slack tolerance.
- **Nonlinear Synergy Optimizer (`solver.hpp`):** Evaluates quadratic self-synergy (`0.1x^2 + 1.5x`) and bilinear cross-synergy (`Base * Multiplier`) subject to capacity, support/morale, and budget constraints.
- **Config-Driven CLI:** Load any ASCII map grid and entity ruleset from plain text files without recompiling.

## Repository Structure

```text
grid-layout-optimizer/
├── CMakeLists.txt
├── README.md
├── examples/
│   ├── irregular_grid.map
│   └── economy_rules.cfg
└── src/
    ├── main.cpp
    ├── packer.hpp
    └── solver.hpp
```

## Build Instructions

### Windows (MSVC / Visual Studio 2022)
```powershell
cl /O2 /std:c++20 /EHsc /Isrc src\main.cpp /Fe:grid-layout-optimizer.exe
```

### CMake (Windows / Linux / macOS)
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Usage

```bash
./grid-layout-optimizer --map examples/irregular_grid.map --config examples/economy_rules.cfg
```

### Map File Format (`.map`)
Use `#` for fixed obstacles or locked cells, and `.` for open buildable cells (up to 32 columns wide):

```text
#######...............
#######...............
#######...............
#######...............
#######...............
#######...............
......................
......................
................######
................######
```

## Mathematical Model

For resource systems governed by multiplicative synergy:

$$\text{Total Output}_k = \left(\sum_{i} n_i \cdot B_{i,k}\right) \times \left(M_{\text{base}, k} + \sum_{i} n_i \cdot M_{i,k}\right)$$

1. **Stage 1 (Branch-and-Bound):** Enumerates feasible integer counts $n_i$ satisfying total area, capacity balance ($\sum C_{\text{prov}} \ge \sum C_{\text{req}}$), support thresholds, and entity count bounds.
2. **Stage 2 (Bitmask Exact-Cover):** Sorts top candidate plans by objective score and verifies exact 2D geometric placement using first-uncovered-cell symmetry breaking.

## License

MIT License
