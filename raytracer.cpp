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
#include <cmath>
#include <iostream>
#include <cstdlib>

#pragma GCC diagnostic ignored "-Wwrite-strings"

using namespace std;

Raytracer::Raytracer() : _lightSource(NULL) {
	_root = new SceneDagNode();
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
        Vector3D shadowDir = curLight->light->get_position() - ray.intersection.point;
        shadowDir.normalize();
        Point3D shadowOrigin = ray.intersection.point + 1e-6 * shadowDir; // avoid self-intersection
        Ray3D shadowRay(shadowOrigin, shadowDir, 0, 0);
        traverseScene(_root, shadowRay);

        ray.intersection.shadow = (!shadowRay.intersection.none);

        // actually shade
        curLight->light->shade(ray);

        // Texture
        if (ray.intersection.mat->texture != NULL && ray.intersection.canTexture) {
            //cout << ray.col << "\n";
            ray.col = ray.col * ray.intersection.mat->texture->getCol(
                    ray.intersection.tex_x, ray.intersection.tex_y);
            //cout << ray.col << "\n";
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

    const int nAA = 1;
    double AA_increment = 0.5 / factor / nAA;
    double AA_start = -0.5 / factor / nAA / 2 + AA_increment / 2;

    Vector3D AA_offsets[nAA*nAA];
    for (int i=0; i < nAA; i++) {
        for (int j=0; j < nAA; j++) {
            AA_offsets[nAA*i + j][0] = AA_start + AA_increment * i;
            AA_offsets[nAA*i + j][1] = AA_start + AA_increment * j;
            AA_offsets[nAA*i + j][2] = 0;
        }
    }

	// Construct a ray for each pixel.
    long total = _scrHeight * _scrWidth;
	for (int i = 0; i < _scrHeight; i++) {
		for (int j = 0; j < _scrWidth; j++) {
			Point3D origin(0, 0, 0);
			Point3D imagePlane;
			imagePlane[0] = (-double(width)/2 + 0.5 + j)/factor;
			imagePlane[1] = (-double(height)/2 + 0.5 + i)/factor;
			imagePlane[2] = -1;

            
            Colour col(0, 0, 0);
            for (int k=0; k < nAA * nAA; k++) {
                Point3D rayTarget = imagePlane + AA_offsets[k];
                Ray3D ray;
                ray.origin = viewToWorld * origin;
                ray.dir = viewToWorld * (rayTarget - origin);
                ray.dir.normalize();

                col = col + shadeRay(ray);
            }

            col = (1.0 / nAA / nAA) * col;

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

	// Build your scene and setup your camera here, by calling 
	// functions from Raytracer.  The code here sets up an example
	// scene and renders it from two different view points, DO NOT
	// change this if you're just implementing part one of the 
	// assignment.  
	Raytracer raytracer;
	int width = 320; 
	int height = 240; 

    for (int i = 1; i < argc; i++) {
		width = atoi(argv[1]);
		height = atoi(argv[2]);
    }

	// Camera parameters.
	Point3D eye(0, 0, 1);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);
	double fov = 70;

	// Defines a material for shading.
	Material gold( Colour(0.3, 0.3, 0.3), Colour(0.75164, 0.60648, 0.22648), 
			Colour(0.628281, 0.555802, 0.366065), 
			51.2 , true, 0.5 * Colour(0.628281, 0.555802, 0.366065));
	Material jade( Colour(0.054, 0.089, 0.063), Colour(0.54, 0.89, 0.63), 
			Colour(0.316228, 0.316228, 0.316228), 
			12.8 );
    Material mirror(Colour(0.1, 0.1, 0.1), Colour(0, 0, 0), 
            Colour(0.5, 0.5, 0.5), 50,
            true, Colour(1, 1, 1));

    Material glass(Colour(0.03, 0.03, 0.03), Colour(0, 0, 0), 
            Colour(1, 1, 1), 100,
            false, Colour(0.05, 0.05, 0.05), 
            true, Colour(0.9, 0.9, 0.9), 1);

    Material red( Colour(0.3, 0, 0), Colour(0.7, 0, 0), Colour(0.2, 0, 0), 10);
    Material blue( Colour(0., 0, 0.3), Colour(0, 0, 0.7), Colour(0, 0, 0.2), 10);
    Material green( Colour(0, 0.3, 0), Colour(0, 0.7, 0), Colour(0, 0.2, 0), 10);
    Material white( Colour(0.3, 0.3, 0.3), Colour(0.7, 0.7, 0.7), Colour(0.2, 0.2, 0.2), 10);

    Material worldMat( Colour(0.6, 0.6, 0.6), Colour(0.7, 0.7, 0.7), Colour(0.2, 0.2, 0.2), 10);
    Texture earthTex = Texture("./textures/earth-1024x512.bmp");
    worldMat.texture = &earthTex;

	// Defines a point light source.
	raytracer.addLightSource( new PointLight(Point3D(0, 6, 0), 
				Colour(0.9, 0.9, 0.9) ) );

	// Add a unit square into the scene with material mat.
	//SceneDagNode* plane = raytracer.addObject( new UnitSquare(), &jade );
	SceneDagNode* checker = raytracer.addObject( new Checkerboard(), &jade );
	//SceneDagNode* sphere = raytracer.addObject( new UnitSphere(), &gold );
	//SceneDagNode* sphere2 = raytracer.addObject( new UnitSphere(), &jade );
	//SceneDagNode* circle = raytracer.addObject( new UnitCircle(), &gold );
//	SceneDagNode* mirrorSphere = raytracer.addObject( new UnitSphere(), &mirror );
	//SceneDagNode* glassSphere = raytracer.addObject( new UnitSphere(), &glass );
	//SceneDagNode* mirrorSquare = raytracer.addObject( new UnitSquare(), &mirror );
	//SceneDagNode* cylinder = raytracer.addObject( new UnitCylinder(), &gold );

	//SceneDagNode* redSphere = raytracer.addObject( new UnitSphere(), &red );
	//SceneDagNode* blueSphere = raytracer.addObject( new UnitSphere(), &blue );
	//SceneDagNode* greenSphere = raytracer.addObject( new UnitSphere(), &green );
	//SceneDagNode* whiteSphere = raytracer.addObject( new UnitSphere(), &white );

	//SceneDagNode* cube = raytracer.addObject( new Mesh("obj/cube.obj"), &mirror );
	//SceneDagNode* cow = raytracer.addObject( new Mesh("obj/cow-nonormals.obj"), &mirror );
	SceneDagNode* crab = raytracer.addObject( new Mesh("obj/crab.obj"), &red );

//	SceneDagNode* picture = raytracer.addObject( new UnitSquare(), &worldMat );
//	SceneDagNode* globe = raytracer.addObject( new UnitSphere(), &worldMat );
	
	// Apply some transformations to the unit square.
	double factor1[3] = { 1.0, 2.0, 1.0 };
	double factor2[3] = { 6.0, 6.0, 6.0 };
	double factor3[3] = { 2.0, 2.0, 2.0 };

    //raytracer.translate(plane, Vector3D(0, -1, -5));
    //raytracer.rotate(plane, 'x', -90);
    //raytracer.scale(plane, Point3D(0, 0, 0), factor2);

    raytracer.rotate(checker, 'x', -90);
    raytracer.translate(checker, Vector3D(0, 0,-1));

    raytracer.translate(crab, Vector3D(0, 0, -3));
    //raytracer.scale(crab, Point3D(0, 0, 0), factor3);

//    raytracer.translate(picture, Vector3D(1, 0, -3));
//    raytracer.scale(picture, Point3D(0, 0, 0), factor3);

//    raytracer.translate(globe, Vector3D(0, 0, -2));
    //raytracer.rotate(globe, 'z', 180);
    //raytracer.rotate(globe, 'y', 180);
//    raytracer.rotate(globe, 'x', 45);
//    raytracer.scale(globe, Point3D(0, 0, 0), factor3);


    //raytracer.translate(cow, Vector3D(0, -1, -7));
    //raytracer.rotate(cow, 'x', -45);
    //raytracer.rotate(cow, 'z', -45);

    //raytracer.translate(cube, Vector3D(0, -1, -3));
    //raytracer.rotate(cube, 'x', -45);
    //raytracer.rotate(cube, 'z', -45);

    //raytracer.translate(mirrorSquare, Vector3D(0, -1, -8));
    //raytracer.scale(mirrorSquare, Point3D(0, 0, 0), factor2);
    //raytracer.scale(mirrorSquare, Point3D(0, 0, 0), factor2);
    
	//raytracer.translate(sphere, Vector3D(-2, -0.5, -3));	

	//raytracer.translate(sphere2, Vector3D(2, -0.5, -3));	

    //raytracer.translate(mirrorSphere, Vector3D(2.3, 0, -2));
//    raytracer.translate(mirrorSphere, Vector3D(-2.3, 0, -2));
//    raytracer.scale(mirrorSphere, Point3D(0, 0, 0), factor3);

    //raytracer.translate(glassSphere, Vector3D(0, 0, -2));
    //raytracer.scale(glassSphere, Point3D(0, 0, 0), factor3);

    //raytracer.translate(circle, Vector3D(0, 1, -3));

    //raytracer.translate(cylinder, Vector3D(0, 0, -3));
    //raytracer.rotate(cylinder, 'y', -45);
    //raytracer.rotate(cylinder, 'x', -45);

    //raytracer.translate( redSphere, Vector3D(3, -1, -2));
    //raytracer.translate( blueSphere, Vector3D(-3, -1, -2));
    //raytracer.translate( greenSphere, Vector3D(3, -1, -7));
    //raytracer.translate( whiteSphere, Vector3D(-3, -1, -7));

	// Render the scene, feel free to make the image smaller for
	// testing purposes.	
	raytracer.render(width, height, eye, view, up, fov, "view1.bmp");
	
	// Render it from a different point of view.
	Point3D eye2(0, 3, -3);
	Vector3D view2(0, -3, -3);
	//raytracer.render(width, height, eye2, view2, up, fov, "view2.bmp");
	
	return 0;
}

