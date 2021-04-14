#ifndef __METABALL_H__
#define __METABALL_H__

#include "../scene/scene.h"
#include <vector>



class Ball {
public:
	Ball(vec3f p, double r)
	{
		position = p; radius = r;
	};
	vec3f position;
	double radius;
};

class Metaball
	: public MaterialSceneObject
{
public:
	vector<Ball> balls;
	Metaball(Scene* scene, Material* mat)
		: MaterialSceneObject(scene, mat)
	{
	}

	virtual bool intersectLocal(const ray& r, isect& i) const;
	virtual bool hasBoundingBoxCapability() const { return true; }

	void addBall(vec3f p, double r)
	{
		balls.push_back(Ball(p, r));
	}

	double getMaxR()
	{
		double R = 0;
		for (vector<Ball>::const_iterator j = balls.begin(); j != balls.end(); j++)
		{
			if ((*j).radius > R)
				R = (*j).radius;
		}
		return R;
	};

	double getMaxCoor(int k)
	{
		double Coor = balls[0].position[k];
		for (vector<Ball>::const_iterator j = balls.begin(); j != balls.end(); j++)
		{
			if ((*j).position[k] > Coor)
				Coor = (*j).position[k];
		}
		return Coor;
	};

	double getMinCoor(int k)
	{
		double Coor = balls[0].position[k];
		for (vector<Ball>::const_iterator j = balls.begin(); j != balls.end(); j++)
		{
			if ((*j).position[k] < Coor)
				Coor = (*j).position[k];
		}
		return Coor;
	};

	virtual BoundingBox ComputeLocalBoundingBox()
	{
		BoundingBox localbounds;
		double R = getMaxR();
		localbounds.min = vec3f(getMinCoor(0) - R, getMinCoor(1) - R, getMinCoor(2) - R);
		localbounds.max = vec3f(getMaxCoor(0) + R, getMaxCoor(1) + R, getMaxCoor(2) + R);
		return localbounds;
	}

};
#endif // __SPHERE_H__
