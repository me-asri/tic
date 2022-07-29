#ifndef TIC_BOARD_H
#define TIC_BOARD_H

class Board
{
public:
    static constexpr int BOARD_SIDES{3};
    static constexpr int BOARD_TILES{BOARD_SIDES * BOARD_SIDES};

    enum Mark
    {
        NONE = 0,
        X = 1,
        O = -1
    };

    enum State
    {
        INCOMPLETE,
        X_WINNER,
        O_WINNER,
        DRAW
    };

private:
    Mark *board{};
    Mark currentTurn{Mark::X};

    Mark getWinner() const;
    bool canMark() const;

public:
    Board();
    Board(const Board &b);
    Board(Board &&b);

    ~Board();

    Mark getMark(int index) const;
    Mark getMark(int i, int j) const;

    bool mark(int index);
    bool mark(int i, int j);

    Mark getTurn() const;

    void clear();

    State getState() const;
};

#endif