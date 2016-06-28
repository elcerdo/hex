#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/errors.hpp>
#include <iostream>

struct Options
{
    int board_size;
    std::string player0_name;
    std::string player1_name;
    double uct_constant;
    double uct_time;
};

Options
parse_options(int argc, char* argv[])
{
    namespace po = boost::program_options;
    using std::cout;
    using std::cerr;
    using std::endl;

    Options options;
    po::options_description po_options("dgtal_ambrosi [options]");
    po_options.add_options()
        ("help,h", "display this message")
        ("size,s", po::value<int>(&options.board_size)->default_value(11))
        ("white,w", po::value<std::string>(&options.player0_name)->default_value("human"))
        ("black,b", po::value<std::string>(&options.player1_name)->default_value("human"))
        ("uct-constant,c", po::value<double>(&options.uct_constant)->default_value(.5))
        ("uct-time,t", po::value<double>(&options.uct_time)->default_value(3));

    try
    {
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(po_options).run(), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            cout << po_options;
            std::exit(0);
        }
    }
    catch (std::exception& ex)
    {
        cerr << ex.what() << endl;
        cout << po_options;
        std::exit(1);
    }

    return options;

}

#include "viewer.h"
#include "player_qt.h"
#include "player_random.h"
#include "player_uct.h"
#include "game.h"
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>
#include <chrono>

int main(int argc, char* argv[])
{
    using std::cout;
    using std::cerr;
    using std::endl;

    const Options options = parse_options(argc, argv);

    QApplication app(argc, argv);

    cout << "board_size " << options.board_size << endl;
    Board board(options.board_size);
    cout << "graph " << lemon::countNodes(board.graph) << "/" << lemon::countEdges(board.graph) << endl;

    QWidget* main = new QWidget();

    Viewer *viewer = new Viewer(board);

    const std::function<Player*(const std::string&)> dispatch_player = [&options, &viewer, &board](const std::string& name)
    {
        if (name == "human") return static_cast<Player*>(new PlayerQt(board, viewer));
        if (name == "random") return static_cast<Player*>(new PlayerRandom(board, std::chrono::system_clock::now().time_since_epoch().count()));
        if (name == "uct") return static_cast<Player*>(new PlayerUct(board, options.uct_constant, options.uct_time, std::chrono::system_clock::now().time_since_epoch().count()));
        cerr << "bad player name '" << name << "'" << endl;
        std::exit(1);
        return static_cast<Player*>(NULL);
    };

    Player *player0 = dispatch_player(options.player0_name);
    Player *player1 = dispatch_player(options.player1_name);
    GameLoop *game = new GameLoop(player0, player1, viewer);

    QObject::connect(game, SIGNAL(updateState(const BoardState*)), viewer, SLOT(displayState(const BoardState*)));

    QLabel* status = new QLabel();

    QObject::connect(game, SIGNAL(statusChanged(const QString&)), status, SLOT(setText(const QString&)));

    QLabel* title = new QLabel();

    title->setText(QString("%1 versus %2").arg(QString::fromStdString(player0->name)).arg(QString::fromStdString(player1->name)));

    {
        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget(title);
        layout->addWidget(viewer);
        layout->addWidget(status);
        main->setLayout(layout);
    }

    main->resize(800, 600);
    main->show();
    game->start();

    return app.exec();
}

