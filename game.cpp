#include "game.h"

#include <cassert>
#include <iostream>

static const std::vector<std::string> colors = {"white", "black"};

int run_game_loop(const Board& board, BoardState& state, Player* player0, Player* player1, Status* status)
{
    using std::cout;
    using std::endl;

    const int nplayers = board.getNumberOfPlayers();

    assert( player0 );
    assert( player1 );
    assert( &board == &player0->board );
    assert( &board == &player1->board );
    assert( nplayers == 2 );

    player0->player = 0;
    player1->player = 1;
    std::vector<Player*> players = {player0, player1};

    cout << "hash " << std::hex << hash_value(state) << std::dec << endl;
    for (Player* player : players) player->update(state);
    if (status) status->updateState(state);

    int player_current = state.count%nplayers;
    int player_winner = state.getWinner();
    while (player_winner < 0)
    {
        cout << "loop for p" << player_current << endl;
        if (status) {
            std::stringstream ss;
            ss << "requesting move for " << colors[player_current];
            status->updateMessage(ss.str());
        }
        const Move move = players[player_current]->getMove();

        cout << "playing move" << endl;
        if (!state.playMove(move))
        {
            cout << "bad move" << endl;
            if (status)
            {
                std::stringstream ss;
                ss << "bad move for " << colors[player_current];
                status->updateMessage(ss.str());
            }
            player_winner = 1-player_current;
            break;
        }

        cout << "hash " << std::hex << hash_value(state) << std::dec << endl;
        for (Player* player : players) player->update(state);
        if (status) status->updateState(state);

        player_winner = state.getWinner();

        player_current++;
        player_current %= nplayers;
    }

    cout << "finished" << endl;
    cout << "p" << player_winner << " wins!!" << endl;
    if (status)
    {
        std::stringstream ss;
        ss << colors[player_winner] << " wins!!" << endl;
        status->updateMessage(ss.str());
    }

    return player_winner;
}

