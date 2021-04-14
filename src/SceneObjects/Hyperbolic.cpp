#include <cmath>

#include "Hyperbolic.h"



bool Hyperbolic::intersectLocal(const ray& r, isect& i) const
{
	vec3f p = r.getPosition();
	vec3f d = r.getDirection();

	double a = (d[0] * d[0] - d[1] * d[1] - d[2] * d[2]);
	double b = 2 * (p[0] * d[0] - p[1] * d[1] - p[2] * d[2]);
	double c = p[0] * p[0] - p[1] * p[1] - p[2] * p[2] - 1;

	double delta = b * b - 4 * a * c;
	if (delta < 0)
		return false;

	double t1 = (-b + sqrt(delta)) / 2.0 / a;
	double t2 = (-b - sqrt(delta)) / 2.0 / a;

	i.obj = this;
	if (t1 <= RAY_EPSILON && t2 <= RAY_EPSILON)
		return false;
	else if (t1 > RAY_EPSILON && t2 <= RAY_EPSILON)
	{
		i.t = t1;
		vec3f i_p = r.at(t1);
		i.N = -vec3f(2 * i_p[0], -2 * i_p[1], -2 * i_p[2]).normalize();
	}
	else if (t1 <= RAY_EPSILON && t2 > RAY_EPSILON)
	{
		i.t = t2;
		vec3f i_p = r.at(t2);
		i.N = -vec3f(2 * i_p[0], -2 * i_p[1], -2 * i_p[2]).normalize();
	}
	else
	{
		if (t1 < t2)
		{
			i.t = t1;
			vec3f i_p = r.at(t1);
			i.N = -vec3f(2 * i_p[0], -2 * i_p[1], -2 * i_p[2]).normalize();
		}
		else
		{
			i.t = t2;
			vec3f i_p = r.at(t2);
			i.N = -vec3f(2 * i_p[0], -2 * i_p[1], -2 * i_p[2]).normalize();
		}
	}

	return true;
}

