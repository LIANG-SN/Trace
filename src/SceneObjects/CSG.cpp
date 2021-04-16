
#include <cmath>

#include "CSG.h"

bool CSG::intersectLocal(const ray& r, isect& i) const
{
	isect max_t,min_t;
	bool isInter = csg_operation_intersectLocal(r, min_t, max_t, instr);
	if (isInter)
	{
		if (min_t.t <= RAY_EPSILON)
			i = max_t;
		else
			i = min_t;
	}
	return isInter;
}

bool CSG::csg_operation_intersectLocal(const ray& r, isect& min_t, isect& max_t, Instruction* instr) const
{
	isect obj1_min, obj1_max, obj2_min, obj2_max;
	bool isInter1 = false;
	bool isInter2 = false;
	//cout << oper[0]->getScalar() << endl;
	//cout << 1 << endl;
	if (instr->is1Object)
	{
		double value = instr->obj1 + 0.1;
		int index = value;
		iter i= object_start;
		for (int a = 0; a < index; i++,a++);
		isInter1=(*i)->csg_intersect(r, obj1_min, obj1_max);
	}
	else
	{
		isInter1=csg_operation_intersectLocal(r, obj1_min, obj1_max, instr->oper1);
	}

	if (instr->is2Object)
	{
		double value = instr->obj2 + 0.1;
		int index = value;
		iter i= object_start;
		for (int a = 0; a < index; i++,a++);
		isInter2=(*i)->csg_intersect(r, obj2_min, obj2_max);
	}
	else
	{
		isInter2=csg_operation_intersectLocal(r, obj2_min, obj2_max, instr->oper2);
	}


	if (isInter1 || isInter2)
	{



		if (instr->operation==0)
		{
			if (isInter1 && isInter2)
			{
				//cout << 1;
				double st = max(obj1_min.t, obj2_min.t);
				double lt = min(obj1_max.t, obj2_max.t);
				if (st <= lt)
				{
					if (obj1_min.t < obj2_min.t)
						min_t = obj2_min;
					else
						min_t = obj1_min;

					if (obj1_max.t < obj2_max.t)
						max_t = obj1_max;
					else
						max_t = obj2_max;
					return true;
				}
				else
					return false;
			}
			else
				return false;
		}
		else if (instr->operation == 1)
		{
			if (isInter1 && isInter2)
			{
				if (obj1_min.t < obj2_min.t)
					min_t = obj1_min;
				else
					min_t = obj2_min;


				if (obj1_max.t > obj2_max.t)
					max_t = obj1_max;
				else
					max_t = obj2_max;
			}
			else if (isInter1 && !isInter2)
			{
				min_t = obj1_min;
				max_t = obj1_max;
			}
			else if (!isInter1 && isInter2)
			{
				min_t = obj2_min;
				max_t = obj2_max;
			}

			return true;
		}
		else if (instr->operation == 2)
		{
			if (isInter1 && isInter2)
			{
				double delta_min = obj1_min.t - obj2_min.t;
				double delta_max = obj1_max.t - obj2_max.t;
				if (delta_min <= RAY_EPSILON &&
					delta_max >= RAY_EPSILON)
				{
					min_t = obj1_min;
					max_t = obj1_max;
				}
				else if (-delta_min <= RAY_EPSILON &&
					-delta_max >= RAY_EPSILON)
				{
					return false;
				}
				else if (delta_min <= RAY_EPSILON && delta_max <= RAY_EPSILON)
				{
					if (obj1_max.t < obj2_min.t)
					{
						min_t = obj1_min;
						max_t = obj1_min;
					}
					else
					{
						min_t = obj1_min;
						max_t = obj2_min;
						max_t.N = -max_t.N;
					}
				}
				else if(-delta_min <= RAY_EPSILON && -delta_max <= RAY_EPSILON)
				{
					if (obj2_max.t > obj1_min.t)
					{
						min_t = obj1_min;
						max_t = obj1_max;

					}
					else
					{
						min_t = obj2_max;
						max_t = obj1_max;
						min_t.N = -min_t.N;
					}
				}
				return true;
			}
			else if (isInter1 && !isInter2)
			{
				min_t = obj1_min;
				max_t = obj1_max;
				return true;
			}
			else
				return false;
		}
	}
	else
		return false;

}