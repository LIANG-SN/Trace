#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"
#include <iostream>

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
	virtual double distanceAttenuation( const vec3f& P ) const = 0;
	virtual vec3f getColor( const vec3f& P ) const = 0;
	virtual vec3f getDirection( const vec3f& P ) const = 0;

protected:
	Light( Scene *scene, const vec3f& col )
		: SceneElement( scene ), color( col ) {}

	vec3f 		color;
};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight( Scene *scene, const vec3f& orien, const vec3f& color )
		: Light( scene, color ), orientation( orien ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f 		orientation;
};

class PointLight
	: public Light
{
public:
	PointLight( Scene *scene, const vec3f& pos, const vec3f& color,
	double constant_attenuation = 0.25, double linear_attenuation = 0.003372407,
		double quadratic_attenuation = 0.000045492)
		: Light( scene, color ), position( pos ), constant_atten(constant_attenuation),
	linear_atten(linear_attenuation), quadratic_atten(quadratic_attenuation) 
	{}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f position;
	double constant_atten;
	double linear_atten;
	double quadratic_atten;
};


class SpotLight
	: public Light
{
public:
	SpotLight(Scene* scene, const vec3f& pos, const vec3f& dir, double r,
		const vec3f& color
		)
		: Light(scene, color), position(pos),direction(dir),radius(r)
	{

	};
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;
	double getRadius() const;
protected:
	vec3f position;
	vec3f direction;
	double radius;
};

class AmbientLight
	: public Light
{
public:
	AmbientLight(Scene* scene, const vec3f& color)
		: Light(scene, color){}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;

};
#endif // __LIGHT_H__
