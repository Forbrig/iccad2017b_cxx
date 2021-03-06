#pragma once

#include <algo.hpp>
#include <base.hpp>
#include <model.hpp>

#include <tuple>
#include <vector>

namespace iccad {

    using std::tuple, std::vector;


    struct Route {
        vector<PT> route;
        void simplify();
        void add_point(PT p);
        friend ostream & print2D(ostream &out, const PT & p);
        friend ostream & operator<<(ostream &out, const Route & r);    
        int length() const ;
    };



    struct AStar {

        using index = tuple<int, int, int>;

        const Treap & shapes;
        const Treap & obstacles;
        V1D boundary;
        vector<int> xs, ys, zs;
        const Shape & source;
        const Shape & target;

        AStar(const Treap & sh, const Treap & obs, const Shape & s1, const Shape & s2, V1D b);



        Route run() ;


private:
        vector<index> neighboors(index i);
        const PT make_pt(index i);
        int find(int c, const vector<int> & v) const ;
        index find(const PT p) const;

        void remove_duplicates(vector<int> & v) ;
        void fix_boundaries(vector<int> & v, int min_bound, int max_bound);
        void add_shape(const Shape & s) ;
        // Route bad_run(const PT s, const Shape & ts) ;
        Route run1(const Shape & shape_s, const Shape & shape_t) ;
    };

}