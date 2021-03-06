// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "ui/TraceUI.h"
#include "SceneObjects/trimesh.h"
#include "SceneObjects/Sphere.h"
#include "SceneObjects/Square.h"
#include "SceneObjects/Box.h"
#include <stack>



extern TraceUI* traceUI;
stack<double> refrac_stack;


// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y,int i,int j )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	while (!refrac_stack.empty())
		refrac_stack.pop();
	double thres = traceUI->getThreshold();

	if (traceUI->motion) // motion blur
	{
		vec3f average(0, 0, 0);
		for (int i = 0; i < 5; i++)
		{
		    vec3f p_ = r.getPosition() + vec3f(0.05 * i, 0.05 * i, 0.05 * i);
		    ray r_ = ray(p_, r.getDirection());
		    average += traceRay(scene, r_, vec3f(thres, thres, thres), 0, i, j).clamp();
		}
		return average / 5;
	}
	else if (traceUI->depth_of_field)
	{
		// vec3f t = scene->getCamera()->getLook();
		//cout << t[0] << " " << t[1] << " " << t[2] << endl;
		vec3f average(0, 0, 0);
		for (int row = 0; row < 11; row++)
		{
			for (int col = 0; col < 11; col++)
			{
				double x_bias = (row - 5) / 10.0 / 50 ; // (-0.5, 0.5)
				double y_bias = (col - 5) / 10.0 / 50;
				Camera c = *(scene->getCamera());
				vec3f bias(x_bias, y_bias, 0);
				c.setEye(c.getEye() + bias);
				//c.look[2] = -traceUI->focal_length;
				//
				/*double x_ = x - 0.5;
				double y_ = y - 0.5;
				vec3f dir = c.look + x * c.u + y * c.v;
				r = ray(c.eye, dir.normalize());*/
				c.rayThrough(x, y, r);
				average += traceRay(scene, r, vec3f(thres, thres, thres), 0, i, j).clamp();
			}
		}
		return average / 121;
	}
	else
		return traceRay( scene, r, vec3f(thres, thres, thres), 0 , i, j ).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth, int x,int y )
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
		//return vec3f(1, 1, 1);

		const Material& m = i.getMaterial();
		vec3f rayDirection = r.getDirection().normalize();
		vec3f N = i.N.normalize();
		//cout << N << endl;
		// shade
		vec3f shade= m.shade(scene, r, i);


		
		//cout << shade[0] << endl;
		if (depth >= traceUI->getDepth())
			return shade;
		vec3f finalI = shade;


		if (traceUI->caustic && m.kt[0] == 0)
		{
			vec3f p = r.at(i.t);
			if (p[0]*p[0] + p[1] * p[1] <= 1)
			{
				int x_ = p[0] * 100 + 100;
				int y_ = p[1] * 100 + 100;
				// add photon to photon map
				vec3f scale(1.0 / 2.5 , 0, 0);
				finalI += scale * photonMap[x_][y_];
			}
		}


		if (shade[0] < thresh[0] && shade[1] < thresh[1] && shade[2] < thresh[2])
			return finalI;

		// reflection
		if (refrac_stack.empty() && (m.kr[0] > 0 || m.kr[1] > 0 || m.kr[2] > 0))
		{
			if (traceUI->glossy)
			{
				// 1.5 to 2.5
				vec3f average(0.0, 0.0, 0.0);
				for (int k = 0; k < 11; k++)
				{
					double scale = (k - 5) / 10.0 / 5; // (-0.5, 0.5)
					// distributed direction
					vec3f reflectDirection =
						-((2 + scale) * (rayDirection.dot(N)) * N - rayDirection).normalize();
					ray reflectRay(r.at(i.t), reflectDirection);
					vec3f reflect = traceRay(scene, reflectRay, thresh, depth + 1, x, y);
					vec3f kr_reflect = m.kr.multiply(reflect);
					average += m.kr.multiply(reflect);
				}
				finalI += average / 11;
			}
			else
			{
		        vec3f reflectDirection = 
		        	 - (2 * (rayDirection.dot(N)) * N - rayDirection).normalize();
		        ray reflectRay(r.at(i.t), reflectDirection);
		        vec3f reflect = traceRay(scene, reflectRay,  thresh, depth + 1,x,y);
			    vec3f kr_reflect = m.kr.multiply(reflect);
			    if (kr_reflect[0] > thresh[0] || kr_reflect[1] > thresh[1] || kr_reflect[2] > thresh[2])
			    	finalI += m.kr.multiply(reflect);
			    else
			    	return finalI;
			}
		}
		// refraction
		//cou//t << m.kt[0] << endl;
		if (m.kt[0] != 0 || m.kt[1] != 0 || m.kt[2] != 0)
		{
			//c//out << m.kt[0] << " " << m.kt[1] << endl;
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
				if(!refrac_stack.empty())
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
		    	vec3f refrac = traceRay(scene, refracRay, thresh, depth + 1,x,y);
				vec3f kt_refrac = m.kt.multiply(refrac);
				if (kt_refrac[0] > thresh[0] || kt_refrac[1] > thresh[1] || kt_refrac[2] > thresh[2])
					finalI += m.kt.multiply(refrac);
				else
					return finalI;
		    }

		}



		return finalI;
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.
		if (m_isBackground && x>=0 && y>=0 && m_nBackground!=NULL)
		{
			if (depth == 0)
			{
				vec3f light = vec3f();
				light[0] = *(m_nBackground + (x * m_nBackground_width / buffer_width
					+ y  * m_nBackground_width / buffer_width * m_nBackground_width) * 3 + 0) / 255.0;
				light[1] = *(m_nBackground + (x * m_nBackground_width / buffer_width
					+ y * m_nBackground_width / buffer_width * m_nBackground_width) * 3 + 1) / 255.0;
				light[2] = *(m_nBackground + (x * m_nBackground_width / buffer_width
					+ y * m_nBackground_width / buffer_width * m_nBackground_width) * 3 + 2) / 255.0;
				return light;
			}
			else if(depth <= traceUI->getDepth()){
				float v = (r.getDirection().normalize() * scene->getCamera()->getLook().normalize());

					vec3f project_plane = r.getDirection().normalize()-v * scene->getCamera()->getLook().normalize();
					float x_projection = project_plane * scene->getCamera()->getU();
					float y_projection = project_plane * scene->getCamera()->getV();
					float x_ratio = abs(x_projection) * project_plane.length();
					float y_ratio = abs(y_projection) * project_plane.length();
					int new_x = x_ratio * buffer_width;
					int new_y = y_ratio * project_plane.length() * buffer_width;

					vec3f light = vec3f();
					light[0] = *(m_nBackground + (new_x * m_nBackground_width / buffer_width
						+ new_y * m_nBackground_width / buffer_width * m_nBackground_width) * 3 + 0) / 255.0;
					light[1] = *(m_nBackground + (new_x * m_nBackground_width / buffer_width
						+ new_y * m_nBackground_width / buffer_width * m_nBackground_width) * 3 + 1) / 255.0;
					light[2] = *(m_nBackground + (new_x * m_nBackground_width / buffer_width
						+ new_y * m_nBackground_width / buffer_width * m_nBackground_width) * 3 + 2) / 255.0;
					return light;

			}
		}
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
	delete [] m_nBackground;
	delete [] m_nBumpMap;
	delete [] m_nTextureMap;
	delete [] m_nSolidTexture;
	delete [] visualize_sample_buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}
void RayTracer::getSampleBuffer(unsigned char*& buf, int& w, int& h)
{
	buf = visualize_sample_buffer;
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

void RayTracer::setSceneBumpMap()
{
	scene->m_nBumpMap_width = m_nBumpMap_width;
	scene->m_nBumpMap_height = m_nBumpMap_height;
	scene->m_nBumpMap = m_nBumpMap;
	scene->m_isBumpMap = m_isBumpMap;
}

void RayTracer::setSceneTextureMap()
{
	scene->m_nTextureMap_width = m_nTextureMap_width;
	scene->m_nTextureMap_height = m_nTextureMap_height;
	scene->m_nTextureMap = m_nTextureMap;
	scene->m_isTextureMap = m_isTextureMap;
}

void RayTracer::setSceneSolidTexture()
{
	scene->m_nSolidTexture_width = m_nSolidTexture_width;
	scene->m_nSolidTexture_height = m_nSolidTexture_height;
	scene->m_nSolidTexture = m_nSolidTexture;
	scene->m_isSolidTexture = m_isSolidTexture;
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
	visualize_sample_buffer = new unsigned char[bufferSize / 3];
	
	// add height field here
	if (traceUI->height_field && m_nHeight_field)
		loadHeightField();


	// separate objects into bounded and unbounded
	scene->initScene();
	
	// init caustic (after init bvh)
	if (traceUI->caustic)
		initCaustic();

	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::loadHeightField()
{
	Material* m = new Material();
	m->ka = vec3f(0.3, 0.3, 0.3);
	m->kd = vec3f(0.8, 0.3, 0.1);
	m->ks = vec3f(0.9, 0.4, 0.0);
	m->shininess = 0.6;

	Trimesh* trimesh = new Trimesh(scene, m, &scene->transformRoot);

	double scale = 3;
	for (int w = 0; w < m_nHeight_field_width - 1; w++)
	{
		for (int h = 0; h < m_nHeight_field_height - 1; h++)
		{
			double y1 =
				(*(m_nHeight_field + (h * (m_nHeight_field_width)+w) * 3) * 0.299
					+ *(m_nHeight_field + (h * (m_nHeight_field_width)+w) * 3 + 1) * 0.587
					+ *(m_nHeight_field + (h * (m_nHeight_field_width)+w) * 3 + 2) * 0.114)
				/ 255.0 * scale;
			double y2 =
				(*(m_nHeight_field + (h * (m_nHeight_field_width)+w + 1) * 3) * 0.299
					+ *(m_nHeight_field + (h * (m_nHeight_field_width)+w + 1) * 3 + 1) * 0.587
					+ *(m_nHeight_field + (h * (m_nHeight_field_width)+w + 1) * 3 + 2) * 0.114)
				/ 255.0 * scale;
			double y3 =
				(*(m_nHeight_field + ((h + 1) * (m_nHeight_field_width)+w + 1) * 3) * 0.299
					+ *(m_nHeight_field + ((h + 1) * (m_nHeight_field_width)+w + 1) * 3 + 1) * 0.587
					+ *(m_nHeight_field + ((h + 1) * (m_nHeight_field_width)+w + 1) * 3 + 2) * 0.114)
				/ 255.0 * scale;
			double y4 =
				(*(m_nHeight_field + ((h + 1) * (m_nHeight_field_width)+w) * 3) * 0.299
					+ *(m_nHeight_field + ((h + 1) * (m_nHeight_field_width)+w) * 3 + 1) * 0.587
					+ *(m_nHeight_field + ((h + 1) * (m_nHeight_field_width)+w) * 3 + 2) * 0.114)
				/ 255.0 * scale;

			vec3f p1((double)h / (m_nHeight_field_height - 1) * scale, y1,
				(double)w / (m_nHeight_field_width - 1) * scale);
			vec3f p2((double)(h) / (m_nHeight_field_height - 1) * scale, y2,
				(double)(w + 1) / (m_nHeight_field_width - 1) * scale);
			vec3f p3((double)(h + 1) / (m_nHeight_field_height - 1) * scale, y3,
				(double)(w + 1) / (m_nHeight_field_width - 1) * scale);
			vec3f p4((double)(h + 1) / (m_nHeight_field_height - 1) * scale, y4,
				(double)(w) / (m_nHeight_field_width - 1) * scale);

			trimesh->addVertex(p1);
			trimesh->addVertex(p2);
			trimesh->addVertex(p3);

			trimesh->addVertex(p1);
			trimesh->addVertex(p3);
			trimesh->addVertex(p4);

		}
	}
	// add all faces
	for (int i = 0; i < (m_nHeight_field_height - 1)
		* (m_nHeight_field_width - 1) * 2; i++)
	{
		trimesh->addFace(i * 3, i * 3 + 1, i * 3 + 2);
	}
	// cout << trimesh->faces.size() << endl;
	trimesh->generateNormals();

	char* error;
	if (error = trimesh->doubleCheck())
		throw ParseError(error);
	scene->add(trimesh);
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
		delete[] visualize_sample_buffer;
		visualize_sample_buffer = new unsigned char[bufferSize / 3];
	}
	memset( buffer, 0, w*h*3 );
	memset(visualize_sample_buffer, 0, w * h);
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
vec3f RayTracer::adaptiveSample(double center_x, double center_y,
	double range_x, double range_y, int& num_samples)
{
	// check depth
	int n = traceUI->sample_per_pixel;

	double thresh = traceUI->adaptive_thresh;
	vec3f LT = trace(scene, center_x - range_x / 2, center_y + range_y / 2, -1, -1);
	vec3f LB = trace(scene, center_x - range_x / 2, center_y - range_y / 2, -1, -1);
	vec3f RT = trace(scene, center_x + range_x / 2, center_y + range_y / 2, -1, -1);
	vec3f RB = trace(scene, center_x + range_x / 2, center_y - range_y / 2, -1, -1);
	vec3f average = (LT + LB + RT + RB) / 4;
	num_samples += 4;
	double diff = max((LT - average).length(), (LB - average).length(),
		(RT - average).length(), (RB - average).length());
	if (diff <= thresh || num_samples + 16 > n * n)
	{
		return average;
	}
	else
	{
		//  c1		 c2
		//		 c
		//  c3		 c4
		double center_x1 = center_x - range_x / 4;
		double center_x2 = center_x + range_x / 4;
		double center_x3 = center_x - range_x / 4;
		double center_x4 = center_x + range_x / 4;
		double center_y1 = center_y + range_y / 4;
		double center_y2 = center_y + range_y / 4;
		double center_y3 = center_y - range_y / 4;
		double center_y4 = center_y - range_y / 4;

		return (adaptiveSample(center_x1, center_y1, range_x / 2, range_y / 2, num_samples)
			+ adaptiveSample(center_x2, center_y2, range_x / 2, range_y / 2, num_samples)
			+ adaptiveSample(center_x3, center_y3, range_x / 2, range_y / 2, num_samples)
			+ adaptiveSample(center_x4, center_y4, range_x / 2, range_y / 2, num_samples) ) / 4;
	}
}
static int line = 0;
void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;
	// super-sampling
	double pix_w = 1.0 / double(buffer_width);
	double pix_h = 1.0 / double(buffer_height);
	if (traceUI->adaptive_super_sampling) // if adaptive super sample
	{
		int num_samples = 0;
		col = adaptiveSample(double(i) / double(buffer_width), double(j) / double(buffer_height),
			pix_w, pix_h, num_samples);
		// store num_samples to visualize
		num_samples = min(num_samples, 255);
		*(visualize_sample_buffer + j * buffer_width + i) = num_samples;
	}
	else if (traceUI->super_sampling) // if not adaptive but average down
	{
		int n = traceUI->sample_per_pixel; 
		vec3f average(0, 0, 0);
		for (int s = 0; s <= n; s++)
		{
			for (int t = 0; t <= n; t++)
			{
				bool is_jitter = traceUI->jittering;
				double jitter = is_jitter ? (double)(rand() % 9 - 4) / 9.0 : 0;
				double x = double(i) / double(buffer_width) - pix_w / 2 + (float(s) / n) * pix_w
					+ jitter / n * pix_w;
				double y = double(j) / double(buffer_height) - pix_h / 2 + (float(t) / n) * pix_h
					+ jitter / n * pix_h;
				average += trace(scene, x, y, -1, -1);
			}
		}
	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y, -1, -1);


		col = average / (float)(n * n);
	}
	else
		col = trace(scene, double(i) / double(buffer_width), double(j) / double(buffer_height),i,j);

	unsigned char* pixel = buffer + (i + j * buffer_width) * 3;
	pixel[0] = (int)(255.0 * col[0]);
	pixel[1] = (int)(255.0 * col[1]);
	pixel[2] = (int)(255.0 * col[2]);

}

vec3f RayTracer::tracePhoton(const ray& r)
{
	isect i;

	if (scene->intersect(r, i))
	{
		const Material& m = i.getMaterial();
		vec3f rayDirection = r.getDirection().normalize();
		vec3f N = i.N.normalize();

		// refraction
		if (m.kt[0] != 0 || m.kt[1] != 0 || m.kt[2] != 0)
		{
			double n_i, n_t;
			if (rayDirection.dot(N) <= 0) // enter an object
			{
				n_i = 1;
				n_t = m.index;
			}
			else // exit an object
			{
				n_i = m.index;
				n_t = 1;
				N *= -1;
			}
			double n = n_i / n_t;
			double cos_theta_i = N.dot(-rayDirection);
			if (1 - n * n * (1 - cos_theta_i * cos_theta_i) > 0)
			{
				double cos_theta_t = sqrt(1 - n * n * (1 - cos_theta_i * cos_theta_i));
				vec3f refracDir = ((n * cos_theta_i - cos_theta_t) * N - n * -rayDirection).normalize();
				ray refracRay(r.at(i.t), refracDir);
				return tracePhoton(refracRay);
			}
			else
				return vec3f(0, 0, 10); // exception

		}
		else
			return r.at(i.t);

	}
	else
		;// cout << "photon disappear" << endl;
}
void RayTracer::initCaustic()
{
	int numPhoton = 200; // 20x20
	int z = 2;
	int strange_photon = 0;
	for (int i = 0; i < numPhoton; i++)
	{
		for (int j = 0; j < numPhoton; j++)
		{
			double x = -1 + (double)i / numPhoton * 2;
			double y = -1 + (double)j / numPhoton * 2;
			vec3f p(x, y, z);
			vec3f dir(0, 0, -1);
			vec3f map_pos = tracePhoton(ray(p, dir));
			// cout << map_pos[0] << "," << map_pos[1] << endl;
			if (abs(map_pos[0]) <= 1 && abs(map_pos[1]) <= 1 && map_pos[2] != 10)
			{
				int x_ = map_pos[0] * 100 + 100;
				int y_ = map_pos[1] * 100 + 100;
				// add photon to photon map
				photonMap[x_][y_]++;
			}
			else
				strange_photon++;
		}
	}
}