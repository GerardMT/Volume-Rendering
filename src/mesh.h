
#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <vector>
#include <string>

using namespace std;

class Mesh {
public:
    void readPly(string file);

    struct face {
        unsigned int a;
        unsigned int b;
        unsigned int c;
    };

public:
    vector<glm::vec3> vertices_;
    vector<glm::vec3> normals_;

    vector<face> faces_;

    glm::vec3 min_;
    glm::vec3 max_;
};

#endif // MESH_H
