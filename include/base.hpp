#pragma once

#include <array>

namespace iccad {
    /**
     * An array representing a 3D point.
     * Each position represents the x, y and z coordinates, in this order.
     */
    using PT = std::array<int, 3>;
    /**
     * Two points that represent the oposite points of a cuboid
     * The first point is the low value for the x, y and z coordinates.
     * The second point is the high value for the x, y, and z coordinates.
     */
    using Shape = std::array<PT, 2>;


    PT min(PT a, PT b) {
        using std::min;
        return PT{
            min(a[0], b[0]), 
            min(a[1], b[1]), 
            min(a[2], b[2])};
    }

    PT max(PT a, PT b) {
        using std::max;
        return PT{
            max(a[0], b[0]), 
            max(a[1], b[1]), 
            max(a[2], b[2])};
    }


}
