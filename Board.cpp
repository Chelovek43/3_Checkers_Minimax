#include "Board.hpp"

Board::Board() {
    reset();
}

void Board::reset() {
    board.fill({ CellState::Empty });
    for (int y = 0; y < 3; y++)
        for (int x = (y % 2 == 0 ? 0 : 1); x < 8; x += 2)
            board[y][x] = CellState::White;

    for (int y = 5; y < 8; y++)
        for (int x = (y % 2 == 0 ? 0 : 1); x < 8; x += 2)
            board[y][x] = CellState::Black;
}

CellState Board::getCell(int x, int y) const {
    return board[y][x];
}

void Board::setCell(int x, int y, CellState state) {
    board[y][x] = state;
}

bool Board::isCellEmpty(int x, int y) const {
    return getCell(x, y) == CellState::Empty;
}
