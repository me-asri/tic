#include <algorithm>
#include <stdexcept>

#include "board.h"

Board::Board()
{
    board = new Mark[BOARD_TILES];
    std::fill(board, board + BOARD_TILES, Mark::NONE);
}

Board::Board(const Board &b)
{
    currentTurn = b.currentTurn;

    board = new Mark[BOARD_TILES];
    std::copy(b.board, b.board + BOARD_TILES, board);
}

Board::Board(Board &&b)
    : board(b.board), currentTurn(b.currentTurn)
{
    b.board = nullptr;
}

Board::~Board()
{
    if (board != nullptr)
        delete[] board;
}

Board::Mark Board::getMark(int index) const
{
    return board[index];
}

Board::Mark Board::getMark(int i, int j) const
{
    return getMark((i * BOARD_SIDES) + j);
}

bool Board::mark(int index)
{
    if (board[index] != Mark::NONE)
        return false;
    if (getState() != State::INCOMPLETE)
        return false;

    board[index] = currentTurn;
    currentTurn = (currentTurn == Mark::X) ? Mark::O : Mark::X;

    return true;
}

bool Board::mark(int i, int j)
{
    return mark((i * BOARD_SIDES) + j);
}

Board::Mark Board::getTurn() const
{
    return currentTurn;
}

void Board::clear()
{
    currentTurn = Mark::X;
    std::fill(board, board + BOARD_TILES, Mark::NONE);
}

Board::State Board::getState() const
{
    Mark winner = getWinner();

    switch (winner)
    {
    case Mark::X:
        return State::X_WINNER;
    case Mark::O:
        return State::O_WINNER;
    case Mark::NONE:
        for (int i{0}; i < BOARD_TILES; i++)
            if (board[i] == Mark::NONE)
                return State::INCOMPLETE;

        return State::DRAW;
    default:
        throw std::range_error("getState(): unknown mark");
    }
}

Board::Mark Board::getWinner() const
{
    int sum{0};
    Mark player{};

    // Check rows and columns
    for (int i{0}; i < BOARD_SIDES; i++)
    {
        // Check row
        sum = 0;
        player = getMark(i, 0);
        for (int j{0}; j < BOARD_SIDES; j++)
            sum += getMark(i, j);
        if (sum != 0 && sum == 3 * player)
            return player;
        // Check column
        sum = 0;
        player = getMark(0, i);
        for (int j{0}; j < BOARD_SIDES; j++)
            sum += getMark(j, i);
        if (sum != 0 && sum == 3 * player)
            return player;
    }

    player = getMark(BOARD_SIDES / 2, BOARD_SIDES / 2);
    // Check diagonal
    sum = 0;
    for (int i{0}; i < BOARD_SIDES; i++)
        sum += getMark(i, i);
    if (sum != 0 && sum == 3 * player)
        return player;
    // Check anti-diagonal
    sum = 0;
    for (int i{0}; i < BOARD_SIDES; i++)
        sum += getMark(BOARD_SIDES - i - 1, i);
    if (sum != 0 && sum == 3 * player)
        return player;

    return Mark::NONE;
}