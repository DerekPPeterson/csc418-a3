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
    Material* mat = ray.intersection.mat;

    Colour ambient = _col_ambient * ray.intersection.mat->ambient;
    col = col + ambient;

    if (ray.intersection.shadow) {
        ray.col = col;
        return;
    }

    // Ray from intersection point to light source
    Vector3D lightDir = get_position() - ray.intersection.point;
    Vector3D normal = ray.intersection.normal;
    normal.normalize();
    lightDir.normalize();

    Colour diffuse = max(0.0, normal.dot(lightDir)) * ray.intersection.mat->diffuse * _col_diffuse;
    col = col + diffuse;

    // Perfect reflection dir
    Vector3D reflectDir = 2 * (normal.dot(lightDir)) * normal - lightDir;
    Point3D origin(0, 0, 0);
    Vector3D viewDir = - ray.dir;
    viewDir.normalize();
    Colour specular = pow(max(reflectDir.dot(viewDir), 0.0), mat->specular_exp)
            * _col_specular * ray.intersection.mat->specular ;
    col = col + specular;

    col.clamp();
    ray.col = col;

}

