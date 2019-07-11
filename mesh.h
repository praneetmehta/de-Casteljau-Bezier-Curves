#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

using namespace std;
/* Triangle Class to create triangulated faces from the surface coordinates*/
class Triangle
{
public:
    glm::vec3 A;
    glm::vec3 B;
    glm::vec3 C;

    Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
    {
        A = v1;
        B = v2;
        C = v3;
    }
};

class Mesh
{
private:
    vector<GLfloat> vertices;
    vector<Triangle> triangles;
    vector<glm::vec3> normals;


public:
    Mesh();
    // populate the vertexes of the triangles
    void Vertex(glm::vec3 vertex);
    // popuate the triangulated faces
    void Triangulate(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
    // populate quad faces and then triangulate them
    void addQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);
    // utility functions to get vertices triangles and normals
    vector<GLfloat> getVertices();
    vector<Triangle> getTriangles();
    vector<glm::vec3> getNormals();
    void transform(glm::mat4 transformation);
    void constructMesh(Mesh mesh);
    void writeToOFF(string filename);
};



#endif
