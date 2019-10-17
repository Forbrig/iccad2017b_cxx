#include <parser.hpp>
#include <router.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>

#include <RTree.h>
#include <RTree_utils.hpp>

using namespace std;
using namespace std::chrono;
using namespace iccad;

typedef pair<int, int> ii;

// coloca em uma fila o id do item encontrado e a distancia
bool search_neighboors(int id, Shape ref, const vector<Shape> &shapes, set<ii> &neighboors) {
    neighboors.insert(ii(distance(ref, shapes[id]), id));
    return true;
}

void insertion_test(const vector<Shape> s) {
    typedef RTree<int, int, 3, float> MyTree;
    MyTree tree;

    unsigned int j;
    for (j = 0; j < s.size(); j++) {
        int low[] = {s[j].a.x, s[j].a.y, s[j].a.z};
        int high[] = {s[j].b.x, s[j].b.y, s[j].b.z};

        tree.Insert(low, high, j); // Note, all values including zero are fine in this version
    }
}

void insertion_search_test(const vector<Shape> s) {
    typedef RTree<int, int, 3, float> MyTree;
    MyTree tree;

    unsigned int j;
    for (j = 0; j < s.size(); j++) {
        int low[] = {s[j].a.x, s[j].a.y, s[j].a.z};
        int high[] = {s[j].b.x, s[j].b.y, s[j].b.z};

        tree.Insert(low, high, j);
    }

    for (j = 0; j < s.size(); j++) {
        int low_s[] = {s[j].a.x, s[j].a.y, s[j].a.z};
        int high_s[] = {s[j].b.x, s[j].b.y, s[j].b.z};

        tree.Search(low_s, high_s, [](int id) { return true; });
    }
}

// shape used, shapes inserted, number of neighbors
void search_neighbors_test(const Shape s, const vector<Shape> shapes, const int n) {
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

    // primeiro procura shapes nas dimensões do s (incluindo o s)
    rtree_search_shape(tree, s, [&s, &shapes, &neighboors](int id)->bool { return search_neighboors(id, s, shapes, neighboors); } );

    Shape z = s;

    int i = 0;
    for (i = 1; ns < n && i < 123456; i = i * 2) { // para quando encontrar a quantidade de retângulos n

        vector<Shape> direcs = neighbooring_quadrants(z, i);

        for (int side = 0; side < 6; side++) {
            rtree_search_shape(tree, direcs[side], [&s, &shapes, &neighboors](int id)->bool { return search_neighboors(id, s, shapes, neighboors); } );
        }

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

    // int l = 0;
    // print_shape(s, 2);
    // cout << "\n";

    // cout << "Execute({";
    // for (int i = 0; i < found.size(); i++) {
    //     if (found[i] == true) {
            
    //         l++;
    //     }
    //     print_shape(shapes[i], 3);
    //     cout << ",";
    // }
    // cout << "})\n";
    // cout << l << "\n";


    int err = 0;

    cout << i << "\n";
    cout << "Execute({";

    print_shape(s, 3);

    // testa se os elementos que sobraram estao realmente mais distantes que i
    for (int k = 0; k < shapes.size(); k++) {
        if ((found[k] == false) && (distance(s, shapes[k]) < i)) {
            if (distance(s, shapes[k]) < i) {
                cout << ",";
                print_shape(shapes[k], 3);

                err++;
            }
        }
    }
    cout << "})\n";

    cout << "qtd erro:" << err << "\n";
}

int main(int argc, char ** argv) {
    Input i;
    parser::parse_file(i, argv[1]);
    // ofstream fout(argv[2]);

    // if (argc != 3) {
    //     cout << "usage: " << argv[0] << " <testcase> <output_file_for_solution>\n";
    //     return -1;
    // }

    vector<Shape> shapes = get_routed_shapes(i);
    vector<Shape> obstacles = get_obstacles(i);

    // cout << "number of rectangles is " << shapes.size() + obstacles.size() << "\n";

	// auto t1 = std::chrono::high_resolution_clock::now();
    // insertion_test(shapes);
    // insertion_test(obstacles);
    // auto t2 = std::chrono::high_resolution_clock::now();
    // cout << "the duration was " << duration_cast<microseconds>( t2 - t1 ).count() << "\n";

	// t1 = std::chrono::high_resolution_clock::now();
    // insertion_search_test(shapes);
    // insertion_search_test(obstacles);
    // t2 = std::chrono::high_resolution_clock::now();
    // cout << "the duration was " << duration_cast<microseconds>( t2 - t1 ).count() << "\n";

	// auto t1 = std::chrono::high_resolution_clock::now();


    // nao pega por 3 unidades de distancia
    // Shape m, n;
    // m.a.x = 60;
    // m.a.y = 393;
    // m.a.z = 0;
    // m.b.x = 129;
    // m.b.y = 422;
    // m.b.z = 0;

    // n.a.x = 80;
    // n.a.y = 931;
    // n.a.z = 0;
    // n.b.x = 230;
    // n.b.y = 968;
    // n.b.z = 0;

    // cout << distance(m, n) << "\n";


    search_neighbors_test(shapes[55], shapes, 20);
    // search_neighbors_test(obstacles[0], obstacles, 100);

    // auto t2 = std::chrono::high_resolution_clock::now();
    // cout << "the duration was " << duration_cast<microseconds>( t2 - t1 ).count() << "\n";
    
    return 0;
}