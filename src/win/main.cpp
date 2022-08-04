#include "win/main.h"

#include <random>
#include <thread>
#include <stdexcept>

#include <Fl/Fl_Widget.H>

#include "app.h"
#include "strings.h"

// As we can't pass member functions as callbacks we have to use lambdas as a workaround
#define FL_SET_CB_LONG(widget, func, data) \
    widget->callback([](Fl_Widget *w, long d) { static_cast<MainWindow *>(w->window())->func(w, d); }, data)

static const int AI_CANCEL{-1};
static const int AI_FAIL{-2};

struct AiThreadData
{
    int index;
    MainWindow *win;

    AiThreadData(int index, MainWindow *win)
        : index(index), win(win) {}
};

MainWindow::MainWindow()
    : MainWindowView()
{
    setUiState(IDLE);

    setTilesCb();
}

void MainWindow::tilePressCb(Fl_Widget *w, long d)
{
    // Prevent player from interacting with the board while AI is running
    if (gApp.aiThinking)
        return;

    gAppMutex.lock();

    Board::State state{gApp.board.getState()};

    // If board in finished state do nothing
    if (state != Board::State::INCOMPLETE)
        return;

    if (!gApp.board.mark(d))
    {
        gAppMutex.unlock();
        return;
    }
    drawBoard(gApp.board);

    if (checkGameOver(gApp.board))
    {
        gAppMutex.unlock();
        return;
    }

    aiMakeMove(diff);

    gAppMutex.unlock();
}

void MainWindow::turnSelectionCb(Fl_Widget *w, long d)
{
    starter = static_cast<Player>(d);
}

void MainWindow::diffChoiceCb(Fl_Widget *w, long d)
{
    diff = static_cast<Difficulty>(d);
}

void MainWindow::startButtonCb(Fl_Widget *w, void *d)
{
    switch (state)
    {
    case IDLE:
        setUiState(INGAME);

        if (starter == Player::Human)
            turnLabel->label(strings::LABEL_YOUR_TURN);
        else
            aiMakeMove(diff);

        break;

    case INGAME:
        gAppMutex.lock();

        gApp.aiThinking = false;

        gApp.board.clear();
        drawBoard(gApp.board);

        gAppMutex.unlock();

        setUiState(IDLE);

        break;
    }
}

void MainWindow::setTilesCb()
{
    Fl_Widget *const *tiles{tilesGroup->array()};

    int count{tilesGroup->children()};
    for (int i{0}; i < count; i++)
        FL_SET_CB_LONG(tiles[i], tilePressCb, i);
}

void MainWindow::drawBoard(const Board &board)
{
    Fl_Widget *const *tiles{tilesGroup->array()};
    int count{tilesGroup->children()};

    for (int i{0}; i < count; i++)
    {
        Fl_Widget *tile{tiles[i]};

        Board::Mark mark{board.getMark(i)};
        switch (mark)
        {
        case Board::Mark::X:
            tile->label(strings::LABEL_X);
            tile->labelcolor(FL_BLUE);
            break;
        case Board::Mark::O:
            tile->label(strings::LABEL_O);
            tile->labelcolor(FL_RED);
            break;
        default:
            tile->label(nullptr);
            break;
        }
    }
}

void MainWindow::setUiState(UiState state)
{
    switch (state)
    {
    case IDLE:
        turnLabel->label(nullptr);
        tilesGroup->deactivate();
        startButton->label(strings::BUTTON_START);
        playerGroup->activate();
        diffChoice->activate();
        break;
    case INGAME:
        tilesGroup->activate();
        startButton->label(strings::BUTTON_RESTART);
        playerGroup->deactivate();
        diffChoice->deactivate();
        break;
    }

    this->state = state;
}

bool MainWindow::checkGameOver(const Board &board)
{
    switch (board.getState())
    {
    case Board::State::X_WINNER:
        turnLabel->label((starter == Player::Human) ? strings::LABEL_WIN : strings::LABEL_LOSE);
        tilesGroup->deactivate();
        return true;
    case Board::State::O_WINNER:
        turnLabel->label((starter == Player::PC) ? strings::LABEL_WIN : strings::LABEL_LOSE);
        tilesGroup->deactivate();
        return true;
    case Board::State::DRAW:
        turnLabel->label(strings::LABEL_DRAW);
        tilesGroup->deactivate();
        return true;
    default:
        return false;
    }
}

void MainWindow::aiMakeMove(Difficulty diff)
{
    turnLabel->label(strings::LABEL_AI_TURN);

    double randomChance{};
    switch (diff)
    {
    case Difficulty::Easy:
        randomChance = 0.75;
        break;
    case Difficulty::Medium:
        randomChance = 0.50;
        break;
    case Difficulty::Hard:
        randomChance = 0.30;
        break;
    case Difficulty::Unbeatable:
        randomChance = 0;
        break;
    }

    std::random_device dev{};
    std::mt19937 gen{dev()};

    std::uniform_real_distribution<double> doubleDist(0, 1);
    double rand{doubleDist(gen)};

    gAppMutex.lock();

    if (rand < randomChance)
    {
        std::vector<int> emptyTiles{};
        for (int i{0}; i < gApp.board.BOARD_TILES; i++)
            if (gApp.board.getMark(i) == Board::Mark::NONE)
                emptyTiles.push_back(i);

        std::uniform_int_distribution<int> intDist(0, 10);
        moveCb(emptyTiles[intDist(gen) % emptyTiles.size()]);

        gAppMutex.unlock();
        return;
    }

    gApp.aiThinking = true;

    // clang-format off
    std::thread aiThread([&]() {
        int index{};

        try
        {
            index = gApp.minimax.nextMove(gApp.aiThinking);
        }
        catch (Minimax::CancelledException const &)
        {
            index = AI_CANCEL;
        }
        catch (...)
        {
            index = AI_FAIL;
        }

        Fl::awake([](void *d) {
            AiThreadData *data{static_cast<AiThreadData *>(d)};
            data->win->moveCb(data->index);

            gApp.aiThinking = false;
            gAppMutex.unlock();

            delete data;
        }, new AiThreadData(index, this));
    });
    // clang-format on

    aiThread.detach();
}

void MainWindow::moveCb(int index)
{
    if (index == AI_CANCEL)
        return;

    if (index == AI_FAIL)
        throw std::logic_error(strings::EXCEPT_AI_FAIL);

    gApp.board.mark(index);

    drawBoard(gApp.board);

    if (!checkGameOver(gApp.board))
        turnLabel->label(strings::LABEL_YOUR_TURN);
}