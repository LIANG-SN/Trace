#include "ray.h"
#include "material.h"
#include "light.h"
#include <algorithm>
// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
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
			vec3f R = (2 * (L.dot(N)) * N - L).normalize();
			vec3f V = (scene->getCamera()->getEye() - p).normalize();
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
	return intensity;
}
