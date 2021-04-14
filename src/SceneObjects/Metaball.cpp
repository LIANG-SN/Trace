#include <cmath>

#include "Metaball.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

inline double g(vec3f r,double radius)
{
	double d = r.length_squared()/radius/radius;
	if (d > 0.5)
		return 0.0;
	else
	{
		double g = 0.25-d+d*d;
		//cout << g << endl;
		return g * radius * radius;
	}
	//return 1 / d;
}


inline vec3f g_grad(vec3f r, double radius)
{
	double d = r.length_squared() / radius / radius;
	if (d > 0.5)
		return vec3f(0,0,0);
	else
	{
		double dis = max(2 * d - 1, 1,0);
		return vec3f(2 * dis * r[0], 2 * dis * r[1], 2 * dis * r[2]);
	}
}
bool Metaball::intersectLocal(const ray& r, isect& i) const
{
	vec3f v = r.getPosition();
	vec3f d = r.getDirection().normalize();

	i.obj = this;
	vector<Ball> i_balls;
	double max_t = RAY_EPSILON;
	double min_t = 999.0;

	for (vector<Ball>::const_iterator j = balls.begin(); j != balls.end(); j ++)
	{
		double radius = (*j).radius;
		vec3f p = (*j).position;

		double dis = (p - v).length_squared()- ((p - v) *d)* ((p - v) * d);
		if (dis <= 0.5 * radius )
		{


			double a = (d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
			double b = 2 * (d[0] * (v[0] - p[0]) + d[1] * (v[1] - p[1]) + d[2] * (v[2] - p[2])) ;
			double c = ((v[0] - p[0]) * (v[0] - p[0]) + (v[1] - p[1]) * (v[1] - p[1])
				+ (v[2] - p[2]) * (v[2] - p[2])) - 0.5 * radius * radius;

			double delta = b * b - 4 * a * c;
			if (delta < 0)
				continue;

			double t1 = (-b + sqrt(delta)) / 2.0 / a;
			double t2 = (-b - sqrt(delta)) / 2.0 / a;

			if(t1< RAY_EPSILON && t2< RAY_EPSILON)
				continue;

			i_balls.push_back(*j);

			if (t1 > max_t)
				max_t = t1;

			if (t2 > max_t)
				max_t = t2;

			if (t1 < min_t && t1>RAY_EPSILON)
				min_t = t1;

			if (t2 < min_t && t2>RAY_EPSILON)
				min_t = t2;
		}
		else
			continue;
	}


	for (int a = 0; a < 500 && a * 0.01 < max_t - min_t; a++)
	{
		double t = min_t + a * 0.01;
		double sum = 0;
		for (vector<Ball>::const_iterator j = i_balls.begin(); j != i_balls.end(); j ++)
		{
			double radius = (*j).radius;
			vec3f p = (*j).position;
			sum += g(r.at(t)-p, radius);
		}

		if (sum >= 0.1)
		{
			i.t = t;
			i.N = vec3f(0, 0, 0);
			for (vector<Ball>::const_iterator j = i_balls.begin(); j != i_balls.end(); j ++)
			{
				double radius = (*j).radius;
				vec3f p = (*j).position;
				double a = (d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
				double b = 2 * (d[0] * (v[0] - p[0]) + d[1] * (v[1] - p[1]) + d[2] * (v[2] - p[2]));
				double c = ((v[0] - p[0]) * (v[0] - p[0]) + (v[1] - p[1]) * (v[1] - p[1])
					+ (v[2] - p[2]) * (v[2] - p[2])) - 0.5 * radius * radius;

				double delta = b * b - 4 * a * c;
				if (delta < 0)
					continue;

				double t1 = (-b + sqrt(delta)) / 2.0 / a;
				double t2 = (-b - sqrt(delta)) / 2.0 / a;
				if(t1*t2<0)
					i.N += -g(r.at(t)-p, radius)*(r.at(t) - p).normalize();
				else
					i.N += g(r.at(t) - p, radius) * (r.at(t) - p).normalize();
			}
			i.N.normalize();
			return true;
		}
	}

	return false;
}





		//double a = (d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
		//double b = 2 * (d[0] * (v[0] - p[0]) + d[1] * (v[1] - p[1]) + d[2] * (v[2] - p[2]));
		//double c = (v[0] - p[0]) * (v[0] - p[0]) + (v[1] - p[1]) * (v[1] - p[1])
		//	+ (v[2] - p[2]) * (v[2] - p[2]) - radius * radius;

		//double delta = b * b - 4 * a * c;
		//if (delta < 0)
		//	continue;

		//double t1 = (-b + sqrt(delta)) / 2.0 / a;
		//double t2 = (-b - sqrt(delta)) / 2.0 / a;

		//i.obj = this;
		//if (t1 <= RAY_EPSILON && t2 <= RAY_EPSILON)
		//	return false;
		//else if (t1 > RAY_EPSILON && t2 <= RAY_EPSILON)
		//{
		//	i.t = t1;
		//	vec3f i_p = r.at(t1);
		//	i.N = -vec3f(2 * i_p[0], -1, 2 * i_p[2]).normalize();
		//}
		//else if (t1 <= RAY_EPSILON && t2 > RAY_EPSILON)
		//{
		//	i.t = t2;
		//	vec3f i_p = r.at(t2);
		//	i.N = -vec3f(2 * i_p[0], -1, 2 * i_p[2]).normalize();
		//}
		//else
		//{
		//	if (t1 < t2)
		//	{
		//		i.t = t1;
		//		vec3f i_p = r.at(t1);
		//		i.N = vec3f(2 * i_p[0] + v[0] - p[0], 2 * i_p[1] + v[1] - p[1], 2 * i_p[2] + v[2] - p[2]).normalize();
		//	}
		//	else
		//	{
		//		i.t = t2;
		//		vec3f i_p = r.at(t2);
		//		i.N = vec3f(2 * i_p[0] + v[0] - p[0], 2 * i_p[1] + v[1] - p[1], 2 * i_p[2] + v[2] - p[2]).normalize();
		//	}
		//}
		//return true;
