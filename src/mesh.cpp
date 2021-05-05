
#include "mesh.h"

#include <fstream>

#include <iostream>

void Mesh::readPly(string filename)
{
    const string ELEMENT_VERTEX("element vertex");
    const string ELEMENT_FACE("element face");
    const string FORMAT("format");

    vertices_.clear();
    normals_.clear();
    faces_.clear();

    unsigned int num_vertices;
    unsigned int num_faces;

    ifstream stream(filename);
    string line;

    getline(stream, line);
    if (line != "ply") {
        throw 1;
    }

    while (line != "end_header") {
        if (line.compare(0, ELEMENT_VERTEX.length(), ELEMENT_VERTEX) == 0) {
            num_vertices = stoi(line.substr(ELEMENT_VERTEX.length()));
        } else if (line.compare(0, ELEMENT_FACE.length(), ELEMENT_FACE) == 0) {
            num_faces = stoi(line.substr(ELEMENT_FACE.length()));
        } else if (line.compare(0, FORMAT.length(), FORMAT) == 0) {
            if (line.substr(FORMAT.length() + 1) != "ascii 1.0") {
                throw 1;
            }
        }

        if (!getline(stream, line)) {
            throw 1;
        }
    }

    vertices_.resize(num_vertices);
    normals_.resize(num_vertices);
    for (unsigned int i = 0; i < num_vertices; ++i) {
        if (!(stream >> vertices_[i].x >> vertices_[i].y >> vertices_[i].z)) {
            throw 1;
        }
        if (!(stream >> normals_[i].x >> normals_[i].y >> normals_[i].z)) {
            throw 1;
        }
    }

    faces_.resize(num_faces);
    for (unsigned int i = 0; i < num_faces; ++i) {
        int vertexs;
        if (!(stream >> vertexs)) {
            throw 1;
        }
        if (vertexs != 3) {
            throw 1;
        }

        if (!(stream >> faces_[i].a >> faces_[i].b >> faces_[i].c)) {
            throw 1;
        }
    }
}
