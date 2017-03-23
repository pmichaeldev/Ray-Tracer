/*
* SPHERE CLASS IMPLEMENTATION (Geometric Shape)
*/
#include "Sphere.h"

// --- CONSTRUCTOR --- //
Sphere::Sphere(vec3 center, float radius, vec3 ambient, vec3 diffuse, vec3 specular, float alpha)
	: Geometry(ambient, diffuse, specular, alpha) // Build from base class
{
	this->center = center;
	this->radius = radius;
}

Sphere::~Sphere() { } // Default destructor

// --- SETTERS --- //
void Sphere::setRadius(float radius)	{ this->radius = radius;	}
void Sphere::setCenter(vec3 center)		{ this->center = center;	}

// --- GETTERS --- //
float Sphere::getRadius()	const { return this->radius; }
vec3 Sphere::getCenter()	const { return this->center; }

// --- HELPERS --- //
/*
pair<bool, float> Sphere::intersection(Rays ray) {
	// --- SETUP ALL NECESSARY VARIABLES
	// Need: origin [from COP] (o), direction (l), radius (r), center (c)
	// Then need to perform some math to find necessary values
	vec3 o = ray.getOrigin(); // Ray Origin
	vec3 l = ray.getDirection(); // Ray Direction
	float r = this->getRadius(); // Radius
	float r2 = pow(r, 2); // r squared
	vec3 c = this->getCenter(); // Sphere center
	vec3 o_minus_c = o - c; // Origin - Center
	float o_minus_c_dot = dot(o_minus_c, o_minus_c); // dot( origin - center,  origin - center)
	float l_dot_o_minus_c = dot(l, o_minus_c); // dot(direction, origin - center)
	float l_dot_o_minus_c_2 = pow(l_dot_o_minus_c, 2); //  dot(direction, origin - center) ^2
	float l_dot_l = dot(l, l); // dot(direction, direction)

	// GEOMETRIC METHOD
	// Compute the result
	// result of: dot(l, o - c)^2 - dot(l, l) x (dot(o - c, o - c) - r^2)
	//float result = l_dot_o_minus_c_2 - l_dot_l * (o_minus_c_dot - r2);
	
	// b*b - 4 * a * c
	float result = pow(2 * l_dot_o_minus_c, 2) - 4 * l_dot_l * (o_minus_c_dot - r2);

	float t = 0.0f;
	// --------------------------------

	if (result < 0.0f) {
		cout << "NO INTERSECTION. -- SPHERE Result < 0" << endl;
		return make_pair(false, -1);
	}
	else if (result == 0.0f) {
		// Result for t is -dot(l, o - c) / dot(l, l)
		t = -l_dot_o_minus_c / l_dot_l;
	}
	else {
		// Assume result > 0 here
		// Distance is the sqrt of the result
		float dist = sqrt(result);
		// Solve to obtain t1 and t2
		float t1 = (2*-l_dot_o_minus_c + dist) / (2*l_dot_l); // -b + sqrt(result) / 2a
		float t2 = (2*-l_dot_o_minus_c - dist) / (2*l_dot_l); // -b - sqrt(result) / 2a

		if (t1 >= 0.0f && t2 >= 0.0f) {
			// If t1, t2 > 0, return min(t1, t2)
			t = std::min(t1, t2);
			cout << "FOUND INTERSECTION -- SPHERE" << endl;
		}
		else if ((t1 < 0.0f && t2 >= 0.0f) || (t2 < 0.0f && t1 >= 0.0f)) {
			// Opposite case of the above, so return max instead
			t = std::max(t1, t2);
			cout << "FOUND INTERSECTION -- SPHERE" << endl;
		}
		else {
			cout << "NO INTERSECTION. -- SPHERE t has unknown behavior." << endl; // Default, should not reach [error]
			return make_pair(false, -1);
		}
	}

	// Return a pair to signal if it intersected, and our end t value (the distance)
	return make_pair(t >=0, t);
}
*/

// SPHERE INTERSECTION CASE
pair<bool, float> Sphere::intersection(Rays ray) {
	// Acquire necessary data
	vec3 direction = ray.getDirection(); // Direction vector (normalized)
	vec3 origin = ray.getOrigin(); // Origin vector
	vec3 center = this->getCenter(); // Get current sphere's center vec3
	
	// Setup necessary variables
	float r = this->getRadius();
	float r2 = pow(r, 2);
	vec3 o_c = origin - center;
	float b = 2 * dot(direction, o_c);
	float b2 = pow(b, 2);
	float c = dot(o_c, o_c) - r2;
	// Discriminant = b^2 - 4c
	float disc = b2 - 4 * c;

	// Check with discriminant
	if (disc < 0) {
		//cout << "NO INTERSECTION -- SPHERE -> case disc < 0.0f" << endl;
		return make_pair(false, -1); // No intersect
	}
	else if (disc == 0.0f) {
		//cout << "FOUND INTERSECTION -- SPHERE -> case disc == 0.0f" << endl;
		return make_pair(true, -b / 2);
	}
	else {
		float t1 = (-b + sqrt(disc)) / 2; // (-b + sqrt(discriminant)) / 2
		float t2 = (-b - sqrt(disc)) / 2; // (-b - sqrt(discriminant)) / 2

		// If t1 is strictly less than 0, give back t2
		if (t1 < 0) {
			//cout << "FOUND INTERSECTION -- SPHERE -> case t1 < 0" << endl;
			return make_pair(true, t2);
		}
		// If t2 is strictly less than 0, give back t1
		else if (t2 < 0) {
			//cout << "FOUND INTERSECTION -- SPHERE -> case t2 < 0" << endl;
			return make_pair(true, t1);
		}
		else {
			//cout << "FOUND INTERSECTION -- SPHERE -> case min(t1, t2)" << endl;
			return make_pair(true, std::min(t1, t2)); // The case we want: return the minimum of the two
		}
	}
}

// Phong light implementation for sphere
vec3 Sphere::phong(vec3 q, Lights* light) {
	// Get the light position
	vec3 l = light->getPosition() - q;
	// K differential 
	float Kd = 0.5f;
	// L.N
	float l_dot_n = std::max(dot(l, normalize(q - this->getCenter())), 0.0f);
	// d is length of L
	float d = length(l);
	// Get the attenuation factor
	float attenDenom = pow(((d / this->getRadius()) + 1), 2);
	float atten = 1 / attenDenom;

	// Set up RGB
	float r = atten * Kd * l_dot_n * (light->getColor().r + this->getDiffuse().r);
	float g = atten * Kd * l_dot_n * (light->getColor().g + this->getDiffuse().g);
	float b = atten * Kd * l_dot_n * (light->getColor().b + this->getDiffuse().b);

	// Return the color vector, (light color + diffuse color) * L.N * attenuation
	return vec3((light->getColor() + this->getDiffuse()) * l_dot_n * atten);
}