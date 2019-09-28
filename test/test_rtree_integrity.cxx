// #include <parser.hpp>
#include <router.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>
#include <rapidcheck.h>
#include "generators.hpp"

#include <RTree.h>
#include <RTree_utils.hpp>

using namespace std;
using namespace std::chrono;
using namespace iccad;


bool KeepGoingCallback(int id, int found_shapes) {
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

// void test_rtree_closest_n_shapes(const Shape s, const vector<Shape> shapes, unsigned int n) {
//     if (shapes.size() == 0 || n == 0) {
//         RC_DISCARD("discarding empty testcase");
//     }

//     // std::vector<int[3]> low, high;
//     typedef RTree<int, int, 3, float> MyTree;
//     MyTree tree;

//     // insert on tree
//     for (int j = 0; j < shapes.size(); j++) {
//         int low[] = {shapes[j].a.x, shapes[j].a.y, 0};
//         int high[] = {shapes[j].b.x, shapes[j].b.y, 0};

//         tree.Insert(low, high, j);
//     }

//     // search nearests
//     ns = 0;

//     int x1 = s.a.x;
//     int x2 = s.b.x;
//     int y1 = s.a.y;
//     int y2 = s.b.y;
//     vector<bool> found(shapes.size(), false); // salva os shapes que colidem

//     for (int i = 1; ns < n && i < 123456789; i = i * 2) { // para quando encontrar a quantidade de retângulos n
//         // testa ele mesmo

//         // cout << i << "\n";

//         {
//             // em cima
//             int low_s[] = {x1 - i, y1 - i, 0};
//             int high_s[] = {x2 + i, y1, 0};
//             tree.Search(low_s, high_s,
//                         [&shapes, &found, &low_s, &high_s](int id) {
//                             if (found[id] == false) {
//                                 Shape aux;
//                                 aux.a.x = low_s[0];
//                                 aux.b.x = high_s[0];
//                                 aux.a.y = low_s[1];
//                                 aux.b.y = high_s[1];
//                                 aux.a.z = shapes[id].a.z;
//                                 aux.b.z = shapes[id].b.z;
//                                 RC_ASSERT(collides(shapes[id], aux));
//                                 ns++;
//                             }
//                             found[id] = true;
//                             return true; 
//                         });
//         }

//         {
//             // em baixo
//             int low_s[] = {x1 - i, y2, 0};
//             int high_s[] = {x2 + i, y2 + i, 0};
//             tree.Search(low_s, high_s,
//                         [&shapes, &found, &low_s, &high_s](int id) {
//                             if (found[id] == false) {
//                                 Shape aux;
//                                 aux.a.x = low_s[0];
//                                 aux.b.x = high_s[0];
//                                 aux.a.y = low_s[1];
//                                 aux.b.y = high_s[1];
//                                 aux.a.z = shapes[id].a.z;
//                                 aux.b.z = shapes[id].b.z;
//                                 RC_ASSERT(collides(shapes[id], aux));
//                                 ns++;
//                             }
//                             found[id] = true;
//                             return true; 
//                         });
//         }
        
//         {
//             // esquerda
//             int low_s[] = {x1 - i, y1, 0};
//             int high_s[] = {x1, y2, 0};
//             tree.Search(low_s, high_s,
//                         [&shapes, &found, &low_s, &high_s](int id) {
//                             if (found[id] == false) {
//                                 Shape aux;
//                                 aux.a.x = low_s[0];
//                                 aux.b.x = high_s[0];
//                                 aux.a.y = low_s[1];
//                                 aux.b.y = high_s[1];
//                                 aux.a.z = shapes[id].a.z;
//                                 aux.b.z = shapes[id].b.z;
//                                 RC_ASSERT(collides(shapes[id], aux));
//                                 ns++;
//                             }
//                             found[id] = true;
//                             return true; 
//                         });
//         }
        
//         {
//             // direita
//             int low_s[] = {x2, y1, 0};
//             int high_s[] = {x2 + i, y2, 0};
//             tree.Search(low_s, high_s,
//                         [&shapes, &found, &low_s, &high_s](int id) {
//                             if (found[id] == false) {
//                                 Shape aux;
//                                 aux.a.x = low_s[0];
//                                 aux.b.x = high_s[0];
//                                 aux.a.y = low_s[1];
//                                 aux.b.y = high_s[1];
//                                 aux.a.z = shapes[id].a.z;
//                                 aux.b.z = shapes[id].b.z;
//                                 RC_ASSERT(collides(shapes[id], aux));
//                                 ns++;
//                             }
//                             found[id] = true;
//                             return true; 
//                         });
//         }
        
//         x1 = x1 - i;
//         x2 = x2 + i;
//         y1 = y1 - i;
//         y2 = y2 + i;
//     }
// }

void test_rtree_closest_n_shapes(const Shape s, const vector<Shape> shapes, unsigned int n) {
    if (shapes.size() == 0 || n == 0) {
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

    vector<bool> found(shapes.size(), false); // salva os shapes que colidem

    for (int i = 1; ns < n && i < 123456789; i = i * 2) { // para quando encontrar a quantidade de retângulos n

        {
            shape aux;
            // em cima
            int low_s[] = {x1 - i, y1 - i, 0};
            int high_s[] = {x2 + i, y1, 0};

            rtree_search_shape();

        }

        {
            // em baixo
            int low_s[] = {x1 - i, y2, 0};
            int high_s[] = {x2 + i, y2 + i, 0};
            tree.Search(low_s, high_s,
                        [&shapes, &found, &low_s, &high_s](int id) {
                            if (found[id] == false) {
                                Shape aux;
                                aux.a.x = low_s[0];
                                aux.b.x = high_s[0];
                                aux.a.y = low_s[1];
                                aux.b.y = high_s[1];
                                aux.a.z = shapes[id].a.z;
                                aux.b.z = shapes[id].b.z;
                                RC_ASSERT(collides(shapes[id], aux));
                                ns++;
                            }
                            found[id] = true;
                            return true; 
                        });
        }
        
        {
            // esquerda
            int low_s[] = {x1 - i, y1, 0};
            int high_s[] = {x1, y2, 0};
            tree.Search(low_s, high_s,
                        [&shapes, &found, &low_s, &high_s](int id) {
                            if (found[id] == false) {
                                Shape aux;
                                aux.a.x = low_s[0];
                                aux.b.x = high_s[0];
                                aux.a.y = low_s[1];
                                aux.b.y = high_s[1];
                                aux.a.z = shapes[id].a.z;
                                aux.b.z = shapes[id].b.z;
                                RC_ASSERT(collides(shapes[id], aux));
                                ns++;
                            }
                            found[id] = true;
                            return true; 
                        });
        }
        
        {
            // direita
            int low_s[] = {x2, y1, 0};
            int high_s[] = {x2 + i, y2, 0};
            tree.Search(low_s, high_s,
                        [&shapes, &found, &low_s, &high_s](int id) {
                            if (found[id] == false) {
                                Shape aux;
                                aux.a.x = low_s[0];
                                aux.b.x = high_s[0];
                                aux.a.y = low_s[1];
                                aux.b.y = high_s[1];
                                aux.a.z = shapes[id].a.z;
                                aux.b.z = shapes[id].b.z;
                                RC_ASSERT(collides(shapes[id], aux));
                                ns++;
                            }
                            found[id] = true;
                            return true; 
                        });
        }
        
        s.expand(i);
    }
}


int main(int argc, char ** argv) {

    // rapidckeck tests
    rc::check("Check that added shapes can be queried", test_inserted_shapes);
    rc::check("Check that collected shapes are colliding with the query", test_rtree_collect);
    rc::check("Check that not collected shapes collide with the query", test_rtree_collect_neg);
    rc::check("Check that nearests shapes collide with area", test_rtree_closest_n_shapes);

    return 0;
}