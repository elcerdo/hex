#include "game.h"
#include "player_uct.h"

int main(int argc, char* argv[])
{
    const Board board(5);
    PlayerUct player0(board, 1, 5, 4, 123456789);
    PlayerUct player1(board, 1, 5, 4, 123456789);
    BoardState state(board);

    const int winner = run_game_loop(board, state, &player0, &player1, NULL);

    return 0;
}

