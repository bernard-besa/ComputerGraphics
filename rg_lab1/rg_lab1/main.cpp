#include <cstdio>
#include <glm.hpp>
#include <gtc/matrix_access.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <cmath>
#include <GL/glut.h>
using namespace std;

//*********************************************************************************
//  Razredi potrebni za iscrtavanje tijela.
//  Vertex - predstavlja vrh/toèku u prostoru s koordinatama x, y, z
//  Polygon - predstavlja poligon u prostoru s vrhovima v1, v2, v3
//  Trajectory - predstavlja putanju po kojoj se tijelo animira
//*********************************************************************************


std::vector<glm::vec3> BSplineVertices;

glm::mat4 B = glm::mat4(
    glm::vec4(-1, 3, -3, 1),
    glm::vec4(3, -6, 0, 4),
    glm::vec4(-3, 3, 3, 1),
    glm::vec4(1, 0, 0, 0));
glm::mat4x3 dB = glm::mat4x3(
    glm::vec3(-1, 2, -1),
    glm::vec3(3, -4, 0),
    glm::vec3(-3, 2, 1),
    glm::vec3(1, 0, 0));
glm::mat4x2 ddB = glm::mat4x2(
    glm::vec2(-1, 1),
    glm::vec2(3, -2),
    glm::vec2(-3, 1),
    glm::vec2(1, 0));

glm::vec3 calculateP(double t, int i) {
    glm::vec4 T = glm::vec4(pow(t, 3), pow(t, 2), t, 1);
    glm::mat3x4 R = glm::mat3x4(
        glm::vec4(BSplineVertices[i - 1].x, BSplineVertices[i].x, BSplineVertices[i + 1].x, BSplineVertices[i + 2].x),
        glm::vec4(BSplineVertices[i - 1].y, BSplineVertices[i].y, BSplineVertices[i + 1].y, BSplineVertices[i + 2].y),
        glm::vec4(BSplineVertices[i - 1].z, BSplineVertices[i].z, BSplineVertices[i + 1].z, BSplineVertices[i + 2].z));
    glm::vec3 p = T * 0.1666666f * B * R;
    return p;
}

glm::vec3 calculateDiffP(double t, int i) {
    glm::vec3 T = glm::vec3(pow(t, 2), t, 1);
    glm::mat3x4 R = glm::mat3x4(
        glm::vec4(BSplineVertices[i - 1].x, BSplineVertices[i].x, BSplineVertices[i + 1].x, BSplineVertices[i + 2].x),
        glm::vec4(BSplineVertices[i - 1].y, BSplineVertices[i].y, BSplineVertices[i + 1].y, BSplineVertices[i + 2].y),
        glm::vec4(BSplineVertices[i - 1].z, BSplineVertices[i].z, BSplineVertices[i + 1].z, BSplineVertices[i + 2].z));
    glm::vec3 diffP = T * 0.5f * dB * R;
    return diffP;
}

glm::vec3 calculateDoubleDiffP(double t, int i) {
    glm::vec2 T = glm::vec2(t, 1);
    glm::mat3x4 R = glm::mat3x4(
        glm::vec4(BSplineVertices[i - 1].x, BSplineVertices[i].x, BSplineVertices[i + 1].x, BSplineVertices[i + 2].x),
        glm::vec4(BSplineVertices[i - 1].y, BSplineVertices[i].y, BSplineVertices[i + 1].y, BSplineVertices[i + 2].y),
        glm::vec4(BSplineVertices[i - 1].z, BSplineVertices[i].z, BSplineVertices[i + 1].z, BSplineVertices[i + 2].z));
    glm::vec3 doubleDiffP = T * ddB * R;
    return doubleDiffP;
}

vector<glm::vec3> vertices;
vector<vector<glm::vec3>> polygons;
glm::vec3 center;
double t = 0.0, clock = 0.0, previousClock = 0.0;
int speed = 1, i = 1;
bool is_stopped = false, usingDCM = true;

GLuint window;
GLuint width = 700, height = 500;

void myDisplay();
void myIdle();
void myKeyboard(unsigned char theKey, int mouseX, int mouseY);
void loadObject(string filePath);
void loadBSpline(string filePath);
void drawObject(glm::mat3 R);
void drawBSpline();

int main(int argc, char** argv) {
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutInit(&argc, argv);
    window = glutCreateWindow("RG lab1");
    //ako se koristi DCM
    if (usingDCM)
        gluLookAt(0, 0.5, 1, 0, 20, 50, 0, 5, 5);
    else
    {
        glMatrixMode(GL_PROJECTION);
    }
    loadObject("../objFiles/skull.obj");
    loadBSpline("../BSpline2.txt");

    glutDisplayFunc(myDisplay);
    glutKeyboardFunc(myKeyboard);
    glutIdleFunc(myIdle);
    glutMainLoop();
    return 0;
}

void myIdle() {
    if (is_stopped)
    {
        return;
    }
    clock += 0.01f;
    double timePassed = clock - previousClock;
    if (timePassed > speed)
    {
        myDisplay();
        clock = 0;
    }
}

void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1);

    //racunanje tangente
    glColor3f(0.0, 0.0, 1.0);
    glm::vec3 tangent = calculateDiffP(t, i);
    tangent = glm::normalize(tangent);
    glm::vec3 tangentStart = calculateP(t, i);
    glm::vec3 tangentEnd = tangentStart + 5.f * tangent;
    glBegin(GL_LINES);
    glVertex3f(tangentStart.x / 25.f, tangentStart.y / 25.f, tangentStart.z / 25.f);
    glVertex3f(tangentEnd.x / 25.f, tangentEnd.y / 25.f, tangentEnd.z / 25.f);
    glEnd();

    //racunanje normale
    glColor3f(0.0, 1.0, 0.0);
    glm::vec3 ddp = calculateDoubleDiffP(t, i);
    glm::vec3 normal = glm::normalize(glm::cross(tangent, ddp));
    glm::vec3 normalStart = calculateP(t, i);
    glm::vec3 normalEnd = normalStart + 5.f * normal;
    glBegin(GL_LINES);
    glVertex3f(normalStart.x / 25.f, normalStart.y / 25.f, normalStart.z / 25.f);
    glVertex3f(normalEnd.x / 25.f, normalEnd.y / 25.f, normalEnd.z / 25.f);
    glEnd();

    //racunanje binormale
    glColor3f(1.0, 0.0, 1.0);
    glm::vec3 binormal = glm::normalize(glm::cross(tangent, normal));
    glm::vec3 binormalStart = calculateP(t, i);
    glm::vec3 binormalEnd = binormalStart + 5.f * binormal;
    glBegin(GL_LINES);
    glVertex3f(binormalStart.x / 25.f, binormalStart.y / 25.f, binormalStart.z / 25.f);
    glVertex3f(binormalEnd.x / 25.f, binormalEnd.y / 25.f, binormalEnd.z / 25.f);
    glEnd();

    //rotacijska matrica (DCM)
    glm::mat3 rotationMatrix = glm::mat3(tangent, normal, binormal);

    drawBSpline();
    drawObject(rotationMatrix);

    t += 0.1;
    if (t >= 1.)
    {
        i++;
        t = 0.1;
    }

    if (i >= BSplineVertices.size() - 2)
    {
        i = 1;
        t = 0.;
    }
    glFlush();
}

void myKeyboard(unsigned char theKey, int mouseX, int mouseY) {
    switch (theKey)
    {
    case 27:  exit(0);
        break;
    }
}

void loadBSpline(string filename) {
    ifstream file(filename);
    string line;
    while (getline(file, line))
    {
        double x, y, z;
        istringstream iss(line);
        iss >> x >> y >> z;
        glm::vec3 vertex(x, y, z);
        BSplineVertices.push_back(vertex);
    }
}

void loadObject(string fileName) {
    ifstream file(fileName);
    string line;
    center = glm::vec3(0, 0, 0);
    while (getline(file, line))
    {
        //vrh
        if (line.compare(0, 1, "v") == 0)
        {
            double x, y, z;
            char c;
            istringstream iss(line);
            iss >> c >> x >> y >> z;
            glm::vec3 vertex(x, y, z);
            vertices.push_back(vertex);
        }
        //poligon
        else if (line.compare(0, 1, "f") == 0)
        {
            int indexV1, indexV2, indexV3;
            char c;
            istringstream iss(line);
            iss >> c >> indexV1 >> indexV2 >> indexV3;
            glm::vec3 vertex1 = vertices[indexV1 - 1];
            glm::vec3 vertex2 = vertices[indexV2 - 1];
            glm::vec3 vertex3 = vertices[indexV3 - 1];

            center += vertex1 + vertex2 + vertex3;

            glm::mat4 rotation = glm::mat4(
                glm::vec4(cos(-80), 0, sin(-80), 0),
                glm::vec4(0, 1, 0, 0),
                glm::vec4(-sin(-80), 0, cos(-80), 0),
                glm::vec4(0, 0, 0, 1));
            glm::vec4 p1 = glm::vec4(vertex1.x, vertex1.y, vertex1.z, 1) * rotation;
            glm::vec4 p2 = glm::vec4(vertex2.x, vertex2.y, vertex2.z, 1) * rotation;
            glm::vec4 p3 = glm::vec4(vertex3.x, vertex3.y, vertex3.z, 1) * rotation;

            vertex1 = glm::vec3(p1.x, p1.y, p1.z);
            vertex2 = glm::vec3(p2.x, p2.y, p2.z);
            vertex3 = glm::vec3(p3.x, p3.y, p3.z);
            std::vector<glm::vec3> poly;
            poly.push_back(vertex1);
            poly.push_back(vertex2);
            poly.push_back(vertex3);
            polygons.push_back(poly);
        }
        else
        {
            continue;
        }
    }
    center /= polygons.size();
}

void drawBSpline() {
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POINTS);
    for (auto& point : BSplineVertices) {
        glVertex3f(point.x / 25.f, point.y / 25.f, point.z / 25.f);
    }
    glEnd();
}

void drawObject(glm::mat3 R) {
    if (!usingDCM)
    {
        glm::vec3 tangent = glm::normalize(glm::column(R, 0));
        glm::vec3 start = glm::normalize(glm::vec3(0, 0.89, 0));
        glm::vec3 axis = glm::normalize(glm::cross(start, tangent));
        double fi = acos(glm::dot(start, tangent));
        glRotatef((fi * 180) / (atan(1) * 4), axis.x, axis.y, axis.z);
    }

    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_TRIANGLES);
    for (auto& p : polygons)
    {
        glm::vec3 position = calculateP(t, i);
        glm::vec3 position_vec = glm::vec3(position.x, position.y, position.z);

        glm::vec3 v1 = p[0];
        glm::vec3 v1_vec = glm::vec3(v1.x, v1.y, v1.z);
        glm::vec3 v1_final = (v1_vec - center) / 5.f + position_vec / 25.f;
        if (usingDCM)
            v1_final = ((v1_vec - center) * glm::inverse(R)) / 5.f + position_vec / 25.f;

        glm::vec3 v2 = p[1];
        glm::vec3 v2_vec = glm::vec3(v2.x, v2.y, v2.z);
        glm::vec3 v2_final = (v2_vec - center) / 5.f + position_vec / 25.f;
        if (usingDCM)
            v2_final = ((v2_vec - center) * glm::inverse(R)) / 5.f + position_vec / 25.f;

        glm::vec3 v3 = p[2];
        glm::vec3 v3_vec = glm::vec3(v3.x, v3.y, v3.z);
        glm::vec3 v3_final = (v3_vec - center) / 5.f + position_vec / 25.f;
        if (usingDCM)
            v3_final = ((v3_vec - center) * glm::inverse(R)) / 5.f + position_vec / 25.f;

        glVertex3f(v1_final.x, v1_final.y, v1_final.z);
        glVertex3f(v2_final.x, v2_final.y, v2_final.z);
        glVertex3f(v3_final.x, v3_final.y, v3_final.z);
    }
    glEnd();
}