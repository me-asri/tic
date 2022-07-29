#ifndef TIC_STRINGS_H
#define TIC_STRINGS_H

namespace strings
{
    inline constexpr char LABEL_X[] = "X";
    inline constexpr char LABEL_O[] = "O";
    inline constexpr char LABEL_LOSE[] = "You lose!";
    inline constexpr char LABEL_WIN[] = "You win!";
    inline constexpr char LABEL_DRAW[] = "It's a draw.";
    inline constexpr char LABEL_YOUR_TURN[] = "Your turn";
    inline constexpr char LABEL_AI_TURN[] = "Computer is thinking...";

    inline constexpr char BUTTON_START[] = "Start game";
    inline constexpr char BUTTON_RESTART[] = "Restart";

    inline constexpr char MENU_DIFF_EASY[] = "Easy";
    inline constexpr char MENU_DIFF_MEDIUM[] = "Medium";
    inline constexpr char MENU_DIFF_HARD[] = "Hard";
    inline constexpr char MENU_DIFF_UNBEATABLE[] = "Unbeatable";

    inline constexpr char EXCEPT_AI_FAIL[] = "Minmax failed";
    inline constexpr char EXCEPT_CANCEL[] = "Operation cancelled";
};

#endif