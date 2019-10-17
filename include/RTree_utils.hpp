#pragma once

#include <iostream>
#include <base.hpp>
#include <RTree.h>

using namespace std;
using namespace iccad;

// retorna os quadrantes adjacentes a shape de referencia com base na expansao i
vector<Shape> neighbooring_quadrants(const Shape &ref, int i) {
    vector<Shape> result;
    Shape aux;

    int x1 = ref.a.x;
    int y1 = ref.a.y;
    int z1 = ref.a.z;
    int x2 = ref.b.x;
    int y2 = ref.b.y;
    int z2 = ref.b.z;

    // direita
    aux.a.x = x2;
    aux.a.y = y1 - i;
    aux.a.z = z1 - i;
    aux.b.x = x2 + i;
    aux.b.y = y2 + i;
    aux.b.z = z2 + i;
    result.push_back(aux);

    // esquerda
    aux.a.x = x1 - i;
    aux.a.y = y1 - i;
    aux.a.z = z1 - i;
    aux.b.x = x1;
    aux.b.y = y2 + i;
    aux.b.z = z2 + i;
    result.push_back(aux);

    // frente
    aux.a.x = x1;
    aux.a.y = y1 - i;
    aux.a.z = z2;
    aux.b.x = x2;
    aux.b.y = y2 + i;
    aux.b.z = z2 + i;
    result.push_back(aux);

    // atras
    aux.a.x = x1;
    aux.a.y = y1 - i;
    aux.a.z = z1 - i;
    aux.b.x = x2;
    aux.b.y = y2 + i;
    aux.b.z = z1;  
    result.push_back(aux);

    // em cima
    aux.a.x = x1;
    aux.a.y = y1 - i;
    aux.a.z = z1;
    aux.b.x = x2;
    aux.b.y = y1;
    aux.b.z = z2;
    result.push_back(aux);

    // em baixo
    aux.a.x = x1;
    aux.a.y = y2;
    aux.a.z = z1;
    aux.b.x = x2;
    aux.b.y = y2 + i;
    aux.b.z = z2;
    result.push_back(aux);

    return result;
}

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
            
    return tree.Search(low_s, high_s, f);
}

void print_shape(Shape z, int i) {
    if (i == 1) {
       
    } else if (i == 2) { // min(x, y, z) max(x, y, z) 
        cout << "min(" << z.a.x << "," << z.a.y << "," << z.a.z << ")\n";
        cout << "max(" << z.b.x << "," << z.b.y << "," << z.b.z << ")\n";
        // cin.get();
    } else if (i == 3) {
        // omitindo z
        cout << "\"Polygon((" << z.a.x << "," << z.a.y << "),";
        cout << "(" << z.a.x << "," << z.b.y << "),";
        cout << "(" << z.b.x << "," << z.b.y << "),";
        cout << "(" << z.b.x << "," << z.a.y << "))\"\n";
        // cin.get();

        // // omitindo x
        // cout << "{\"Polygon((" << z.a.z << "," << z.a.y << "),";
        // cout << "(" << z.a.z << "," << z.b.y << "),";
        // cout << "(" << z.b.z << "," << z.b.y << "),";
        // cout << "(" << z.b.z << "," << z.a.y << "))\"\n";
        // // cin.get();

        // // omitindo y
        // cout << "{\"Polygon((" << z.a.x << "," << z.a.z << "),";
        // cout << "(" << z.a.x << "," << z.b.z << "),";
        // cout << "(" << z.b.x << "," << z.b.z << "),";
        // cout << "(" << z.b.x << "," << z.a.z << "))\"\n";
        // cin.get();
    }
}