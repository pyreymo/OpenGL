#include "pch.h"
#include "ogldev_math_3d.h"
#pragma comment(lib, "glew32.lib")
#include <iostream>
#include <GL/glew.h>
#define NODEBUG
#include <GL\freeglut.h>
#include <GLFW/glfw3.h>


const GLfloat PI = 3.1415926536f;

GLint times = 1;

GLint fps = 100;							//FPS
GLint mspf = 1000 / fps;				//速度调控
GLfloat frame_total = (GLfloat)1.3 * fps;
GLfloat frame_now = 0.0f;

GLint polygon_points = 50;
GLfloat Ypos = 0.0f;

void init() {
	glClearColor(0.0, 0.0, 0.0, 0.0);	//背景色
	glShadeModel(GL_SMOOTH);			//多变性填充模式
}


void timer_func(int id) {
	frame_total == ceil(frame_total / times);
	Ypos = 4 * (frame_now / frame_total)*(1 - frame_now / frame_total) / pow(times, 1.5);	//运动控制方程
	if (fabs(frame_now - frame_total) < 1e-4) {
		frame_now = 0;
		times > 5 ? times += 4 : times++;	//修正
		if (mspf > 2)mspf = mspf / pow(times, 0.2);
	}
	else { frame_now++; }

	glutPostRedisplay();
	glutTimerFunc(mspf, timer_func, 1);
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0f, 0.0f, 0.4f);

	GLint i = 0;
	GLfloat r = 0.1;
	glBegin(GL_POLYGON);
	while (i < polygon_points) {
		glVertex3f(r*cos(2 * PI / polygon_points * i),
			1.5*Ypos + r - 1 + r * sin(2 * PI / polygon_points * i), 0.0);	//运动显示
		i++;
	}
	glEnd();
	glFlush();


	glutSwapBuffers();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("OpenGL Test 1");
	init();
	glutDisplayFunc(display);
	glutTimerFunc(mspf, timer_func, 1);
	glFrustum(-1.0, 1.0, -1.0, 1.0, 0.0, 1.0);
	glutMainLoop();
	return 0;
}
