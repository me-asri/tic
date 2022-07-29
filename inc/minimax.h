#ifndef TIC_MINIMAX_H
#define TIC_MINIMAX_H

#include <vector>
#include <atomic>
#include <stdexcept>

#include "board.h"

class Minimax
{
private:
    const Board &board;

    int calcScore(const Board &state, int depth);
    int minimax(const Board &state, int depth, bool maximizing, std::atomic<bool> &running);

    static std::vector<Board> getPossibleStates(const Board &state);

public:
    Minimax(const Board &board);

    int nextMove(std::atomic<bool> &running);
    int nextMove();

    class CancelledException : public std::runtime_error
    {
    public:
        CancelledException(const char *msg) : std::runtime_error(msg){};
    };
};

#endif