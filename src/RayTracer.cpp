// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "ui/TraceUI.h"
#include <stack>

extern TraceUI* traceUI;
stack<double> refrac_stack;
// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	while (!refrac_stack.empty())
		refrac_stack.pop();
	return traceRay( scene, r, vec3f(1.0,1.0,1.0), 0 ).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth )
{

	isect i;
	
	if( scene->intersect( r, i ) ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		const Material& m = i.getMaterial();
		vec3f rayDirection = r.getDirection().normalize();
		vec3f N = i.N.normalize();
		// shade
		vec3f shade = m.shade(scene, r, i);
		if (depth >= traceUI->getDepth())
			return shade;
		vec3f finalI = shade;
		// reflection
		if (refrac_stack.empty())
		{
		    vec3f reflectDirection = 
		    	 - (2 * (rayDirection.dot(N)) * N - rayDirection).normalize();
		    ray reflectRay(r.at(i.t), reflectDirection);
		    vec3f reflect = traceRay(scene, reflectRay,  thresh, depth + 1);
		    finalI += m.kr.multiply(reflect);
		}
		// refraction
		if (m.kt[0] != 0 || m.kt[1] != 0 || m.kt[2] != 0)
		{
		    double n_i, n_t;
		    if (rayDirection.dot(N) <= 0) // enter an object
		    {
				n_i = refrac_stack.empty() ? 1 : refrac_stack.top();
				n_t = m.index;
				refrac_stack.push(n_t);
		    }
		    else // exit an object
		    {
		    	n_i = m.index;
				refrac_stack.pop();
		    	n_t = refrac_stack.empty() ? 1 : refrac_stack.top();
		    	N *= -1;
		    }
		    double n = n_i / n_t;
		    double cos_theta_i = N.dot(-rayDirection);
		    if (1 - n * n * (1 - cos_theta_i * cos_theta_i) > 0)
		    {
		    	double cos_theta_t = sqrt(1 - n * n * (1 - cos_theta_i * cos_theta_i));
		    	vec3f refracDir = ((n * cos_theta_i - cos_theta_t) * N - n * -rayDirection).normalize();
		    	ray refracRay(r.at(i.t), refracDir);
		    	vec3f refrac = traceRay(scene, refracRay, thresh, depth + 1);
		    	finalI += m.kt.multiply(refrac);
		    }
		    finalI[0] = min(finalI[0], thresh[0]);
		    finalI[1] = min(finalI[1], thresh[1]);
		    finalI[2] = min(finalI[2], thresh[2]);

		}
		return finalI;
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f( 0.0, 0.0, 0.0 );
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}