/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		Implementations of functions in raytracer.h, 
		and the main function which specifies the 
		scene to be rendered.	

***********************************************************/


#include "raytracer.h"
#include "bmp_io.h"
#include "scenes.h"
#include <cmath>
#include <iostream>
#include <cstdlib>

using namespace std;

Raytracer::Raytracer() : _lightSource(NULL) {
	_root = new SceneDagNode();

    // set the default render options
    setFeatures(1, 1, 0, 1, 2, 2, true);
}

void Raytracer::setFeatures(int nAA, int nDOF, double aperture, 
        double focal_length, int refract_depth, int reflect_depth, 
        bool shadows) 
{
    _nAA = nAA;
    _nDOF = nDOF;
    _aperture = aperture;
    _focal_length = focal_length;
    _refract_depth = refract_depth;
    _reflect_depth = reflect_depth;
    _shadows = shadows;
    _signature = false;
    _specular = true;
}

void Raytracer::setFeatures(int nAA, int nDOF, double aperture, 
        double focal_length, int refract_depth, int reflect_depth, 
        bool shadows, bool signature, bool specular) 
{
    _nAA = nAA;
    _nDOF = nDOF;
    _aperture = aperture;
    _focal_length = focal_length;
    _refract_depth = refract_depth;
    _reflect_depth = reflect_depth;
    _shadows = shadows;
    _signature = signature;
    _specular = specular;
}

Raytracer::~Raytracer() {
	delete _root;
}

SceneDagNode* Raytracer::addObject( SceneDagNode* parent, 
		SceneObject* obj, Material* mat ) {
	SceneDagNode* node = new SceneDagNode( obj, mat );
	node->parent = parent;
	node->next = NULL;
	node->child = NULL;
	
	// Add the object to the parent's child list, this means
	// whatever transformation applied to the parent will also
	// be applied to the child.
	if (parent->child == NULL) {
		parent->child = node;
	}
	else {
		parent = parent->child;
		while (parent->next != NULL) {
			parent = parent->next;
		}
		parent->next = node;
	}
	
	return node;;
}

LightListNode* Raytracer::addLightSource( LightSource* light ) {
	LightListNode* tmp = _lightSource;
	_lightSource = new LightListNode( light, tmp );
	return _lightSource;
}

void Raytracer::rotate( SceneDagNode* node, char axis, double angle ) {
	Matrix4x4 rotation;
	double toRadian = 2*M_PI/360.0;
	int i;
	
	for (i = 0; i < 2; i++) {
		switch(axis) {
			case 'x':
				rotation[0][0] = 1;
				rotation[1][1] = cos(angle*toRadian);
				rotation[1][2] = -sin(angle*toRadian);
				rotation[2][1] = sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'y':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][2] = sin(angle*toRadian);
				rotation[1][1] = 1;
				rotation[2][0] = -sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'z':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][1] = -sin(angle*toRadian);
				rotation[1][0] = sin(angle*toRadian);
				rotation[1][1] = cos(angle*toRadian);
				rotation[2][2] = 1;
				rotation[3][3] = 1;
			break;
		}
		if (i == 0) {
		    node->trans = node->trans*rotation; 	
			angle = -angle;
		} 
		else {
			node->invtrans = rotation*node->invtrans; 
		}	
	}
}

void Raytracer::translate( SceneDagNode* node, Vector3D trans ) {
	Matrix4x4 translation;
	
	translation[0][3] = trans[0];
	translation[1][3] = trans[1];
	translation[2][3] = trans[2];
	node->trans = node->trans*translation; 	
	translation[0][3] = -trans[0];
	translation[1][3] = -trans[1];
	translation[2][3] = -trans[2];
	node->invtrans = translation*node->invtrans; 
}

void Raytracer::scale( SceneDagNode* node, Point3D origin, double factor[3] ) {
	Matrix4x4 scale;
	
	scale[0][0] = factor[0];
	scale[0][3] = origin[0] - factor[0] * origin[0];
	scale[1][1] = factor[1];
	scale[1][3] = origin[1] - factor[1] * origin[1];
	scale[2][2] = factor[2];
	scale[2][3] = origin[2] - factor[2] * origin[2];
	node->trans = node->trans*scale; 	
	scale[0][0] = 1/factor[0];
	scale[0][3] = origin[0] - 1/factor[0] * origin[0];
	scale[1][1] = 1/factor[1];
	scale[1][3] = origin[1] - 1/factor[1] * origin[1];
	scale[2][2] = 1/factor[2];
	scale[2][3] = origin[2] - 1/factor[2] * origin[2];
	node->invtrans = scale*node->invtrans; 
}

Matrix4x4 Raytracer::initInvViewMatrix( Point3D eye, Vector3D view, 
		Vector3D up ) {
	Matrix4x4 mat; 
	Vector3D w;
	view.normalize();
	up = up - up.dot(view)*view;
	up.normalize();
	w = view.cross(up);

	mat[0][0] = w[0];
	mat[1][0] = w[1];
	mat[2][0] = w[2];
	mat[0][1] = up[0];
	mat[1][1] = up[1];
	mat[2][1] = up[2];
	mat[0][2] = -view[0];
	mat[1][2] = -view[1];
	mat[2][2] = -view[2];
	mat[0][3] = eye[0];
	mat[1][3] = eye[1];
	mat[2][3] = eye[2];

	return mat; 
}

void Raytracer::traverseScene( SceneDagNode* node, Ray3D& ray ) {
	SceneDagNode *childPtr;

	// Applies transformation of the current node to the global
	// transformation matrices.
	_modelToWorld = _modelToWorld*node->trans;
	_worldToModel = node->invtrans*_worldToModel; 
	if (node->obj) {
		// Perform intersection.
		if (node->obj->intersect(ray, _worldToModel, _modelToWorld)) {
			ray.intersection.mat = node->mat;
		}
	}
	// Traverse the children.
	childPtr = node->child;
	while (childPtr != NULL) {
		traverseScene(childPtr, ray);
		childPtr = childPtr->next;
	}

	// Removes transformation of the current node from the global
	// transformation matrices.
	_worldToModel = node->trans*_worldToModel;
	_modelToWorld = _modelToWorld*node->invtrans;
}

void Raytracer::computeShading( Ray3D& ray ) {
	LightListNode* curLight = _lightSource;
	for (;;) {
		if (curLight == NULL) break;
		// Each lightSource provides its own shading function.

        // shadows
        if (_shadows) {
            Vector3D shadowDir = curLight->light->get_position() - ray.intersection.point;
            shadowDir.normalize();
            Point3D shadowOrigin = ray.intersection.point + 1e-6 * shadowDir; // avoid self-intersection
            Ray3D shadowRay(shadowOrigin, shadowDir, 0, 0);
            traverseScene(_root, shadowRay);

            ray.intersection.shadow = (!shadowRay.intersection.none);
        }

        // actually shade
        curLight->light->shade(ray);

        // Texture
        if (ray.intersection.mat->texture != NULL && ray.intersection.canTexture) {
            ray.col = ray.col * ray.intersection.mat->texture->getCol(
                    ray.intersection.tex_x, ray.intersection.tex_y);
        }

        curLight = curLight->next;
	}
}

void Raytracer::initPixelBuffer() {
	int numbytes = _scrWidth * _scrHeight * sizeof(unsigned char);
	_rbuffer = new unsigned char[numbytes];
	_gbuffer = new unsigned char[numbytes];
	_bbuffer = new unsigned char[numbytes];
	for (int i = 0; i < _scrHeight; i++) {
		for (int j = 0; j < _scrWidth; j++) {
			_rbuffer[i*_scrWidth+j] = 0;
			_gbuffer[i*_scrWidth+j] = 0;
			_bbuffer[i*_scrWidth+j] = 0;
		}
	}
}

void Raytracer::flushPixelBuffer( char *file_name ) {
	bmp_write( file_name, _scrWidth, _scrHeight, _rbuffer, _gbuffer, _bbuffer );
	delete _rbuffer;
	delete _gbuffer;
	delete _bbuffer;
}

Colour Raytracer::shadeRay( Ray3D& ray ) {
	Colour col(0.0, 0.0, 0.0); 
	traverseScene(_root, ray); 

    // scene signature
    if (_signature) {
        if (ray.intersection.none) {
            return col;
        } else {
            col = ray.intersection.mat->diffuse;
            return col;
        }
    }
    
    // scene signature
    if (!_specular) {
        if (ray.intersection.none) {
            return col;
        } else {
            Material *rayMat = ray.intersection.mat;
            Material temp(rayMat->ambient, rayMat->diffuse, Colour(0, 0, 0), 0);
            ray.intersection.mat = &temp;
            computeShading(ray);
            return ray.col;
        }
    }

	// Don't bother shading if the ray didn't hit 
	// anything.
	if (!ray.intersection.none) {

		computeShading(ray); 
		col = ray.col;  

        // Reflection
        if (ray.reflectCount > 0 && ray.intersection.mat->reflects) {
            Vector3D normal = ray.intersection.normal;
            normal.normalize();
            Vector3D dir = ray.dir;
            dir.normalize();

            // correct wrong facing normals
            if (normal.dot(dir) > 0) {
                normal = -normal;
            }

            Vector3D reflectDir = dir - ( normal.dot(dir) * 2 * normal);

            Ray3D reflectRay(ray.intersection.point + 1e-6 * reflectDir, reflectDir, 
                    ray.reflectCount - 1, ray.refractCount);

            col = 
                col + ray.intersection.mat->reflective * shadeRay(reflectRay);
            col.clamp();
        }

        // Refraction
        if (ray.refractCount > 0 && ray.intersection.mat->clear) {
            Vector3D normal = ray.intersection.normal;
            normal.normalize();
            normal = normal;
            Vector3D dir = ray.dir;
            dir.normalize();
            double n = ray.intersection.mat->n;

            double c = - normal.dot(dir);
            double r;

            // if entering a solid
            if (!ray.inside) {
                r = 1.0 / n;
            // if exiting
            } else {
                r = n;
            }
            
            // correct surface normal if wrong way
            if (c < 0) {
                normal = -normal;
                c = -c;
            }

            Vector3D refractDir = r * dir + (r * c - sqrt( 1 - r* r * ( 1 - c* c))) * normal;
            refractDir.normalize();

            // only if no totoal internal reflection
            //if (refractDir.dot(dir) > 0) {
            if (true) {
                Ray3D refractRay(ray.intersection.point + 2e-6 * refractDir , refractDir, 
                        ray.reflectCount, ray.refractCount - 1);
                refractRay.inside = !ray.inside;
                col = 
                    col + ray.intersection.mat->transparency * shadeRay(refractRay);
                col.clamp();
            }
        }
	}

	return col; 
}	

void Raytracer::render( int width, int height, Point3D eye, Vector3D view, 
		Vector3D up, double fov, char* fileName ) {

	Matrix4x4 viewToWorld;
	_scrWidth = width;
	_scrHeight = height;
	double factor = (double(height)/2)/tan(fov*M_PI/360.0);

	initPixelBuffer();
	viewToWorld = initInvViewMatrix(eye, view, up);


   // DOF
   const int nDOF = _nDOF;
   const double aperture = _aperture;
   const double focal_length = _focal_length;
   
   // Random AA
   const int nAA = _nAA;
   double AA_max = 1.0 / factor;

	// Construct a ray for each pixel.
    long total = _scrHeight * _scrWidth;
	for (int i = 0; i < _scrHeight; i++) {
		for (int j = 0; j < _scrWidth; j++) {
			Point3D imagePlane;
			imagePlane[0] = (-double(width)/2 + 0.5 + j)/factor * focal_length;
			imagePlane[1] = (-double(height)/2 + 0.5 + i)/factor * focal_length;
			imagePlane[2] = -focal_length;

            Colour col(0, 0, 0);
            for (int k=0; k < nAA; k++) {
                for (int w=0; w < nDOF; w++) {


                    Point3D rayTarget;
                    if (nAA > 1) {
                        // calculate random AA offsets in the pixel
                        Vector3D AA_offset;
                        AA_offset[0] = ((double) rand() / (RAND_MAX) - 0.5) 
                            * AA_max * focal_length;
                        AA_offset[1] = ((double) rand() / (RAND_MAX) - 0.5) 
                            * AA_max * focal_length;
                        AA_offset[2] = 0;
                        rayTarget = imagePlane + AA_offset;
                    } else {
                        rayTarget = imagePlane;
                    }

			        Point3D origin(0, 0, 0);
                    if (nDOF > 1) {
                        Vector3D DOF_offset;
                        double r2 = ((double) rand() / (RAND_MAX)) 
                            * aperture * aperture;
                        double theta = ((double) rand() / (RAND_MAX)) * 2 * M_PI;
                        DOF_offset[0] = sqrt(r2) * cos(theta);
                        DOF_offset[1] = sqrt(r2) * sin(theta);
                        DOF_offset[2] = 0;
                        origin = origin + DOF_offset;
                    }

                    Ray3D ray;
                    origin = origin;
                    ray.origin = viewToWorld * origin;
                    ray.dir = viewToWorld * (rayTarget - origin);
                    ray.dir.normalize();
                    ray.reflectCount = _reflect_depth;
                    ray.refractCount = _reflect_depth;

                    col = col + shadeRay(ray);
                }
            }

            col = (1.0 / nAA / nDOF) * col;

			_rbuffer[i*width+j] = int(col[0]*255);
			_gbuffer[i*width+j] = int(col[1]*255);
			_bbuffer[i*width+j] = int(col[2]*255);
		}
        long done = (i+1) * _scrWidth;
        double percent = double(done) / total * 100;
        cout << fileName << ": " << done << " of " << total << " (";
        printf("%.2f", percent);
        cout << "%)\r";
        fflush(stdout);
	}
    cout << "\n";

	flushPixelBuffer(fileName);
}

int main(int argc, char* argv[])
{	
    int width = 320;
    int height = 240;
    for (int i = 1; i < argc; i++) {
		width = atoi(argv[1]);
		height = atoi(argv[2]);
    }

    //part1(width, height);
    //mega_render(width, height);
    //meshes_cube_guy(width, height);
    textures(width, height);
	
	return 0;
}

