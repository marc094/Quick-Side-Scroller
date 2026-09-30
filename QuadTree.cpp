#include "QuadTree.h"
#include "PhysBody.h"

#define QUADTREE_MAX_DEPTH 48
#define QUADTREE_STACK_SIZE (4 * QUADTREE_MAX_DEPTH + 4)

static inline int Quadrant(svec2 centre, svec2 p)
{
	return (p.x >= centre.x ? 1 : 0) | (p.y >= centre.y ? 2 : 0);
}

int QuadTree::NewNode(svec2 centre, scalar half)
{
	nodes.emplace_back();
	nodes.back().centre = centre;
	nodes.back().half = half;
	return (int)nodes.size() - 1;
}

void QuadTree::Build(const PhysBody* bodies, int count)
{
	nodes.clear();
	nextBody.assign(count, -1);

	svec2 lo(0, 0), hi(0, 0);
	bool any = false;
	for (int i = 0; i < count; ++i)
	{
		if (!bodies[i].active)
			continue;

		if (!any)
		{
			lo = hi = bodies[i].pos;
			any = true;
		}
		lo.x = std::min(lo.x, bodies[i].pos.x);
		lo.y = std::min(lo.y, bodies[i].pos.y);
		hi.x = std::max(hi.x, bodies[i].pos.x);
		hi.y = std::max(hi.y, bodies[i].pos.y);
	}

	// Always create a root so queries on an empty tree are valid
	svec2 centre = (lo + hi) * 0.5;
	scalar half = std::max(hi.x - lo.x, hi.y - lo.y) * 0.5 * 1.001 + 1.0;
	NewNode(centre, half);

	if (!any)
		return;

	nodes.reserve(count * 2);
	for (int i = 0; i < count; ++i)
	{
		if (bodies[i].active)
			Insert(bodies, 0, i, 0);
	}

	Aggregate(bodies, 0);
}

void QuadTree::Insert(const PhysBody* bodies, int node, int body, int depth)
{
	while (true)
	{
		if (nodes[node].leaf)
		{
			if (nodes[node].firstBody == -1)
			{
				nodes[node].firstBody = body;
				nextBody[body] = -1;
				return;
			}

			if (depth >= QUADTREE_MAX_DEPTH)
			{
				// Can't subdivide further (coincident bodies): keep them as a chain
				nextBody[body] = nodes[node].firstBody;
				nodes[node].firstBody = body;
				return;
			}

			// Turn into an internal node and push the resident bodies down
			int chain = nodes[node].firstBody;
			nodes[node].firstBody = -1;
			nodes[node].leaf = false;
			while (chain != -1)
			{
				int next = nextBody[chain];
				Insert(bodies, node, chain, depth);
				chain = next;
			}
		}

		int q = Quadrant(nodes[node].centre, bodies[body].pos);
		if (nodes[node].child[q] == -1)
		{
			scalar quarter = nodes[node].half * 0.5;
			svec2 centre = nodes[node].centre;
			centre.x += (q & 1) ? quarter : -quarter;
			centre.y += (q & 2) ? quarter : -quarter;
			int created = NewNode(centre, quarter);	// may reallocate: don't hold references across this
			nodes[node].child[q] = created;
		}

		node = nodes[node].child[q];
		depth++;
	}
}

void QuadTree::Aggregate(const PhysBody* bodies, int node)
{
	Node& n = nodes[node];
	scalar mass = 0;
	svec2 weighted = sZero;
	scalar maxRadius = 0;

	if (n.leaf)
	{
		for (int b = n.firstBody; b != -1; b = nextBody[b])
		{
			mass += (scalar)bodies[b].mass;
			weighted += bodies[b].pos * (scalar)bodies[b].mass;
			maxRadius = std::max(maxRadius, bodies[b].circle.radius);
		}
	}
	else
	{
		for (int q = 0; q < 4; ++q)
		{
			if (n.child[q] == -1)
				continue;

			Aggregate(bodies, n.child[q]);
			const Node& c = nodes[n.child[q]];	// nodes isn't resized during Aggregate, so `n` stays valid
			mass += c.mass;
			weighted += c.com * c.mass;
			maxRadius = std::max(maxRadius, c.maxRadius);
		}
	}

	n.mass = mass;
	n.com = mass > 0 ? weighted / mass : n.centre;
	n.maxRadius = maxRadius;
}

void QuadTree::QueryOverlaps(const PhysBody* bodies, int self, std::vector<int>& out) const
{
	const PhysBody& a = bodies[self];
	int stack[QUADTREE_STACK_SIZE];
	int top = 0;
	stack[top++] = 0;

	while (top > 0)
	{
		const Node& n = nodes[stack[--top]];
		if (n.mass == 0)
			continue;

		// The cell, grown by the largest radius inside it plus ours, must contain our centre
		scalar reach = n.half + n.maxRadius + a.circle.radius;
		if (fabs(a.pos.x - n.centre.x) > reach || fabs(a.pos.y - n.centre.y) > reach)
			continue;

		if (n.leaf)
		{
			for (int b = n.firstBody; b != -1; b = nextBody[b])
			{
				if (b != self && bodies[b].active)
					out.push_back(b);
			}
		}
		else
		{
			for (int q = 0; q < 4; ++q)
			{
				if (n.child[q] != -1)
					stack[top++] = n.child[q];
			}
		}
	}
}

svec2 QuadTree::Force(const PhysBody* bodies, int self, scalar theta) const
{
	const PhysBody& a = bodies[self];
	const scalar thetaSqr = theta * theta;
	svec2 force = sZero;

	int stack[QUADTREE_STACK_SIZE];
	int top = 0;
	stack[top++] = 0;

	while (top > 0)
	{
		const Node& n = nodes[stack[--top]];
		if (n.mass == 0)
			continue;

		if (n.leaf)
		{
			for (int b = n.firstBody; b != -1; b = nextBody[b])
			{
				if (b == self)
					continue;

				svec2 distance = bodies[b].pos - a.pos;
				scalar sqrDistance = distance.sqrLength();
				if (sqrDistance <= 0)
					continue;

				// Bodies that overlap (only possible right after a merge) are softened
				scalar radiusSum = a.circle.radius + bodies[b].circle.radius;
				scalar softened = std::max(sqrDistance, radiusSum * radiusSum);
				force += distance * ((G_CONSTANT * a.mass * bodies[b].mass) / (softened * sqrt(sqrDistance)));
			}
			continue;
		}

		svec2 distance = n.com - a.pos;
		scalar sqrDistance = distance.sqrLength();
		bool inside = fabs(a.pos.x - n.centre.x) <= n.half && fabs(a.pos.y - n.centre.y) <= n.half;
		scalar size = n.half * 2;

		if (!inside && size * size < thetaSqr * sqrDistance)
		{
			force += distance * ((G_CONSTANT * a.mass * n.mass) / (sqrDistance * sqrt(sqrDistance)));
		}
		else
		{
			for (int q = 0; q < 4; ++q)
			{
				if (n.child[q] != -1)
					stack[top++] = n.child[q];
			}
		}
	}

	return force;
}
