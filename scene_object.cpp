/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements scene_object.h

***********************************************************/

#include <cmath>
#include <iostream>
#include "scene_object.h"

using namespace std;

bool UnitSquare::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSquare, which is
	// defined on the xy-plane, with vertices (0.5, 0.5, 0), 
	// (-0.5, 0.5, 0), (-0.5, -0.5, 0), (0.5, -0.5, 0), and normal
	// (0, 0, 1).
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.

    // Ray in object space
    Ray3D obRay = Ray3D();
    obRay.origin = worldToModel * ray.origin;
    obRay.dir    = worldToModel * ray.dir;

    Vector3D normal = Vector3D(0, 0, 1);
    Point3D point = Point3D(0, 0, 0);

    double denominator = obRay.dir.dot(normal);

    // If denominator is 0 then ray is parallel
    if (abs(denominator) < 1e-6) {
        ray.intersection.none = true;
        return false;
    } else {

        // Calculate intersection point
        double numerator = (point - obRay.origin).dot(normal);
        obRay.intersection.t_value = numerator / denominator;
        obRay.intersection.point = 
            obRay.origin + obRay.intersection.t_value * obRay.dir;
        obRay.intersection.normal = normal;


        // Check if in bounds
        double x = obRay.intersection.point[0];
        double y = obRay.intersection.point[1];
        if (abs(x) > 0.5 || abs(y) > 0.5) {
            ray.intersection.none = true;
            return false;
        }
    }
    
    // check if closer intersection
    if (!ray.intersection.none && 
            (obRay.intersection.t_value > ray.intersection.t_value)) {
        return false;
    }

    // convert back to world space
    ray.intersection.none = false;
    ray.intersection.point = modelToWorld * obRay.intersection.point;
    ray.intersection.normal = transNorm(worldToModel, normal);
    ray.intersection.t_value = obRay.intersection.t_value;

	return true;
}

bool UnitSphere::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSphere, which is centred 
	// on the origin.  
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
    
    // Ray in object space
    Ray3D obRay = Ray3D();
    obRay.origin = worldToModel * ray.origin;
    obRay.dir    = worldToModel * ray.dir;


    Point3D origin = Point3D(0, 0, 0);
    Vector3D rayOrginVec = obRay.origin - origin;

    // Parameters TODO see wikipedia
    double a = obRay.dir.dot(obRay.dir);
    double b = 2 * obRay.dir.dot(rayOrginVec);
    double c = rayOrginVec.dot(rayOrginVec) - 1;

    // determin if intersection
    double det = b * b - 4 * a * c;

    if (det < 0) {
        // no intersection
        return false;
    } else {
        // one or 2 intersections
        double t_val_norm1 = (-b + sqrt(det)) / 2 / a;
        double t_val_norm2 = (-b - sqrt(det)) / 2 / a;

        obRay.intersection.t_value = t_val_norm2;
        
        // check if closer intersection
        if (!ray.intersection.none && 
                obRay.intersection.t_value > ray.intersection.t_value) {
        }

        obRay.intersection.point = 
            obRay.origin + obRay.intersection.t_value * obRay.dir;

        Vector3D normal = obRay.intersection.point - origin;

        
        ray.intersection.none = false;
        ray.intersection.t_value = obRay.intersection.t_value;
        ray.intersection.point = modelToWorld * obRay.intersection.point;
        ray.intersection.normal = transNorm(worldToModel, normal);

        return true;
    }
}

