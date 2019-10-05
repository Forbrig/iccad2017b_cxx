// #include <parser.hpp>
#include <router.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>
#include <rapidcheck.h>
#include <base.hpp>
#include <set>
#include "generators.hpp"

#include <RTree.h>
#include <RTree_utils.hpp>

using namespace std;
using namespace std::chrono;
using namespace iccad;

// id do shape encotrado, vetor dos shapes, vetor dos encontrados (bool), contador
bool search_n_callback(int id, vector<bool> &found, int &n) {  
    if (found[id] == false) {
        found[id] = true;
        n++;
    }
    return true;
}

// coloca em uma fila o id do item encontrado e a distancia
bool search_neighboors(int id, Shape ref, const vector<Shape> &shapes, set<pair<int, int>> &neighboors) {
    int found_dist = distance(ref, shapes[id]);
    neighboors.insert({id, found_dist});
    
    return true;
}

vector<Shape> directions(const Shape &ref, int i) {

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

void test_inserted_shapes(vector<Shape> shapes) {
    std::vector<int[3]> low, high;
    typedef RTree<int, int, 3, float> MyTree;
    MyTree tree;

    // insert on tree
    for (int j = 0; j < shapes.size(); j++) {
        int low[] = {shapes[j].a.x, shapes[j].a.y, shapes[j].a.z};
        int high[] = {shapes[j].b.x, shapes[j].b.y, shapes[j].b.z};

        tree.Insert(low, high, j);
    }

    // find the shapes
    for (int j = 0; j < shapes.size(); j++) {
        int low[] = {shapes[j].a.x, shapes[j].a.y, shapes[j].a.z};
        int high[] = {shapes[j].b.x, shapes[j].b.y, shapes[j].b.z};

        int found = tree.Search(low, high, [](int id) { return true; });
        RC_ASSERT(found > 0);
    }
}

void test_rtree_collect(const Shape s, const vector<Shape> shapes) {

    if (shapes.size() == 0) {
        RC_DISCARD("discarding empty testcase");
    }

    typedef RTree<int, int, 3, float> MyTree;
    MyTree tree;

    // insert on tree
    for (int j = 0; j < shapes.size(); j++) {
        int low[] = {shapes[j].a.x, shapes[j].a.y, shapes[j].a.z};
        int high[] = {shapes[j].b.x, shapes[j].b.y, shapes[j].b.z};

        tree.Insert(low, high, j);
    }

    int low_s[] = {s.a.x, s.a.y, s.a.z};
    int high_s[] = {s.b.x, s.b.y, s.b.z};
    int found = tree.Search(low_s, high_s, 
            [&shapes, &s](int id) { 
                RC_ASSERT(collides(shapes[id], s));
                return true; 
            });
}

void test_rtree_collect_neg(const Shape s, const vector<Shape> shapes) {
    if (shapes.size() == 0) {
        RC_DISCARD("discarding empty testcase");
    }

    // std::vector<int[3]> low, high;
    typedef RTree<int, int, 3, float> MyTree;
    MyTree tree;

    // insert on tree
    for (int j = 0; j < shapes.size(); j++) {
        int low[] = {shapes[j].a.x, shapes[j].a.y, shapes[j].a.z};
        int high[] = {shapes[j].b.x, shapes[j].b.y, shapes[j].b.z};

        tree.Insert(low, high, j);
    }

    vector<bool> found(shapes.size(), false);
    int low_s[] = {s.a.x, s.a.y, s.a.z};
    int high_s[] = {s.b.x, s.b.y, s.b.z};
    tree.Search(low_s, high_s, 
            [&found](int id) {
                found[id] = true;
                return true; 
            });

    for (int i = 0; i < shapes.size(); i++) {
        if (found[i] == false) {
            RC_ASSERT(!collides(shapes[i], s));            
        }
    }
}

void test_rtree_closest_n_shapes(const Shape s, const vector<Shape> shapes, unsigned int p) {
    
    if (shapes.size() == 0) {
        RC_DISCARD("discarding empty testcase");
    }

    unsigned n = p % shapes.size();

    if (n == 0) {
        RC_DISCARD("discarding empty testcase");
    }

    typedef RTree<int, int, 3, float> MyTree;
    MyTree tree;

    // insert on tree
    for (int j = 0; j < shapes.size(); j++) {
        int low[] = {shapes[j].a.x, shapes[j].a.y, 0};
        int high[] = {shapes[j].b.x, shapes[j].b.y, 0};

        tree.Insert(low, high, j);
    }

    // search nearests
    int ns = 0;
    // vector<bool> found(shapes.size(), false); // salva os shapes que colidem

    // fila de prioridades com as shapes mais próximas
    set <pair<int, int>> neighboors;

    {
        // primeiro procura shapes nas dimensões do s
        rtree_search_shape(tree, s, [&s, &shapes, &neighboors](int id)->bool { return search_neighboors(id, s, shapes, neighboors); } );
    }

    for (int i = 1; ns < n && i < 123456789; i = i * 2) { // para quando encontrar a quantidade de retângulos n

        vector<Shape> direcs = directions(s, i);

        for (int side = 0; side < 6; side++) {
            rtree_search_shape(tree, direcs[side], [&s, &shapes, &neighboors](int id)->bool { return search_neighboors(id, s, shapes, neighboors); } );
        }

        // inseri todos 
        // agora retiro os que eu sei que estao na distancia i 
        // para cada um que eu sei que esta na distancia ns++
        
        // https://github.com/splucs/Competitive-Programming/blob/master/Macacário/Graphs/dijkstra.cpp


        s.expand(i);
    }


    // // distancia do s e cada um dos found bool
    // // pegar a maior distanicia d
    // // ver se a distancia dos não encontrados é menor que n
    // int maior_distancia = 0;
    // int aux_dist;
    // for (int i = 0; i < found.size(); i++) {
    //     if (found[i] == true) {
    //         aux_dist = distance(s, shapes[i]);
    //         if (aux_dist > maior_distancia) {
    //             maior_distancia = aux_dist;
    //         }
    //     }
    // }

    // // procura em todos os não encontrados e vê se a distância deles é menor que a maior dos encontrados
    // for (int i = 0; i < found.size(); i++) {
    //     if (found[i] == false) {
    //         aux_dist = distance(s, shapes[i]);
    //         if (aux_dist <= maior_distancia) {
    //             cout << aux_dist << " " << maior_distancia << "\n";
    //         }
            
    //         // RC_ASSERT(aux_dist <= maior_distancia);
    //     }
    // }

    // cout << "n: " << n << " ns: " << ns << "\n";
}

int main(int argc, char ** argv) {
    // rapidckeck tests
    rc::check("Check that added shapes can be queried", test_inserted_shapes);
    rc::check("Check that collected shapes are colliding with the query", test_rtree_collect);
    rc::check("Check that not collected shapes collide with the query", test_rtree_collect_neg);
    rc::check("Check that nearests shapes collide with area", test_rtree_closest_n_shapes);

    return 0;
}