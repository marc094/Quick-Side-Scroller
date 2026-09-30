#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include "Defs.h"
#include "vec2.h"
#include <vector>

class PhysBody;

// Barnes-Hut quadtree over the active bodies of an array. Bodies are referred to by their array index.
// The tree is a snapshot: rebuild it after bodies move.
class QuadTree
{
public:
	void Build(const PhysBody* bodies, int count);

	// Appends the index of every active body (other than `self`) whose circle may overlap the one of `self`
	void QueryOverlaps(const PhysBody* bodies, int self, std::vector<int>& out) const;

	// Gravitational force on `self`. Cells with size/distance < theta are treated as point masses.
	svec2 Force(const PhysBody* bodies, int self, scalar theta) const;

private:
	struct Node
	{
		svec2 centre;
		scalar half = 0;			// half of the cell width
		int child[4] = { -1, -1, -1, -1 };
		bool leaf = true;
		int firstBody = -1;			// leaves: head of a chain of bodies (>1 only at the depth limit)
		scalar mass = 0;
		svec2 com = sZero;
		scalar maxRadius = 0;
	};

	int NewNode(svec2 centre, scalar half);
	void Insert(const PhysBody* bodies, int node, int body, int depth);
	void Aggregate(const PhysBody* bodies, int node);

	std::vector<Node> nodes;
	std::vector<int> nextBody;
};

#endif
