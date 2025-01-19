//#include <Gl/GL.h>
//#include <GL/glu.h>
//#include <GL/glut.h>
//#define _CRT_SECURE_NO_DEPRECATE
#include<GL/glut.h>
#include <math.h>
#include<vector>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include<iostream>
#include<fstream>



//#include <bits/stdc++.h>
using namespace std;

GLfloat width = 700;
GLfloat height = 700;

GLfloat floor_width = 40;
GLfloat floor_height = 50;

float rotation = 0;

GLfloat leftLightHight = 40;
GLfloat rightLightHight = 40;
GLfloat spotLightHight = 40;

bool light1_on = true;
bool light2_on = true;
bool leftLight_on = true;
bool rightLight_on = true;
bool spotLight_on = true;
bool amb_on = true;
bool dif_on = true;
bool spe_on = true;
float spt_cutoff = 30;

bool redSignal = false;
GLfloat carStep = 0.5;
GLfloat car1Initial = 0;
GLfloat car2Initial = 0;
GLfloat car3Initial = 0;

bool planeMovement = true;
GLfloat planePosition = 75;
GLfloat planeStepSize = 0.5;

bool cloudMovement = true;
GLfloat cloudPosition = 110;
GLfloat cloudStepSize = 0.5;

GLfloat eyeX = 0;
GLfloat eyeY = 5;
GLfloat eyeZ = -27;

GLfloat lookX = 0;
GLfloat lookY = 5;
GLfloat lookZ = 30;

vector<int> v;
unsigned int ID;

static void res(int width, int height)
{
    glViewport(0, 0, width, height);
}

static GLfloat v_cube[8][3] =
{
    {0,0,0},
    {0,0,1},
    {0,1,0},
    {0,1,1},

    {1,0,0},
    {1,0,1},
    {1,1,0},
    {1,1,1}
};

static GLubyte c_ind[6][4] =
{
    {3,1,5,7},  //front
    {6,4,0,2},  //back
    {2,3,7,6},  //top
    {1,0,4,5},  //bottom
    {7,5,4,6},  //right
    {2,0,1,3}   //left
};

//Input: A(x1,y1,z1), B(x2,y2,z2), C(x3,y3,z3)
//Tinh tich  co huong cua AB va AC
//Sau do chuan hoa
static void getNormal3p(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2 - x1;
    Uy = y2 - y1;
    Uz = z2 - z1;

    Vx = x3 - x1;
    Vy = y3 - y1;
    Vz = z3 - z1;

    Nx = Uy * Vz - Uz * Vy;
    Ny = Uz * Vx - Ux * Vz;
    Nz = Ux * Vy - Uy * Vx;

    glNormal3f(Nx, Ny, Nz);         //Sets the current normal vector
}

//Input: R,G,B, Val
//Ve lap phuong voi cac thong so mau sac, chieu cao
void cube(float R = 0.5, float G = 0.5, float B = 0.5, float val = 1)
{
    GLfloat m_no[] = { 0, 0, 0, 1.0 };
    GLfloat m_amb[] = { R,G,B,1 };          //Do phan chieu
    GLfloat m_diff[] = { R,G,B,1 };         //Do phan tan
    GLfloat m_spec[] = { 1,1,1,1 };         //Do phan xa
    GLfloat m_sh[] = { 30 };                //do bong cua vat lieu

    //Thiet lap cac thuoc tinhs cho vat lieu truoc mat
    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);      //Moi truong xung quanh
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);     //Khuech tan
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);    //Phan chieu
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);     //Sang bong

    //Ve tu giac
    glBegin(GL_QUADS);
    for (GLint i = 0; i < 6; i++)
    {
        getNormal3p(v_cube[c_ind[i][0]][0], v_cube[c_ind[i][0]][1], v_cube[c_ind[i][0]][2],
            v_cube[c_ind[i][1]][0], v_cube[c_ind[i][1]][1], v_cube[c_ind[i][1]][2],
            v_cube[c_ind[i][2]][0], v_cube[c_ind[i][2]][1], v_cube[c_ind[i][2]][2]);
        glTexCoord2f(0, val);                       //Set the current texture coordinates
        glVertex3fv(&v_cube[c_ind[i][0]][0]);       //Specifies a vertex
        glTexCoord2f(0, 0);
        glVertex3fv(&v_cube[c_ind[i][1]][0]);
        glTexCoord2f(val, 0);
        glVertex3fv(&v_cube[c_ind[i][2]][0]);
        glTexCoord2f(val, val);
        glVertex3fv(&v_cube[c_ind[i][3]][0]);
    }
    glEnd();
}


/// Texture
class BmpLoader{
public:
    unsigned char* textureData;
    int iWidth, iHeight;

    BmpLoader(const char*);
    ~BmpLoader();

private:
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
};

BmpLoader::BmpLoader(const char* filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        std::cout << "File not found" <<endl<<filename<< std::endl;
    file.read(reinterpret_cast<char*>(&bfh), sizeof(BITMAPFILEHEADER));
    if (bfh.bfType != 0x4D42)
        std::cout << "Not a valid bitmap" << std::endl;
    file.read(reinterpret_cast<char*>(&bih), sizeof(BITMAPINFOHEADER));
    if (bih.biSizeImage == 0)
        bih.biSizeImage = bih.biHeight * bih.biWidth * 3;
    textureData = new unsigned char[bih.biSizeImage];
    file.seekg(bfh.bfOffBits, std::ios::beg);
    file.read(reinterpret_cast<char*>(textureData), bih.biSizeImage);
    unsigned char temp;
    for (int i = 0; i < bih.biSizeImage; i += 3)
    {
        temp = textureData[i];
        textureData[i] = textureData[i + 2];
        textureData[i + 2] = temp;
    }

    iWidth = bih.biWidth;
    iHeight = bih.biHeight;
    file.close();
}


BmpLoader::~BmpLoader()
{
    delete[] textureData;
}

void LoadTexture(const char* filename)
{
    glGenTextures(1, &ID);                                                  //It generates texture names
    glBindTexture(GL_TEXTURE_2D, ID);                                       //It enables the creation of a named texture that is bounded to a texture target
    glPixelStorei(GL_UNPACK_ALIGNMENT, ID);                                 //Set Pixel Storage modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);       //Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    BmpLoader bl(filename);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData);      //Creates 2-D mipmaps
}

void texture_image()
{
    /// road in z-direction
    LoadTexture("Image\\z-direction.bmp");
    v.push_back(ID);

    /// road in x-direction
    LoadTexture("Image\\x-direction.bmp");
    v.push_back(ID);

    /// road in crossing point
    LoadTexture("Image\\crossing.bmp");
    v.push_back(ID);

    /// left-front building x-front wall
    LoadTexture("Image\\left-front-building-front.bmp");
    v.push_back(ID);

    /// left-front building x-back wall
    LoadTexture("Image\\left-front-building-back.bmp");
    v.push_back(ID);

    /// left-front building z-front-back wall
    LoadTexture("Image\\left-front building-front-back.bmp");
    v.push_back(ID);

    /// left-front building roof
    LoadTexture("Image\\left-front-building-roof.bmp");
    v.push_back(ID);

    /// left-front building ground
    LoadTexture("Image\\grass-texture.bmp");
    v.push_back(ID);

    /// clock
    LoadTexture("Image\\clock.bmp");
    v.push_back(ID);

    LoadTexture("Image\\yellow-floor.bmp");
    v.push_back(ID);


    LoadTexture("Image\\front-yellow.bmp");
    v.push_back(ID);

    
}

/// Lighting
void leftLight()
{
    //light
    GLfloat l_no[] = { 0, 0, 0, 1.0 };
    GLfloat l_amb[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat l_dif[] = { 1,1,1,1 };
    GLfloat l_spec[] = { 1,1,1,1 };
    GLfloat l_pos[] = { 10, leftLightHight, 0 };

    glEnable(GL_LIGHT0);                            //enable OpenGL capabilities

    if (leftLight_on){
        glLightfv(GL_LIGHT0, GL_AMBIENT, l_amb);    //Return light source parameter values
        glLightfv(GL_LIGHT0, GL_DIFFUSE, l_dif);
        glLightfv(GL_LIGHT0, GL_SPECULAR, l_spec);
    }
    else
    {
        glLightfv(GL_LIGHT0, GL_AMBIENT, l_no);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, l_no);
        glLightfv(GL_LIGHT0, GL_SPECULAR, l_no);
    }

    if (amb_on && leftLight_on) glLightfv(GL_LIGHT0, GL_AMBIENT, l_amb);
    else glLightfv(GL_LIGHT0, GL_AMBIENT, l_no);

    if (dif_on && leftLight_on) glLightfv(GL_LIGHT0, GL_DIFFUSE, l_dif);
    else glLightfv(GL_LIGHT0, GL_DIFFUSE, l_no);

    if (spe_on && leftLight_on) glLightfv(GL_LIGHT0, GL_SPECULAR, l_spec);
    else glLightfv(GL_LIGHT0, GL_SPECULAR, l_no);

    glLightfv(GL_LIGHT0, GL_POSITION, l_pos);
}

void leftLightIndicator(){
    glPushMatrix();                             //Push the current matrix stack
    glTranslatef(10, leftLightHight + 1, 0);    //Multiplies the current matrix by a translation matrix
    glScalef(1, 1, 1);                          //Multiplies the current matrix by a general scaling matrix
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
}

void rightLight()
{
    //light
    GLfloat l_no[] = { 0, 0, 0, 1.0 };
    GLfloat l_amb[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat l_dif[] = { 1,1,1,1 };
    GLfloat l_spec[] = { 1,1,1,1 };
    GLfloat l_pos[] = { -10, rightLightHight, 0 };

    glEnable(GL_LIGHT1);

    if (rightLight_on)
    {
        glLightfv(GL_LIGHT1, GL_AMBIENT, l_amb);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, l_dif);
        glLightfv(GL_LIGHT1, GL_SPECULAR, l_spec);
    }
    else
    {
        glLightfv(GL_LIGHT1, GL_AMBIENT, l_no);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, l_no);
        glLightfv(GL_LIGHT1, GL_SPECULAR, l_no);
    }

    if (amb_on && rightLight_on) glLightfv(GL_LIGHT1, GL_AMBIENT, l_amb);
    else glLightfv(GL_LIGHT1, GL_AMBIENT, l_no);

    if (dif_on && rightLight_on) glLightfv(GL_LIGHT1, GL_DIFFUSE, l_dif);
    else glLightfv(GL_LIGHT1, GL_DIFFUSE, l_no);

    if (spe_on && rightLight_on) glLightfv(GL_LIGHT1, GL_SPECULAR, l_spec);
    else glLightfv(GL_LIGHT1, GL_SPECULAR, l_no);

    glLightfv(GL_LIGHT1, GL_POSITION, l_pos);
}

void rightLightIndicator()
{
    glPushMatrix();
    glTranslatef(-10, rightLightHight + 1, 0);
    glScalef(1, 1, 1);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.000, 1.0, 1.0, 1.0);
    glPopMatrix();
}

void spotLight()
{
    //light
    GLfloat l_no[] = { 0, 0, 0, 1.0 };
    GLfloat l_amb[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat l_dif[] = { 1,1,1,1 };
    GLfloat l_spec[] = { 1,1,1,1 };
    GLfloat l_pos[] = { 0, spotLightHight, 0 };

    glEnable(GL_LIGHT2);

    if (spotLight_on)
    {
        glLightfv(GL_LIGHT2, GL_AMBIENT, l_amb);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, l_dif);
        glLightfv(GL_LIGHT2, GL_SPECULAR, l_spec);
    }
    else
    {
        glLightfv(GL_LIGHT2, GL_AMBIENT, l_no);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, l_no);
        glLightfv(GL_LIGHT2, GL_SPECULAR, l_no);
    }

    if (amb_on && spotLight_on) glLightfv(GL_LIGHT2, GL_AMBIENT, l_amb);
    else glLightfv(GL_LIGHT2, GL_AMBIENT, l_no);

    if (dif_on && spotLight_on) glLightfv(GL_LIGHT2, GL_DIFFUSE, l_dif);
    else glLightfv(GL_LIGHT2, GL_DIFFUSE, l_no);

    if (spe_on && spotLight_on) glLightfv(GL_LIGHT2, GL_SPECULAR, l_spec);
    else glLightfv(GL_LIGHT2, GL_SPECULAR, l_no);

    glLightfv(GL_LIGHT2, GL_POSITION, l_pos);

    GLfloat l_spt[] = { 0, -1, 0, 1 };
    GLfloat spt_ct[] = { spt_cutoff };
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, l_spt);
    glLightfv(GL_LIGHT2, GL_SPOT_CUTOFF, spt_ct);
}

void spotLightIndicator()
{
    glPushMatrix();
    glTranslatef(0, spotLightHight + 1, 0);
    glScalef(1, 1, 1);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.000, 1.0, 1.0, 1.0);
    glPopMatrix();
}

void eyeIndicator()
{
    glPushMatrix();
    glTranslatef(eyeX, eyeY, eyeZ);
    glScalef(1, 1, 1);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.000, 0, 0, 1.0);
    glPopMatrix();
}

void lookAtPointIndicator()
{
    glPushMatrix();
    glTranslatef(lookX, lookY, lookZ);
    glScalef(1, 1, 1);
    glTranslatef(-0.5, 0, -0.5);
    cube(0, 1.0, 0, 1.0);
    glPopMatrix();
}

/// Grounding
void axes(){
    float length = 0.10;
    float width = 0.3;

    // X-axis
    glPushMatrix();
    glTranslatef(length / 2, 0, 0);
    glScalef(length, width, width);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(0.8, 0.1, 0.1);
    glPopMatrix();

    // Y-axis
    glPushMatrix();
    glTranslatef(0, length / 2, 0);
    glScalef(width, length, width);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(0.1, 0.8, 0.1);
    glPopMatrix();

    // Z-axis
    glPushMatrix();
    glTranslatef(0, 0, length / 2);
    glScalef(width, width, length);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(0.1, 0.1, 0.8);
    glPopMatrix();
}

void flr(){
    glPushMatrix();
    glScalef(floor_width, 1, floor_height);
    glTranslatef(-0.5, -1, -0.5);
    cube(1, 1, 1);
    glPopMatrix();
}

void ground()
{
    /// left Front floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[7]);
    glPushMatrix();
    glTranslatef(10, 0, -12.5);
    glScalef(20.0, 0.1, 25.0);
    glTranslatef(-0.5, -0.95, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// left Back floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[7]);
    glPushMatrix();
    glTranslatef(10, 0, 12.5);
    glScalef(20.0, 0.1, 25.0);
    glTranslatef(-0.5, -0.95, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// right Front floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[7]);
    glPushMatrix();
    glTranslatef(-10, 0, -12.5);
    glScalef(20.0, 0.1, 25.0);
    glTranslatef(-0.5, -0.95, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    /// right Back floor
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[7]);
    glPushMatrix();
    glTranslatef(-10, 0, 12.5);
    glScalef(20.0, 0.1, 25.0);
    glTranslatef(-0.5, -0.95, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void road()
{
    /// road in z-direction
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[0]);
    glPushMatrix();
    glScalef(15, 1, floor_height);
    glTranslatef(-0.5, -0.9, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// road in x-direction
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[1]);
    glPushMatrix();
    glScalef(floor_width, 1, 15);
    glTranslatef(-0.5, -0.9, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// tow roads intersection
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[2]);
    glPushMatrix();
    glScalef(15, 1, 15);
    glTranslatef(-0.5, -0.8, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}


/// Building & Clock
void building()
{
    /// LeftFront FrontXCube
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[5]);
    glPushMatrix();
    glTranslatef(15.0, 20, -17.0);
    glScalef(0.1, 10, 10);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[3]);
    glPushMatrix();
    glTranslatef(15.0, 10, -17.0);
    glScalef(0.1, 10, 10);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// LeftFront BackXCube
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[9]);
    glPushMatrix();
    glTranslatef(25.0, 0, -17.0);
    glScalef(0.1, 30, 10);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// LeftFront FrontZCube (Back Wall)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[9]);
    glPushMatrix();
    glTranslatef(20.0, 0, -22.0);
    glScalef(10, 30, 0.5);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// LeftFront FrontZCube (Front Wall)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[9]);
    glPushMatrix();
    glTranslatef(20.0, 0, -12.0);
    glScalef(10, 30, 0.5);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    /// LeftFront RoofCube
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[6]);
    glPushMatrix();
    glTranslatef(20.0, 30, -17.0);
    glScalef(10, 0.1, 10);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);



}

void buildings()
{
    glPushMatrix();
    building();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, 34);
    building();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-18,0,-30);
    glRotated(-90, 0, 1, 0);
    building();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0, 0, -35);
    glRotated(-180, 0, 1, 0);
    building();
    glPopMatrix();

    glPushMatrix();
    glRotated(-180, 0, 1, 0);
    ///building();
    glPopMatrix();
}

void ClockRaw()
{
    /// clock base
    /*glPushMatrix();
    glScalef(4, 2, 3);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.941, 0.502, 0.502, 1.0);
    glPopMatrix();*/
    /// clock root
    glPushMatrix();
    GLfloat m_amb[] = { 0.941, 0.502, 0.502, 1.0 };
    GLfloat m_diff[] = { 0.941, 0.502, 0.502, 1.0 };
    GLfloat m_spec[] = { 0.941, 0.502, 0.502, 1.0 };
    GLfloat m_sh[] = { 30 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    glRotated(90, -1, 0, 0);
    glBegin(GL_POLYGON);
    GLUquadricObj* clock_obj = gluNewQuadric();
    gluCylinder(clock_obj, 1.0, 1.0, 32.0, 300.0, 300.0);
    glEnd();
    glPopMatrix();
    /// clock holder
    glPushMatrix();
    glTranslatef(0, 30, 0);
    glScalef(4, 0.2, 4);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.502, 0.000, 0.000, 1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 34, 0);
    glScalef(4, 0.2, 4);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.502, 0.000, 0.000, 1.0);
    glPopMatrix();


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[8]);
    glPushMatrix();
    glTranslatef(2, 30, 0);
    glScalef(0.2, 4, 4);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[8]);
    glPushMatrix();
    glTranslatef(-2, 30, 0);
    glScalef(0.2, 4, 4);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[8]);
    glPushMatrix();
    glTranslatef(0, 30, 2);
    glScalef(4, 4, 0.2);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[8]);
    glPushMatrix();
    glTranslatef(0, 30, -2);
    glScalef(4, 4, 0.2);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void Clock()
{
    glPushMatrix();
    glTranslatef(0, 0, -3);
    ClockRaw();
    glPopMatrix();
}


/// Sky
void sky()
{
    GLfloat m_amb[] = {0.529, 0.808, 0.922, 1.0, 1.0};
    GLfloat m_diff[] = { 0.529, 0.808, 0.922, 1.0, 1.0 };
    GLfloat m_spec[] = { 0.529, 0.808, 0.922, 1.0, 1.0 };
    GLfloat m_sh[] = { 30 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    glPushMatrix();
    glTranslatef(0, 0, -230);
    ///glTranslatef(0, 0, -250);
    ///glTranslatef(0, 0, -225);
    glutSolidSphere(300, 300, 300);
    glPopMatrix();
}

void sun(){
    GLfloat m_amb[] = { 1,1,0,1 };
    GLfloat m_diff[] = { 1,1,0,1 };
    GLfloat m_spec[] = { 1,1,0,1 };
    GLfloat m_sh[] = { 30 };

        glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
        glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
        glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);
        glPushMatrix();
        glTranslatef(20, 80, 40);
        glutSolidSphere(2, 40, 40);
        glPopMatrix();
}

void cloud(){
    GLfloat m_amb[] = { 1,1,1,1 };
    GLfloat m_diff[] = { 1,1,1,1 };
    GLfloat m_spec[] = { 1,1,1,1 };
    GLfloat m_sh[] = { 30 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    glPushMatrix();
    glTranslated(-18.0, 40, 47);
    glutSolidSphere(3, 30, 30);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-18.0, 40, 50);
    glutSolidSphere(2, 30, 34);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-21.0, 40, 48);
    glutSolidSphere(2, 30, 34);
    glPopMatrix();

    glPushMatrix(); 
    glTranslated(-15.0, 42, 50);
    glutSolidSphere(2.5, 30, 30);
    glPopMatrix();
}

void clouds()
{
    glPushMatrix();
    if (cloudMovement)   glTranslated(cloudPosition -= cloudStepSize, 30, 0);
    else    glTranslated(cloudPosition, 30, 0);

    if (cloudPosition < 65) cloudPosition = 110;

    glScalef(3, 2, 1);
    cloud();
    glPopMatrix();
}

void planeRaw()
{
    GLfloat m_amb[] = { 0.416, 0.353, 0.804,1 };
    GLfloat m_diff[] = { 0.416, 0.353, 0.804,1 };
    GLfloat m_spec[] = { 0.416, 0.353, 0.804,1 };
    GLfloat m_sh[] = { 30 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);


    /// body
    glPushMatrix();
    glTranslated(5, 15, -17);
    glBegin(GL_POLYGON);
    GLUquadricObj* roo_obj = gluNewQuadric();
    gluCylinder(roo_obj, 1.5, 1.5, 8.0, 300.0, 300.0);
    glEnd();
    glPopMatrix();
    /// front-back
    glPushMatrix();
    glTranslated(5, 15, -9.0);
    glutSolidCone(1.5, 4.0, 300, 300);
    glPopMatrix();
    glPushMatrix();
    glTranslated(5, 13.6, -17);
    glScalef(2.7, 2.7, 0.2);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.416, 0.353, 0.804, 1);
    glPopMatrix();
    /// wings - left
    glPushMatrix();
    glTranslated(8.0, 15, -11.0);
    glRotated(30, 0, 1, 0);
    glScalef(4.0, 0.2, 1.5);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 0, 1.0, 1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslated(7.5, 15, -15.0);
    glRotated(30, 0, 1, 0);
    glScalef(3.0, 0.2, 1.5);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 0, 1.0, 1.0);
    glPopMatrix();
    /// wings - right
    glPushMatrix();
    glTranslated(2.0, 15, -11.0);
    glRotated(30, 0, -1, 0);
    glScalef(4.0, 0.2, 1.5);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 0, 1.0, 1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslated(2.5, 15, -15.0);
    glRotated(30, 0, -1, 0);
    glScalef(3.0, 0.2, 1.5);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 0, 1.0, 1.0);
    glPopMatrix();
    /// wings - back
    glPushMatrix();
    glTranslated(5.0, 16.0, -15.5);
    glRotated(30, -1, 0, 0);
    glScalef(0.2, 4.0, 3.0);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 0, 1.0, 1.0);
    glPopMatrix();
}

void plane()
{
    glPushMatrix();
    if (planeMovement)   glTranslated(-30, 110, planePosition += planeStepSize);
    else    glTranslated(-30, 110, planePosition);

    if (planePosition > 100) planePosition = 76;

    ///glTranslated(-30, 120, 100);
    ///glRotated(30, 0, -1, 0);
    glRotated(60, -1, 0, 0);
    glScalef(2, 2, 2);
    planeRaw();
    glPopMatrix();
}


/// Den Giao Thong
void lamPost()
{
    GLfloat m_amb[] = { 0.275, 0.510, 0.706, 1.0 };
    GLfloat m_diff[] = { 0.275, 0.510, 0.706, 1.0 };
    GLfloat m_spec[] = { 0.275, 0.510, 0.706, 1.0 };
    GLfloat m_sh[] = { 30 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    /// lamp post
    glPushMatrix();
    glTranslated(9, 0, -22);
    glRotated(90, -1, 0, 0);
    glBegin(GL_POLYGON);
    GLUquadricObj* obj = gluNewQuadric();
    gluCylinder(obj, 0.3, 0.3, 15.0, 300.0, 300.0);
    glEnd();
    glPopMatrix();

    /// lamp holder
    glPushMatrix();
    glTranslatef(7.0, 15.0, -22);
    glScalef(4.0, 0.2, 0.5);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.275, 0.510, 0.706, 1.0);
    glPopMatrix();

    /// lamp
    glPushMatrix();
    glTranslatef(5.5, 14.0, -22);
    glScalef(0.2, 1.0, 0.2);
    glTranslatef(-0.5, 0, -0.5);
    cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
}

void lamPosts()
{
    glPushMatrix();
    lamPost();
    glPopMatrix();
    glPushMatrix();
    glTranslated(0, 0, 10);
    lamPost();
    glPopMatrix();
    glPushMatrix();
    glTranslated(0, 0, 34);
    lamPost();
    glPopMatrix();
    glPushMatrix();
    glTranslated(0, 0, 44);
    lamPost();
    glPopMatrix();
    glPushMatrix();
    glTranslated(0, 0, -34);
    glRotated(-180, 0, 1, 0);
    lamPost();
    glPopMatrix();
    glPushMatrix();
    glTranslated(0, 0, -44);
    glRotated(-180, 0, 1, 0);
    lamPost();
    glPopMatrix();
    glPushMatrix();
    glTranslated(0, 0, -10);
    glRotated(-180, 0, 1, 0);
    lamPost();
    glPopMatrix();
    glPushMatrix();
    glRotated(-180, 0, 1, 0);
    lamPost();
    glPopMatrix();
}


/// Trees
void tree()
{
    GLfloat m_amb[] = { 0.000, 0.392, 0.000,1 };
    GLfloat m_diff[] = { 0.000, 0.392, 0.000,1 };
    GLfloat m_spec[] = { 0.000, 0.392, 0.000,1 };
    GLfloat m_sh[] = { 30 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    /// root
    glPushMatrix();
    glTranslated(11, 0, -17);
    glRotated(90, -1, 0, 0);
    glBegin(GL_POLYGON);
    GLUquadricObj* roo_obj = gluNewQuadric();
    gluCylinder(roo_obj, 0.3, 0.3, 9.0, 300.0, 300.0);
    glEnd();
    glPopMatrix();
    /// branch
    glPushMatrix();
    glTranslated(11, 8.0, -17);
    glRotated(90, -1, 0, 0);
    glutSolidCone(2.5, 4.0, 300, 300);
    glPopMatrix();
    glPushMatrix();
    glTranslated(11, 10.5, -17);
    glRotated(90, -1, 0, 0);
    glutSolidCone(2.5, 4.0, 300, 300);
    glPopMatrix();
}

void trees()
{   
    //Ve cay 1
    glPushMatrix();
    tree();
    glPopMatrix();
    //Cay 2
    glPushMatrix();
    glTranslated(0, 0, 34.0);
    tree();
    glPopMatrix();
    //Cay 3
    glPushMatrix();
    glTranslated(-22.0, 0, 0);
    tree();
    glPopMatrix();
    //Cay 4
    glPushMatrix();
    glTranslated(-22.0, 0, 34.0);
    tree();
    glPopMatrix();
}


/// Jatri Chaone
void jatriChaone()
{
    /// floor
    glPushMatrix();
    glTranslatef(16.5, 0, -17.0);
    glScalef(7, 0.1, 12);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.902, 0.902, 0.980, 1.0);
    glPopMatrix();
    /// roof
    glPushMatrix();
    glTranslatef(16.5, 15, -17.0);
    glScalef(7, 0.1, 12);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.902, 0.902, 0.980, 1.0);
    glPopMatrix();

    /// back wall in z
    glPushMatrix();
    glTranslatef(16.5, 0, -23.0);
    glScalef(7, 15.0, 0.1);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.804, 0.361, 0.361, 1.0);
    glPopMatrix();
    /// front wall in z
    glPushMatrix();
    glTranslatef(16.5, 0, -11.0);
    glScalef(7, 15.0, 0.1);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.804, 0.361, 0.361, 1.0);
    glPopMatrix();
    /// back wall in x
    glPushMatrix();
    glTranslatef(20, 0, -17.0);
    glScalef(0.1, 15.0, 12);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.804, 0.361, 0.361, 1.0);
    glPopMatrix();

    /// back seat in x
    glPushMatrix();
    glTranslatef(18.5, 0, -17.0);
    glScalef(2.0, 3.0, 12.0);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.180, 0.545, 0.341, 1.0);
    glPopMatrix();
    /// back seat in z
    glPushMatrix();
    glTranslatef(15.5, 0, -12.0);
    glScalef(4.0, 3.0, 2.0);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.180, 0.545, 0.341, 1.0);
    glPopMatrix();
    /// front seat in z
    glPushMatrix();
    glTranslatef(15.5, 0, -22.0);
    glScalef(4.0, 3.0, 2.0);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.180, 0.545, 0.341, 1.0);
    glPopMatrix();
}

void jatriChaonePositioned()
{
    glPushMatrix();
    glTranslatef(1.0, 0, -2.0);
    glRotated(-180, 0, 1, 0);
    jatriChaone();
    glPopMatrix();
}


/// Traffic System
void trafficSystem()
{
    /// traffic light base
    glPushMatrix();
    glScalef(1.5, 1.5, 1.5);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.941, 0.502, 0.502, 1.0);
    glPopMatrix();
    /// traffic light root

    GLfloat m_amb[] = { 0.941, 0.502, 0.502, 1.0 };
    GLfloat m_diff[] = { 0.941, 0.502, 0.502, 1.0 };
    GLfloat m_spec[] = { 0.941, 0.502, 0.502, 1.0 };
    GLfloat m_sh[] = { 30 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    glPushMatrix();
    glRotated(90, -1,0 , 0);
    glBegin(GL_POLYGON);
    GLUquadricObj* clock_obj = gluNewQuadric();
    gluCylinder(clock_obj, 0.5, 0.5, 20, 300.0, 300.0);
    glEnd();
    glPopMatrix();

    /// traffic light holder
    glPushMatrix();
    glTranslatef(0, 25.5, 0);
    glScalef(3, 8, 3);
    glTranslatef(-0.5, -1.0, -0.5);
    cube(0.0, 0.0, 1.0, 1.0);
    glPopMatrix();
}

void trafficSystem1() {
    trafficSystem();

    glPushMatrix();
    glTranslatef(-2.0, 30.0, 0.0);
    glScalef(0.1, 1.5, 1.0);
    glTranslatef(-0.5, -5.0, -0.5);
    if (!redSignal) cube(1.0, 0.0, 0.0, 1.0);
    else cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.0, 28.0, 0.0);
    glScalef(0.1, 1.5, 1.0);
    glTranslatef(-0.5, -5.0, -0.5);
    cube(1.0, 1.0, 1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.0, 26.0, 0.0);
    glScalef(0.1, 1.5, 1.0);
    glTranslatef(-0.5, -5.0, -0.5);
    if (!redSignal) cube(1.0, 1.000, 1.000, 1.0);
    else cube(0.0, 1.0, 0.0, 1.0);
    glPopMatrix();

}

void trafficSystem2() {
    trafficSystem();
    glPushMatrix();
    glTranslatef(0, 30.0, 2.0);
    glScalef(1.0, 1.5, 0.1);
    glTranslatef(-0.5, -5.0, -0.5);
    if (!redSignal) cube(1.0, 1.000, 1.000, 1.0);
    else cube(1.0, 0.000, 0.000, 1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 28.0, 2.0);
    glScalef(1.0, 1.5, 0.1);
    glTranslatef(-0.5, -5.0, -0.5);
    if (!redSignal) cube(1.0, 1.000, 1.000, 1.0);
    else cube(1.000, 1.000, 1.000, 1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 26.0, 2.0);
    glScalef(1.0, 1.5, 0.1);
    glTranslatef(-0.5, -5.0, -0.5);
    if (!redSignal) cube(0.0, 1.000, 0.000, 1.0);
    else cube(1.000, 1.000, 1.000, 1.0);
    glPopMatrix();
}

void trafficSystem3() {
    trafficSystem();
    glPushMatrix();
    glTranslatef(0.0, 30.0, -2.0);
    glScalef(1.0, 1.5, 0.1);
    glTranslatef(-0.5, -5.0, -0.5);
    if (!redSignal) cube(1.0, 1.000, 1.000, 1.0);
    else cube(1.0, 0.000, 0.000, 1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 28.0, -2.0);
    glScalef(1.0, 1.5, 0.1);
    glTranslatef(-0.5, -5.0, -0.5);
    if (!redSignal) cube(1.0, 1.000, 1.000, 1.0);
    else cube(1.000, 1.000, 1.000, 1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 26.0, -2.0);
    glScalef(1.0, 1.5, 0.1);
    glTranslatef(-0.5, -5.0, -0.5);
    if (!redSignal) cube(0.0, 1.000, 0.000, 1.0);
    else cube(1.000, 1.000, 1.000, 1.0);
    glPopMatrix();
}

void trafficSystem4() {
    trafficSystem();
    glPushMatrix();
    glTranslatef(2.0, 30.0, 0.0);
    glScalef(0.1, 1.5, 1.0);
    glTranslatef(-0.5, -5.0, -0.5);
    if (!redSignal) cube(1.0, 0.0, 0.0, 1.0);
    else cube(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.0, 28.0, 0.0);
    glScalef(0.1, 1.5, 1.0);
    glTranslatef(-0.5, -5.0, -0.5);
    if (!redSignal) cube(1.0, 1.000, 1.000, 1.0);
    else cube(1.000, 1.000, 1.000, 1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.0, 26.0, 0.0);
    glScalef(0.1, 1.5, 1.0);
    glTranslatef(-0.5, -5.0, -0.5);
    if (!redSignal) cube(1.0, 1.000, 1.000, 1.0);
    else cube(0.0, 1.0, 0.0, 1.0);
    glPopMatrix();
}

void trafficSystemPositioned()
{
    glPushMatrix();
    glTranslatef(-10,0,10);
    trafficSystem1();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10,0,10);
    trafficSystem2();
    glPopMatrix();

   glPushMatrix();
    glTranslatef(-10,0,-10);
    trafficSystem3();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10,0,-10);
    trafficSystem4();
    glPopMatrix();
}


/// Curves & Surfaces
const double PI = 3.14159265389;
const int L = 20;
const int nt = 40;				//number of slices along x-direction
const int ntheta = 20;

GLfloat ctrlpoints[L + 1][3] =
{
    {-0.0359765, 0.0234949, 0},
    {-0.223935, 0.187959, 0},
    {-0.29442, 0.422907, 0},
    {-0.200441, 0.610866, 0},
    {0.0814978, 0.610866, 0},
    {0.48091, 0.657856, 0},
    {0.739354, 0.657856, 0},
    {1.04479, 0.657856, 0},
    {1.30323, 0.657856, 0},
    {1.46769, 0.446402, 0},
    {1.4207, 0.234949, 0},
    {1.32673, 0.117474, 0}
};

void setNormal(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2 - x1;
    Uy = y2 - y1;
    Uz = z2 - z1;

    Vx = x3 - x1;
    Vy = y3 - y1;
    Vz = z3 - z1;

    Nx = Uy * Vz - Uz * Vy;
    Ny = Uz * Vx - Ux * Vz;
    Nz = Ux * Vy - Uy * Vx;

    glNormal3f(-Nx, -Ny, -Nz);
}

long long nCr(int n, int r)
{
    if (r > n / 2) r = n - r; // because C(n, r) == C(n, n - r)
    long long ans = 1;
    int i;

    for (i = 1; i <= r; i++)
    {
        ans *= n - r + i;
        ans /= i;
    }

    return ans;
}

void BezierCurve(double t, float xy[2])
{
    double y = 0;
    double x = 0;
    t = t > 1.0 ? 1.0 : t;
    for (int i = 0; i <= L; i++)
    {
        int ncr = nCr(L, i);
        double oneMinusTpow = pow(1 - t, double(L - i));
        double tPow = pow(t, double(i));
        double coef = oneMinusTpow * tPow * ncr;
        x += coef * ctrlpoints[i][0];
        y += coef * ctrlpoints[i][1];

    }
    xy[0] = float(x);
    xy[1] = float(y);
}

void wheel()
{
    GLfloat m_amb[] = { 0.184, 0.310, 0.310, 1 };
    GLfloat m_diff[] = { 0.184, 0.310, 0.310, 1 };
    GLfloat m_spec[] = { 0.184, 0.310, 0.310, 1 };
    GLfloat m_sh[] = { 30 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);

    int i, j;
    float x, y, z, r;				//current coordinates
    float x1, y1, z1, r1;			//next coordinates
    float theta;

    const float startx = 0, endx = ctrlpoints[L][0];
    //number of angular slices
    const float dx = (endx - startx) / nt;	//x step size
    const float dtheta = 2 * PI / ntheta;		//angular step size

    float t = 0;
    float dt = 1.0 / nt;
    float xy[2];
    BezierCurve(t, xy);
    x = xy[0];
    r = xy[1];
    //rotate about z-axis
    float p1x, p1y, p1z, p2x, p2y, p2z;
    for (i = 0; i < nt; ++i)  			//step through x
    {
        theta = 0;
        t += dt;
        BezierCurve(t, xy);
        x1 = xy[0];
        r1 = xy[1];

        //draw the surface composed of quadrilaterals by sweeping theta
        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= ntheta; ++j)
        {
            theta += dtheta;
            double cosa = cos(theta);
            double sina = sin(theta);
            y = r * cosa;
            y1 = r1 * cosa;	//current and next y
            z = r * sina;
            z1 = r1 * sina;	//current and next z

            //edge from point at x to point at next x
            glVertex3f(x, y, z);

            if (j > 0)
            {
                setNormal(p1x, p1y, p1z, p2x, p2y, p2z, x, y, z);
            }
            else
            {
                p1x = x;
                p1y = y;
                p1z = z;
                p2x = x1;
                p2y = y1;
                p2z = z1;

            }
            glVertex3f(x1, y1, z1);

            //forms quad with next pair of points with incremented theta value
        }
        glEnd();
        x = x1;
        r = r1;
    } //for i
}

void wheelPositioned() {
    /// right wheels
    glPushMatrix();
    glTranslatef(-5, 1.0, -18.0);
    glScalef(1.0, 2.0, 1.5);
    wheel();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-5, 1.0, -21.0);
    glScalef(1.0, 2.0, 1.5);
    wheel();
    glPopMatrix();

    /// left wheels
    glPushMatrix();
    glTranslatef(-1.75, 1.0, -18.0);
    glScalef(1.0, 2.0, 1.5);
    wheel();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-1.75, 1.0, -21.0);
    glScalef(1.0, 2.0, 1.5);
    wheel();
    glPopMatrix();
}

void carBodyRaw() {
    /// Main body (lower part)
    glPushMatrix();
    glTranslatef(-3.0, 1.5, -19.5);
    glScalef(4.0, 1.5, 5.0); // Điều chỉnh tỉ lệ cho thân xe rộng và thấp hơn
    glTranslatef(-0.5, 0, -0.5);
    cube(0.8, 0.2, 0.2, 1.0); // Đổi màu thành đỏ đậm
    glPopMatrix();

    /// Upper body (cabin)
    glPushMatrix();
    glTranslatef(-3.0, 3.0, -20.0);
    glScalef(2.0, 1.2, 3.0); // Điều chỉnh tỉ lệ cabin cho cân đối hơn
    glTranslatef(-0.5, 0, -0.5);
    cube(0.2, 0.2, 0.8, 1.0); // Đổi màu thành xanh đậm
    glPopMatrix();

    /// Side windows
    // Cửa sổ bên trái
    glPushMatrix();
    glTranslatef(-4.2, 3.3, -20.5); // Điều chỉnh vị trí để khớp với cabin
    glScalef(0.1, 0.8, 2.0);        // Điều chỉnh kích thước cửa sổ bên trái
    cube(0.5, 0.5, 0.5, 0.5);       // Màu xám nhạt cho cửa sổ
    glPopMatrix();

    // Cửa sổ bên phải
    glPushMatrix();
    glTranslatef(-1.8, 3.3, -20.5); // Điều chỉnh vị trí để khớp với cabin
    glScalef(0.1, 0.8, 2.0);        // Điều chỉnh kích thước cửa sổ bên phải
    cube(0.5, 0.5, 0.5, 0.5);       // Màu xám nhạt cho cửa sổ
    glPopMatrix();

    /// Wheels
    glPushMatrix();
    glTranslatef(-4.0, 1.0, -18.0); // Bánh trước bên trái
    glScalef(0.8, 0.8, 0.8);
    cube(0.1, 0.1, 0.1, 1.0); // Bánh xe màu đen
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-4.0, 1.0, -22.0); // Bánh sau bên trái
    glScalef(0.8, 0.8, 0.8);
    cube(0.1, 0.1, 0.1, 1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.0, 1.0, -18.0); // Bánh trước bên phải
    glScalef(0.8, 0.8, 0.8);
    cube(0.1, 0.1, 0.1, 1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.0, 1.0, -22.0); // Bánh sau bên phải
    glScalef(0.8, 0.8, 0.8);
    cube(0.1, 0.1, 0.1, 1.0);
    glPopMatrix();
}



void car() {
    /// car 1
    glPushMatrix();
    if (!redSignal) {
        glTranslatef(-1.0, 0, car1Initial += carStep);
    }
    else {
        glTranslatef(-1.0, 0, car1Initial);
    }

    if (car1Initial > 40.0) {
        car1Initial = 0.0;
    }

    wheelPositioned();
    carBodyRaw();
    glPopMatrix();

    /// car 2
    glPushMatrix();
    if (!redSignal) {
        glTranslatef(1.0, 0, car2Initial -= carStep);
    }
    else {
        glTranslatef(1.0, 0, car2Initial);
    }

    if (car2Initial < -40.0) {
        car2Initial = 0.0;
    }

    glRotated(-180, 0, 1, 0);
    wheelPositioned();
    carBodyRaw();
    glPopMatrix();

    glPushMatrix();
    glRotated(-90, 0, 1, 0);
    glTranslatef(-1.0, 0, 4);
    wheelPositioned();
    carBodyRaw();
    glPopMatrix();
}


/// Objects to display
void objects()
{
    axes();

    flr();
    ground();
    road();
    car();

    buildings();
    Clock();
    jatriChaonePositioned();

    leftLight();
    rightLight();
    spotLight();

    //leftLightIndicator();
    //rightLightIndicator();
    //spotLightIndicator();

    ///eyeIndicator();
    //lookAtPointIndicator();

    sky();
    clouds();
    sun();
    plane();

    lamPosts();
    trees();

    trafficSystemPositioned();

    ///cout<<"X : "<<lookX<<" "<<"Y : "<<lookY<<" "<<"Z : "<<lookZ<<endl;
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-5, 5, -5, 5, 3, 100.0);
    gluPerspective(0, 0, 0, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, lookX, lookY, lookZ, 0, 1, 0);

    glRotatef(rotation, 0, 1, 0);

    objects();

    glutSwapBuffers();
}

//Event press key
static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        exit(0);
        break;
    case ',':
        rotation += 5;
        break;
    case '.':
        rotation -= 5;
        break;

        /// amb, dif & spe control
    case 'a':
        amb_on = !amb_on;
        break;
    case 'd':
        dif_on = !dif_on;
        break;
    case 's':
        spe_on = !spe_on;
        break;

        /// zoom in, zoom out & left-right
    case 'g':
        //if(lookX <22) lookX++;
        //else lookX;
        lookX++;
        break;
    case 'h':
        //if(lookX > -22) lookX--;
        //else lookX;
        lookX--;
        break;

    case 'j':
        eyeX++;
        break;
    case 'k':
        eyeX--;
        break;


    case 'u':
        eyeY++;
        lookY++;
        break;
    case 'i':
        if (lookY > 5) {
            lookY--;
            eyeY--;
        }
        else {
            lookY;
            eyeY;
        }
        //eyeY--;
        //lookY--;
        break;

    case '=':
        if (lookZ < 123) {
            lookZ++;
            eyeZ++;
        }
        else {
            lookZ;
            eyeZ;
        }
        //eyeZ++;
        //lookZ++;
        break;
    case '-':
        if (lookZ > 29) {
            lookZ--;
            eyeZ--;
        }
        else {
            lookZ;
            eyeZ;
        }
        //eyeZ--;
        //lookZ--;
        break;

        ///lighting on off
    case '1':
        leftLight_on = !leftLight_on;
        break;
    case '2':
        rightLight_on = !rightLight_on;
        break;
    case '3':
        spotLight_on = !spotLight_on;
        break;

        /// moving objects control
    case 't':
        redSignal = !redSignal;
        break;
    case 'p':
        planeMovement = !planeMovement;
        break;
    case 'c':
        cloudMovement = !cloudMovement;
        break;
    }
    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("1607021 - City Road 3D Scenario");

    glutReshapeFunc(res);                       //
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);

    glEnable(GL_LIGHTING);
    GLfloat globalAmbient[] = { 0.0, 0.0, 0.0, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    texture_image();

    printf("\nPlease, Follow this instructions for zooming & lighting: \n\n");

    printf("\tZoom In -> +\n");
    printf("\tZoom Out -> -\n");
    printf("\tLook Left -> g\n");
    printf("\ttLook Right -> h\n");
    printf("\tMove Camera Left -> j\n");
    printf("\tMove Camera Right -> k\n");
    printf("\tUp -> u\n");
    printf("\tDown -> i\n\n");

    printf("\tPoint Light #1 On/Off -> 1\n");
    printf("\tPoint Light #2 On/Off -> 2\n");
    printf("\tSpot Light On/Off -> 3\n\n");

    printf("\tAmbient Light On/Off -> a\n");
    printf("\tDiffuse Light On/Off -> d\n");
    printf("\tSpecular Light On/Off -> s\n\n");

    printf("\tTraffic System control -> t\n");
    printf("\tCloud movement control -> c\n");
    printf("\tPlane movement control -> p\n");

    glutMainLoop();

    return EXIT_SUCCESS;
}
