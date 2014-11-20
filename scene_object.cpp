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

Intersection rayPlaneIntersect( Ray3D& obRay, double z, 
        bool (*bounds)(double x, double y))
{
    Intersection intersection;
    intersection.none = true;

    Vector3D normal = Vector3D(0, 0, 1);
    Point3D point = Point3D(0, 0, z);

    double denominator = obRay.dir.dot(normal);

    // If denominator is 0 then ray is parallel
    if (abs(denominator) > 1e-6) {

        // Calculate intersection point
        double numerator = (point - obRay.origin).dot(normal);
        intersection.t_value = numerator / denominator;
        intersection.point = obRay.origin + intersection.t_value * obRay.dir;
        intersection.normal = normal;

        // bounds check
        if (bounds(intersection.point[0], intersection.point[1]) 
                && intersection.t_value > 0) {
            intersection.none = false;
        }
    }

    return intersection;
}

bool squareBounds(double x, double y)
{
    return abs(x) < 0.5 && abs(y) < 0.5;
}

bool UnitSquare::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {

    // Ray in object space
    Ray3D obRay = Ray3D();
    obRay.origin = worldToModel * ray.origin;
    obRay.dir    = worldToModel * ray.dir;

    Intersection intersection = rayPlaneIntersect(obRay, 0, squareBounds);
    if (intersection.none) {
        return false;
    }

    // check if closer intersection or reverse intersection
    if (intersection.t_value < 1e-6 ||
            (!ray.intersection.none
            && intersection.t_value > ray.intersection.t_value)) {
        return false;
    }

    // convert back to world space
    ray.intersection.none = false;
    ray.intersection.point = modelToWorld * intersection.point;
    ray.intersection.normal = transNorm(worldToModel, intersection.normal);
    ray.intersection.t_value = intersection.t_value;

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

bool circleBounds(double x, double y) 
{
    return 0.25 > (x * x + y * y);
}

bool UnitCircle::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {

    // Ray in object space
    Ray3D obRay = Ray3D();
    obRay.origin = worldToModel * ray.origin;
    obRay.dir    = worldToModel * ray.dir;

    Intersection intersection = rayPlaneIntersect(obRay, 0, circleBounds);
    if (intersection.none) {
        return false;
    }

    // check if closer intersection or reverse intersection
    if (intersection.t_value < 1e-6 ||
            (!ray.intersection.none
            && intersection.t_value > ray.intersection.t_value)) {
        return false;
    }

    // convert back to world space
    ray.intersection.none = false;
    ray.intersection.point = modelToWorld * intersection.point;
    ray.intersection.normal = transNorm(worldToModel, intersection.normal);
    ray.intersection.t_value = intersection.t_value;

	return true;
}

Intersection chooseIntersection(Intersection I1, Intersection I2)
{
    Intersection I;
    I.none = true;

    if (!I1.none && I1.t_value > 0) {
        I = I1;
        
        if (!I2.none && I2.t_value < I1.t_value) {
            I = I2;
        }
    }

    if (!I2.none && I2.t_value > 0) {
        I = I2;

        if (!I1.none && I1.t_value < I2.t_value) {
            I = I1;
        }
    }

    return I;
}

bool UnitCylinder::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {

    // Ray in object space
    Ray3D obRay = Ray3D();
    obRay.origin = worldToModel * ray.origin;
    obRay.dir    = worldToModel * ray.dir;

    Intersection sideIntersection;
    sideIntersection.none = true;

    // Cylinder side intersection
    // Calculate 2D circle intersection
    Vector3D dir2D(obRay.dir[0], obRay.dir[1], 0);
    Vector3D or2D(obRay.origin[0], obRay.origin[1], 0);

    double a = dir2D.dot(dir2D);
    double b = 2 * dir2D.dot(or2D);
    double c = or2D.dot(or2D) - 0.25;

    double det = b * b - 4 * a * c;

    // Calculate intersection
    if (det < 0) {
        return false;
    } else {
        double t1 = (-b + sqrt(det)) / 2 / a;
        double t2 = (-b - sqrt(det)) / 2 / a;
        sideIntersection.t_value = min(t1, t2);

        sideIntersection.point = obRay.origin + sideIntersection.t_value * obRay.dir;

        // check if intersect in correct height
        if (abs(sideIntersection.point[2]) < 0.5) {
             sideIntersection.normal[0] = sideIntersection.point[0];
             sideIntersection.normal[1] = sideIntersection.point[1];
             sideIntersection.normal[2] = 0;
             sideIntersection.none = false;
        }
    }

    // check top and bottom
    Intersection topIntersection = rayPlaneIntersect(obRay, 0.5, circleBounds);
    Intersection bottomIntersection = rayPlaneIntersect(obRay, -0.5, circleBounds);
    bottomIntersection.normal = Vector3D(0, 0, -1);

    Intersection intersection = sideIntersection;
    intersection = chooseIntersection(intersection, bottomIntersection);
    intersection = chooseIntersection(intersection, topIntersection);

    // check if closer intersection or reverse intersection
    if (intersection.t_value < 1e-6 ||
            (!ray.intersection.none
            && intersection.t_value > ray.intersection.t_value)) {
        return false;
    }

    if (intersection.none && intersection.t_value > 0) {
        return false;
    }
    
    // convert back to world space
    ray.intersection.none = false;
    ray.intersection.point = modelToWorld * intersection.point;
    ray.intersection.normal = transNorm(worldToModel, intersection.normal);
    ray.intersection.t_value = intersection.t_value;

	return true;
}

