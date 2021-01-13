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
#include <algorithm>
#include <ctime>
using namespace std;


class Material
{
public:
    Material(string _name) : name(_name) {}

    string name;
    glm::vec3 Kd;
    glm::vec3 Ks;
    glm::vec3 Ka;
    double d;
};

class MyObject
{
public:
    MyObject(string _name) : name(_name) {}

    string name;
    vector<vector<glm::vec3>> polygons;
    Material* material;
    glm::vec3 color;
};


class Particle
{
public:
    Particle(glm::vec3 _position, glm::vec3 _color, double _size) : position(_position), color(_color), size(_size)
    {
        isGrey = false;
        age = 1.0;
        angle = 0;
        axis = glm::vec3(0.0, 0.0, 0.0);
    }

    void draw()
    {
        glColor3f(color.x, color.y, color.z);
        glPushMatrix();
        glTranslatef(position.x, position.y, position.z);
        glRotatef(angle, axis.x, axis.y, axis.z);
        glutSolidSphere(size, 5, 5);
        glPopMatrix();
    }

    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 axis;
    double angle;
    double size;
    double age;
    bool isGrey;
};

vector<glm::vec2> textures;
vector<glm::vec3> vertices;
vector<glm::vec3> normals;
vector<vector<glm::vec3>> polygons;
vector<MyObject> objects;
vector<Material *> materials;
vector<Particle> particles;
glm::vec3 ociste = glm::vec3(0.0, 4.7, 20.0);
glm::vec3 lightSource = glm::vec3(-4.6, 15.2, 12.0);
glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);
glm::vec3 ambientLightColor = glm::vec3(0.5, 0.5, 0.5);
glm::vec3 particleStartingPosition = glm::vec3(0.0, -7.0, -2.0);
double clockNow = 0.0, previousClock = 0.0;

GLuint window;
GLuint width = 700, height = 500;

void myDisplay();
void myIdle();
void myReshape(int width, int height);
void updatePerspective();
void myKeyboard(unsigned char theKey, int mouseX, int mouseY);
void loadScene(string filePath);
void loadMaterial(string filePath);
void drawScene();
void drawParticles();
void particleControl();
double clamp(double x, double lower, double upper);
glm::vec3 calculateLighting(glm::vec3 kd, glm::vec3 ks, glm::vec3 centroid, glm::vec3 point, double n);


int main(int argc, char** argv) {
    
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("RG lab2");
    
    loadMaterial("../objFiles/fireplace.mtl");
    loadScene("../objFiles/fireplace.obj");
    glutReshapeFunc(myReshape);
    glutDisplayFunc(myDisplay);
    glutKeyboardFunc(myKeyboard);
    glutIdleFunc(myIdle);

    glEnable(GL_CULL_FACE);

    glutMainLoop();
    return 0;
}

void myIdle() {
    int current_time = glutGet(GLUT_ELAPSED_TIME);
    int diff = current_time - previousClock;
    if (diff > 60)
    {
        particleControl();
        myDisplay();
        previousClock = current_time;
    }
}

void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.6, 0.6, 0.8, 1);
    drawScene();
    drawParticles();
    glutSwapBuffers();
}

void myReshape(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, width, height);
    updatePerspective();
}

void updatePerspective()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)width / height, 0.5, 150.0); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(ociste.x, ociste.y, ociste.z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // O x,y,z; glediste x,y,z; up vektor x,y,z
}

void myKeyboard(unsigned char theKey, int mouseX, int mouseY) {
    switch (theKey)
    {
    case 27:  exit(0);
        break;
    case 'd':
        ociste.x += 2.0;
        break;
    case 'a':
        ociste.x -= 2.0;
        break;
    case 's':
        ociste.z -= 2.0;
        break;
    case 'w':
        ociste.z += 2.0;
        break;
    }
    updatePerspective();
    glutPostRedisplay();
}

void loadScene(string fileName) {
    ifstream file(fileName);
    string line, previousLine = "NULL";
    while (true)
    {
        if (line.compare(0, 1, "") == 0 && previousLine.compare(0, 1, "o") == 0)
        {
            break;
        }
        if (previousLine.compare("NULL") == 0)
        {
            if (!getline(file, line))
            {
                break;
            }
        }
        else
        {
            line = previousLine;
        }
        //objekt
        if (line.compare(0, 1, "o") == 0)
        {
            
            string name;
            char c;
            istringstream iss(line);
            vector<vector<glm::vec3>> polys;
            iss >> c >> name;
            MyObject object(name);
            while (getline(file, line))
            {
                //normala vrha
                if (line.compare(0, 2, "vn") == 0)
                {
                    double x, y, z;
                    string vn;
                    istringstream iss(line);
                    iss >> vn >> x >> y >> z;
                    normals.push_back(glm::vec3(x, y, z));
                }
                //textura vrha
                else if (line.compare(0, 2, "vt") == 0)
                {
                    double x, y;
                    string vt;
                    istringstream iss(line);
                    iss >> vt >> x >> y;
                    textures.push_back(glm::vec2(x, y));
                }
                //vrh
                else if (line.compare(0, 1, "v") == 0)
                {
                    double x, y, z;
                    char c;
                    istringstream iss(line);
                    iss >> c >> x >> y >> z;
                    vertices.push_back(glm::vec3(x, y, z));
                }
                //poligon
                else if (line.compare(0, 1, "f") == 0)
                {
                    string firstIndex, secondIndex, thirdIndex, fourthIndex;
                    char c;
                    istringstream iss(line);
                    vector<glm::vec3> poly;
                    iss >> c >> firstIndex >> secondIndex >> thirdIndex >> fourthIndex;
                    
                    int index1 = stoi(firstIndex.substr(0, firstIndex.find("/"))) - 1;
                    int index2 = stoi(secondIndex.substr(0, secondIndex.find("/"))) - 1;
                    int index3 = stoi(thirdIndex.substr(0, thirdIndex.find("/"))) - 1;
                    poly.push_back(vertices[index1]);
                    poly.push_back(vertices[index2]);
                    poly.push_back(vertices[index3]);
                    if (!fourthIndex._Equal("")) {
                        int index4 = stoi(fourthIndex.substr(0, fourthIndex.find("/"))) - 1;
                        poly.push_back(vertices[index4]);
                    }
                    polygons.push_back(poly);
                    polys.push_back(poly);
                }
                //naziv materijala
                else if (line.compare(0, 6, "usemtl") == 0)
                {
                    string name;
                    string usemtl;
                    istringstream iss(line);
                    iss >> usemtl >> name;
                    auto it = find_if(materials.begin(), materials.end(), [&name](const Material* obj) { return obj->name == name; });
                    if (it != materials.end())
                    {
                        auto index = it - materials.begin();
                        object.material = materials[index];
                    }
                }
                //novi objekt
                else if (line.compare(0, 1, "o") == 0)
                {
                    previousLine = line;
                    break;
                }
                else
                {
                    continue;
                }
            }
            object.color = glm::vec3((double)rand() / RAND_MAX, (double)rand() / RAND_MAX, (double)rand() / RAND_MAX);
            object.polygons = polys;
            objects.push_back(object);
        }
    }
}

void drawParticles() {
    for (auto& particle : particles) {
        particle.draw();
    }
}

void loadMaterial(string fileName)
{
    ifstream file(fileName);
    string line;
    while (getline(file, line))
    {
        //novi materijal
        if (line.compare(0, 6, "newmtl") == 0)
        {
            string name;
            string newmtl;
            istringstream iss(line);
            iss >> newmtl >> name;
            Material* material = new Material(name);
            while (getline(file, line))
            {
                //ambijentalna komponenta
                if (line.compare(0, 2, "Ka") == 0)
                {
                    double x, y, z;
                    string Ka;
                    istringstream iss(line);
                    iss >> Ka >> x >> y >> z;
                    glm::vec3 ka(x, y, z);
                    material->Ka = ka;
                }
                //difuzna komponenta
                else if (line.compare(0, 2, "Kd") == 0)
                {
                    double x, y, z;
                    string Kd;
                    istringstream iss(line);
                    iss >> Kd >> x >> y >> z;
                    glm::vec3 kd(x, y, z);
                    material->Kd = kd;
                }
                //spekularna komponenta
                else if (line.compare(0, 2, "Ks") == 0)
                {
                    double x, y, z;
                    string Ks;
                    istringstream iss(line);
                    iss >> Ks >> x >> y >> z;
                    glm::vec3 ks(x, y, z);
                    material->Ks = ks;
                }
                //indeks hrapavosti
                else if (line.compare(0, 1, "d") == 0)
                {
                    double n;
                    char d;
                    istringstream iss(line);
                    iss >> d >> n;
                    material->d = n;
                }
                //kraj
                else if (line.compare(0, 5, "illum") == 0)
                {
                    break;
                }
                else
                {
                    continue;
                }
            }
            materials.push_back(material);
        }
    }
}


void drawScene() {

    for (auto& object : objects)
    {
        glColor3f(object.color.x, object.color.y, object.color.z);
        //if (object.name.find("Text") != string::npos) continue;
        for (auto& poly : object.polygons)
        {
            
            /*
            // Ambijentno (Ia * ka)
            glm::vec3 ka = object.material->Ka;
            double red = ambientLightColor.x * ka.x;
            double green = ambientLightColor.y * ka.y;
            double blue = ambientLightColor.z * ka.z;
            glm::vec3 ambient = glm::vec3(red, green, blue);
            double centroidx = 0.0, centroidy = 0.0, centroidz = 0.0;
            for (auto& point : poly) {
                centroidx += point.x;
            }
            for (auto& point : poly) {
                centroidy += point.y;
            }
            for (auto& point : poly) {
                centroidz += point.z;
            }
            
            glm::vec3 centroid = glm::vec3(centroidx / poly.size(), centroidy / poly.size(), centroidz / poly.size());
            glm::vec3 kd = object.material->Kd;
            glm::vec3 ks = object.material->Ks;
            double n = object.material->d;
            
            */
            
            if (poly.size() == 4) {
                glBegin(GL_POLYGON);
            }
            else {
                glBegin(GL_TRIANGLES);
            }
            
            for (auto& point : poly) {
                
                //glm::vec3 diffuse = calculateLighting(kd, ks, centroid, point, n);
                //glm::vec3 color = ambient + diffuse;
                //glColor3f(clamp(color.x, 0.0, 1.0), clamp(color.y, 0.0, 1.0), clamp(color.z, 0.0, 1.0));
                glVertex3f(point.x, point.y, point.z);
            }
            glEnd();
        }
        
    }
}

glm::vec3 calculateLighting(glm::vec3 kd, glm::vec3 ks, glm::vec3 centroid, glm::vec3 point, double n) {
        glm::vec3 color = glm::vec3(0.0, 0.0, 0.0);

        glm::vec3 L = glm::vec3(lightSource.x - point.x, lightSource.y - point.y, lightSource.z - point.z);
        L = glm::normalize(L);
        glm::vec3 N = glm::vec3(point.x - centroid.x, point.y - centroid.y, point.z - centroid.z);
        N = glm::normalize(N);
        double LN = glm::dot(L, N);

        // Difuzno (Ii * kd * (L*N))
        double red = lightColor.x * kd.x * LN;
        double green = lightColor.y * kd.y * LN;
        double blue = lightColor.z * kd.z * LN;
        color = color + glm::vec3(red, green, blue);

        // R = N * (2 * L*N) - L
        glm::vec3 R = glm::vec3(2 * LN * N.x, 2 * LN * N.y, 2 * LN * N.z) - L;
        R = glm::normalize(R);
        glm::vec3 V = glm::vec3(ociste.x - point.x, ociste.y - point.y, ociste.z - point.z);
        V = glm::normalize(V);
        double RV = glm::dot(R, V);

        // Spekularno (Ii * ks * (R*V)^n)
        red = lightColor.x * ks.x * pow(RV, n);
        green = lightColor.y * ks.y * pow(RV, n);
        blue = lightColor.z * ks.z * pow(RV, n);
        color = color + glm::vec3(red, green, blue);

        return color;
}

double clamp(double x, double lower, double upper)
{
    return min(upper, max(x, lower));
}

void particleControl()
{
    //nove
    glm::vec3 position = particleStartingPosition;
    glm::vec3 color = glm::vec3(1.0, 1.0, 0.0);
    double size = 0.07;
    Particle p1(position, color, size);
    Particle p2(position, color, size);
    Particle p3(position, color, size);
    Particle p4(position, color, size);
    Particle p5(position, color, size);
    Particle p6(position, color, size);
    particles.push_back(p1);
    particles.push_back(p2);
    particles.push_back(p3);
    particles.push_back(p4);
    particles.push_back(p5);
    particles.push_back(p6);
    for (int i = particles.size() - 1; i >= 0; i--)
    {
        //pomak
        glm::vec3 current_position = particles.at(i).position;
        double xTranslation = -(double)rand() / RAND_MAX * 0.3 + (double)rand() / RAND_MAX * 0.3;
        double zTranslation = -(double)rand() / RAND_MAX * 0.2 + (double)rand() / RAND_MAX * 0.2;
        particles.at(i).position = glm::vec3(current_position.x + xTranslation, clamp(current_position.y + 0.01, particleStartingPosition.y, particleStartingPosition.y + 6.0), current_position.z + zTranslation);

        //rotacija
        double xComponent = 0.0 + (double)rand() / RAND_MAX;
        double yComponent = 0.0 + (double)rand() / RAND_MAX;
        double zComponent = 0.0 + (double)rand() / RAND_MAX;
        particles.at(i).axis = glm::vec3(xComponent, yComponent, zComponent);
        double rotationAngle = 0.0 + (double)rand() / RAND_MAX * 360.0;
        particles.at(i).angle = rotationAngle;
        
        //boja
        glm::vec3 newPosition = particles.at(i).position;
        if (abs(abs(newPosition.x) - abs(particleStartingPosition.x)) >= 1.5 || abs(abs(newPosition.z) - abs(particleStartingPosition.z)) >= 1.5)
        {
            //tamnije sivo
            if (particles.at(i).isGrey)
            {
                glm::vec3 currentColor = particles.at(i).color;
                particles.at(i).color = glm::vec3(currentColor.x - 0.04, currentColor.x - 0.04, currentColor.x - 0.04);
            }
            //postavi u sivo
            else
            {
                particles.at(i).isGrey = true;
                particles.at(i).color = glm::vec3(0.3689, 0.3689, 0.3689);
            }
            //unisti
            if (abs(abs(newPosition.x) - abs(particleStartingPosition.x)) >= 1.7 || abs(abs(newPosition.z) - abs(particleStartingPosition.z)) >= 1.7)
            {
                particles.erase(particles.begin() + i);
                continue;
            }
        }
        //potamniti boju
        else
        {
            //u crno
            if (particles.at(i).isGrey)
            {
                particles.at(i).color = glm::vec3(0.0, 0.0, 0.0);
            }
            glm::vec3 currentColor = particles.at(i).color;
            //prema crvenoj
            particles.at(i).color = glm::vec3(currentColor.x, clamp(currentColor.y - 0.01, 0.0, 1.0), currentColor.z);
        }
        
        //unisti zbog vremena
        particles.at(i).age -= 0.005;
        if (particles.at(i).age <= 0.0)
        {
            particles.erase(particles.begin() + i);
        }
        
    }
}