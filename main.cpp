/*
#include "hashed.h"
#include <boost/unordered_set.hpp>

struct A
{
    int x;
    float y;

    bool operator==(const A& other) const
    {
        return x == other.x && y == other.y;
    };
};

size_t
hash_value(const A& a)
{
    size_t hash = 0xaf058c2e;
    boost::hash_combine(hash, a.x);
    boost::hash_combine(hash, a.y);
    return hash;
}
*/

#include "viewer.h"
#include "player_qt.h"
#include "game.h"
#include <iostream>
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Board board(4);
    cout << lemon::countNodes(board.graph) << "/" << lemon::countEdges(board.graph) << endl;

    QWidget* main = new QWidget();

    Viewer *viewer = new Viewer(board);

    PlayerQt *player0 = new PlayerQt(board, viewer);
    PlayerQt *player1 = new PlayerQt(board, viewer);
    GameLoop *game = new GameLoop(player0, player1, viewer);

    QObject::connect(game, SIGNAL(updateState(const BoardState*)), viewer, SLOT(displayState(const BoardState*)));

    QLabel* label = new QLabel();

    QObject::connect(game, SIGNAL(statusChanged(const QString&)), label, SLOT(setText(const QString&)));

    {
        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget(viewer);
        layout->addWidget(label);
        main->setLayout(layout);
    }

    main->resize(800, 600);
    main->show();
    game->start();

    return app.exec();
}

