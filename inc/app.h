#ifndef TIC_APP_H
#define TIC_APP_H

#include <mutex>
#include <atomic>

#include "board.h"
#include "minimax.h"
#include "win/main.h"

class TicApp
{
private:
    MainWindow mainWin{};

public:
    Board board{};
    Minimax minimax{board};

    std::atomic<bool> aiThinking{false};

    TicApp();
};

extern TicApp gApp;
extern std::recursive_mutex gAppMutex;

#endif