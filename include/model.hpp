#pragma once

#include <vector>
#include <set>
#include <utility>

namespace model
{

/**
 * Represents either a point, with size 2
 * or a rectangle with size 4.
 */
using V1D = std::vector<unsigned int>;

struct Input {

  int viaCost, spacing, layers, n_shapes, n_vias, n_obst;
  V1D boundary;

  std::vector<std::vector<V1D>> shapes;
  std::vector<std::vector<V1D>> vias;
  std::vector<std::vector<V1D>> obstacles;
};

struct P3D {
  unsigned x,y,z;
};
bool operator<(const P3D & p1, const P3D & p2);

/**
 * Stores the shapes as points
 */
struct Grid {
  // grid is a set with all the points for all the shapes, vias and obstacles
  std::set<P3D> grid;

  // keypoints is a set with all the points that must be connected
  std::set<P3D> keypoints;

	void add_shape(unsigned int layer, const V1D & v);
	void add_via(unsigned int l1, unsigned int l2, const V1D & v);
	void add_obstacle(unsigned int layer, const V1D & v);

private:
	void add_edge(const P3D p1, const P3D p2);
};

/**
 * Stores the shape for windowing query
 */
struct Shapes {

  // This should build a tree-like structure to query the shapes to be routed,
  // or obstacles to be avoided
  

  void add_shape(unsigned int layer, const V1D & v);
  void add_via(unsigned int l1, unsigned int l2, const V1D & v);
  void add_obstacle(unsigned int layer, const V1D & v);
};

void convert(const Input & i, Grid & g);

}
