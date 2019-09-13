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

// return the shapes found
// bool shapesFound(int* min, int* max, int id) {
//   cout << "Hit data rect " << id << "\n";
//   cout << "In Rect [" << min[0] << "," << min[1] << "], [" << max[0] << "," << max[1] << "]\n";
//   return true; // keep going
// }

// int main(int argc, char ** argv) {
//     Input i;
//     parser::parse_file(i, argv[1]);
//     ofstream fout(argv[2]);

//     if (argc != 3) {
//         cout << "usage: " << argv[0] << " <testcase> <output_file_for_solution>\n";
//         return -1;
//     }
//     vector<Shape> shapes = get_routed_shapes(i);
//     vector<Shape> obstacles = get_obstacles(i);

// 	typedef RTree<int, int, 3, float> MyTree;
//     MyTree tree;

//     unsigned int j;
//     cout << "number of rectangles is " << shapes.size() + obstacles.size() << "\n";

//     auto t1 = std::chrono::high_resolution_clock::now();

//     for (j = 0; j < shapes.size(); j++) {
//         int low[] = {shapes[j].a.x, shapes[j].a.y, shapes[j].a.z};
//         int high[] = {shapes[j].b.x, shapes[j].b.y, shapes[j].b.z};

//         tree.Insert(low, high, j); // Note, all values including zero are fine in this version
//     }

//     auto t2 = std::chrono::high_resolution_clock::now();
//     cout << "the duration was " << duration_cast<microseconds>( t2 - t1 ).count() << "\n";


//     unsigned int nhits;
    
    
//     int low[] = {shapes[j].a.x, shapes[j].a.y, shapes[j].a.z};
//     int high[] = {shapes[j].b.x, shapes[j].b.y, shapes[j].b.z};
    
//     int search_low[] = {63445,114074, 0};
//     int search_high[] = {63466,115543, 0};
//     nhits = tree.Search(search_low, search_high, MySearchCallback);

//     // int search_rect(5463, 3699, 0, 5467, 3714, 0);
//     // nhits = tree.Search(search_rect.min, search_rect.max, MySearchCallback);

//     // Rect search_rect2(904, 415, 0, 908, 458, 0);
//     // nhits = tree.Search(search_rect2.min, search_rect2.max, MySearchCallback);

//     // auto search_low = {3465, 3332, 0};
//     // auto search_hight = {3497, 3348, 0};
//     // tree.Search(search_low, search_hight, MySearchCallback);
    
//     return 0;
// }


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

        int found = tree.Search(low, high, MySearchCallback);
        RC_ASSERT(found > 0);
    }
}

void test_rtree_collect(const Shape s, const vector<Shape> shapes) {

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

    int low_s[] = {s.a.x, s.a.y, s.a.z};
    int high_s[] = {s.b.x, s.b.y, s.b.z};
    int found = tree.Search(low_s, high_s, 
            [&shapes, &s](int id) { 
                RC_ASSERT(collides(shapes[id], s));
                return true; 
            });

    // cout << "numero de colisões: " << found << "\n";
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

    int low_s[] = {s.a.x, s.a.y, s.a.z};
    int high_s[] = {s.b.x, s.b.y, s.b.z};
    int found = tree.Search(low_s, high_s, 
            [](int id) { 
                return true; 
            });

    if (found == 0) {
       for (int i = 0; i < shapes.size(); i++) {
           RC_ASSERT(!collides(shapes[i], s));
       }
    }
} 

int main(int argc, char ** argv) {
    rc::check("Check that added shapes can be queried", test_inserted_shapes);

    rc::check("Check that collected shapes are colliding with the query", test_rtree_collect);

    rc::check("Check that not collected shapes collide with the query", test_rtree_collect_neg);

    return 0;
}