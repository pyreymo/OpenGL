#include "pch.h"
#include <math.h>
#include <GL/freeglut.h>


GLfloat Vertices[][3] = {
	0.5,0.5,0.5,
	-0.5,0.5,0.5,
	-0.5,-0.5,0.5,
	0.5,-0.5,0.5,
	0.5,0.5,-0.5,
	-0.5,0.5,-0.5,
	-0.5,-0.5,-0.5,
	0.5,-0.5,-0.5
};
//矩形的顶点坐标
GLfloat Vertices2[][3] = {
	0.502, 0.502, 0.502,
	-0.502, 0.502, 0.502,
	-0.502, -0.502, 0.502,
	0.502, -0.502, 0.502,
	0.502, 0.502, -0.502,
	-0.502, 0.502, -0.502,
	-0.502, -0.502, -0.502,
	0.502, -0.502, -0.502
};
//描边顶点坐标
GLint index_list[][4] = {
	1, 2, 3, 4,
	1, 2, 6, 5,
	2, 6, 7, 3,
	3, 4, 8, 7,
	1, 4, 8, 5,
	5, 6, 7, 8
};
//矩形顶点的绘制顺序
GLint index_list2[][2] = {
	1, 2,
	2, 3,
	3, 4,
	4, 1,
	5, 6,
	6, 7,
	7, 8,
	8, 5,
	1, 5,
	2, 6,
	3, 7,
	4, 8
};
//矩形顶点的绘制顺序


void init(void) {
	glClearColor(0.2, 0.2, 0.2, 0.0);
	glShadeModel(GL_SMOOTH);								//平滑
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION); 
	glEnable(GL_DEPTH_TEST);
}


void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GLUT_MULTISAMPLE);							//抗锯齿
	glDepthMask(GL_TRUE);

	int i, j;

	//glPushMatrix();

	glBegin(GL_QUADS);
	for (i = 0; i < 6; ++i)
		for (j = 0; j < 4; ++j) {
			glColor3f(fabs(sin(j)), fabs(sin(j-1.0)), fabs(sin(j-2.0)));
			glVertex3fv(Vertices[index_list[i][j] - 1]);
		}

	glEnd();

	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	for (i = 0; i < 12; ++i)
		for (j = 0; j < 2; ++j)
			glVertex3fv(Vertices[index_list2[i][j] - 1]);
	glEnd();
	//glPopMatrix();
	glFlush();
	glutSwapBuffers();
}

void Reshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1.0*w / h, 1, 1000);
	gluLookAt(1, 1, 2, 0, 0, 0, 0, 1, 0);
}



int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 30);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();

	return 0;
}