#include <vector>
#include <limits>
#include <stdexcept>
#include <atomic>

#include "strings.h"
#include "minimax.h"

Minimax::Minimax(const Board &board)
    : board(board)
{
}

int Minimax::nextMove(std::atomic<bool> &running)
{
    int bestMove{-1};

    int value{std::numeric_limits<int>::min()};
    for (int i{0}; i < board.BOARD_TILES; i++)
    {
        if (!running)
            throw CancelledException(strings::EXCEPT_CANCEL);

        if (board.getMark(i) == Board::Mark::NONE)
        {
            Board copy{board};
            copy.mark(i);

            int nextValue{minimax(copy, 0, false, running)};
            if (nextValue > value)
            {
                bestMove = i;
                value = nextValue;
            }
        }
    }

    if (bestMove < 0)
        throw std::logic_error(strings::EXCEPT_AI_FAIL);

    return bestMove;
}

int Minimax::nextMove()
{
    std::atomic<bool> running{true};
    return nextMove(running);
}

int Minimax::calcScore(const Board &state, int depth)
{
    // Maximizer is the current player making move in the original board
    Board::Mark maximizer{board.getTurn()};

    switch (state.getState())
    {
    case Board::State::X_WINNER:
        return (maximizer == Board::Mark::X) ? (10 - depth) : (-10 + depth);
    case Board::State::O_WINNER:
        return (maximizer == Board::Mark::O) ? (10 - depth) : (-10 + depth);
    default:
        return 0;
    }
}

int Minimax::minimax(const Board &state, int depth, bool maximizing, std::atomic<bool> &running)
{
    if (!running)
        throw CancelledException(strings::EXCEPT_CANCEL);

    // Reached a final state
    if (state.getState() != Board::State::INCOMPLETE)
        return calcScore(state, depth);

    int value{};
    if (maximizing)
    {
        // Maximizing player
        value = std::numeric_limits<int>::min();
        for (const Board &state : getPossibleStates(state))
            value = std::max(value, minimax(state, depth + 1, false, running));
    }
    else
    {
        // Minimizing player
        value = std::numeric_limits<int>::max();
        for (const Board &state : getPossibleStates(state))
            value = std::min(value, minimax(state, depth + 1, true, running));
    }
    return value;
}

std::vector<Board> Minimax::getPossibleStates(const Board &state)
{
    std::vector<Board> states{};

    for (int i{0}; i < state.BOARD_TILES; i++)
    {
        if (state.getMark(i) == Board::Mark::NONE)
        {
            Board copy{state};
            if (copy.mark(i))
                states.push_back(std::move(copy));
        }
    }

    return states;
}