#include <iostream>
#include <stdexcept>
#include <random>
#include <vector>
#include <thread>
#include <atomic>

#include <Fl/Fl.H>
#include <Fl/Fl_Widget.H>
#include <Fl/fl_ask.H>

#include "ui/main.h"

#include "strings.h"
#include "minimax.h"
#include "board.h"

static const char FLTK_SCHEME[] = "gleam";

class TicApp
{
private:
    enum Difficulty
    {
        EASY,
        MEDIUM,
        HARD,
        UNBEATABLE
    };

    MainUi ui{};

    Board board{};
    Minimax minimax{board};

    Board::Mark playerMark{Board::Mark::X};
    Difficulty difficulty;

    bool gameRunning{false};

    std::atomic<bool> aiThinking{false};

    struct TileData
    {
        TicApp *app{};
        int index{};

        TileData(TicApp *app, int index)
            : app(app), index(index){};
    };

    struct AiThreadData
    {
        TicApp *app{};
        int index;

        AiThreadData(TicApp *app, int index)
            : app(app), index(index){};
    };

public:
    TicApp()
    {
        setCallbacks();
        populateDiffChoice();

        ui.tilesGroup->deactivate();

        ui.show();
    }

    ~TicApp()
    {
        cleanupCallbacks();
    }

private:
    void startButtonCb()
    {
        if (!gameRunning)
        {
            gameRunning = true;

            ui.tilesGroup->activate();
            ui.startButton->label(strings::BUTTON_RESTART);
            ui.playerGroup->deactivate();
            ui.diffChoice->deactivate();

            // Computer starts first
            if (playerMark == Board::Mark::O)
                aiMakeMove(difficulty);
            else
                ui.turnLabel->label(strings::LABEL_YOUR_TURN);
        }
        else
        {
            gameRunning = false;

            // Stop AI thread
            aiThinking = false;

            board.clear();
            redrawBoard();

            ui.turnLabel->label(nullptr);
            ui.tilesGroup->deactivate();
            ui.startButton->label(strings::BUTTON_START);
            ui.playerGroup->activate();
            ui.diffChoice->activate();
        }
    }

    void diffChoiceCb(Difficulty diff)
    {
        difficulty = diff;
    }

    void markRadioCb()
    {
        playerMark = (playerMark == Board::Mark::X) ? Board::Mark::O : Board::Mark::X;
    }

    void tileCb(int index)
    {
        // Prevent player from interacting with the board while AI is running
        if (aiThinking)
            return;

        Board::State state{board.getState()};
        if (state != Board::State::INCOMPLETE)
            return;

        if (!board.mark(index))
            return;
        redrawBoard();

        if (checkGameOver())
            return;

        aiMakeMove(difficulty);
    }

    void aiThreadCb(int index)
    {
        if (index < 0)
        {
            throw std::logic_error(strings::EXCEPT_AI_FAIL);
        }

        board.mark(index);
        redrawBoard();

        if (!checkGameOver())
            ui.turnLabel->label(strings::LABEL_YOUR_TURN);
    };

    void aiMakeMove(Difficulty diff)
    {
        ui.turnLabel->label(strings::LABEL_AI_TURN);

        double randomChance{};
        switch (diff)
        {
        case Difficulty::EASY:
            randomChance = 0.75;
            break;
        case Difficulty::MEDIUM:
            randomChance = 0.50;
            break;
        case Difficulty::HARD:
            randomChance = 0.30;
            break;
        case Difficulty::UNBEATABLE:
            randomChance = 0;
            break;
        }

        std::random_device dev{};
        std::mt19937 gen{dev()};

        std::uniform_real_distribution<double> doubleDist(0, 1);
        double rand{doubleDist(gen)};
        if (rand < randomChance)
        {
            std::vector<int> emptyTiles{};
            for (int i{0}; i < board.BOARD_TILES; i++)
                if (board.getMark(i) == Board::Mark::NONE)
                    emptyTiles.push_back(i);

            std::uniform_int_distribution<int> intDist(0, 10);
            aiThreadCb(emptyTiles[intDist(gen) % emptyTiles.size()]);
        }
        else
        {
            aiThinking = true;
            std::thread aiThread([&]()
                                 {
                int index{};
                try
                {
                    index = minimax.nextMove(aiThinking);
                }
                catch (Minimax::CancelledException const&)
                {
                    return;
                }
                catch (...)
                {
                    index = -1;
                }

                Fl::awake([](void *d){
                    AiThreadData *data = (AiThreadData *)d;
                    data->app->aiThreadCb(data->index);
                    data->app->aiThinking = false;

                    delete data;
                }, new AiThreadData(this, index)); });
            aiThread.detach();
        }
    }

    bool checkGameOver()
    {
        switch (board.getState())
        {
        case Board::State::X_WINNER:
            ui.turnLabel->label((playerMark == Board::Mark::X) ? strings::LABEL_WIN : strings::LABEL_LOSE);
            ui.tilesGroup->deactivate();
            return true;
        case Board::State::O_WINNER:
            ui.turnLabel->label((playerMark == Board::Mark::O) ? strings::LABEL_WIN : strings::LABEL_LOSE);
            ui.tilesGroup->deactivate();
            return true;
        case Board::State::DRAW:
            ui.turnLabel->label(strings::LABEL_DRAW);
            ui.tilesGroup->deactivate();
            return true;
        default:
            return false;
        }
    }

    void populateDiffChoice()
    {
        ui.diffChoice->add(
            strings::MENU_DIFF_EASY, 0, [](Fl_Widget *, void *d)
            { ((TicApp *)d)->diffChoiceCb(Difficulty::EASY); },
            this);
        ui.diffChoice->add(
            strings::MENU_DIFF_MEDIUM, 0, [](Fl_Widget *, void *d)
            { ((TicApp *)d)->diffChoiceCb(Difficulty::MEDIUM); },
            this);
        ui.diffChoice->add(
            strings::MENU_DIFF_HARD, 0, [](Fl_Widget *, void *d)
            { ((TicApp *)d)->diffChoiceCb(Difficulty::HARD); },
            this);
        int index = ui.diffChoice->add(
            strings::MENU_DIFF_UNBEATABLE, 0, [](Fl_Widget *, void *d)
            { ((TicApp *)d)->diffChoiceCb(Difficulty::UNBEATABLE); },
            this);

        // Select "Unbeatable" difficulty by default
        ui.diffChoice->picked(&ui.diffChoice->menu()[index]);
    }

    void cleanupCallbacks()
    {
        Fl_Widget *const *tiles{ui.tilesGroup->array()};
        int count{ui.tilesGroup->children()};
        for (int i{0}; i < count; i++)
        {
            Fl_Widget *tile{tiles[i]};

            tile->callback((Fl_Callback *)nullptr);
            TileData *data = (TileData *)tile->user_data();
            delete data;
        }
    }

    void setCallbacks()
    {
        // Set callback for startButton
        ui.startButton->callback([](Fl_Widget *widget, void *d)
                                 { ((TicApp *)d)->startButtonCb(); },
                                 this);

        // Set callback for tiles
        Fl_Widget *const *tiles{ui.tilesGroup->array()};
        int count{ui.tilesGroup->children()};
        for (int i{0}; i < count; i++)
        {
            tiles[i]->callback([](Fl_Widget *w, void *d)
                               {
                TileData *data = (TileData *)d;
                data->app->tileCb(data->index); },
                               new TileData(this, i));
        }

        // Set callback for player mark radio buttons
        auto cb = [](Fl_Widget *w, void *d)
        {
            ((TicApp *)d)->markRadioCb();
        };
        ui.xRadioButton->callback(cb, this);
        ui.oRadioButton->callback(cb, this);
    }

    void redrawBoard()
    {
        Fl_Widget *const *tiles{ui.tilesGroup->array()};
        int count{ui.tilesGroup->children()};

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
};

int main()
{
    TicApp app{};

    // Set theme
    Fl::scheme(FLTK_SCHEME);
    // Enable FLTK multithreading support
    Fl::lock();

    // Run FLTK event loop
    return Fl::run();
}