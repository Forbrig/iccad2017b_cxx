#include <parser.hpp>
#include <router.hpp>
#include <iostream>
#include <vector>
// #include <random>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>
#include <rapidcheck.h>
#include "generators.hpp"

#include <RTree.h>

using namespace std;
using namespace std::chrono;
using namespace iccad;

bool MySearchCallback(int id) {
    // cout << "Hit data rect " << id << "\n";
    return true; // keep going
}

bool KeepGoingCallback(int id) {
    return true; // keep going
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

        int found = tree.Search(low, high, KeepGoingCallback);
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

void test_rtree_closest_n_shapes(const Shape s, const vector<Shape> shapes, unsigned int n) {
    if (shapes.size() == 0 || n == 0) {
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

    // search nearests
    int ns = 0;

    int x1 = s.a.x;
    int x2 = s.b.x;
    int y1 = s.a.y;
    int y2 = s.b.y;
    vector<bool> found(shapes.size(), false); // salva os shapes que colidem

    for (int i = 1; ns < n; i = i * 2) { // para quando encontrar a quantidade de retângulos n
        // testa ele mesmo
        cout << i << "\n";

        {
            // em cima
            int low_s[] = {x1 - i, y1, s.a.z};
            int high_s[] = {x2 + i, y1 - i, s.b.z};
            tree.Search(low_s, high_s,
                        [&found, &ns](int id) {
                            if (found[id] == false) {
                                ns++;
                            }
                            found[id] = true;
                            return true; 
                        });
        }

        {
            // em baixo
            int low_s[] = {x1 - i, y2, s.a.z};
            int high_s[] = {x2 + i, y2 + i, s.b.z};
            tree.Search(low_s, high_s, 
                        [&found, &ns](int id) {
                            if (found[id] == false) {
                                ns++;
                            }
                            found[id] = true;
                            return true; 
                        });
        }
        
        {
            // esquerda
            int low_s[] = {x1 - i, y1, s.a.z};
            int high_s[] = {x1, y2, s.b.z};
            tree.Search(low_s, high_s,
                        [&found, &ns](int id) {
                            if (found[id] == false) {
                                ns++;
                            }
                            found[id] = true;
                            return true; 
                        });
        }
        
        {
            // direita
            int low_s[] = {x2, y1, s.a.z};
            int high_s[] = {x2 + i, y2, s.b.z};
            tree.Search(low_s, high_s, 
                        [&found, &ns](int id) {
                            if (found[id] == false) {
                                ns++;
                            }
                            found[id] = true;
                            return true; 
                        });
        }
        
        x1 = x1 - i;
        x2 = x2 + i;
        y1 = y1 - i;
        y2 = y2 + i;
    }

    cout << "n " << ns << "\n";
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

        tree.Search(low_s, high_s, MySearchCallback);
    }
}

// shape used, shapes inserted, number of neighbors
void search_neighbors_test(const Shape s, const vector<Shape> shapes, const int n) {
    typedef RTree<int, int, 3, float> MyTree;
    MyTree tree;

    unsigned int j;
    for (j = 0; j < shapes.size(); j++) {
        int low[] = {shapes[j].a.x, shapes[j].a.y, shapes[j].a.z};
        int high[] = {shapes[j].b.x, shapes[j].b.y, shapes[j].b.z};

        tree.Insert(low, high, j); // Note, all values including zero are fine in this version
    }

    // para cada interação aumenta *2 as dimensões procuradas
    // ex 1, 2, 4, 8 pros lados do shape s
    int ns = 0; // quantidade encontrada

    int x1 = s.a.x;
    int x2 = s.b.x;
    int y1 = s.a.y;
    int y2 = s.b.y;

    for (int i = 1; ns < n; i * 2) { // para quando encontrar a quantidade de retângulos n
        // testa ele mesmo

        {
            // em cima
            int low_s[] = {x1 - i, y1, s.a.z};
            int high_s[] = {x2 + i, y1 - i, s.b.z};
            ns += tree.Search(low_s, high_s, MySearchCallback);
        }

        {
            // em baixo
            int low_s[] = {x1 - i, y2, s.a.z};
            int high_s[] = {x2 + i, y2 + i, s.b.z};
            ns += tree.Search(low_s, high_s, MySearchCallback);
        }
        
        {
            // esquerda
            int low_s[] = {x1 - i, y1, s.a.z};
            int high_s[] = {x1, y2, s.b.z};
            ns += tree.Search(low_s, high_s, MySearchCallback);
        }
        
        {
            // direita
            int low_s[] = {x2, y1, s.a.z};
            int high_s[] = {x2 + i, y2, s.b.z};
            ns += tree.Search(low_s, high_s, MySearchCallback);
        }
        
        x1 = x1 - i;
        x2 = x2 + i;
        y1 = y1 - i;
        y2 = y2 + i;
    }
}

int main(int argc, char ** argv) {

    // rapidckeck tests
    // rc::check("Check that added shapes can be queried", test_inserted_shapes);
    // rc::check("Check that collected shapes are colliding with the query", test_rtree_collect);
    // rc::check("Check that not collected shapes collide with the query", test_rtree_collect_neg);
    rc::check("Check that nearests shapes collide with area", test_rtree_closest_n_shapes);

    

    // Input i;
    // parser::parse_file(i, argv[1]);
    // ofstream fout(argv[2]);

    // if (argc != 3) {
    //     cout << "usage: " << argv[0] << " <testcase> <output_file_for_solution>\n";
    //     return -1;
    // }

    // vector<Shape> shapes = get_routed_shapes(i);
    // vector<Shape> obstacles = get_obstacles(i);

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

    // search_neighbors_test(shapes[0], shapes, 100);
    // search_neighbors_test(obstacles[0], obstacles, 100);

    // auto t2 = std::chrono::high_resolution_clock::now();
    // cout << "the duration was " << duration_cast<microseconds>( t2 - t1 ).count() << "\n";
    
    return 0;
}