#pragma once

#include <tuple>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <set>

#include "connected_circuit.hpp"

namespace Circuit
{
class Circuit final
{
public:
    using size_type = std::size_t;
    using Edges = ConnectedCircuit::Edges;
    using EdgeCur  = typename ConnectedCircuit::EdgeCur;
    using Solution = typename ConnectedCircuit::Solution;

private:
    // C - number of connected circuits in circuit (cirs_.size())
    // MN = max(N_1, N_2, ... N_C) - max number of nodes in connected circuit (N_i - cirs_[i].number_of_nodes())
    // ME = max(E_1, E_2, ... E_C) - max number of edges in connected circuit (E_i - cirs_[i].number_of_edges())
    // N - number of nodes (number_of_nodes_)
    // E - number of edges (number_of_edges_)
    // E/C <= ME <= E
    // N/C <= MC <= N
    Container::Vector<ConnectedCircuit> cirs_ = {};
    size_type number_of_edges_ = 0, number_of_nodes_ = 0;
    
    using Nodes = std::unordered_map<unsigned, Container::Vector<std::pair<unsigned, const Edge*>>>;
    using Node  = typename Nodes::value_type;

    // Complexity: O(E)
    template<std::input_iterator InpIt>
    static Nodes make_nodes(InpIt first, InpIt last)
    {
        Nodes nodes {};
        
        for (;first != last; ++first) // E iterations
        {
            const auto& pair1 = nodes.insert(Node{first->node1_, Container::Vector{std::make_pair(first->node2_, std::to_address(first))}});
            const auto& pair2 = nodes.insert(Node{first->node2_, Container::Vector{std::make_pair(first->node1_, std::to_address(first))}});

            if (!pair1.second)
                pair1.first->second.push_back(std::make_pair(first->node2_, std::to_address(first)));
            if (!pair2.second)
                pair2.first->second.push_back(std::make_pair(first->node1_, std::to_address(first)));
        }

        return nodes;
    }

    // Complexity: O(1)
    static void add_node_in_connected_cir
    (Nodes& nodes, typename Nodes::iterator itr, Container::Vector<Node>& con_cir)
    {
        con_cir.push_back(std::move(*itr));
        nodes.erase(itr);
    }

    // Complexity: O(MN * ME)
    static Container::Vector<Node> make_connected_cir_as_nodes(Nodes& nodes)
    {
        Container::Vector<Node> connected_cir {};
        add_node_in_connected_cir(nodes, nodes.begin(), connected_cir);

        for (size_type i = 0; i < connected_cir.size(); ++i) // MN iterations
            for (const auto& node: connected_cir[i].second) // ME iterations
            {
                auto itr = nodes.find(node.first);
                if (itr != nodes.end())
                    add_node_in_connected_cir(nodes, itr, connected_cir);
            }

        return connected_cir;
    }

    // Complexity: O(MN * ME)
    template<std::input_iterator InpIt> 
    static ConnectedCircuit make_connected_cir(InpIt first, InpIt last)
    {
        std::unordered_set<const Edge*> edges_ptr_set {};

        for (;first != last; ++first) // MN iterations
            for (const auto& pair: first->second) // ME iterations
                edges_ptr_set.insert(pair.second);
                
        Edges edges {};
        for (const auto& edge_ptr: edges_ptr_set) // ME iterations
            edges.push_back(*edge_ptr);

        return ConnectedCircuit(std::move(edges)); // ME iterations
    }

    // Complexity: O(E)
    template<std::input_iterator InpIt>
    static Edges make_edges_from_input_edges(InpIt first, InpIt last)
    {
        Edges edges (first, last); // E iterations
        for (size_type i = 0; i < edges.size(); ++i) // E iterations
            edges[i].ind_ = i;
        return edges;
    }

public:
    // Complexity: O(C * MN * ME)
    template<std::input_iterator InpIt>
    Circuit(InpIt first, InpIt last)
    requires std::is_same<typename std::remove_cvref_t<typename std::iterator_traits<InpIt>::value_type>, InputOutput::InputEdge>::value
    {
        const auto& edges = make_edges_from_input_edges(first, last); // E iterations

        auto nodes = make_nodes(edges.cbegin(), edges.cend()); // E iterations
        number_of_nodes_ = nodes.size();

        size_type nodes_placed = 0;
        while (!nodes.empty()) // C iterations
        {
            const auto& connected_cir = make_connected_cir_as_nodes(nodes); // MN * ME iterations
            cirs_.push_back(make_connected_cir(connected_cir.cbegin(), connected_cir.cend())); // MN * ME iterations
            number_of_edges_ += cirs_.back().number_of_edges();
        }
    }
    
    // Complexity: O(C * MN * ME)
    Circuit(std::initializer_list<InputOutput::InputEdge> ilist): Circuit(ilist.begin(), ilist.end()) {}

    size_type number_of_edges() const {return number_of_edges_;}
    size_type number_of_nodes() const {return number_of_nodes_;}
    size_type number_of_connected_circuits() const {return cirs_.size();}

    // Complexity: O(C * (MN + ME)^3)
    Solution solve_circuit() const;
}; // class Circuit
} // namespace Circuit