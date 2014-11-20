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

    // Ray in object space
    Ray3D obRay = Ray3D();
    obRay.origin = worldToModel * ray.origin;
    obRay.dir    = worldToModel * ray.dir;

    Vector3D normal = Vector3D(0, 0, 1);
    Point3D point = Point3D(0, 0, 0);

    double denominator = obRay.dir.dot(normal);

    // If denominator is 0 then ray is parallel
    if (abs(denominator) < 1e-6) {
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
            return false;
        }
    }
    
    // check if closer intersection or reverse intersection
    if (obRay.intersection.t_value < 1e-6 ||
            (!ray.intersection.none
            && obRay.intersection.t_value > ray.intersection.t_value)) {
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
    
    // Ray in object space
    Ray3D obRay = Ray3D();
    obRay.origin = worldToModel * ray.origin;
    obRay.dir    = worldToModel * ray.dir;


    Point3D origin = Point3D(0, 0, 0);
    Vector3D rayOrginVec = obRay.origin - origin;

    // Parameters TODO see wikipedia
    double a = obRay.dir.dot(obRay.dir);
    double b = 2 * obRay.dir.dot(rayOrginVec);
    double c = rayOrginVec.dot(rayOrginVec) - 0.25;

    // determin if intersection
    double det = b * b - 4 * a * c;

    if (det < 0) {
        // no intersection
        return false;
    } else {
        // one or 2 intersections
        double t_val_norm1 = (-b + sqrt(det)) / 2 / a;
        double t_val_norm2 = (-b - sqrt(det)) / 2 / a;

        if (t_val_norm1 < 0) t_val_norm1 = 0;
        if (t_val_norm2 < 0) t_val_norm2 = 0;

        obRay.intersection.t_value = min(t_val_norm1, t_val_norm2);
        
        // check if closer intersection or reverse intersection
        if (obRay.intersection.t_value < 1e-6 ||
                (!ray.intersection.none
                && obRay.intersection.t_value > ray.intersection.t_value)) {
            return false;
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

bool UnitCircle::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {

    // Ray in object space
    Ray3D obRay = Ray3D();
    obRay.origin = worldToModel * ray.origin;
    obRay.dir    = worldToModel * ray.dir;

    Vector3D normal = Vector3D(0, 0, 1);
    Point3D point = Point3D(0, 0, 0);

    double denominator = obRay.dir.dot(normal);

    // If denominator is 0 then ray is parallel
    if (abs(denominator) < 1e-6) {
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
        if (x * x + y * y > 0.25) {
            return false;
        }
    }
    
    // check if closer intersection or reverse intersection
    if (obRay.intersection.t_value < 1e-6 ||
            (!ray.intersection.none
            && obRay.intersection.t_value > ray.intersection.t_value)) {
        return false;
    }

    // convert back to world space
    ray.intersection.none = false;
    ray.intersection.point = modelToWorld * obRay.intersection.point;
    ray.intersection.normal = transNorm(worldToModel, normal);
    ray.intersection.t_value = obRay.intersection.t_value;

	return true;
}

bool UnitCylinder::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {

    // Ray in object space
    Ray3D obRay = Ray3D();
    obRay.origin = worldToModel * ray.origin;
    obRay.dir    = worldToModel * ray.dir;

    // Calculate 2D circle intersection
    Vector3D dir2D(obRay.dir[0], obRay.dir[1], 0);
    Vector3D or2D(obRay.origin[0], obRay.origin[1], 0);

    double a = dir2D.dot(dir2D);
    double b = 2 * dir2D.dot(or2D);
    double c = or2D.dot(or2D) - 0.25;

    double det = b * b - 4 * a * c;

    // check if any intersection
    if (det < 0) {
        return false;
    }

    double t1 = (-b + sqrt(det)) / 2 / a;
    double t2 = (-b - sqrt(det)) / 2 / a;

    double t_val = min(t1, t2);

    Point3D point = obRay.origin + t_val * obRay.dir;

    // check if intersect in correct height
    if (abs(point[2]) > 0.5) {
        return false;
    }

    // check if closer intersection or reverse intersection
    if (t_val< 1e-6 ||
            (!ray.intersection.none
            && t_val > ray.intersection.t_value)) {
        return false;
    }
    

    Vector3D normal(point[0], point[1], 0);

    // convert back to world space
    ray.intersection.none = false;
    ray.intersection.point = modelToWorld * point;
    ray.intersection.normal = transNorm(worldToModel, normal);
    ray.intersection.t_value = t_val;

	return true;
}
