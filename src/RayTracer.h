#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

	vec3f trace(Scene* scene, double x, double y, int i, int j);
	vec3f traceRay( Scene *scene, const ray& r, const vec3f& thresh, int depth, int x, int y);


	void getBuffer( unsigned char *&buf, int &w, int &h );
	void getSampleBuffer(unsigned char*& buf, int& w, int& h);
	double aspectRatio();
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );
	vec3f RayTracer::adaptiveSample(double center_x, double center_y,
		double range_x, double range_y, int& num_samples);

	bool loadScene( char* fn );

	bool sceneLoaded();

	int			   m_nBackground_width = 0;
	unsigned char* m_nBackground = NULL;
	bool		   m_isBackground = false;

	// bool sample_visualize();

private:
	unsigned char *buffer;
	unsigned char* visualize_sample_buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	Scene *scene;



	bool m_bSceneLoaded;
};

#endif // __RAYTRACER_H__
