/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"
#include <iostream>

using namespace std;

void PointLight::shade( Ray3D& ray ) {
	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.  

    Colour col(0, 0, 0);

    // Ray from intersection point to light source
    Vector3D lightDir = get_position() - ray.intersection.point;
    Vector3D normal = ray.intersection.normal;
    normal.normalize();
    lightDir.normalize();

    // diffuse
    Colour diffuse = max(0.0, normal.dot(lightDir)) * ray.intersection.mat->diffuse * _col_diffuse;
    col = col + diffuse;

    ray.col = col;

}

