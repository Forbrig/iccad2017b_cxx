#include <parser.hpp>
#include <router.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>
#include <math.h>

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

    // auto t1 = std::chrono::high_resolution_clock::now();

    for (j = 0; j < s.size(); j++) {
        int low[] = {s[j].a.x, s[j].a.y, s[j].a.z};
        int high[] = {s[j].b.x, s[j].b.y, s[j].b.z};

        tree.Insert(low, high, j);
    }

    // auto t2 = std::chrono::high_resolution_clock::now();
    // cout << "the insertion duration was " << duration_cast<microseconds>( t2 - t1 ).count() << "\n";


    auto t1 = std::chrono::high_resolution_clock::now();

    for (j = 0; j < s.size(); j++) {
        int low_s[] = {s[j].a.x, s[j].a.y, s[j].a.z};
        int high_s[] = {s[j].b.x, s[j].b.y, s[j].b.z};

        tree.Search(low_s, high_s, [](int id) { return true; });
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    // cout << "the colision duration was " << duration_cast<microseconds>( t2 - t1 ).count() << "\n";
    cout << duration_cast<microseconds>( t2 - t1 ).count() << "\n";
}

void nao_colisao(const vector<Shape> s) {
    typedef RTree<int, int, 3, float> MyTree;
    MyTree tree;

    unsigned int j;
    for (j = 0; j < s.size(); j++) {
        int low[] = {s[j].a.x, s[j].a.y, s[j].a.z};
        int high[] = {s[j].b.x, s[j].b.y, s[j].b.z};

        tree.Insert(low, high, j);
    }

    for (j = 0; j < s.size(); j++) {
        int low[] = {s[j].a.x, s[j].a.y, s[j].a.z};
        int high[] = {s[j].b.x, s[j].b.y, s[j].b.z};

        tree.Insert(low, high, j);
    }

    // vector<bool> found(s.size(), false);
    // int low_s[] = {x.a.x, x.a.y, x.a.z};
    // int high_s[] = {x.b.x, x.b.y, x.b.z};
    // tree.Search(low_s, high_s, 
    //         [&found](int id) {
    //             found[id] = true;
    //             return true; 
    //         });

    // for (int i = 0; i < shapes.size(); i++) {
    //     if (found[i] == false) {
    //         RC_ASSERT(!collides(shapes[i], s));            
    //     }
    // }
}

void pesquisa_arbitraria(const vector<Shape> s, const int n, const int arb) {
    typedef RTree<int, int, 3, float> MyTree;
    MyTree tree;

    unsigned int j;
    for (j = 0; j < s.size(); j++) {
        int low[] = {s[j].a.x, s[j].a.y, s[j].a.z};
        int high[] = {s[j].b.x, s[j].b.y, s[j].b.z};
        tree.Insert(low, high, j);
    }

    unsigned int i;
    // cout << n << " elementos\n";

    auto t1 = std::chrono::high_resolution_clock::now();
    for (i = 0; i < n; i++) {
        int low_s[] = {s[arb+i].a.x, s[arb+i].a.y, s[arb+i].a.z};
        int high_s[] = {s[arb+i].b.x, s[arb+i].b.y, s[arb+i].b.z};

        tree.Search(low_s, high_s, [](int id) { return true; });
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    // cout << "the duration was " << duration_cast<microseconds>( t2 - t1 ).count() << "\n";
    cout << duration_cast<microseconds>( t2 - t1 ).count() << "\n";
}

// shape used, shapes inserted, number of neighbors (still not working, see calls in test())
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

    cout << "Execute({";
    print_shape(z, 3);
    cout << "})\n";

    int i = 0;
    for (i = 1; ns < n && i < 123456; i = i * 2) { // para quando encontrar a quantidade de retângulos n
        // cout << i << "\n";

        cout << "Execute({";
        z = s.expand(i);
        cout << ",";
        print_shape(z, 3);

        vector<Shape> direcs = neighbooring_quadrants(z, i);

        for (int side = 0; side < 6; side++) {
            print_shape(direcs[side], 3);
            cout << ",";

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
        cout << "})\n";
    }

    int err = 0;
    // printa os errados
    cout << "Execute({";
    for (int k = 0; k < shapes.size(); k++) {
        if ((found[k] == false) && (distance(s, shapes[k]) < i)) {
            if (distance(s, shapes[k]) < i) {
                print_shape(shapes[k], 3);
                // cout << "shapes[" << k << "]\n";
                cout << ",";

                err++;
            }
        }
    }
    cout << "})\n";


    // printa os neighboors encontrados
    // for (int k = 0; k < shapes.size(); k++) {
    //     if (found[k] == true) {
    //         cout << ",";
    //         print_shape(shapes[k], 3);
    //         // cout << "shapes[" << k << "]\n";
    //     }
    // }
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
    vector<Shape> all_shapes;

    // merge vectors
    all_shapes.reserve( shapes.size() + obstacles.size() ); // preallocate memory
    all_shapes.insert( all_shapes.end(), shapes.begin(), shapes.end() );
    all_shapes.insert( all_shapes.end(), obstacles.begin(), obstacles.end() );

    // cout << "number of rectangles is " << all_shapes.size() << "\n";

    // for (int i = 0; i < 100; i++) {
    //     auto t1 = std::chrono::high_resolution_clock::now();
    //     insertion_test(all_shapes);
    //     auto t2 = std::chrono::high_resolution_clock::now();
    //     // cout << "insertion_test the duration was " << duration_cast<milliseconds>( t2 - t1 ).count() << "\n";
    //     cout << duration_cast<microseconds>( t2 - t1 ).count() << "\n";
    // }
    
    for (int i = 0; i < 100; i++) {
        insertion_search_test(all_shapes);
    }

    // pesquisa_arbitraria(all_shapes, 1, ceil(all_shapes.size() / 2));
    // pesquisa_arbitraria(all_shapes, 10, ceil(all_shapes.size() / 2));
    // pesquisa_arbitraria(all_shapes, 100, ceil(all_shapes.size() / 2));
    // pesquisa_arbitraria(all_shapes, 1000, ceil(all_shapes.size() / 2));

    // pesquisa_arbitraria(all_shapes, 10000, all_shapes.size() / 2);

    return 0;
}

//
void test(const Shape s, const vector<Shape> shapes, const int n) {

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

    // cout << "Execute({";
    // print_shape(s, 3);
    // cout << ",";
    // print_shape(s.expand(512), 3);
    // cout << ",";
    // print_shape(s.expand(256), 3);
    // cout << ",";
    // print_shape(s.expand(128), 3);
    // cout << ",";
    // print_shape(s.expand(64), 3);
    // cout << ",";
    // print_shape(s.expand(32), 3);
    // cout << ",";
    // print_shape(s.expand(16), 3);
    // cout << ",";
    // print_shape(s.expand(8), 3);
    // cout << ",";
    // print_shape(s.expand(4), 3);
    // cout << ",";
    // print_shape(s.expand(2), 3);
    // cout << ",";
    // print_shape(s.expand(1), 3);
    // cout << "})\n";

    vector<bool> found(shapes.size(), false); // salva os shapes que colidem

    // cout << "Execute({";
    // testa se os elementos que sobraram estao realmente mais distantes que i
    int i = 0;
    for (int k = 0; k < shapes.size(); k++) {
        if ((found[k] == false) && (distance(s, shapes[k]) < i)) {
            if (distance(s, shapes[k]) < i) {
                cout << ",";
                // print_shape(shapes[k], 3);
                cout << "shapes[" << k << "]\n";

                err++;
            }
        }
    }
    // cout << "})\n";

    // cout << "Execute({";
    // printa os neighboors encontrados
    for (int k = 0; k < shapes.size(); k++) {
        if (found[k] == true) {
            cout << ",";
            print_shape(shapes[k], 3);
            cout << "shapes[" << k << "]\n";

        }
    }
    // cout << "})\n";
    
    cout << "qtd erro:" << err << "\n";
}