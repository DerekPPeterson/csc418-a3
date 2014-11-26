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
        if (line[pos2 + 1] == ' ') {
            pos++;
            i--;
            continue;
        }
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
        if (line[pos2 + 1] == ' ') {
            pos++;
            i--;
            continue;
        }
        string num = line.substr(pos + 1, pos2 - pos);
        face.points[i] = vertices[ stoi(num) - 1];
        pos = pos2;
    }

    face.normal = 
        (face.points[0] - face.points[1])
        .cross(face.points[1] - face.points[2]);
    face.normal.normalize();

    return face;
}

TriangleFace* read_obj(const char * filename, int * npFaces)
{
    // Open file
    ifstream obj_file(filename);
    if (!obj_file.is_open()) {
        cout << "error: could not open " << filename << "\n";
    }

    int vbufsize = 1024;
    int fbufsize = 1024;
    Point3D * vertices = new Point3D [vbufsize];
    TriangleFace * faces = new TriangleFace [fbufsize];

    // Parse vertices and faces
    string line;
    int nVertices = 0;
    int nFaces = 0;
    while (getline(obj_file, line)) {
        if (line[0] == 'v') {
            vertices[nVertices] = parse_vertex(line);
            nVertices++;
        } else if (line[0] == 'f') {
            faces[nFaces] = parse_face(line, vertices);
            nFaces++;
        } else {
            continue;
        }

        // expand buffers if needed
        if (nVertices == vbufsize) {
            vbufsize *= 2;
            Point3D * new_vertices = new Point3D [vbufsize];
            copy(vertices, vertices + vbufsize / 2, new_vertices);
            delete[] vertices;
            vertices = new_vertices;
        }
        if (nFaces == fbufsize) {
            fbufsize *= 2;
            TriangleFace * new_faces = new TriangleFace [fbufsize];
            copy(faces, faces + fbufsize / 2, new_faces);
            delete[] faces;
            faces = new_faces;
        }
    }
    delete[] vertices;

    //cout << nFaces << "\n\n";
    //for (int i=0; i < nFaces; i++) {
        //cout << "face " << i << "\n";
        //cout << faces[i].points[0] << "\n";
        //cout << faces[i].points[1] << "\n";
        //cout << faces[i].points[2] << "\n";
        //cout << faces[i].normal << "\n";
    //}

    *npFaces = nFaces;
    return faces;
}
