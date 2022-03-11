#pragma once

#include "kspwlolib/graph.hpp"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/property_map/property_map.hpp>

#include <unordered_set>
#include <utility>
#include <vector>

namespace boost {

struct custom_graph_traversal_category :    public virtual vertex_list_graph_tag,
                                            public virtual edge_list_graph_tag
{
};

template<>
struct graph_traits<Path> {
    using vertex_descriptor = NodeID;
    using edge_descriptor = Edge;

    class vertex_iterator : public iterator_facade< vertex_iterator, vertex_descriptor, random_access_iterator_tag, const vertex_descriptor& >
    {
    public:
        vertex_iterator() = default;

        vertex_iterator( const Path* p )
            : vertex_iterator{ p, std::begin(p->nodes) } {
        }

        vertex_iterator( const Path* p, std::vector<vertex_descriptor>::const_iterator it )
            : p(p)
            , it{ it } {
        }

        vertex_iterator( const vertex_iterator& other ) 
            : vertex_iterator{ other.p, other.it } {
        }

        vertex_iterator& operator=( const vertex_iterator& other ) {
            if( this != &other ) {
                this->it = other.it;
                this->p = other.p;
            }
            return *this;
        }

    private:
        const vertex_descriptor& dereference() const { 
            return *it; 
        }

        bool equal(const vertex_iterator& other) const {
            return it == other.it;
        }

        void increment() {
            it = std::next(it);
        }
        void decrement() { 
            it = std::prev(it);
        }

        std::vector<vertex_descriptor>::const_iterator it;
        const Path* p = nullptr;

        friend class iterator_core_access;
    };

    class edge_iterator : public iterator_facade< edge_iterator, edge_descriptor, forward_iterator_tag, const edge_descriptor& >
    {
    public:
        edge_iterator() = default;

        edge_iterator( const Path* p )
            : edge_iterator{ p, std::begin(p->edges) } {
        }

        edge_iterator( const Path* p, std::unordered_set<edge_descriptor,boost::hash<edge_descriptor>>::const_iterator it )
            : p(p)
            , it{ it } {
        }

        edge_iterator( const edge_iterator& other ) 
            : edge_iterator{ other.p, other.it } {
        }

        edge_iterator& operator=( const edge_iterator& other ) {
            if( this != &other ) {
                this->it = other.it;
                this->p = other.p;
            }
            return *this;
        }

    private:
        const edge_descriptor& dereference() const { 
            return *it; 
        }

        bool equal(const edge_iterator& other) const {
            return it == other.it;
        }

        void increment() {
            it = std::next(it);
        }

        std::unordered_set<edge_descriptor,boost::hash<edge_descriptor>>::const_iterator it;
        const Path* p = nullptr;

        friend class iterator_core_access;
    };

    using directed_category = directed_tag;
    using edge_parallel_category = allow_parallel_edge_tag;
    using traversal_category = custom_graph_traversal_category;
    using vertices_size_type = size_t;
    using edges_size_type = size_t;
};

inline std::pair<typename graph_traits<Path>::edge_iterator, typename graph_traits<Path>::edge_iterator> edges( const Path& p )
{
    using Iter = typename graph_traits<Path>::edge_iterator;
    return std::make_pair( Iter(&p), Iter(&p, std::end( p.edges ) ) );
}

typename graph_traits<Path>::edges_size_type num_edges( const Path& p )
{
    return p.edges.size();
}

inline std::pair<typename graph_traits<Path>::vertex_iterator, typename graph_traits<Path>::vertex_iterator> vertices( const Path& p )
{
    using Iter = typename graph_traits<Path>::vertex_iterator;
    return std::make_pair( Iter(&p), Iter(&p, std::end( p.nodes ) ) );
}

typename graph_traits<Path>::vertices_size_type num_vertices( const Path& p )
{
    return p.nodes.size();
}

template<>
struct vertex_property_type<Path> {
    using type = typename graph_traits<Path>::vertex_descriptor;
};

class path_id_map : public typed_identity_property_map<size_t>
{
public:
    path_id_map() {}
    template < class T > long operator[](T x) const { return x; }
};

inline path_id_map get(vertex_index_t, const Path& p)
{
    return path_id_map();
}

inline size_t get(vertex_index_t, const Path& p, typename graph_traits<Path>::vertex_descriptor vertex)
{
    return path_id_map()[vertex];
}

inline void renumber_vertex_indices(const Path& p)
{
}

} // boost
