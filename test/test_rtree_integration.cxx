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
    // cout << x1 << "\n";
    // cout << y1 << "\n";
    // cout << z1 << "\n";
    // cout << x2 << "\n";
    // cout << y2 << "\n";
    // cout << z2 << "\n";
    
    // cout << s.a.x << "\n";
    // cout << s.a.y << "\n";
    // cout << s.a.z << "\n";
    // cout << s.b.x << "\n";
    // cout << s.b.y << "\n";
    // cout << s.b.z << "\n";

    // cin.get();
    
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
    
    // omitindo z
    // cout << "Polygon((" << z.a.x << "," << z.a.y << "),";
    // cout << "(" << z.a.x << "," << z.b.y << "),";
    // cout << "(" << z.b.x << "," << z.b.y << "),";
    // cout << "(" << z.b.x << "," << z.a.y << "))\n";
    // cin.get();

    // omitindo x
    // cout << "Polygon((" << z.a.z << "," << z.a.y << "),";
    // cout << "(" << z.a.z << "," << z.b.y << "),";
    // cout << "(" << z.b.z << "," << z.b.y << "),";
    // cout << "(" << z.b.z << "," << z.a.y << "))\n";
    // cin.get();

    // // omitindo y
    // cout << "Polygon((" << z.a.x << "," << z.a.z << "),";
    // cout << "(" << z.a.x << "," << z.b.z << "),";
    // cout << "(" << z.b.x << "," << z.b.z << "),";
    // cout << "(" << z.b.x << "," << z.a.z << "))\n";
    // cin.get();

    int i = 0;
    for (i = 1; ns < n && i < 123456; i = i * 2) { // para quando encontrar a quantidade de retângulos n

        vector<Shape> direcs = directions(z, i);

        for (int side = 0; side < 6; side++) {
            //sem a coordenada z
            // cout << "Polygon((" << direcs[side].a.x << "," << direcs[side].a.y << "),";
            // cout << "(" << direcs[side].a.x << "," << direcs[side].b.y << "),";
            // cout << "(" << direcs[side].b.x << "," << direcs[side].b.y << "),";
            // cout << "(" << direcs[side].b.x << "," << direcs[side].a.y << "))\n";
            // cin.get();

            //sem a coordenada x
            // cout << "Polygon((" << direcs[side].a.z << "," << direcs[side].a.y << "),";
            // cout << "(" << direcs[side].a.z << "," << direcs[side].b.y << "),";
            // cout << "(" << direcs[side].b.z << "," << direcs[side].b.y << "),";
            // cout << "(" << direcs[side].b.z << "," << direcs[side].a.y << "))\n";
            // cin.get();

            // //sem a coordenada y
            // cout << "Polygon((" << direcs[side].a.x << "," << direcs[side].a.z << "),";
            // cout << "(" << direcs[side].a.x << "," << direcs[side].b.z << "),";
            // cout << "(" << direcs[side].b.x << "," << direcs[side].b.z << "),";
            // cout << "(" << direcs[side].b.x << "," << direcs[side].a.z << "))\n";
            // cin.get();

            // cout << direcs[side].a.x << "\n";
            // cout << direcs[side].a.y << "\n";
            // cout << direcs[side].a.z << "\n";
            // cout << direcs[side].b.x << "\n";
            // cout << direcs[side].b.y << "\n";
            // cout << direcs[side].b.z << "\n";

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

    // testa se os elementos que sobraram estao realmente mais distantes que i
    // for (int k = 0; k < shapes.size(); k++) {
    //     if ((found[k] == false) && (distance(s, shapes[k]) < i)) {
            // if (distance(s, shapes[k]) < i) {
    //             cout << "erro\n";
                
    //         //     cout << "current dist: " << i << " found dist: "<< found_dist << "\n";

    //         //     cout << s.a.x << " ";
    //         //     cout << s.a.y << " ";
    //         //     cout << s.a.z << "\n";
    //         //     cout << s.b.x << " ";
    //         //     cout << s.b.y << " ";
    //         //     cout << s.b.z << "\n";

    //         //     cout << shapes[k].a.x << " ";
    //         //     cout << shapes[k].a.y << " ";
    //         //     cout << shapes[k].a.z << "\n";
    //         //     cout << shapes[k].b.x << " ";
    //         //     cout << shapes[k].b.y << " ";
    //         //     cout << shapes[k].b.z << "\n";
                // RC_ASSERT(false);
            // }
    //     }
    // }

    // cout << n << " " << ns << "\n";
    // cout << "new\n";
}

int main(int argc, char ** argv) {
    // rapidckeck tests
    rc::check("Check that added shapes can be queried", test_inserted_shapes);
    rc::check("Check that collected shapes are colliding with the query", test_rtree_collect);
    rc::check("Check that not collected shapes collide with the query", test_rtree_collect_neg);
    rc::check("Check that nearests shapes collide with area", test_rtree_closest_n_shapes);

    return 0;
}