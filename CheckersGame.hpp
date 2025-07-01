#pragma once
#include "Board.hpp"
#include <vector>
#include <SFML/Graphics.hpp>

class CheckersGame {
public:
    CheckersGame();
    void reset();
    void draw(sf::RenderWindow& window);
    void handleMouseClick(int mouseX, int mouseY);
    std::vector<std::pair<int, int>> getValidMoves(int x, int y);

    int count_checkers(const BoardType& board, CellState player);
    int count_queens(const BoardType& board, CellState player);
    int distances_to_the_edge(const BoardType& board, CellState player);
    int count_possible_moves(const BoardType& board, CellState player);

    int Evaluation(const BoardType& board, CellState player);

private:
    Board board;
    CellState currentPlayer;
    int selectedX, selectedY;
    std::vector<std::pair<int, int>> validMoves;

    bool isMoveValid(int x, int y);
    void movePiece(int newX, int newY);
    void switchPlayer();
};
