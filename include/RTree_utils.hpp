#pragma once

#include <base.hpp>
#include <RTree.h>

using namespace iccad;

// transforma os pontos em um array (pra rtree)
void to_array(const PT &p, int a[3]) {
    a[0] = p.x;
    a[1] = p.y;
    a[2] = p.z;
}

// const Shape &sp é o shape pesquisado agora
// passa um callback para a função de search da árvore
template <typename Func>
bool rtree_search_shape(const RTree<int, int, 3, float> &tree, const Shape &sp, Func f) {

    int low_s[3];
    to_array(sp.a, low_s);
    int high_s[3];
    to_array(sp.b, high_s);
            
    return tree.Search(low_s, high_s, [f](int id) {
            if (true) {
                f(id);
            }
        }
    );
}