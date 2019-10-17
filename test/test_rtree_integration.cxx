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

#define MAXN 100009

typedef pair<int, int> ii;

// id do shape encotrado, vetor dos shapes, vetor dos encontrados (bool), contador
bool search_n_callback(int id, vector<bool> &found, int &n) {  
    if (found[id] == false) {
        found[id] = true;
        n++;
    }
    return true;
}

// coloca em uma fila o id do item encontrado e a distancia
bool search_neighboors(int id, Shape ref, const vector<Shape> &shapes, set<ii> &neighboors) {
    neighboors.insert(ii(distance(ref, shapes[id]), id));
    return true;
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
        int low[] = {shapes[j].a.x, shapes[j].a.y, shapes[j].a.z};
        int high[] = {shapes[j].b.x, shapes[j].b.y, shapes[j].b.z};

        tree.Insert(low, high, j);
    }

    // search nearests
    int ns = 0;
    vector<bool> found(shapes.size(), false); // salva os shapes que colidem

    // fila de prioridades com as shapes mais próximas
    set<ii> neighboors;

    // primeiro procura shapes nas dimensões do s
    rtree_search_shape(tree, s, [&s, &shapes, &neighboors](int id)->bool { return search_neighboors(id, s, shapes, neighboors); } );

    Shape z = s;

    // z.a.x = 4;
    // z.a.y = 4;
    // z.a.z = 4;
    // z.b.x = 8;
    // z.b.y = 8;
    // z.b.z = 8;
    
    // print_shape(z, 2);

    int i = 0;
    for (i = 1; ns < n && i < 123456; i = i * 2) { // para quando encontrar a quantidade de retângulos n

        vector<Shape> direcs = neighbooring_quadrants(z, i);

        for (int side = 0; side < 6; side++) {
            rtree_search_shape(tree, direcs[side], [&s, &shapes, &neighboors](int id)->bool { return search_neighboors(id, s, shapes, neighboors); } );
        }

        // https://github.com/splucs/Competitive-Programming/blob/master/Macacário/Graphs/dijkstra.cpp

        // inseri todos 
        // agora retiro os que eu sei que estao na distancia i 
        // para cada um que eu sei que esta na distancia ns++

        while (!neighboors.empty()) {
            int dist = neighboors.begin()->first;
            int id = neighboors.begin()->second;
            if (dist < i) { // se a distancia for menor que a expansao procurada entao retira a shape do set
                if (found[id] == false) {
                    found[id] = true;
                    ns++; // contabiliza nos shapes encontrados
                }
                neighboors.erase(neighboors.begin());
            } else { // se for maior para pois o set esta ordenado pela distancia
                break;
            }
        }
        
        z = s.expand(i);
    }

    int err = 0;
    // cout << i << "\n";

    // cout << "Execute({";

    // print_shape(s, 3);
    // cout << ",";

    // testa se os elementos que sobraram estao realmente mais distantes que i
    for (int k = 0; k < shapes.size(); k++) {
        if ((found[k] == false) && (distance(s, shapes[k]) < i)) {
            if (distance(s, shapes[k]) < i) {
                // print_shape(shapes[i], 3);
                // cout << ",";

                err++;
                
                // cout << "current dist: " << i << " found dist: "<< found_dist << "\n";

                // RC_ASSERT(false);
            }
        }
    }
    // cout << "})\n";

    cout << "erro:" << err << "\n";


    // cout << n << " " << ns << "\n";
    // cout << "new\n";
}

int main(int argc, char ** argv) {
    // rapidckeck tests
    // rc::check("Check that added shapes can be queried", test_inserted_shapes);
    // rc::check("Check that collected shapes are colliding with the query", test_rtree_collect);
    // rc::check("Check that not collected shapes collide with the query", test_rtree_collect_neg);
    rc::check("Check that nearests shapes collide with area", test_rtree_closest_n_shapes);

    return 0;
}