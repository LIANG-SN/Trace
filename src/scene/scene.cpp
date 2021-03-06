#include <cmath>

#include "scene.h"
#include "light.h"
#include "../ui/TraceUI.h"
#include "../SceneObjects/CSG.h"
extern TraceUI* traceUI;
void BoundingBox::operator=(const BoundingBox& target)
{
	min = target.min;
	max = target.max;
}

// Does this bounding box intersect the target?
bool BoundingBox::intersects(const BoundingBox &target) const
{
	return ((target.min[0] - RAY_EPSILON <= max[0]) && (target.max[0] + RAY_EPSILON >= min[0]) &&
			(target.min[1] - RAY_EPSILON <= max[1]) && (target.max[1] + RAY_EPSILON >= min[1]) &&
			(target.min[2] - RAY_EPSILON <= max[2]) && (target.max[2] + RAY_EPSILON >= min[2]));
}

// does the box contain this point?
bool BoundingBox::intersects(const vec3f& point) const
{
	return ((point[0] + RAY_EPSILON >= min[0]) && (point[1] + RAY_EPSILON >= min[1]) && (point[2] + RAY_EPSILON >= min[2]) &&
		 (point[0] - RAY_EPSILON <= max[0]) && (point[1] - RAY_EPSILON <= max[1]) && (point[2] - RAY_EPSILON <= max[2]));
}

// if the ray hits the box, put the "t" value of the intersection
// closest to the origin in tMin and the "t" value of the far intersection
// in tMax and return true, else return false.
// Using Kay/Kajiya algorithm.
bool BoundingBox::intersect(const ray& r, double& tMin, double& tMax) const
{
	vec3f R0 = r.getPosition();
	vec3f Rd = r.getDirection();

	tMin = -1.0e308; // 1.0e308 is close to infinity... close enough for us!
	tMax = 1.0e308;
	double ttemp;
	
	for (int currentaxis = 0; currentaxis < 3; currentaxis++)
	{
		double vd = Rd[currentaxis];
		
		// if the ray is parallel to the face's plane (=0.0)
		if( vd == 0.0 )
			continue;

		double v1 = min[currentaxis] - R0[currentaxis];
		double v2 = max[currentaxis] - R0[currentaxis];

		// two slab intersections
		double t1 = v1/vd;
		double t2 = v2/vd;
		
		if ( t1 > t2 ) { // swap t1 & t2
			ttemp = t1;
			t1 = t2;
			t2 = ttemp;
		}

		if (t1 > tMin)
			tMin = t1;
		if (t2 < tMax)
			tMax = t2;

		if (tMin > tMax) // box is missed
			return false;
		if (tMax < 0.0) // box is behind ray
			return false;
	}
	return true; // it made it past all 3 axes.
}


bool Geometry::intersect(const ray&r, isect&i) const
{
    // Transform the ray into the object's local coordinate space
    vec3f pos = transform->globalToLocalCoords(r.getPosition());
    vec3f dir = transform->globalToLocalCoords(r.getPosition() + r.getDirection()) - pos;
    double length = dir.length();
    dir /= length;

    ray localRay( pos, dir );

    if (intersectLocal(localRay, i)) {
        // Transform the intersection point & normal returned back into global space.
		i.N = transform->localToGlobalCoordsNormal(i.N);
		i.t /= length;

		return true;
    } else {
        return false;
    } 
}

bool Geometry::csg_intersect(const ray& r, isect& min_t, isect& max_t) const
{
	vec3f pos = transform->globalToLocalCoords(r.getPosition());
	vec3f dir = transform->globalToLocalCoords(r.getPosition() + r.getDirection()) - pos;
	double length = dir.length();
	dir /= length;

	ray localRay(pos, dir);

	if (csg_intersectLocal(localRay, min_t, max_t)) {
		// Transform the intersection point & normal returned back into global space.
		min_t.N = transform->localToGlobalCoordsNormal(min_t.N);
		min_t.t /= length;

		max_t.N = transform->localToGlobalCoordsNormal(max_t.N);
		max_t.t /= length;

		return true;
	}
	else {
		return false;
	}
}

bool Geometry::intersectLocal( const ray& r, isect& i ) const
{
	return false;
}

bool Geometry::csg_intersectLocal(const ray& r, isect& min_t, isect& max_t) const
{
	return false;
}

bool Geometry::hasBoundingBoxCapability() const
{
	// by default, primitives do not have to specify a bounding box.
	// If this method returns true for a primitive, then either the ComputeBoundingBox() or
    // the ComputeLocalBoundingBox() method must be implemented.

	// If no bounding box capability is supported for an object, that object will
	// be checked against every single ray drawn.  This should be avoided whenever possible,
	// but this possibility exists so that new primitives will not have to have bounding
	// boxes implemented for them.
	
	return false;
}

Scene::~Scene()
{
    giter g;
    liter l;
    
	for( g = objects.begin(); g != objects.end(); ++g ) {
		delete (*g);
	}

	for( g = boundedobjects.begin(); g != boundedobjects.end(); ++g ) {
		delete (*g);
	}

	for( g = nonboundedobjects.begin(); g != boundedobjects.end(); ++g ) {
		delete (*g);
	}

	for( l = lights.begin(); l != lights.end(); ++l ) {
		delete (*l);
	}
}

// Get any intersection with an object.  Return information about the 
// intersection through the reference parameter.
bool Scene::intersect( const ray& r, isect& i ) const
{
	typedef list<Geometry*>::const_iterator iter;
	iter j;

	isect cur;
	bool have_one = false;

	iter last = objects.end();
	last--;
	if (dynamic_cast<CSG*>(*last))
	{
		if ((*last)->intersect(r, cur)) {
			if (!have_one || (cur.t < i.t)) {
				i = cur;
				have_one = true;
			}
		}
		return have_one;
	}

	// try the non-bounded objects
	for( j = nonboundedobjects.begin(); j != nonboundedobjects.end(); ++j ) {
		if( (*j)->intersect( r, cur ) ) {
			if( !have_one || (cur.t < i.t) ) {
				i = cur;
				have_one = true;
			}
		}
	}

	// try the bounded objects
	if (traceUI->use_bvh)
	{
		have_one = bvh->intersect(r, i);
	}
	else
	{
	  for( j = boundedobjects.begin(); j != boundedobjects.end(); ++j ) {
	  	if( (*j)->intersect( r, cur ) ) {
	  		if( !have_one || (cur.t < i.t) ) {
	  			i = cur;
	  			have_one = true;
	  		}
	  	}

	  }
	}

	return have_one;
}

void Scene::initScene()
{
	bool first_boundedobject = true;
	BoundingBox b;
	
	typedef list<Geometry*>::const_iterator iter;
	// split the objects into two categories: bounded and non-bounded
	for( iter j = objects.begin(); j != objects.end(); ++j ) {
		if( (*j)->hasBoundingBoxCapability() )
		{
			boundedobjects.push_back(*j);

			// widen the scene's bounding box, if necessary
			if (first_boundedobject) {
				sceneBounds = (*j)->getBoundingBox();
				first_boundedobject = false;
			}
			else
			{
				b = (*j)->getBoundingBox();
				sceneBounds.max = maximum(sceneBounds.max, b.max);
				sceneBounds.min = minimum(sceneBounds.min, b.min);
			}
		}
		else
			nonboundedobjects.push_back(*j);
	}
	objects.sort([](Geometry* x, Geometry* y) -> bool 
		{
			BoundingBox box1 = x->getBoundingBox();
			BoundingBox box2 = y->getBoundingBox();
			return box1.min[0] + box1.max[0] < box2.min[0] + box2.max[0]; 
		} );
	bvh = new BVH(boundedobjects.begin(), boundedobjects.end());
}



void Scene::getGradientOfPoint(const int x, const int y, int& Gx, int& Gy)
{
	if (x < 1 || x > m_nBumpMap_width-2 || y < 1 || y > m_nBumpMap_height-2)
	{
		Gx = Gy = 0;
		return;
	}

	int sobel_x[3][3] =
	{
		{ 1, 0, -1 },
		{ 2, 0, -2 },
		{ 1, 0, -1 }
	};
	Gx = 0;
	Gy = 0;
	for (int a = -1; a <= 1; a++)
	{
		for (int b = -1; b <= 1; b++)
		{
			int i = x + a;
			int j = y + b;
			unsigned char* pixel = m_nBumpMap + (i + j * m_nBumpMap_width) * 3;
			// formula from tutorial doc page 19
			int pixelValue = 0.299 * pixel[0] + 0.587 * pixel[1] + 0.114 * pixel[2];
			Gx += sobel_x[a + 1][b + 1] * pixelValue;
			Gy += sobel_x[b + 1][a + 1] * pixelValue;
		}
	}

}


BVHNode::BVHNode(list<Geometry*>::const_iterator start,
	list<Geometry*>::const_iterator end, int size)
	: left(left), right(right)
{
	if (size == 1)
	{
		obj = (*start);
		bound = nullptr;
		left = nullptr;
		right = nullptr;
	}
	else
	{
		obj = nullptr;

		// bound = compute bound
		bound = new BoundingBox;
		for (list<Geometry*>::const_iterator p = start; p != end; p++)
		{
			if (p == start)
			{
				(*bound) = (*p)->getBoundingBox();
			}
			else
			{
				BoundingBox temp = (*p)->getBoundingBox();
				bound->max = maximum(bound->max, temp.max);
				bound->min = minimum(bound->min, temp.min);
			}
		}

		Scene::cgiter mid = start;
		std::advance(mid, size / 2);


		int size_l = size / 2;
		int size_r = size - size_l;
		left = new BVHNode(start, mid, size_l);
		right = new BVHNode(mid, end, size_r);
	}
}

bool BVHNode::intersect(const ray& r, isect& i) const
{
	if (bound == nullptr)
	{
		return obj->intersect(r, i);
	}
	else
	{
		double tMin = 0, tMax = 0;
		if (bound->intersect(r, tMin, tMax))
		{
			isect i_left;
			isect i_right;
			// intersect left and right
			bool intersect_l = left->intersect(r, i_left);
			bool intersect_r = right->intersect(r, i_right);

			if (intersect_l && intersect_r)
			{
				if (i_left.t < i_right.t)
					i = i_left;
				else
					i = i_right;
				return true;
			}
			else if (intersect_l)
			{
				i = i_left;
				return true;
			}
			else if (intersect_r)
			{
				i = i_right;
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
}

BVHNode::~BVHNode()
{
	if (bound != nullptr)
	{
		delete bound;
		delete left;
		delete right;
	}
}

BVH::BVH(list<Geometry*>::const_iterator begin,
	list<Geometry*>::const_iterator end)
{
	int size = 0;
	list<Geometry*>::const_iterator p = begin;
	while (p != end)
	{
		size++;
		p++;
	}

	root = new BVHNode(begin, end, size);
}

bool BVH::intersect(const ray& r, isect& i) const
{
	// cout << "bvh intersect" << endl;
	return root->intersect(r, i);
}

BVH::~BVH()
{
	delete root;
}
