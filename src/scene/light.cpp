#include <cmath>

#include "light.h"
#include "../ui/TraceUI.h"

extern TraceUI* traceUI;

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	vec3f d = -orientation.normalize();
	isect i;
	ray r(P, d);
	vec3f atten;
	if (scene->intersect(r, i))
		atten = i.obj->getMaterial().kt.multiply(shadowAttenuation(r.at(i.t)));
	else atten = { 1, 1, 1 };

	return atten;
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	double d = (P - position).length();
	return min(1.0, 1.0 / (constant_atten + linear_atten * d + quadratic_atten * d * d));
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	vec3f atten;
	if (traceUI->soft_shadow)
	{
		double t = 0.5;
		vec3f soft_atten(0, 0, 0);
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				vec3f soft_pos(position[0] - t / 2 + (i / 10.0) * t,
					position[1], position[2] - t / 2 + (j / 10.0) * t);
				vec3f d = (soft_pos - P).normalize();
				isect i;
				ray r(P, d);
				if (!scene->intersect(r, i))
					soft_atten += { 1, 1, 1 };
				else
				{

					vec3f Q = r.at(i.t);
					vec3f PQ = Q - P;
					vec3f PL = position - P;
					if (PQ.length() < PL.length())
						soft_atten += i.obj->getMaterial().kt;
					else
						soft_atten += { 1, 1, 1 };

					if (i.t < RAY_EPSILON)
						soft_atten += { 1, 1, 1 };
				}

			}
		}
		atten = soft_atten / 100;
	}
	else
	{
	vec3f d = (position - P).normalize();
	isect i;
	ray r(P, d);
	
	if (! scene->intersect(r, i))
		atten = { 1, 1, 1 };
	else
	{
		
		  vec3f Q = r.at(i.t);
		  vec3f PQ = Q - P;
		  vec3f PL = position - P;
		  if (PQ.length() < PL.length())
		  	atten = i.obj->getMaterial().kt.multiply(shadowAttenuation(Q));
		  else
		  	atten = { 1, 1, 1 };
		  
		  if(i.t< RAY_EPSILON)
		  	atten = { 1, 1, 1 };
	}
	}
	return atten;
}

vec3f SpotLight::shadowAttenuation(const vec3f& P) const
{
	double distance_from_center = (P - position).length() * sqrt(1 - pow((P - position).normalize() * direction.normalize(), 2.0));
	vec3f atten;
	if (distance_from_center <= radius)
	{
		vec3f d = -direction.normalize();
		isect i;
		ray r(P, d);

		if (scene->intersect(r, i))
			atten = i.obj->getMaterial().kt;
		else atten = { 1, 1, 1 };
	}
	else
		return vec3f(1, 1, 1);
	return atten;
}

double SpotLight::distanceAttenuation(const vec3f& P) const
{
	return 1.0;
}

vec3f SpotLight::getColor(const vec3f& P) const
{
	double distance_from_center = (P - position).length() * sqrt(1 - pow((P - position).normalize() * direction.normalize(), 2.0));
	if (distance_from_center <= radius )
		return color;
	else
		return vec3f(0.0, 0.0, 0.0);
}

vec3f SpotLight::getDirection(const vec3f& P) const
{
	return -direction;
}

vec3f ShapeLight::shadowAttenuation(const vec3f& P) const
{
	vec3f atten;
	double x = P[0] - position[0];
	double y = P[2] - position[2];
	if (type == 0)
	{
		double length = 0.5;
		if (x < length / 2  && x > -length / 2 && y < length / 2 && y > -length / 2)
		{
			
			vec3f d = -direction;
			isect i;
			ray r(P, d);
			if (scene->intersect(r, i))
			{
				atten = i.obj->getMaterial().kt;
			}
			else

			{
				atten = { 1, 1, 1 };
				
			}
		}
		else
			atten = { 0, 0, 0 };
	    
	}
	else if (type == 1)
	{
		double t = 0.5;
		if (y > - t / 2 && y < sqrt(3) * x + t && y < -sqrt(3) * x + t) // in
		{
			vec3f d = -direction.normalize();
			isect i;
			ray r(P, d);
			if (scene->intersect(r, i))
				atten = i.obj->getMaterial().kt;
			else atten = { 1, 1, 1 };
		}
		else
			atten = { 0, 0, 0 };
	}
	else if (type == 2)
	{
		double t = 0.5;
		if ((y > -t / 2 && y < sqrt(3) * x + t && y < -sqrt(3) * x + t)
			|| (y < t / 2 && y > sqrt(3) * x - t && y > -sqrt(3) * x - t)) // in
		{
			vec3f d = -direction.normalize();
			isect i;
			ray r(P, d);
			if (scene->intersect(r, i))
				atten = i.obj->getMaterial().kt;
			else atten = { 1, 1, 1 };
		}
		else
			atten = { 0, 0, 0 };
	}
	else 
		atten = { 0, 0, 0 };
	return atten;
}

double ShapeLight::distanceAttenuation(const vec3f& P) const
{
	return 1.0;
}

vec3f ShapeLight::getColor(const vec3f& P) const
{
	return color;
}

vec3f ShapeLight::getDirection(const vec3f& P) const
{
	return -direction;
}


double SpotLight::getRadius() const
{
	return radius;
}


vec3f AmbientLight::getColor(const vec3f& P) const
{
	// Color doesn't depend on P 
	return color;
}
double AmbientLight::distanceAttenuation(const vec3f& P) const
{
	return 1.0;
}
vec3f AmbientLight::shadowAttenuation(const vec3f& P) const
{
	return vec3f(1.0, 1.0, 1.0);
}
vec3f AmbientLight::getDirection(const vec3f& P) const
{
	// no direction
	return vec3f(0.0, 0.0, 0.0);
}