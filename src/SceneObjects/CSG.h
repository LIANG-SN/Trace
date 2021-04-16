#ifndef __CSG_H__
#define __CSG_H__

#include "../scene/scene.h"
#include "../fileio/parse.h"

class Instruction
{
public:
	bool is1Object = false;
	bool is2Object = false;
	double obj1, obj2;
	Instruction* oper1=NULL;
	Instruction* oper2=NULL;
	int operation;
	~Instruction() {
		if (!oper1) delete oper1;
		if (!oper2) delete oper2;
	}

};

class CSG
	: public MaterialSceneObject
{
public:

	typedef list<Geometry*>::const_iterator iter;
	iter object_start;
	Instruction* instr;

	CSG(Scene* scene, Material* mat)
		: MaterialSceneObject(scene, mat)
	{
		object_start = scene->beginObjects();
		instr = new Instruction();
		//for (iter i = object_start; i != scene->endObjects(); i++)
		//{
		//	cout << 1 << endl;
		//}
	}
	~CSG()
	{
		delete instr;
	}

	virtual bool intersectLocal(const ray& r, isect& i) const;
	bool csg_operation_intersectLocal(const ray& r, isect& min_t, isect& max_t, Instruction* oper) const;
	virtual bool hasBoundingBoxCapability() const { return true; }


	virtual BoundingBox ComputeLocalBoundingBox()
	{
		BoundingBox localbounds;
		localbounds.min = vec3f(-1.0f, -1.0f, 0.0f);
		localbounds.max = vec3f(1.0f, 1.0f, 1.0f);
		iter end = scene->endObjects();
		if(end!= object_start)
			end--;
		for (iter i = object_start; i != end; i++)
		{
			for (int a = 0; a < 3; a++)
			{
				if ((*i)->getBoundingBox().min[a] < localbounds.min[a])
					localbounds.min[a] = (*i)->getBoundingBox().min[a];
				if ((*i)->getBoundingBox().max[a] > localbounds.max[a])
					localbounds.max[a] = (*i)->getBoundingBox().max[a];
			}

		}
		return localbounds;
	}

};

#endif // __CYLINDER_H__
