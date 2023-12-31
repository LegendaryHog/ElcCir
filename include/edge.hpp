#pragma once

#include <unordered_map>

namespace Circuit
{

namespace InputOutput
{
struct InputEdge
{
    unsigned node1_ = 0, node2_ = 0;
    double resistance_ = 0.0, emf_ = 0.0;
}; // struct IOEdge
} // namespace InputOutput

struct Edge : private InputOutput::InputEdge
{
    using base = InputOutput::InputEdge;

    using base::node1_, base::node2_, base::resistance_, base::emf_;
    unsigned ind_ = 0;
    
    Edge() = default;
    Edge(unsigned n1, unsigned n2, double res, double emf, unsigned i)
    :base{n1, n2, res, emf}, ind_ {i}
    {}

    Edge(const InputOutput::InputEdge& ie, unsigned ind = 0)
    :base(ie), ind_ {ind}
    {}
}; // struct Edge

bool operator==(const Edge& e1, const Edge& e2);
} // namespace Circuit
