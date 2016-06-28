#include "graph_uct.h"

#include <fstream>
#include <iomanip>
#include <lemon/connectivity.h>

GraphData::GraphData(const double uct_constant) :
    uct_constant(uct_constant),
    graph(),
    node_uct_datas(graph),
    node_state_hashes(graph),
    node_available_moves(graph),
    arc_moves(graph),
    states_cache(),
    nodes_cache()
{
}

void
GraphData::prune_below(const Node& node)
{
    assert( graph.valid(node) );

    typedef Graph::NodeMap<bool> NodeFilter;
    NodeFilter filter(graph, false);

    lemon::bfs(graph).reachedMap(filter).run(node);

    typedef lemon::FilterNodes<const Graph> BelowNodeGraph;
    const BelowNodeGraph below_node_graph(graph, filter);

    //std::cout << "all " << lemon::countNodes(graph) << " " << lemon::countArcs(graph) << std::endl;
    //std::cout << "below node " << lemon::countNodes(below_node_graph) << " " << lemon::countArcs(below_node_graph) << std::endl;

    GraphData graphdata_new(uct_constant);
    lemon::DigraphCopy<BelowNodeGraph, Graph> copy_below(below_node_graph, graphdata_new.graph);
    copy_below.nodeMap(node_uct_datas, graphdata_new.node_uct_datas);
    copy_below.nodeMap(node_state_hashes, graphdata_new.node_state_hashes);
    copy_below.nodeMap(node_available_moves, graphdata_new.node_available_moves);
    copy_below.arcMap(arc_moves, graphdata_new.arc_moves);
    copy_below.run();

    graphdata_new.states_cache = states_cache;

    graph.clear();
    lemon::DigraphCopy<Graph, Graph> copy_back(graphdata_new.graph, graph);
    copy_back.nodeMap(graphdata_new.node_uct_datas, node_uct_datas);
    copy_back.nodeMap(graphdata_new.node_state_hashes, node_state_hashes);
    copy_back.nodeMap(graphdata_new.node_available_moves, node_available_moves);
    copy_back.arcMap(graphdata_new.arc_moves, arc_moves);
    copy_back.run();

    states_cache.clear();
    nodes_cache.clear();
    for (NodeIt ni(graph); ni!=lemon::INVALID; ++ni)
    {
        const Hash& state_hash = node_state_hashes[ni];
        nodes_cache.insert(std::make_pair(state_hash, ni));
        assert( graphdata_new.states_cache.find(state_hash) != graphdata_new.states_cache.end() );
        states_cache.insert(*graphdata_new.states_cache.find(state_hash));
    }
}

template <typename Pair>
bool
less_pair_first(const Pair& aa, const Pair& bb)
{
    return aa.first < bb.first;
}

bool
GraphData::contains(const HashedPair<BoardState>& hashed_state) const
{
    return states_cache.find(hashed_state.hash) != states_cache.end();
}

bool
GraphData::valid(const Node& node) const
{
    return graph.valid(node);
}

size_t
GraphData::available_moves(const Node& node) const
{
    assert( graph.valid(node) );
    return node_available_moves[node].size();
}

GraphData::Node
GraphData::get_node(const HashedPair<BoardState>& hashed_pair) const
{
    assert( contains(hashed_pair) );
    return nodes_cache.find(hashed_pair.hash)->second;
}

GraphData::Node
GraphData::create_node(const HashedPair<BoardState>& hashed_state, RandomEngine& re)
{
    //assert( !hashed_state.value.is_finished() );
    assert( !contains(hashed_state) );
    const Node& node = graph.addNode();

    states_cache.insert(std::make_pair(hashed_state.hash, hashed_state.value));
    nodes_cache.insert(std::make_pair(hashed_state.hash, node));

    UctData uct_data;
    uct_data.parent_player = (hashed_state.value.count+1) % 2;
    uct_data.parent_score = 0;
    uct_data.count = 0;
    node_uct_datas[node] = uct_data;

    node_state_hashes[node] = hashed_state.hash;

    Moves available_moves = hashed_state.value.getAvailableMoves();
    std::random_shuffle(available_moves.begin(), available_moves.end(), [&re](const size_t size){ return re()%size; });
    node_available_moves[node] = available_moves;

    return node;
}

std::pair<bool,GraphData::Node>
GraphData::get_or_create_node(const HashedPair<BoardState>& hashed_state, RandomEngine& re)
{
    const NodesCache::const_iterator& nodes_cache_iter = nodes_cache.find(hashed_state.hash);

    if (nodes_cache_iter != nodes_cache.end()) return std::make_pair(false, nodes_cache_iter->second);

    return std::make_pair(true, create_node(hashed_state, re));
}

GraphData::Arc
GraphData::create_arc(const Node& from_node, const Node& to_node, const Move& move)
{
    const Arc& arc = graph.addArc(from_node, to_node);
    assert( lemon::dag(graph) ); // directed acyclic graph

    arc_moves[arc] = move;

    assert( std::none_of(
        node_available_moves[from_node].begin(),
        node_available_moves[from_node].end(),
        [&move](const Move& available_move) { return available_move == move; }) );

    return arc;
}

std::pair<bool, GraphData::Node>
GraphData::get_or_create_child(BoardState& state, const Move& move, RandomEngine& re)
{
    const Node& root = get_node(make_hashed_pair(state));

    bool valid = state.playMove(move);
    assert( valid );

    {  // already inserted
        OutArcIt oai(graph, root);
        while (oai != lemon::INVALID && arc_moves[oai] != move) ++oai;
        if (oai != lemon::INVALID )
        {
            assert( oai != lemon::INVALID );
            assert( arc_moves[oai] == move );
            return std::make_pair(false, graph.target(oai));
        }
    }

    // create or get child and add create new arc
    const std::pair<bool,Node>& child_pair = get_or_create_node(make_hashed_pair(state), re);
    create_arc(root, child_pair.second, move);

    assert( hash_value(state) == hash_value(get_state(child_pair.second)) );

    return child_pair;
}

const BoardState&
GraphData::get_state(const Node& node) const
{
    assert( graph.valid(node) );
    const Hash& state_hash = node_state_hashes[node];
    assert( states_cache.find(state_hash) != states_cache.end() );
    return states_cache.find(state_hash)->second;
}

const GraphData::UctData&
GraphData::get_uct_data(const Node& node) const
{
    assert( graph.valid(node) );
    return node_uct_datas[node];
}

GraphData::Node
GraphData::get_best_child(const Node& parent, RandomEngine& re) const
{
    assert( available_moves(parent) == 0 );

    typedef std::pair<UctData, Node> UctDataNodePair;
    typedef std::vector<UctDataNodePair> ChildrenUctDatas;
    ChildrenUctDatas children_uct_datas;
    int total = 0;

    for (GraphData::OutArcIt oai(graph, parent); oai!=lemon::INVALID; ++oai)
    {
        const GraphData::Node& child = graph.target(oai);
        const GraphData::UctData& child_uct_data = node_uct_datas[child];

        assert( child_uct_data.count > 0 );
        total += child_uct_data.count;
        children_uct_datas.push_back(std::make_pair(child_uct_data, child));
    }

    if (children_uct_datas.empty())
        return lemon::INVALID;

    assert( !children_uct_datas.empty() );
    assert( total >= node_uct_datas[parent].count - 1 );
    std::random_shuffle(children_uct_datas.begin(), children_uct_datas.end(), [&re](const size_t size) { return re()%size; });

    const double factor = uct_constant*sqrt(2*log(total));

    const std::function<double(const UctData&)> score = [&factor](const UctData& data)
        { return factor/sqrt(data.count) + static_cast<double>(data.parent_score)/data.count; };

    return std::max_element(children_uct_datas.begin(), children_uct_datas.end(),
        [&score](const UctDataNodePair& aa, const UctDataNodePair& bb) { return score(aa.first) < score(bb.first); })->second;
}

struct ScoreComputerDirection
{
    bool
    operator()(const GraphData::UctDataArcPair& pair_aa, const GraphData::UctDataArcPair& pair_bb) const
    {
        assert( pair_aa.first.count != 0 );
        assert( pair_bb.first.count != 0 );

        const double& score_aa = pair_aa.first.parent_score/pair_aa.first.count;
        const double& score_bb = pair_bb.first.parent_score/pair_bb.first.count;

        return score_aa < score_bb;
    }
};

Move
GraphData::get_best_move(const Node& parent, RandomEngine& re) const
{
    assert( available_moves(parent) == 0 );

    typedef std::vector<UctDataArcPair> ChildrenUctDatas;
    ChildrenUctDatas children_uct_datas;
    int total = 0;

    for (GraphData::OutArcIt oai(graph, parent); oai!=lemon::INVALID; ++oai)
    {
        const GraphData::Node& child = graph.target(oai);
        const GraphData::UctData& child_uct_data = node_uct_datas[child];

        total += child_uct_data.count;
        children_uct_datas.push_back(std::make_pair(child_uct_data, oai));
    }

    //std::cout << "max scoring direction count check " <<
    //std::cout << std::hex << node_state_hashes[parent] << std::dec << " ";
    //std::cout << total << " " << node_uct_datas[parent].count << std::endl;
    assert( total >= node_uct_datas[parent].count - 1 );

    //FIXME not useful??
    std::random_shuffle(children_uct_datas.begin(), children_uct_datas.end(), [&re](const size_t size){ return re()%size; });
    const UctDataArcPair& max_pair = *std::max_element(children_uct_datas.begin(), children_uct_datas.end(), ScoreComputerDirection());

    return arc_moves[max_pair.second];
}

Move
GraphData::pop_random_available_move(const Node& node)
{
    assert( available_moves(node) > 0 );
    Moves& moves = node_available_moves[node];
    const Move move = moves.back();
    moves.pop_back();
    return move;
}

void
GraphData::back_propagate_winner(const Nodes& nodes, const BoardState& state)
{
    const int winner = state.getWinner();
    assert( winner >= 0 );

    for (Nodes::const_reverse_iterator ni=nodes.rbegin(), nie=nodes.rend(); ni!=nie; ni++)
    {
        const GraphData::Node& node = *ni;
        UctData& uct_data = node_uct_datas[node];
        uct_data.count++;

        assert( uct_data.parent_player >= 0 );
        if (uct_data.parent_player == winner) uct_data.parent_score++;
    }
}

void
GraphData::print_from_root(std::ostream& os, const Node& root, const std::string& indent, const int& max_depth) const
{
    os << std::hex;
    os << indent;
    print_from_root_internal(os, root, indent, max_depth);
    os << std::dec;
}

void
GraphData::print_from_root_internal(std::ostream& os, const Node& root, const std::string& indent, const int& max_depth) const
{
    const UctData& root_uct_data = node_uct_datas[root];
    const BoardState& state = get_state(root);

    os << node_state_hashes[root];
    os << " p" << ((root_uct_data.parent_player+1) % 2);
    os << " " << available_moves(root);
    os << std::endl;

    if (max_depth==0) return;

    for (OutArcIt oai(graph, root); oai!=lemon::INVALID; ++oai)
    {
        const bool& last_arc = (++OutArcIt(oai) == lemon::INVALID);

        const Move& move = arc_moves[oai];
        const Board::Coord& coord = state.board.coords[move];
        std::stringstream move_ss;
        move_ss << coord.first << "x" << coord.second;

        const UctData& child_uct_data = node_uct_datas[graph.target(oai)];
        assert( (root_uct_data.parent_player+1) % 2 == child_uct_data.parent_player );
        const double& proba = 100*static_cast<double>(child_uct_data.parent_score)/child_uct_data.count;

        std::stringstream new_indent_first_stream;
        new_indent_first_stream << std::fixed << std::setprecision(0) << indent;
        if (!last_arc) new_indent_first_stream << "├─";
        else new_indent_first_stream << "└─";
        new_indent_first_stream << move_ss.str() << "─";
        new_indent_first_stream << "(" << proba << "%)─<" << child_uct_data.count << ">─";

        std::stringstream new_indent_stream;
        new_indent_stream << indent;
        if (!last_arc) new_indent_stream << "| ";
        else new_indent_stream << "  ";
        while (new_indent_stream.str().size() != new_indent_stream.str().size())
            new_indent_stream << " ";

        os << new_indent_first_stream.str();
        print_from_root_internal(os, graph.target(oai), new_indent_stream.str(), max_depth-1);
    }
}

void
GraphData::get_sizes(int& node_count, int& arc_count, int& parallel_arc_count) const
{
    node_count = lemon::countNodes(graph);
    arc_count = lemon::countArcs(graph);
    parallel_arc_count = arc_count-node_count+1;
}

std::ostream&
operator<<(std::ostream& os, const GraphData& graph_data)
{
    const size_t& node_count = lemon::countNodes(graph_data.graph);
    const size_t& arc_count = lemon::countArcs(graph_data.graph);

    os << "<GraphData";
    os << " c=" << graph_data.uct_constant;
    os << " n=" << node_count << "/" << graph_data.states_cache.size();
    os << " e=" << arc_count;
    os << " pe=" << arc_count-node_count+1;
#if !defined(NDEBUG)
    if (!lemon::parallelFree(graph_data.graph)) os << " PARALLEL";
    if (lemon::dag(graph_data.graph)) os << " DAG";
    if (lemon::tree(lemon::undirector(graph_data.graph))) os << " TREE";
#endif
    os << ">";

    assert( node_count == graph_data.states_cache.size() );
    assert( node_count == graph_data.nodes_cache.size() );

    return os;
}

