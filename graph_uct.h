#pragma once

#include "hashed.h"
#include "board.h"
#include <random>

struct GraphData
{
    GraphData(const double uct_constant);

    double uct_constant;

    typedef std::default_random_engine Rng;
    typedef lemon::SmartDigraph Graph;
    typedef Graph::Node Node;

    /**
     * drops data that is not accessible from node.
     * node should be a valid node.
     * @param node node
     */
    void
    prune_below(const Node& node);

    /**
     * checked is state is in data structure.
     * @param hashed_state hashed state
     * @return bool
     */
    bool
    contains(const HashedPair<BoardState>& hashed_state) const;

    /**
     * check if node is a leaf or not.
     * a leaf is a node with children instantiated in every direction.
     * node should be a valid node.
     * @return bool
     */
    bool
    is_leaf(const Node& node) const;

    /**
     * return node for state.
     * state should already been inserted into the struture using get_or_create_node.
     * @param hashed_state hashed state
     * @return node
     */
    Node
    get_node(const HashedPair<BoardState>& hashed_state) const;

    /**
     * return node for state.
     * if state isn't part of the structure, it is inserted.
     * @param hashed_state hashed state
     * @return node
     */
    std::pair<bool, Node>
    get_or_create_node(const HashedPair<BoardState>& state);

    /**
     * get child node from state node after playing move.
     * initial state should already be inserted in structure.
     * update state!! if state comes from game, game should be updated too.
     * @param state modified state
     * @param move move to child
     * @return node
     */
    std::pair<bool, Node>
    get_or_create_child(BoardState& state, const Move& move);

    /**
     * get state for node.
     * node should be a valid node.
     * @param node node
     * @return state
     */
    const BoardState&
    get_state(const Node& node) const;

    struct UctData
    {
        int parent_hero_int;
        double score_for_parent;
        int count;
    };

    /**
     * get uct data for node.
     * node should be a valid node.
     * @param node node
     * @return uct data
     */
    const UctData&
    get_uct_data(const Node& node) const;

    /**
     * get maximum scoring child for **non leaf** node.
     * node should be a valid **non leaf** node.
     * used during uct descent.
     * @param parent parent
     * @param rng random number generator
     * @return best child
     */
    Node
    get_best_child(const Node& parent, Rng& rng) const;

    /**
     * get maximum scoring move for **non leaf** node.
     * node should be a valid **non leaf** node.
     * used to choose best next move.
     * @param parent parent
     * @param rng random number generator
     * @return best move
     */
    Move
    get_best_move(const Node& parent, Rng& rng) const;

    /**
     * get random available move for **leaf** node.
     * node should be a valid **leaf** node.
     * @param node node
     * @param rng random number generator
     * @return move
     */
    Move
    get_random_available_move(const Node& node, Rng& rng) const;

    typedef std::list<GraphData::Node> Nodes;

    /**
     * update score and count of the uct data along nodes.
     * compute reward from winner and uct data parent_hero_int.
     * used at the end of uct to insert mc result in the structure.
     * @param nodes top down path to be updated
     * @param state final state
     */
    void
    back_propagate_winner(const Nodes& nodes, const BoardState& state);

    /**
     * dump structure into os as a tree from root.
     * @param os output stream
     * @param root root
     * @param ident indentation string
     * @param max_depth maximum depth displayed
     */
    void
    print_from_root(std::ostream& os, const Node& root, const std::string& indent = "", const int& max_depth=-1) const;

    /**
     * save graph structure as a dot file.
     * @param filename file name
     */
    void
    save_dot_file(const std::string& filename) const;

    /**
     * fill structure sizes.
     * @param node_count node count
     * @param arc_count arc count
     * @param parallel_arc_count parallel arc count
     */
    void
    get_sizes(int& node_count, int& arc_count, int& parallel_arc_count) const;

    /**
     * light dump of structure.
     * @param os output stream
     * @param graph_data structure
     * @return output stream
     */
    friend
    std::ostream&
    operator<<(std::ostream& os, const GraphData& graph_data);

    /**
     * merge both graph to have the same representation
     * @param graph_data_a first graph data
     * @param graph_data_b second graph data
     */
    friend
    void
    merge_graph_data(GraphData& graph_data_a, GraphData& graph_data_b);

    typedef std::pair<UctData, Node> UctDataNodePair;
    //typedef std::pair<UctData, int> UctDataDirectionIntPair;

private:

    typedef Graph::NodeIt NodeIt;
    typedef Graph::Arc Arc;
    typedef Graph::ArcIt ArcIt;
    typedef Graph::OutArcIt OutArcIt;

    //typedef boost::array<bool, 5> ArcConstructedFlags;

    typedef Graph::NodeMap<UctData> NodeUctDatas;
    typedef Graph::NodeMap<Hash> NodeStateHashes;
    //typedef Graph::NodeMap<ArcConstructedFlags> NodeArcConstructedFlags;
    //typedef Graph::ArcMap<int> ArcDirectionInts;

    //typedef std::map<Hash, const State> StatesCache;
    //typedef std::map<Hash, const Node> NodesCache;

    Graph graph;

    NodeUctDatas node_uct_datas;
    NodeStateHashes node_state_hashes;
    //NodeArcConstructedFlags node_arc_constructed_flags;
    //ArcDirectionInts arc_direction_ints;

    //StatesCache states_cache;
    //NodesCache nodes_cache;

    Arc
    create_arc(const Node& from_node, const Node& to_node, const Move& move);

    Node
    create_node(const HashedPair<BoardState>& hashed_state);

    void
    print_from_root_internal(std::ostream& os, const Node& node, const std::string& indent, const int& max_depth) const;

};

