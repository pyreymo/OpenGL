#include "pch.h"
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <GL/freeglut.h>

typedef GLfloat Vertices3f[3][3];
typedef GLfloat Point3f[3];
Vertices3f globalVertices;
GLint globalTime = 0, colorkey = 0;
Vertices3f vertices = { {-0.606,-0.35,0.0},{0.606,-0.35,0.0},{0.0,0.7,0.0} };

void init(void) {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
}

void draw_triangle(GLfloat* a, GLfloat* b, GLfloat* c, GLfloat t) {
	glColor3f(sin(t), sin(t - 1), sin(t + 1));
	glVertex3fv(a);
	glVertex3fv(b);
	glVertex3fv(c);
}
float p(int k) {
	return (GLfloat)k * 0.03142;
}
void divide_triangle(GLfloat* a, GLfloat* b, GLfloat* c, int k, int color) {
	GLfloat ab[3], ac[3], bc[3];
	int j;
	colorkey = color;
	if (k > 0) {
		for (j = 0; j < 3; j++) ab[j] = (a[j] + b[j]) / 2;
		for (j = 0; j < 3; j++) ac[j] = (a[j] + c[j]) / 2;
		for (j = 0; j < 3; j++) bc[j] = (b[j] + c[j]) / 2;
		divide_triangle(a, ab, ac, k - 1, p(globalTime)); 
		divide_triangle(c, ac, bc, k - 1, p(globalTime - 66)); 
		divide_triangle(b, bc, ab, k - 1, p(globalTime + 66)); 
	}
	else {
		draw_triangle(a, b, c, colorkey);
	}
}

void timer(int id) {
	if (globalTime % 100 == 0) globalTime = 1;
	float r = p(globalTime);
	globalTime += 1;
	glRotatef(1, 0.0, 0.0, 1.0);
	glutPostRedisplay();
	glutTimerFunc(33, timer, 1);
}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_TRIANGLES);

	divide_triangle(vertices[0], vertices[1], vertices[2], 5, 0);

	glEnd();
	glFlush();
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	srand(time(NULL));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(RenderScene);
	timer(1);
	glutMainLoop();

	return 0;
}