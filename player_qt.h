#pragma once

#include "player.h"
#include "viewer.h"
#include <QObject>

class PlayerQt : public QObject, public Player
{
    Q_OBJECT
    public:
        PlayerQt(const Board& board, Viewer* viewer);

        void update(const BoardState& state) override;
        Move getMove() override;

    protected:
        Viewer* viewer;
};

