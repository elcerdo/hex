#pragma once

#include "game.h"
#include <QThread>

class GameLoop : public QThread, public Status
{
    Q_OBJECT
    public:
        GameLoop(Player* player0, Player* player1, QObject* parent=NULL);
        ~GameLoop();

        void updateState(const BoardState& state) override;
        void updateMessage(const std::string& message) override;

    signals:
        void stateUpdated(const BoardState* state);
        void messageUpdated(const QString& message);

    protected:
        void run();

    protected:
        const Board& board;
        Player* player0;
        Player* player1;
        BoardState* state;
};

