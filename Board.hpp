#pragma once
#include <array>

enum class CellState { Empty, White, Black };

using BoardType = std::array<std::array<CellState, 8>, 8>;

class Board {
public:
    Board();
    void reset();
    CellState getCell(int x, int y) const;
    void setCell(int x, int y, CellState state);
    bool isCellEmpty(int x, int y) const;

private:
    BoardType board;
};
