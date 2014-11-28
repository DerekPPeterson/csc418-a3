#include "scenes.h"
#include "util.h"
#include "raytracer.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

// Material definitions
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
        true, Colour(0.05, 0.05, 0.05), 
        true, Colour(0.9, 0.9, 0.9), 1.3);

Material red( Colour(0.3, 0, 0), Colour(0.7, 0, 0), Colour(0.2, 0, 0), 10);
Material blue( Colour(0., 0, 0.3), Colour(0, 0, 0.7), Colour(0, 0, 0.2), 10);
Material green( Colour(0, 0.3, 0), Colour(0, 0.7, 0), Colour(0, 0.2, 0), 10);
Material white( Colour(0.3, 0.3, 0.3), Colour(0.7, 0.7, 0.7), Colour(0.2, 0.2, 0.2), 10);

Texture earthTex = Texture("./textures/earth-1024x512.bmp");
Material worldMat( Colour(0.6, 0.6, 0.6), Colour(0.7, 0.7, 0.7), Colour(0.2, 0.2, 0.2),
        10, &earthTex);

void mega_render(int width, int height)
{
	Raytracer raytracer;

	// Defines a point light source.
	raytracer.addLightSource( new PointLight(Point3D(0, 6, 0), 
				Colour(0.9, 0.9, 0.9) ) );
    
    // Add objects
	SceneDagNode* checker = raytracer.addObject( new Checkerboard(), &jade );
	SceneDagNode* mirrorSphere = raytracer.addObject( new UnitSphere(), &mirror );
	SceneDagNode* glassSphere = raytracer.addObject( new UnitSphere(), &glass );
	SceneDagNode* cylinder = raytracer.addObject( new UnitCylinder(), &gold );

	SceneDagNode* redSphere = raytracer.addObject( new UnitSphere(), &red );
	SceneDagNode* blueSphere = raytracer.addObject( new UnitSphere(), &blue );
	SceneDagNode* greenSphere = raytracer.addObject( new UnitSphere(), &green );
	SceneDagNode* whiteSphere = raytracer.addObject( new UnitSphere(), &white );

	SceneDagNode* cube = raytracer.addObject( new Mesh("obj/cube.obj"), &gold );
	SceneDagNode* globe = raytracer.addObject( new UnitSphere(), &worldMat );
    
	// Apply some transformations to the unit square.
	double factor1[3] = { 1.0, 2.0, 1.0 };
	double factor2[3] = { 6.0, 6.0, 6.0 };
	double factor3[3] = { 2.0, 2.0, 2.0 };

    raytracer.rotate(checker, 'x', -90);
    raytracer.translate(checker, Vector3D(0, 0,-1));

    raytracer.translate(globe, Vector3D(0, 1.5, -3));
    raytracer.rotate(globe, 'y', 180);
    raytracer.scale(globe, Point3D(0, 0, 0), factor3);

    raytracer.translate(cube, Vector3D(-2.5, 1.5, -3));
    raytracer.rotate(cube, 'x', -45);
    raytracer.rotate(cube, 'z', -45);

    raytracer.translate(mirrorSphere, Vector3D(-3, 0, -3));
    raytracer.scale(mirrorSphere, Point3D(0, 0, 0), factor3);

    raytracer.translate(glassSphere, Vector3D(3, 0, -3));
    raytracer.scale(glassSphere, Point3D(0, 0, 0), factor3);

    raytracer.translate(cylinder, Vector3D(2, 1.5, -3));
    raytracer.rotate(cylinder, 'y', -45);
    raytracer.rotate(cylinder, 'x', -45);

    raytracer.translate( redSphere, Vector3D(-1, 0, -3));
    raytracer.translate( blueSphere, Vector3D(0, 0, -6));
    raytracer.translate( greenSphere, Vector3D(1, 0, -9));
    raytracer.translate( whiteSphere, Vector3D(2, 0, -12));
    
	// Camera parameters.
	Point3D eye(0, 0, 1);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);
	double fov = 70;

    raytracer.setFeautures(10, 1, 0, 1, 2, 2);
	raytracer.render(width, height, eye, view, up, fov, "view1.bmp");
    
	// Render it from a different point of view.
	Point3D eye2(3, 3, 0);
	Vector3D view2(-3, -3, -2);
	raytracer.render(width, height, eye2, view2, up, fov, "view2.bmp");
}
