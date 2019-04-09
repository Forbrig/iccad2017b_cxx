#pragma once

#include <base.hpp>
#include <memory>
#include <iostream>

namespace iccad {
    using std::unique_ptr, std::make_unique,
          iccad::min, iccad::max,
          std::cout;



    struct Node {
        const Shape x;
        PT low, high;
        int count;

        unique_ptr<Node> left, right;

        void add(const Shape & shape, int level=0) {
            int a = x[0][level % 3];
            int b = x[1][level % 3];
            int l = shape[0][level % 3];
            int r = shape[1][level % 3];
            if(r < a) {
                if(left) {
                    left->add(shape, level+1);
                }
                else {
                    left = make_unique<Node>(Node{shape, shape[0], shape[1], 1});
                }
            }
            else {
                if(right) {
                    right->add(shape, level+1);
                }
                else {
                    right = make_unique<Node>(Node{shape, shape[0], shape[1], 1});
                }
            }
            if(left) {
                low = min(low, left->low);
                high = max(high, left->high);
            }
            if(right) {
                low = min(low, right->low);
                high = max(high, right->high);
            }
            count = 1 + (left?left->count:0) + (right?right->count:0);
        }

        int query(const PT l, const PT r, int level = 0) {

            //int a = x[0][level % 3];
            //int b = x[1][level % 3];
            //int l = shape[0][level % 3];
            //int r = shape[1][level % 3];

            //printf("query(%d, %d) -> (%d, %d)\n", l, r, a, b);

            if(l <= low && r >= high) {
                return count;
            }
            if(l > high || r < low) {
                return 0;
            }

            // TODO
            //bool hits = (l <= b && r >= b) || (l <= a && r >= a);
            //if(hits) { printf("(%d ,%d)\n", a, b); }
            bool hits = false;

            return (hits?1:0) 
                + (left?left->query(l, r, level+1):0) 
                + (right?right->query(l, r, level+1):0);
        }

        void print(int h=0, int level=0) {
            int a = x[0][level % 3];
            int b = x[1][level % 3];

            if(right) {
                //for(int i = 0; i < h; ++i) { cout << ' '; }
                //cout << "r\n";
                right->print(h+2, level+1);
            }
            for(int i = 0; i < h; ++i) { cout << ' '; }
            printf("%d:{{(%d, %d, %d), (%d , %d, %d)} (%d, %d, %d) (%d, %d, %d) %d}\n",
                    level,
                    x[0][0], 
                    x[0][1], 
                    x[0][2], 
                    x[1][0], 
                    x[1][1], 
                    x[1][2], 
                    low[0], 
                    low[1], 
                    low[2], 
                    high[0], 
                    high[1], 
                    high[2], 
                    count);
            if(left) {
                //for(int i = 0; i < h; ++i) { cout << ' '; }
                //cout << "l\n";
                left->print(h+4, level+1);
            }
        }
    };

}
