#pragma once

#include "player.h"
#include <QThread>

class GameLoop : public QThread
{
    Q_OBJECT
    public:
        GameLoop(Player* player0, Player* player1, QObject* parent=NULL);
        ~GameLoop();

    signals:
        void updateState(const BoardState* state);
        void statusChanged(const QString& message);

    protected:
        void run();

    protected:
        const Board& board;
        Player* player0;
        Player* player1;
        BoardState* state;
};
