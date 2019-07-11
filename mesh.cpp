#include <vector>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "mesh.h"
#include <stdio.h>

Mesh::Mesh(){}

void Mesh::Vertex(glm::vec3 vertex)
{
    /* Add vertices of triangles to the vertices vector */
    vertices.push_back(vertex.x);
    vertices.push_back(vertex.y);
    vertices.push_back(vertex.z);
}


void Mesh::Triangulate(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
    /* Create triangular faces objects from the Triangle Class and populate
    the face normals
    */
    triangles.push_back(Triangle(v1, v2, v3));
    Vertex(v1);
    Vertex(v2);
    Vertex(v3);

    normals.push_back(glm::cross(v1-v2, v2-v3));
}

void Mesh::addQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4)
{
    /* Recieve Quad faces and triangulate them and push the triangles into
    the triangles vector to be lates written to the off file
    */
    triangles.push_back(Triangle(v1, v2, v3));
    triangles.push_back(Triangle(v1, v3, v4));
    Triangulate(v1, v2, v3);
    Triangulate(v1, v3, v4);
}

vector<GLfloat> Mesh::getVertices()
{
  //return vertices
    return vertices;
}

vector<Triangle> Mesh::getTriangles()
{
  //return triangles
    return triangles;
}

vector<glm::vec3> Mesh::getNormals()
{
  //return normals
    return normals;
}

void Mesh::transform(glm::mat4 transformation)
{
  // Mesh Transformation Function
    vector<Triangle> triangles = this->triangles;
    Mesh mesh = Mesh();
    for (int i=0; i < triangles.size(); i++)
    {
        Triangle T = triangles[i];
        glm::vec4 A1 = transformation * glm::vec4(T.A, 1);
        glm::vec4 B1 = transformation * glm::vec4(T.B, 1);
        glm::vec4 C1 = transformation * glm::vec4(T.C, 1);
        mesh.Triangulate(glm::vec3(A1), glm::vec3(B1), glm::vec3(C1));
    }
    this->vertices = mesh.getVertices();
    this->triangles = mesh.getTriangles();
    this->normals = mesh.getNormals();
}

void Mesh::constructMesh(Mesh mesh)
{
  // Properly reorder and link mesh faces to construct the final mesh
  // with the triangulated faces and normals
    vector<GLfloat> vertices2 = mesh.getVertices();
    vector<Triangle> triangles2 = mesh.getTriangles();

    vector<GLfloat> updated_vertices;
    updated_vertices.reserve(vertices.size() + vertices2.size());
    updated_vertices.insert(updated_vertices.end(), vertices.begin(), vertices.end());
    updated_vertices.insert(updated_vertices.end(), vertices2.begin(), vertices2.end());

    vector<Triangle> updated_triangles;
    updated_triangles.reserve(triangles.size() + triangles2.size());
    updated_triangles.insert(updated_triangles.end(), triangles.begin(), triangles.end());
    updated_triangles.insert(updated_triangles.end(), triangles2.begin(), triangles2.end());

    vertices = updated_vertices;
    triangles = updated_triangles;
}

void Mesh::writeToOFF(string filename)
{
  // write the .OFF file with the triangulated faces and their normals and the vertices
    ofstream OFFfile(filename + ".off");
    cout << "Writing Mesh to file." << endl;
    uint num_triangles = triangles.size();
    OFFfile << "OFF\n";
    OFFfile << num_triangles*3 << " " << num_triangles << " " << 0 << endl;
    for (uint i = 0; i < num_triangles; i++)
    {
        Triangle T = triangles[i];
        OFFfile << T.A.x << " " << T.A.y << " " << T.A.z << endl;
        OFFfile << " " << T.B.x << " " << T.B.y << " " << T.B.z << endl;
        OFFfile << " " << T.C.x << " " << T.C.y << " " << T.C.z << endl;
    }
    int vertexCounter = 0;
    for (uint i = 0; i < num_triangles; i++)
    {
        OFFfile << 3 << " " << vertexCounter + 0 << " " << vertexCounter + 1 << " " << vertexCounter + 2 << endl;
        vertexCounter += 3;
    }
    OFFfile.close();
}
