/*  FATAL FX' Dragnet 2k (C) 1996, 1998 Fatal FX
 *  Version 1.1
 *  Contact: roger.wetzel@fatalfx.com
 *  How to compile on a SGI (GLUT is needed):
 *  cc -o dragnet2k dragnet2k.c -lglut -lGLU -lGL -lXmu -lXext -lX11 -lm
 */

#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SPACE 35.0
#define EDGELEN 0.08
#define POINTSPERSIDE 32
#define STARTCOORD (POINTSPERSIDE-1)*EDGELEN/2
#define ANIMLEN 100

struct Point3d
{
    GLfloat x, y, z;
};

struct Point3d p[POINTSPERSIDE][POINTSPERSIDE];
GLfloat material1[] = {0.1, 0.5, 0.8, 1.0};
GLfloat material2[] = {0.8, 0.8, 0.8, 1.0};
GLfloat spinX = 0.0, spinY = -50.0, spinZ = 0.0;
GLfloat x = 0.0, y = 0.0, z = 0.7;
int dispList = 1;
int material = 0, anim = -1, spin = -1;

static inline void normalize(float v[3])
{
    float d = sqrtf(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
    if (d != 0.0) /* avoid division by zero */
    {
	v[0] /= d;
	v[1] /= d;
	v[2] /= d;
    }
}

static inline void normCrossProd(float v1[3], float v2[3], float out[3])
{
    out[0] = v1[1]*v2[2] - v1[2]*v2[1];
    out[1] = v1[2]*v2[0] - v1[0]*v2[2];
    out[2] = v1[0]*v2[1] - v1[1]*v2[0];
    normalize(out);
}

void changeMaterial(void)
{
    if (material)
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material1);
    else
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material2);
    material = !material;    
}

void resetMaterial(void)
{
    material = 0;
    changeMaterial();
    material = 0;
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glRotatef(spinY, 1.0, 0.0, 0.0);
    glRotatef(spinX, 0.0, 1.0, 0.0);
    glRotatef(spinZ, 0.0, 0.0, 1.0);
    glCallList(dispList);
    glPopMatrix();
    glFinish();
    glutSwapBuffers();
}

void idle(void)
{
    if (spin)
    {
	spinX += x;
	if (spinX >= 360.0)
	    spinX -= 360.0;
	spinY += y;
	if (spinY >= 360.0)
	    spinY -= 360.0;
	spinZ += z;
	if (spinZ >= 360.0)
	    spinZ -= 360.0;
    }

    if (anim)
    {
	dispList--;
	if (!dispList)
	    dispList = ANIMLEN;
    }

    glutPostRedisplay();
}

void motion(int xPos, int yPos)
{
    x = y = 0.0;
    spinX = (GLfloat)xPos;
    spinY = (GLfloat)yPos;
}

void initDisplayList(int anim)
{
    int i, j, k;
    GLfloat x, y, v1[3], v2[3], normal[3];

    for (y=STARTCOORD, j=0; j<POINTSPERSIDE; y-=EDGELEN, j++)
    {
	for (x=-STARTCOORD, k=0; k<POINTSPERSIDE; x+=EDGELEN, k++)
	{
	    p[k][j].x = x;
	    p[k][j].y = y;
	    p[k][j].z = 0.30*sinf(0.1*sqrtf(x*SPACE*x*SPACE + y*SPACE*y*SPACE) + 
		(GLfloat)anim*2.0*M_PI/(GLfloat)ANIMLEN);
	}
    }

    glNewList(anim+1, GL_COMPILE);
    resetMaterial();
    for (j=0; j<POINTSPERSIDE-1; j++)
    {
	for (i=0; i<POINTSPERSIDE-1; i++)
	{
	    changeMaterial();
	    glBegin(GL_QUADS);

	    /* do the normal vector */
	    v1[0] = p[j][i].x - p[j+1][i].x;
	    v1[1] = p[j][i].y - p[j+1][i].y;
	    v1[2] = p[j][i].z - p[j+1][i].z;
	    
	    v2[0] = p[j+1][i+1].x - p[j+1][i].x;
	    v2[1] = p[j+1][i+1].y - p[j+1][i].y;
	    v2[2] = p[j+1][i+1].z - p[j+1][i].z;

	    normCrossProd(v1, v2, normal);
	    glNormal3fv(normal);

	    /* vertexes */
	    glVertex3f(    p[j][i].x,     p[j][i].y,     p[j][i].z);
	    glVertex3f(  p[j+1][i].x,   p[j+1][i].y,   p[j+1][i].z);
	    glVertex3f(p[j+1][i+1].x, p[j+1][i+1].y, p[j+1][i+1].z);
	    glVertex3f(  p[j][i+1].x,   p[j][i+1].y,   p[j][i+1].z);
	    glEnd();
	}
    }
    glEndList(); 
}

void init(void)
{   
    int i;
    GLfloat lightPosition[] = {-3.0, 0.0, 1.5, 1.0};
    GLfloat matSpecular[] = {0.8, 0.8, 0.8, 1.0};
    GLfloat matShininess[] = {80.0};

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    for (i=0; i<ANIMLEN; i++)
	initDisplayList(i);

    dispList = 1;
}

void keyboard(unsigned char key, int xPos, int yPos)
{
    switch (key)
    {
	case 27:
	    exit(0);
	    
	case '1':
	    x--;
	    break;

	case '2':
	    x = 0.0;
	    break;

	case '3':
	    x++;
	    break;

	case 'q':
	    y--;
	    break;

	case 'w':
	    y = 0.0;
	    break;

	case 'e':
	    y++;
	    break;

	case 'a':
	    z--;
	    break;

	case 's':
	    z = 0.0;
	    break;

	case 'd':
	    z++;
	    break;
	
	case 'x':
	    anim = !anim;
	    break;

	case 'z':
	    spin = !spin;
	    break;
    }
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -3.5);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("FATAL FX' Dragnet 2k [www.fatalfx.com]");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(motion);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
