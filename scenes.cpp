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

    raytracer.setFeatures(10, 1, 0, 1, 2, 2, true);
	raytracer.render(width, height, eye, view, up, fov, "mega_render_view1.bmp");
    
	// Render it from a different point of view.
	Point3D eye2(3, 3, 0);
	Vector3D view2(-3, -3, -2);
	raytracer.render(width, height, eye2, view2, up, fov, "mega_render_view2.bmp");
}

void part1(int width, int height)
{
	// Build your scene and setup your camera here, by calling 
	// functions from Raytracer.  The code here sets up an example
	// scene and renders it from two different view points, DO NOT
	// change this if you're just implementing part one of the 
	// assignment.  
	Raytracer raytracer;

	// Camera parameters.
	Point3D eye(0, 0, 1);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);
	double fov = 60;

	// Defines a material for shading.
	Material gold( Colour(0.3, 0.3, 0.3), Colour(0.75164, 0.60648, 0.22648), 
			Colour(0.628281, 0.555802, 0.366065), 
			51.2 );
	Material jade( Colour(0, 0, 0), Colour(0.54, 0.89, 0.63), 
			Colour(0.316228, 0.316228, 0.316228), 
			12.8 );

	// Defines a point light source.
	raytracer.addLightSource( new PointLight(Point3D(0, 0, 5), 
				Colour(0.9, 0.9, 0.9) ) );

	// Add a unit square into the scene with material mat.
	SceneDagNode* sphere = raytracer.addObject( new UnitSphere(), &gold );
	SceneDagNode* plane = raytracer.addObject( new UnitSquare(), &jade );
	
	// Apply some transformations to the unit square.
	double factor1[3] = { 1.0, 2.0, 1.0 };
	double factor2[3] = { 6.0, 6.0, 6.0 };
	raytracer.translate(sphere, Vector3D(0, 0, -5));	
	raytracer.rotate(sphere, 'x', -45); 
	raytracer.rotate(sphere, 'z', 45); 
	raytracer.scale(sphere, Point3D(0, 0, 0), factor1);

	raytracer.translate(plane, Vector3D(0, 0, -7));	
	raytracer.rotate(plane, 'z', 45); 
	raytracer.scale(plane, Point3D(0, 0, 0), factor2);

	// Render the scene, feel free to make the image smaller for
	// testing purposes.	
	Point3D eye2(4, 2, 1);
	Vector3D view2(-4, -2, -6);

    // signature
    raytracer.setFeatures(1, 1, 0, 1, 0, 0, false, true, false);
	raytracer.render(width, height, eye, view, up, fov, "part1_signature_view1.bmp");
	raytracer.render(width, height, eye2, view2, up, fov, "part1_signature_view2.bmp");
    
    // ambient diffues
    raytracer.setFeatures(1, 1, 0, 1, 0, 0, false, false, false);
	raytracer.render(width, height, eye, view, up, fov, "part1_ambient_diffuse_view1.bmp");
	raytracer.render(width, height, eye2, view2, up, fov, "part1_ambient_diffuse_view2.bmp");
    
    // phong
    raytracer.setFeatures(1, 1, 0, 1, 0, 0, false);
	raytracer.render(width, height, eye, view, up, fov, "part1_phong_view1.bmp");
	raytracer.render(width, height, eye2, view2, up, fov, "part1_phong_view2.bmp");
    
    // AA
    raytracer.setFeatures(10, 1, 0, 1, 0, 0, false);
	raytracer.render(width, height, eye, view, up, fov, "part1_AA_view1.bmp");
	raytracer.render(width, height, eye2, view2, up, fov, "part1_AA_view2.bmp");
}
