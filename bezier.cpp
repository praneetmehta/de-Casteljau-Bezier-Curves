#include <windows.h>
#include <bits/stdc++.h>
#include <GLFW/glfw3.h>
#include <unistd.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "mesh.h"

using namespace std;

//constants
const double SW = 1366;
const double SH = 768;

//globalse
string filename;
char MODE = 'a';                    //default mode
vector<glm::vec2> controlPoints;    //control point store
vector<glm::vec3> curvePoints;      //curve points store
glm::vec2 coordinate_mouse;         //current mouse coordinates
int identifiedPoint = -1;           //Point to move when in the drag mode
void generateSurface();             //function to generate bezier surface


/* Identify the point that is supposed to be selected for operations
like deletion or drag. This is done by calculating the distance from the
mouse coordinates to each point and identifying the point closest to the click
event */
int identifyPoint(glm::vec2 P)
{
    int result = -1;
    float min_dist = SH + SW;
    for (int i = 0; i < controlPoints.size(); i++)
    {
        glm::vec2 P2 = controlPoints[i];
        float dist = glm::dot(P - P2, P - P2);
        if (dist < min_dist && dist < 100)
        {
            min_dist = dist;
            result = i;
        }
    }
    return result;
}

/* Handle mouse events for interaction with the control points
  LEFT CLICK = add control point  (if mode == 'a')
               move control point (if mode == 'm')

  RIGHT CLICK = delete control point nearest to the click
*/
void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    double x;
    double y;
    glfwGetCursorPos(window, &x, &y);
    y = SH - y;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        coordinate_mouse = glm::vec2(x, y);                 // current mouse coordinates
        if(MODE=='a')                                       // if mode 'a' then add control points on click
        {
          controlPoints.push_back(coordinate_mouse);        // populate the control points vector with the added control point
        }
        else
        {
          identifiedPoint = identifyPoint(glm::vec2(x, y)); // identify which point to move by finding distance of all points from the mouse click position
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
      coordinate_mouse = glm::vec2(x, y);                  // current mouse position when the click is released
      if(MODE == 'm')
      {
        if(identifiedPoint==-1)
        {
          return;
        }
        else
        {
          // update the selected point
          controlPoints.erase(controlPoints.begin() + identifiedPoint);
          controlPoints.insert(controlPoints.begin() + identifiedPoint, coordinate_mouse);
        }
      }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
      // delete event for control points mapped to right mouse click
      coordinate_mouse = glm::vec2(x, y);
      int i = identifyPoint(glm::vec2(x, y));
      controlPoints.erase(controlPoints.begin() + i);
    }

}
/* Process keyboard events for the glfw window instance
  if pressed
    A         - Toggle Mode 'a'
    M         - Toggle Mode 'm'
    D         - Toggle Mode 'd'
    SpaceBar  - Generate Bezier Surface
*/
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        generateSurface();
    }

    if(glfwGetKey(window, 'A'))
    {
      MODE = 'a';
    }
    else if(glfwGetKey(window, 'M'))
    {
      MODE = 'm';
      identifiedPoint = -1;
    }
}

/* Plot Individual Pixels on the window*/
void drawPixel(GLfloat x, GLfloat y, int pointSize, bool line=false)
{
    GLfloat pointVertex[] = {x, y};
    GLfloat rgb_vector[] = {1, 0, 0};
    if(line)
    {
      rgb_vector[1] = 1, rgb_vector[2] = 1;
    }
    glColorPointer(3, GL_FLOAT, 0, rgb_vector);

    glEnable(GL_POINT_SMOOTH);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glPointSize(pointSize);
    glVertexPointer(2, GL_FLOAT, 0, pointVertex);
    glDrawArrays(GL_POINTS, 0, 1);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisable(GL_POINT_SMOOTH);
}
//De Castlejau Linear Interpolation Function
glm::vec2 Lerp(glm::vec2 A, glm::vec2 B, float t)
{
    return (1-t)*A + t*B;
}

/* De Castlejau Algorithm Recursive Implementaion
  CurvePoints are generated recursively using linear interpolation
  and the curve point vector is populated
*/
glm::vec2 getCurvePoint(double t, vector<glm::vec2> controlPoints)
{
  if (controlPoints.size() == 1) return controlPoints[0];
  vector<glm::vec2> tmp;
  for (int i = 1; i < controlPoints.size(); i++) {
    tmp.push_back(Lerp(controlPoints[i - 1], controlPoints[i], t));
  }
  return getCurvePoint(t, tmp);
}

/* Draw the Beizer Curve and the axis of rotation*/
void drawBezierCurve(vector<glm::vec2> controlPoints)
{
    // Axis of rotation
    glLineWidth(1);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(SW/2, SH,0.0);
    glVertex3f(SW/2, 0, 0);
    glEnd();

    //recursion deapth of de castlejau algorith for linear interpolation
    int degree = controlPoints.size() - 1;

    if (degree < 0)
        return;

    // redraw all the control points
    for (int i = 0; i <= degree; i++)
    {
        drawPixel(controlPoints[i].x, controlPoints[i].y, 10);
    }
    curvePoints.clear();

    // with t = 0.001 draw caculate the curvePoints and draw on the scene
    for(float t = 0; t < 1; t += 0.001)
    {
        glm::vec2 CP = getCurvePoint(t, controlPoints);
        curvePoints.push_back(glm::vec3(CP.x, CP.y, 1));
        drawPixel(CP.x, CP.y, 2, true);
    }
}


/* Generate the Beizer Surface by rotating sampled points about the axis of rotation*/
void generateSurface()
{
    int degree = controlPoints.size() - 1;
    if (degree > 0)
    {
        int samples = 100 ;                           // samples per rotation or the number of points to considered per rotation
        float step = 2*M_PI/(float)samples;           // theta increment steps

        vector<vector <glm::vec3> > p;
        float angle;

        // generate points on the surface of the curve by rotaing curvePoints
        for (int i = 0; i < curvePoints.size(); i += (1.0 * curvePoints.size() / samples)) {
          vector<glm::vec3> v;
          for (double theta = M_PI/2.0; theta > -M_PI / 2.0; theta -= (M_PI/samples)){
            glm::vec3 point = curvePoints[i];
            v.push_back(glm::vec3((point.x-SW/2) * cos(theta), point.y, (point.x-SW/2) * sin(theta)));
          }
          for (double theta = -M_PI/2.0; theta < M_PI / 2.0; theta += (M_PI/samples)){
            glm::vec3 point = curvePoints[i];
            v.push_back(glm::vec3(-1 * (point.x-SW/2) * cos(theta), point.y, (point.x-SW/2) * sin(theta)));
          }
          p.push_back(v);
        }

        // Mesh Instance
        Mesh surface = Mesh();
        int i, j;
        // Form Quads by connecting the vertices created by rotation of
        // curve points
        for (i = 1; i < p.size(); i++) {
          for (int j = 1; j < p[i].size(); j++) {
            surface.addQuad(p[i-1][j-1], p[i-1][j], p[i][j], p[i][j-1]);
          }surface.addQuad(p[i-1][p[i].size()-1], p[i-1][0], p[i][0], p[i][p[i].size()-1]);
        }

        int len = (surface.getVertices()).size();

        // write the bezier surface to .OFF file
        surface.writeToOFF(filename);
    }
}



void demoPrimitiveDrawing()
{
    drawBezierCurve(controlPoints);
}

int main(int argv, char* argc[])
{
    filename = argc[1];
    GLFWwindow *window;
    // Initialize the library
    if (!glfwInit()) {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(SW, SH, "IS F311 Computer Graphics Assignment", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context
    glfwMakeContextCurrent(window);

    //Mouse callback function
    glfwSetMouseButtonCallback(window, mouse_callback);

    glViewport(0.0f, 0.0f, SW, SH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SW, 0, SH, 0, 1); // essentially set coordinate system
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);
        {
            demoPrimitiveDrawing();
            glfwSwapBuffers(window);
        }
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
