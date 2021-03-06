#include "ray.h"
#include "material.h"
#include "light.h"
#include <algorithm>
#include "../SceneObjects/Sphere.h"
#include "../SceneObjects/Square.h"
// Apply the phong model to this point on the surface of the object, returning
// the color of that point.


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define PI acos(-1)

double D_f(vec3f n, vec3f h,double m)
{
	double a = acos(n * h);
	double cosa_4 = max(0.000001, pow(cos(a), 4.0));
	return exp(-tan(a) * tan(a) / m / m) / (PI * m * m * cosa_4);
}

double G_f(vec3f n, vec3f h, vec3f l, vec3f v)
{
	double v_n = max(0.000001, (v * n));
	return min(1, min(2 * (h * n) * (v * n) / v_n, 2 * (h * n) * (l * n) / v_n));
}

void sphereTextureMapAlgorithm(double& rx, double& ry, vec3f& local)
{
	vec3f Sp = vec3f(0, -1, 0);
	vec3f Se = vec3f(0, 0, 1);

	ry = acos(Sp * local.normalize()) / PI;

	rx = acos(Se * local.normalize() / sin(acos(Sp * local.normalize())) )/ (2 * PI);

}

void squareTextureMapAlgorithm(double& rx, double& ry, vec3f& local)
{
	rx = min(local[0] + 0.5, 1.0);
	ry = min(local[1] + 0.5, 1.0);
}



vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.
	using namespace std;
	vec3f intensity = { 0, 0, 0 };
	vec3f p = r.at(i.t);
	intensity += ke;
	int c = 0;

	for (Scene::cliter iter = scene->beginLights(); iter != scene->endLights(); iter++)
	{
		vec3f I_light = (*iter)->getColor(p);
		if (dynamic_cast<AmbientLight*>((*iter)) != nullptr)
		{
			intensity += ka.multiply(I_light).multiply(vec3f(1.0, 1.0, 1.0) - kt);
			// std::cout << intensity[0] << std::endl;
		}
		else
		{
			vec3f L = (*iter)->getDirection(p);
			vec3f N = i.N;


			if (scene->m_isBumpMap && scene->m_nBumpMap!=NULL)
			{
				Geometry* obj_ptr = (*(scene->beginBoundedobjects()));
				vec3f local = obj_ptr->getTransform()->globalToLocalCoords(r.at(i.t));
				double rx, ry;

				//cout << rx << " " << ry << endl;
				if (dynamic_cast<Square*>(obj_ptr))
					squareTextureMapAlgorithm(rx, ry, local);

				int w = scene->m_nBumpMap_width;
				int h = scene->m_nBumpMap_height;
				vec3f color;
				int u = min(w * rx, w - 1);
				int v = min(h * ry, h - 1);
				//cout << u << " " << v << endl;

				int Gx, Gy;
				scene->getGradientOfPoint(u, v, Gx, Gy);
				
				vec3f d_x= obj_ptr->getTransform()->globalToLocalCoords(vec3f(-1,0,0)).normalize();
				vec3f d_y= obj_ptr->getTransform()->globalToLocalCoords(vec3f(0, 1, 0)).normalize();
				double rate = 6.0;
				N += rate*Gx / 255.0 * d_x;
				N += rate*Gy / 255.0 * d_y;
			}
			
			
			vec3f R = (2 * (L.dot(N)) * N - L).normalize();
			vec3f V = (scene->getCamera()->getEye() - p).normalize();
			vec3f kd = this->kd;


			if (scene->m_isTextureMap && scene->m_nTextureMap != NULL)
			{
				Geometry* obj_ptr = (*(scene->beginBoundedobjects()));
				vec3f local = obj_ptr->getTransform()->globalToLocalCoords(r.at(i.t));

				double rx, ry;

				if (dynamic_cast<Sphere*>(obj_ptr))
					sphereTextureMapAlgorithm(rx, ry, local);
				else if (dynamic_cast<Square*>(obj_ptr))
					squareTextureMapAlgorithm(rx, ry, local);
				int w = scene->m_nTextureMap_width;
				int h = scene->m_nTextureMap_height;
				vec3f color;
				int u = min(w * rx, w - 1);
				int v = min(h * ry, h - 1);

				cout << u << " " << v << endl;
				color[0] = *(scene->m_nTextureMap + (u + v * w) * 3 + 0);
				color[1] = *(scene->m_nTextureMap + (u + v * w) * 3 + 1);
				color[2] = *(scene->m_nTextureMap + (u + v * w) * 3 + 2);
				kd = color / 255.0;
			}
			if (scene->m_isSolidTexture && scene->m_nSolidTexture != NULL)
			{
				Geometry* obj_ptr = (*(scene->beginBoundedobjects()));
				vec3f local = obj_ptr->getTransform()->globalToLocalCoords(r.at(i.t));
				double rx = (local[0] + obj_ptr->ComputeLocalBoundingBox().max[0]) /
					(obj_ptr->ComputeLocalBoundingBox().max[0] - obj_ptr->ComputeLocalBoundingBox().min[0]);
				double ry = (local[1] + obj_ptr->ComputeLocalBoundingBox().max[1]) / 
					(obj_ptr->ComputeLocalBoundingBox().max[1] - obj_ptr->ComputeLocalBoundingBox().min[1]);
				double rz = (local[2] + obj_ptr->ComputeLocalBoundingBox().max[2]) / 
					(obj_ptr->ComputeLocalBoundingBox().max[2] - obj_ptr->ComputeLocalBoundingBox().min[2]);

				//cout << rx << " " << ry << " " << rz << endl;
				int w = scene->m_nSolidTexture_width;
				int h = scene->m_nSolidTexture_height;

				int p1_x = min(w * rx, w - 1);
				int p1_y = min(h * ry, h - 1);

				int p2_x = min(w * rz, w - 1);
				int p2_y = min(h * ry, h - 1);

				int p3_x = min(w * rx, w - 1);
				int p3_y = min(h * rz, h - 1);

				int r = *(scene->m_nSolidTexture + (p1_x + p1_y * w) * 3 + 0) +
					*(scene->m_nSolidTexture + (p2_x + p2_y * w) * 3 + 0) +
					*(scene->m_nSolidTexture + (p3_x + p3_y * w) * 3 + 0);
				int g = *(scene->m_nSolidTexture + (p1_x + p1_y * w) * 3 + 1) +
					*(scene->m_nSolidTexture + (p2_x + p2_y * w) * 3 + 1) +
					*(scene->m_nSolidTexture + (p3_x + p3_y * w) * 3 + 1);
				int b = *(scene->m_nSolidTexture + (p1_x + p1_y * w) * 3 + 2) +
					*(scene->m_nSolidTexture + (p2_x + p2_y * w) * 3 + 2) +
					*(scene->m_nSolidTexture + (p3_x + p3_y * w) * 3 + 2);
				
				kd = vec3f(r / 3 / 255.0, g / 3 / 255.0, b / 3 / 255.0);
			}



			if (scene->m_isPhysicalShade)
			{
				double fl = 1 / PI;
				vec3f h = (V + L).normalize();
				vec3f F = ks +  (1.0 - pow((V * h), 5.0))*(vec3f(1, 1, 1) - ks);
				double D = D_f(N, h, m);
				double G = G_f(N, h, L, V);
				double n_h = max(0.000001, N * h);
				double n_v = max(0.000001, N * V);

				intensity[0] += (*iter)->shadowAttenuation(p)[0] * (*iter)->distanceAttenuation(p) * I_light[0] *
					(kd[0] * (1 - kt[0]) * fl + ks[0] * F[0] * D * G / 4 / n_h / n_v);
				intensity[1] += (*iter)->shadowAttenuation(p)[0] * (*iter)->distanceAttenuation(p) * I_light[0] *
					(kd[1] * (1 - kt[1]) * fl + ks[1] * F[1] * D * G / 4 / n_h / n_v);
				intensity[2] += (*iter)->shadowAttenuation(p)[0] * (*iter)->distanceAttenuation(p) * I_light[0] *
					(kd[2] * (1 - kt[2]) * fl + ks[2] * F[2] * D * G / 4 / n_h / n_v);
			}
			else
			{
				intensity[0] += (*iter)->shadowAttenuation(p)[0] *
					(*iter)->distanceAttenuation(p) * I_light[0] *
					(kd[0] * (1 - kt[0]) * max(0.0, (N.dot(L))) + ks[0] * pow(max(0.0, V.dot(R)), shininess * 128));
				intensity[1] += (*iter)->shadowAttenuation(p)[1] *
					(*iter)->distanceAttenuation(p) * I_light[1] *
					(kd[1] * (1 - kt[1]) * max(0.0, (N.dot(L))) + ks[1] * pow(max(0.0, V.dot(R)), shininess * 128));
				intensity[2] += (*iter)->shadowAttenuation(p)[2] *
					(*iter)->distanceAttenuation(p) * I_light[2] *
					(kd[2] * (1 - kt[2]) * max(0.0, (N.dot(L))) + ks[2] * pow(max(0.0, V.dot(R)), shininess * 128));
			}
		}
	}
	return intensity;
}
