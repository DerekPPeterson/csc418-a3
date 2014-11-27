/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements scene_object.h

***********************************************************/

#include <cmath>
#include <iostream>
#include "scene_object.h"
#include "obj_io.h"

using namespace std;

Intersection rayPlaneIntersect( Ray3D& obRay, Point3D point, Vector3D normal, 
        bool (*bounds)(double x, double y))
{
    Intersection intersection;
    intersection.none = true;

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

bool checkerBounds(double x, double y)
{
    if (long(x) % 2 != x > 0) {
        return long(y) % 2 != y > 0;
    } else {
        return long(y) % 2 == 0 != y > 0;
    }
}

bool Checkerboard::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {

    // Ray in object space
    Ray3D obRay = Ray3D();
    obRay.origin = worldToModel * ray.origin;
    obRay.dir    = worldToModel * ray.dir;

    Intersection intersection = rayPlaneIntersect(obRay, Point3D(0.0, 0.0, 0.0), 
            Vector3D(0, 0, 1), checkerBounds);
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


bool UnitSquare::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {

    // Ray in object space
    Ray3D obRay = Ray3D();
    obRay.origin = worldToModel * ray.origin;
    obRay.dir    = worldToModel * ray.dir;

    Intersection intersection = rayPlaneIntersect(obRay, Point3D(0, 0, 0), 
            Vector3D(0, 0, 1), squareBounds);
    if (intersection.none) {
        return false;
    }

    // check if closer intersection or reverse intersection
    if (intersection.t_value < 1e-6 ||
            (!ray.intersection.none
            && intersection.t_value > ray.intersection.t_value)) {
        return false;
    }
    
    // make texture mapping
    ray.intersection.canTexture = true;
    ray.intersection.tex_x = intersection.point[0] + 0.5;
    ray.intersection.tex_y = intersection.point[1] + 0.5;

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
    double c = rayOrginVec.dot(rayOrginVec) - 0.5 * 0.5;

    // determin if intersection
    double det = b * b - 4 * a * c;

    if (det < 0) {
        // no intersection
        return false;
    } else {
        // one or 2 intersections
        double t_val_norm1 = (-b + sqrt(det)) / 2 / a;
        double t_val_norm2 = (-b - sqrt(det)) / 2 / a;

        double t_val = -1;
        if (t_val_norm1 > 0) {
            t_val = t_val_norm1;
            
            if (t_val_norm2 > 0 && t_val_norm2 < t_val_norm1) {
                t_val = t_val_norm2;
            }
        }
        if (t_val_norm2 > 0) {
            t_val = t_val_norm2;
            
            if (t_val_norm1 > 0 && t_val_norm1 < t_val_norm2) {
                t_val = t_val_norm1;
            }
        }

        obRay.intersection.t_value = t_val;
        
        // check if closer intersection or reverse intersection
        if (obRay.intersection.t_value < 1e-6 ||
                (!ray.intersection.none
                && obRay.intersection.t_value > ray.intersection.t_value)) {
            return false;
        }

        obRay.intersection.point = 
            obRay.origin + obRay.intersection.t_value * obRay.dir;

        Vector3D normal = obRay.intersection.point - origin;

        // Texturing
        ray.intersection.canTexture = true;
        ray.intersection.tex_x = 1 -0.5 - atan2(- obRay.intersection.point[2] * 2, 
                - obRay.intersection.point[0] * 2) / 2 / M_PI;
        ray.intersection.tex_y = 0.5 - asin(- obRay.intersection.point[1] * 2) / M_PI;

        // Convert back to world space
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

    Intersection intersection = rayPlaneIntersect(obRay, Point3D(0, 0, 0), 
            Vector3D(0, 0, 1), circleBounds);
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
    Intersection topIntersection = rayPlaneIntersect(obRay, Point3D(0, 0, 0.5), 
            Vector3D(0, 0, 1), circleBounds);
    Intersection bottomIntersection = rayPlaneIntersect(obRay, 
            Point3D(0, 0, -0.5), Vector3D(0, 0, -1), circleBounds);
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

bool fullPlane(double x, double y)
{
    return true;
}

bool Mesh::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {

    // Ray in object space
    Ray3D obRay = Ray3D();
    obRay.origin = worldToModel * ray.origin;
    obRay.dir    = worldToModel * ray.dir;

    Vector3D origin(0, 0, 0);

    Intersection intersection;
    intersection.none = true;

    faces = this->faces;
    for (int i = 0; i < this->nFaces; i++) {
        TriangleFace face = faces[i];

//        cout << face.points[0] << "\n";
//        cout << face.points[1] << "\n";
//        cout << face.points[2] << "\n\n";

        // Need to pass triangle points into the bounds function, so have to 
        // do the plane boundry checking outside
        Intersection faceIntersect = rayPlaneIntersect(obRay, face.points[0],
                face.normal, fullPlane);
        faceIntersect.none = true;

        // Test if point is inside the triangle using barycentric coordinates
        Vector3D u = face.points[1] - face.points[0];
        Vector3D v = face.points[2] - face.points[0];
        Vector3D w = faceIntersect.point - face.points[0];

        // Precompute cross products
        Vector3D vcw = v.cross(w);
        Vector3D ucw = u.cross(w);
        Vector3D vcu = v.cross(u);
        Vector3D ucv = -vcu;

        // Signs of parameters
        double r_sign = vcw.dot(vcu);
        double t_sign = ucw.dot(ucv);
        if (! ( r_sign > 0 && t_sign > 0)) {
            continue;
        }

        Vector3D x = faceIntersect.point-face.points[0];

        // Magintudes of parameters
        double den = sqrt(ucv.dot(ucv));
        double r = sqrt(vcw.dot(vcw)) / den;
        double t = sqrt(ucw.dot(ucw)) / den;
        if (! (r + t <= 1) ) {
            continue;
        }

        faceIntersect.none = false;
        intersection = chooseIntersection(intersection, faceIntersect);
    }

    if (intersection.none) {
        return false;
    }

    // convert back to world space
    ray.intersection.none = false;
    ray.intersection.point = modelToWorld * intersection.point;
    ray.intersection.normal = transNorm(worldToModel, intersection.normal);
    ray.intersection.t_value = intersection.t_value;

	return true;
}

