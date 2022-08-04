#ifndef TIC_WIN_MAIN_H
#define TIC_WIN_MAIN_H

#include "view/main.h"

#include <Fl/Fl_Widget.H>

#include "board.h"

class MainWindow : public MainWindowView
{
private:
    enum UiState
    {
        IDLE,
        INGAME
    };

    UiState state{IDLE};

    Player starter{Human};
    Difficulty diff{Easy};

    virtual void tilePressCb(Fl_Widget *w, long d);
    virtual void turnSelectionCb(Fl_Widget *w, long d);
    virtual void diffChoiceCb(Fl_Widget *w, long d);
    virtual void startButtonCb(Fl_Widget *w, void *d);

    void setTilesCb();

    void drawBoard(const Board &board);

    void setUiState(UiState state);

    bool checkGameOver(const Board &board);

    void aiMakeMove(Difficulty diff);

    void moveCb(int index);

public:
    MainWindow();
};

#endif