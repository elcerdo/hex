#include "graph.h"

#include <fstream>
#include <iomanip>
#include <lemon/connectivity.h>

GraphData::GraphData(const double uct_constant) :
    uct_constant(uct_constant),
    graph(),
    node_uct_datas(graph),
    node_state_hashes(graph),
    node_arc_constructed_flags(graph),
    arc_direction_ints(graph),
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
    copy_below.nodeMap(node_arc_constructed_flags, graphdata_new.node_arc_constructed_flags);
    copy_below.arcMap(arc_direction_ints, graphdata_new.arc_direction_ints);
    copy_below.run();

    graphdata_new.states_cache = states_cache;

    graph.clear();
    lemon::DigraphCopy<Graph, Graph> copy_back(graphdata_new.graph, graph);
    copy_back.nodeMap(graphdata_new.node_uct_datas, node_uct_datas);
    copy_back.nodeMap(graphdata_new.node_state_hashes, node_state_hashes);
    copy_back.nodeMap(graphdata_new.node_arc_constructed_flags, node_arc_constructed_flags);
    copy_back.arcMap(graphdata_new.arc_direction_ints, arc_direction_ints);
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

void
merge_graph_data(GraphData& graph_data_a, GraphData& graph_data_b)
{
    GraphData::StatesCache final_states_cache;
    std::merge(
            graph_data_a.states_cache.begin(), graph_data_a.states_cache.end(),
            graph_data_b.states_cache.begin(), graph_data_b.states_cache.end(),
            std::inserter(final_states_cache, final_states_cache.begin()),
            less_pair_first<GraphData::StatesCache::value_type>);

    for (GraphData::StatesCache::const_iterator fsci=final_states_cache.begin(), fsce=final_states_cache.end(); fsci!=fsce; fsci++)
    {
        const GraphData::NodesCache::const_iterator nci_a = graph_data_a.nodes_cache.find(fsci->first);
        const GraphData::NodesCache::const_iterator nci_b = graph_data_b.nodes_cache.find(fsci->first);
        const bool state_in_a = (nci_a != graph_data_a.nodes_cache.end());
        const bool state_in_b = (nci_b != graph_data_b.nodes_cache.end());

        //std::cout << "  merge " << state_in_a << " " << state_in_b << std::endl;
        assert( state_in_a || state_in_b );
    }
}

bool
GraphData::contains(const HashedPair<State>& hashed_state) const
{
    return states_cache.find(hashed_state.hash) != states_cache.end();
}

bool
GraphData::is_leaf(const Node& node) const
{
    assert( graph.valid(node) );

    const size_t& out_arcs_count = lemon::countOutArcs(graph, node);

#if !defined(NDEBUG)
    const ArcConstructedFlags& arc_constructed_flags = node_arc_constructed_flags[node];
    size_t foo = 0;
    for (ArcConstructedFlags::const_iterator ai=arc_constructed_flags.begin(), aie=arc_constructed_flags.end(); ai!=aie; ai++)
        if (*ai)
            foo++;
    assert( foo == out_arcs_count );
#endif

    return out_arcs_count != 5;
}

GraphData::Node
GraphData::get_node(const HashedPair<State>& hashed_pair) const
{
    assert( contains(hashed_pair) );
    return nodes_cache.find(hashed_pair.hash)->second;
}

GraphData::Node
GraphData::create_node(const HashedPair<State>& hashed_state)
{
    //assert( !hashed_state.value.is_finished() );
    assert( !contains(hashed_state) );
    const Node& node = graph.addNode();

    states_cache.insert(std::make_pair(hashed_state.hash, hashed_state.value));
    nodes_cache.insert(std::make_pair(hashed_state.hash, node));

    UctData uct_data;
    uct_data.parent_hero_int = (hashed_state.value.next_hero_index+3) % 4;
    uct_data.score_for_parent = 0;
    uct_data.count = 0;
    node_uct_datas[node] = uct_data;

    node_state_hashes[node] = hashed_state.hash;

    ArcConstructedFlags arc_constructed_flags;
    std::fill(arc_constructed_flags.begin(), arc_constructed_flags.end(), false);
    node_arc_constructed_flags[node] = arc_constructed_flags;

    return node;
}

std::pair<bool,GraphData::Node>
GraphData::get_or_create_node(const HashedPair<State>& hashed_state)
{
    const NodesCache::const_iterator& nodes_cache_iter = nodes_cache.find(hashed_state.hash);

    if (nodes_cache_iter != nodes_cache.end()) return std::make_pair(false, nodes_cache_iter->second);

    return std::make_pair(true, create_node(hashed_state));
}

GraphData::Arc
GraphData::create_arc(const Node& from_node, const Node& to_node, const Direction& direction)
{
    const int& direction_int = static_cast<int>(direction);

    const Arc& arc = graph.addArc(from_node, to_node);
    assert( lemon::dag(graph) ); // directed acyclic graph

    arc_direction_ints[arc] = direction_int;

    assert( node_arc_constructed_flags[from_node][direction_int] == false );
    node_arc_constructed_flags[from_node][direction_int] = true;

    return arc;
}

std::pair<bool, GraphData::Node>
GraphData::get_or_create_child(State& state, const Direction& direction)
{
    const int& direction_int = static_cast<int>(direction);

    const Node& root = get_node(make_hashed_pair(state));

    state.update(direction);

    {   // already inserted
        const ArcConstructedFlags& arc_constructed_flags = node_arc_constructed_flags[root];
        if (arc_constructed_flags[direction_int])
        {
            OutArcIt oai(graph, root);
            while (oai != lemon::INVALID && arc_direction_ints[oai] != direction_int)
                ++oai;
            assert( oai != lemon::INVALID );
            assert( arc_direction_ints[oai] == direction_int );
            return std::make_pair(false, graph.target(oai));
        }
    }

    // create or get child and add create new arc
    const std::pair<bool,Node>& child_pair = get_or_create_node(make_hashed_pair(state));
    create_arc(root, child_pair.second, direction);

    assert( state == get_state(child_pair.second) );

    return child_pair;
}

const State&
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

struct ScoreComputerNode
{
    ScoreComputerNode(const double& uct_constant, const int& total) :
        factor(uct_constant*sqrt(2*log(total)))
    {
    }

    bool
    operator()(const GraphData::UctDataNodePair& pair_aa, const GraphData::UctDataNodePair& pair_bb) const
    {
        if (pair_aa.first.count == 0) return false;
        if (pair_bb.first.count == 0) return true;

        const double& score_aa = factor/sqrt(pair_aa.first.count) + pair_aa.first.score_for_parent/pair_aa.first.count;
        const double& score_bb = factor/sqrt(pair_bb.first.count) + pair_bb.first.score_for_parent/pair_bb.first.count;

        return score_aa < score_bb;
    }

    const double factor;
};

GraphData::Node
GraphData::get_best_child(const Node& parent, Rng& rng) const
{
    assert( !is_leaf(parent) );

    typedef boost::array<UctDataNodePair, 5> ChildrenUctDatas;
    ChildrenUctDatas children_uct_datas;
    int total = 0;

    for (GraphData::OutArcIt oai(graph, parent); oai!=lemon::INVALID; ++oai)
    {
        const int& direction_int = arc_direction_ints[oai];
        assert( direction_int < 5 );

        const GraphData::Node& child = graph.target(oai);
        const GraphData::UctData& child_uct_data = node_uct_datas[child];

        total += child_uct_data.count;
        children_uct_datas[direction_int] = std::make_pair(child_uct_data, child);
    }

    //std::cout << "max scoring child count check " <<
    //std::cout << std::hex << node_state_hashes[parent] << std::dec << " ";
    //std::cout << total << " " << node_uct_datas[parent].count << std::endl;
    assert( total >= node_uct_datas[parent].count - 1 );

    typedef SizeRng<uint8_t> SizeRngUInt8;
    SizeRngUInt8 size_rng(rng);
    std::random_shuffle(children_uct_datas.begin(), children_uct_datas.end(), size_rng);
    const UctDataNodePair& max_pair = *std::max_element(children_uct_datas.begin(), children_uct_datas.end(), ScoreComputerNode(uct_constant, total));

    return max_pair.second;
}

struct ScoreComputerDirection
{
    bool
    operator()(const GraphData::UctDataDirectionIntPair& pair_aa, const GraphData::UctDataDirectionIntPair& pair_bb) const
    {
        assert( pair_aa.first.count != 0 );
        assert( pair_bb.first.count != 0 );

        const double& score_aa = pair_aa.first.score_for_parent/pair_aa.first.count;
        const double& score_bb = pair_bb.first.score_for_parent/pair_bb.first.count;

        return score_aa < score_bb;
    }
};

Direction
GraphData::get_best_direction(const Node& parent, Rng& rng) const
{
    assert( !is_leaf(parent) );

    typedef boost::array<UctDataDirectionIntPair, 5> ChildrenUctDatas;
    ChildrenUctDatas children_uct_datas;
    int total = 0;

    for (GraphData::OutArcIt oai(graph, parent); oai!=lemon::INVALID; ++oai)
    {
        const int& direction_int = arc_direction_ints[oai];
        assert( direction_int < 5 );

        const GraphData::Node& child = graph.target(oai);
        const GraphData::UctData& child_uct_data = node_uct_datas[child];

        total += child_uct_data.count;
        children_uct_datas[direction_int] = std::make_pair(child_uct_data, direction_int);
    }

    //std::cout << "max scoring direction count check " <<
    //std::cout << std::hex << node_state_hashes[parent] << std::dec << " ";
    //std::cout << total << " " << node_uct_datas[parent].count << std::endl;
    assert( total >= node_uct_datas[parent].count - 1 );

    //FIXME not useful??
    typedef SizeRng<uint8_t> SizeRngUInt8;
    SizeRngUInt8 size_rng(rng);
    std::random_shuffle(children_uct_datas.begin(), children_uct_datas.end(), size_rng);
    const UctDataDirectionIntPair& max_pair = *std::max_element(children_uct_datas.begin(), children_uct_datas.end(), ScoreComputerDirection());

    return static_cast<Direction>(max_pair.second);
}

Direction
GraphData::get_random_available_direction(const Node& node, Rng& rng) const
{
    assert( is_leaf(node) );

    typedef std::vector<int> AvailableDirectionInts;

    const ArcConstructedFlags& arc_constructed_flags = node_arc_constructed_flags[node];

    AvailableDirectionInts available_direction_ints;
    for (size_t kk=0; kk<arc_constructed_flags.size(); kk++)
        if (!arc_constructed_flags[kk])
            available_direction_ints.push_back(kk);

    assert( !available_direction_ints.empty() );

    typedef SizeRng<uint8_t> SizeRngUInt8;
    SizeRngUInt8 size_rng(rng);
    std::random_shuffle(available_direction_ints.begin(), available_direction_ints.end(), size_rng);

    return static_cast<Direction>(available_direction_ints[0]);
}

void
GraphData::back_propagate_winner(const Nodes& nodes, const State& state, const Game::Allies& allies)
{
    const int& winner = state.get_winner();

    int max_gold = 0;
    if (winner >= 0)
    {
        max_gold = state.heroes[winner].gold;
    }
    else
    {
        for (size_t kk=0; kk<state.heroes.size(); kk++)
            max_gold = std::max(max_gold, state.heroes[kk].gold);
    }

    const double factor = max_gold == 0 ? 0 : 1/(3*static_cast<double>(max_gold));

    for (Nodes::const_reverse_iterator ni=nodes.rbegin(), nie=nodes.rend(); ni!=nie; ni++)
    {
        const GraphData::Node& node = *ni;
        UctData& uct_data = node_uct_datas[node];
        uct_data.count++;

        assert( uct_data.parent_hero_int >= 0 );
        if (uct_data.parent_hero_int == winner) uct_data.score_for_parent++;
        else {
            uct_data.score_for_parent += state.heroes[uct_data.parent_hero_int].gold*factor;
            //const Game::HeroAllies& hero_allies = allies[uct_data.parent_hero_int];
            //if (hero_allies.find(winner) != hero_allies.end())
            //    uct_data.score_for_parent += state.heroes[winner].gold*factor;
        }
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
#if !defined(NDEBUG)
    const ArcConstructedFlags& arc_constructed_flags = node_arc_constructed_flags[root];
#endif

    const UctData& root_uct_data = node_uct_datas[root];

    os << node_state_hashes[root];
    os << " @" << ((root_uct_data.parent_hero_int+1) % 4)+1;
    if (is_leaf(root)) os << " @";
    os << std::endl;

    if (max_depth==0) return;

    for (OutArcIt oai(graph, root); oai!=lemon::INVALID; ++oai)
    {
        const bool& last_arc = (++OutArcIt(oai) == lemon::INVALID);

        const int& direction_int = arc_direction_ints[oai];
        const Direction& direction = static_cast<Direction>(direction_int);
        assert( arc_constructed_flags[direction_int] == true );

        std::string direction_string = to_string(direction);
        direction_string.resize(2);

        const UctData& child_uct_data = node_uct_datas[graph.target(oai)];
        assert( (root_uct_data.parent_hero_int+1) % 4 == child_uct_data.parent_hero_int );
        const double& mean_reward = 100*child_uct_data.score_for_parent/child_uct_data.count;

        std::stringstream new_indent_first_stream;
        new_indent_first_stream << std::fixed << std::setprecision(0) << indent;
        if (!last_arc) new_indent_first_stream << "├─";
        else new_indent_first_stream << "└─";
        new_indent_first_stream << direction_string << "─";
        new_indent_first_stream << "(" << mean_reward << "%)─<" << child_uct_data.count << ">─";

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
GraphData::save_dot_file(const std::string& filename) const
{
    std::cout << "saving " << filename << std::endl;

    std::ofstream handle(filename.c_str());

    handle << "digraph {" << std::endl;

    for (NodeIt ni(graph); ni!=lemon::INVALID; ++ni)
    {
        const Hash& state_hash = node_state_hashes[ni];
        std::stringstream hash_state_stream;
        hash_state_stream << std::hex;
        hash_state_stream << state_hash;
        hash_state_stream << std::dec;

        std::string hash_state_string = hash_state_stream.str();
        hash_state_string.insert(8,"\\n");

        const UctData& uct_data = node_uct_datas[ni];

#if !defined(NDEBUG)
        assert( states_cache.find(state_hash) != states_cache.end() );
        const State& state = states_cache.find(state_hash)->second;
        assert( (uct_data.parent_hero_int+1) % 4 == state.next_hero_index );
#endif

        handle << graph.id(ni);
        handle << " [label=\"" << uct_data.count << " " << uct_data.score_for_parent << "\\n" << hash_state_string << "\\n@" << ((uct_data.parent_hero_int+1) % 4)+1 << "\"";
        if (is_leaf(ni)) handle << ",shape=diamond";
        handle << "];" << std::endl;
    }

    for (ArcIt ai(graph); ai!=lemon::INVALID; ++ai)
    {
        handle << graph.id(graph.source(ai)) << " -> " << graph.id(graph.target(ai));

        const int& direction_int = arc_direction_ints[ai];
        const Direction& direction = static_cast<Direction>(direction_int);
        assert( node_arc_constructed_flags[graph.source(ai)][direction_int] == true );

        std::string direction_string = to_string(direction);
        direction_string.resize(2);

        handle << " [label=\"" << direction_string << "\"]";
        handle << ";" << std::endl;
    }

    handle << "}" << std::endl;
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
    assert( node_count == graph_data.states_cache.size() );
    assert( node_count == graph_data.nodes_cache.size() );
    const size_t& arc_count = lemon::countArcs(graph_data.graph);

    os << "<GraphData";
    os << " n=" << node_count;
    os << " e=" << arc_count;
    os << " pe=" << arc_count-node_count+1;
#if !defined(NDEBUG)
    if (lemon::dag(graph_data.graph)) os << " DAG";
    if (lemon::tree(lemon::undirector(graph_data.graph))) os << " TREE";
#endif
    os << ">";
    return os;
}

