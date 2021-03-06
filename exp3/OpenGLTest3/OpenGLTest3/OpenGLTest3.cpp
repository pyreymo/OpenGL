#include "pch.h"
#include <GL/freeglut.h>
#include <math.h>
#include <stdlib.h>
#include "glDraw.h"		

GLfloat rotate_angle_xz = 0.0;										//xz面旋转角度参数
GLfloat rotate_angle_y = 0.0;										//y向旋转角度参数
GLfloat rotate_angle = 0.0;											//R旋转角度
GLint isRotate = 0;

void init(void) {
	glClearColor(0.8, 0.8, 0.8, 0.0);
	glColor3f(1.0, 1.0, 1.0);
	glShadeModel(GL_SMOOTH);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, -1.0, 100.0);
	//gluLookAt(-0.5, 1.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);		//看圆角度
	//gluLookAt(0.8, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);		//单独看四面体角度
	gluLookAt(0.0, 4.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);			//看茶壶角度

	glMatrixMode(GL_PROJECTION);

	GLfloat m_position[] = { -10.0f, 10.0f, 10.0f, 1.0f };				//点光源位置
	GLfloat m_ambient[] = { 1.0f, 0.0f, 0.2f, 1.0f };				//环境光
	GLfloat m_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };				//扩散
	GLfloat m_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };				//反射

	glLightfv(GL_LIGHT0, GL_POSITION, m_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, m_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, m_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, m_specular);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
}


void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glutSolidTeapot(3.0);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); 
	glTranslated(-3.5, 0.0, 0.0);
	drawTetrahedron(1);												//画细分1次的四面体
	glTranslated(1.8, 0.0, 0.0);
	drawTetrahedron(2);												//画细分2次的四面体
	glTranslated(1.8, 0.0, 0.0);
	drawTetrahedron(3);												//画细分3次的四面体
	glTranslated(1.8, 0.0, 0.0);
	drawTetrahedron(4);												//画细分4次的四面体
	glTranslated(1.8, 0.0, 0.0);
	drawTetrahedron(5);												//画细分5次的四面体
	glTranslated(-3.0, 0.0, 3.0);
	drawBall(1,10);													//画半径为1.0，分块数10的球
	glPopMatrix();
	
	glFlush();
	glutSwapBuffers();
}

void Reshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1.0*w / h, 1, 1000);
}

void Idle() {
	if (isRotate) {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//gluLookAt(3.0*sin(rotate_angle_xz), 2.5, 3.0*cos(2 * rotate_angle_xz),
		//	0.0, 0.0, 0.0, 0.0, 1.0, 0.0);							//转换视角(第一种方式)

		glRotatef(1.0, 0.0, 1.0, 0.0);
		//rotate_angle += 1;										//转动速度

		glMatrixMode(GL_PROJECTION);
		glutPostRedisplay();
	}
}

void KeyboardFunc(unsigned char key, int x, int y) {
	if (key == 'p')						//暂停
		if (isRotate) isRotate = 0;
	if (key == 'r')						//旋转
		isRotate ? isRotate = 0 : isRotate = 1;
	if (key == 27)						//退出
		exit(0);
}

void SpecialFunc(int specialkey, int x, int y) {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	switch (specialkey) {
	case GLUT_KEY_LEFT:
		rotate_angle_xz += 0.05;
		break;
	case GLUT_KEY_RIGHT:
		rotate_angle_xz -= 0.05;
		break;
	case GLUT_KEY_UP:
		rotate_angle_y -= 0.05;
		break;
	case GLUT_KEY_DOWN:
		rotate_angle_y += 0.05;
		break;
	}

	gluLookAt(10.0*sin(rotate_angle_xz), 4.0*cos(rotate_angle_y), 10.0*cos(rotate_angle_xz),
		0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 30);
	glutCreateWindow(argv[0]);
	init();
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialFunc);
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();

	return 0;
}