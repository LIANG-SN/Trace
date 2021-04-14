#ifndef __HYPERBOLIC_H__
#define __HYPERBOLIC_H__

#include "../scene/scene.h"

class Hyperbolic
	: public MaterialSceneObject
{
public:
	Hyperbolic(Scene* scene, Material* mat)
		: MaterialSceneObject(scene, mat)
	{
	}

	virtual bool intersectLocal(const ray& r, isect& i) const;
	virtual bool hasBoundingBoxCapability() const { return true; }

	virtual BoundingBox ComputeLocalBoundingBox()
	{
		BoundingBox localbounds;
		localbounds.min = vec3f(-3.0f, 18.0f, -3.0f);
		localbounds.max = vec3f(3.0f, 18.0f, 3.0f);
		return localbounds;
	}

};
#endif 