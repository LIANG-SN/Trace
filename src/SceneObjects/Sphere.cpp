#include <cmath>

#include "Sphere.h"



bool Sphere::intersectLocal( const ray& r, isect& i ) const
{
	vec3f v = -r.getPosition();
	double b = v.dot(r.getDirection());
	double discriminant = b*b - v.dot(v) + 1;

	if( discriminant < 0.0 ) {
		return false;
	}

	discriminant = sqrt( discriminant );
	double t2 = b + discriminant;

	if( t2 <= RAY_EPSILON ) {
		return false;
	}

	i.obj = this;

	double t1 = b - discriminant;

	if( t1 > RAY_EPSILON ) {
		i.t = t1;
		i.N = r.at( t1 ).normalize();
	} else {
		i.t = t2;
		i.N = r.at( t2 ).normalize();
	}

	return true;
}

bool Sphere::csg_intersectLocal(const ray& r, isect& min_t, isect& max_t) const
{
	vec3f v = -r.getPosition();
	double b = v.dot(r.getDirection());
	double discriminant = b * b - v.dot(v) + 1;

	if (discriminant < 0.0) {
		return false;
	}

	discriminant = sqrt(discriminant);
	double t1 = b - discriminant;
	double t2 = b + discriminant;

	if (t1 <= RAY_EPSILON || t2 <= RAY_EPSILON)
		return false;

	min_t.obj = this;
	min_t.t = t1;
	min_t.N = r.at(t1).normalize();

	max_t.obj = this;
	max_t.t = t2;
	max_t.N = r.at(t2).normalize();
	return true;

}