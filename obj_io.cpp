#include "obj_io.h"
#include <iostream>
#include <fstream>
#include <string>
#include "util.h"

using namespace std;

Point3D parse_vertex(string line)
{
    int pos;
    pos = line.find(" ");

    Point3D vertex;

    for (int i = 0; i < 3; i++) {
        int pos2 = line.find(" ", pos + 1);
        string num = line.substr(pos + 1, pos2 - pos);
        vertex[i] = stod(num);
        pos = pos2;
    }

    return vertex;
}

TriangleFace parse_face(string line, Point3D * vertices)
{
    int pos;
    pos = line.find(" ");

    TriangleFace face;

    for (int i = 0; i < 3; i++) {
        int pos2 = line.find(" ", pos + 1);
        string num = line.substr(pos + 1, pos2 - pos);
        face.points[i] = vertices[ stoi(num) ];
        pos = pos2;
    }

    face.normal = 
        (face.points[0] - face.points[1])
        .cross(face.points[1] - face.points[2]);
    face.normal.normalize();

    return face;
}

int read_obj(const char * filename, TriangleFace *faces)
{
    // Open file
    ifstream obj_file(filename);
    if (!obj_file.is_open()) {
        cout << "error: could not open " << filename << "\n";
    }

    int bufsize = 1024;
    Point3D * vertices = new Point3D [bufsize];
    faces = new TriangleFace [bufsize];

    // Parse vertices and faces
    string line;
    int nVertices = 0;
    int nFaces = 0;
    while (getline(obj_file, line)) {
        switch (line[0]) {
            case 'v':
                vertices[nVertices] = parse_vertex(line);
                nVertices++;
            case 'f':
                faces[nFaces] = parse_face(line, vertices);
                nFaces++;
        }

        // expand buffers if needed
        if (nVertices > bufsize) {
            bufsize *= 2;
            Point3D * new_vertices = new Point3D [bufsize];
            copy(vertices, vertices + bufsize / 2, new_vertices);
            delete(vertices);
            vertices = new_vertices;
        }
        if (nFaces > bufsize) {
            bufsize *= 2;
            TriangleFace * new_faces = new TriangleFace [bufsize];
            copy(faces, faces + bufsize / 2, new_faces);
            delete(faces);
            faces = new_faces;
        }
    }
    delete vertices;
    return nFaces;
}
