#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.
	double t[6] = { -1 };
	vec3f p = r.getPosition();
	vec3f d = r.getDirection();

	if (d[0] != 0.0) // x
	{
		t[0] = (0.5 - p[0]) / d[0];
		t[1] = (-0.5 - p[0]) / d[0];
	}
	if (d[1] != 0.0) // y
	{
		t[2] = (0.5 - p[1]) / d[1];
		t[3] = (-0.5 - p[1]) / d[1];
	}
	if (d[2] != 0.0) // z
	{
		t[4] = (0.5 - p[2]) / d[2];
		t[5] = (-0.5 - p[2]) / d[2];
	}
	double t0 = 999;
	bool intersect = false;
	for (int k = 0; k < 6; k++)
	{
		if (t[k] > RAY_EPSILON && t[k] < t0)
		{
			vec3f pos = r.at(t[k]);
			double bound = 0.5 + RAY_EPSILON;
			if (abs(pos[0]) <= bound && abs(pos[1]) <= bound && abs(pos[2]) <= bound)
			{
				intersect = true;
				t0 = t[k];
				i.obj = this;
				i.t = t0;
				switch (k)
				{
				case 0:
					i.N = vec3f(1.0, 0.0, 0.0);
					break;
				case 1:
					i.N = vec3f(-1.0, 0.0, 0.0);
					break;
				case 2:
					i.N = vec3f(0.0, 1.0, 0.0);
					break;
				case 3:
					i.N = vec3f(0.0, -1.0, 0.0);
					break;
				case 4:
					i.N = vec3f(0.0, 0.0, 1.0);
					break;
				case 5:
					i.N = vec3f(0.0, 0.0, -1.0);
				}

			}
		}
	}
	return intersect;
}


bool Box::csg_intersectLocal(const ray& r, isect& min_t , isect& max_t) const
{
	// YOUR CODE HERE:
// Add box intersection code here.
// it currently ignores all boxes and just returns false.
	double t[6] = { -1 };
	vec3f p = r.getPosition();
	vec3f d = r.getDirection();

	if (d[0] != 0.0) // x
	{
		t[0] = (0.5 - p[0]) / d[0];
		t[1] = (-0.5 - p[0]) / d[0];
	}
	if (d[1] != 0.0) // y
	{
		t[2] = (0.5 - p[1]) / d[1];
		t[3] = (-0.5 - p[1]) / d[1];
	}
	if (d[2] != 0.0) // z
	{
		t[4] = (0.5 - p[2]) / d[2];
		t[5] = (-0.5 - p[2]) / d[2];
	}
	double least_t = 999;
	double largest_t = RAY_EPSILON;
	bool intersect = false;
	for (int k = 0; k < 6; k++)
	{
		if (t[k] > RAY_EPSILON && t[k] < 999)
		{
			vec3f pos = r.at(t[k]);
			double bound = 0.5 + RAY_EPSILON;
			if (abs(pos[0]) <= bound && abs(pos[1]) <= bound && abs(pos[2]) <= bound)
			{
				intersect = true;
				min_t.obj = this;
				max_t.obj = this;
				if (t[k] > largest_t)
				{
					max_t.t = t[k];
					largest_t = t[k];
					switch (k)
					{
					case 0:
						max_t.N = vec3f(1.0, 0.0, 0.0);
						break;
					case 1:
						max_t.N = vec3f(-1.0, 0.0, 0.0);
						break;
					case 2:
						max_t.N = vec3f(0.0, 1.0, 0.0);
						break;
					case 3:
						max_t.N = vec3f(0.0, -1.0, 0.0);
						break;
					case 4:
						max_t.N = vec3f(0.0, 0.0, 1.0);
						break;
					case 5:
						max_t.N = vec3f(0.0, 0.0, -1.0);
					}
				}

				if(t[k] < least_t)
				{
					min_t.t = t[k];
					least_t = t[k];
					switch (k)
					{
					case 0:
						min_t.N = vec3f(1.0, 0.0, 0.0);
						break;
					case 1:
						min_t.N = vec3f(-1.0, 0.0, 0.0);
						break;
					case 2:
						min_t.N = vec3f(0.0, 1.0, 0.0);
						break;
					case 3:
						min_t.N = vec3f(0.0, -1.0, 0.0);
						break;
					case 4:
						min_t.N = vec3f(0.0, 0.0, 1.0);
						break;
					case 5:
						min_t.N = vec3f(0.0, 0.0, -1.0);
					}
				}

			}
		}
	}

	return intersect;
}
