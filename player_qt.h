#pragma once

#include "player.h"
#include "viewer.h"
#include <QObject>
#include <QMutex>
#include <QWaitCondition>

class PlayerQt : public QObject, public Player
{
    Q_OBJECT
    public:
        PlayerQt(const Board& board, Viewer* viewer);

        void update(const BoardState& state) override;
        Move getMove() override;

    public slots:
        void gotPlayerMove(const int player, const Move& move);

    signals:
        void requestPlayerMove(const int player);

    protected:
        QMutex answer_mutex;
        QWaitCondition answer_condition;
        Move move;
};

