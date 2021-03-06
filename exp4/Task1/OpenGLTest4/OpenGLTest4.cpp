#include "pch.h"
#include <stdlib.h>
#include <windows.h>
#include <GL/freeglut.h>

constexpr auto MAX_CHAR = 128;
enum displayMode { C_BEGIN, C_COLOR, C_SIZE, C_TYPE, C_DRAW };	//显示模式的id序列
GLint DISPLAY_MODE = C_BEGIN;
const GLint LINETYPE = 6;										//共有六种默认线形
GLint lineType[8] = {
	1,						//线形编号
	4,						//线形重复次数
	0b1111111111111111 ,	//几种默认的线形
	0b0010011111111111 ,
	0b1010101010101010 ,
	0b0111100001111000 ,
	0b0100010001000100 ,
	0b0010011100100111
};
GLint win_width = 0, win_height = 0;
GLdouble linePos[100][4];										//最多存100条线，表示为(x1,y1,x2,y2)四元组
GLint clickCnt = 0;												//画橡皮条时鼠标点击的次数
GLfloat penWidth = 1.0;											//线宽
GLubyte* penColorp = (GLubyte*)malloc(3);						//线颜色
GLubyte* colorBufferp = (GLubyte*)malloc(3);					//预览颜色的缓存
GLint x_color = 0, y_color = 0;									//鼠标点击选择颜色时的位置坐标
GLint x_now = 0, y_now = 0;										//当前鼠标所指的位置
GLdouble x_adpt = 0.0, y_adpt = 0.0;							//转换成openGL坐标之后的鼠标位置


void init() {
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glEnable(GL_LINE_STIPPLE);									//允许设置线型
	glColor3ubv(penColorp);										//设置默认颜色
	glLineStipple(4, 0b1111111111111111);						//设置默认线形
}

//在窗口上显示字符
void glPrint(const char* str)
{
	int len, i;
	wchar_t* wstring;
	HDC hDC = wglGetCurrentDC();
	GLuint list = glGenLists(1);

	// 计算字符的个数
	len = 0;
	for (i = 0; str[i] != '\0'; ++i)
	{
		if (IsDBCSLeadByte(str[i]))
			++i;
		++len;
	}

	// 将混合字符转化为宽字符
	wstring = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
	if (wstring != NULL) {
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstring, len);
		wstring[len] = L'\0';

		for (i = 0; i < len; ++i)
		{
			wglUseFontBitmapsW(hDC, wstring[i], 1, list);
			glCallList(list);
		}

		//垃圾回收
		free(wstring);
		glDeleteLists(list, 1);
	}
}

//设置显示字符的属性
void glSetFont(int size, int charset, const char* face) {
	//调用WindowsAPI的部分我还云里雾里
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}

//对应菜单的Color选项
void changeColor() {
	glColor3d(0.0, 0.0, 0.0);
	glSetFont(28, ANSI_CHARSET, "Microsoft YaHei");
	glRasterPos2d(-0.3, -0.9);
	glPrint("点选颜色自动保存");

	//画出色块三角形
	GLdouble cR = 1.0;
	glBegin(GL_POLYGON);
	glColor3d(1.0, 0.0, 0.0);
	glVertex3d(-0.8660254038 * cR, -0.75 * cR, 0.0);
	glColor3d(0.0, 1.0, 0.0);
	glVertex3d(0.8660254038 * cR, -0.75 * cR, 0.0);
	glColor3d(0.0, 0.0, 1.0);
	glVertex3d(0, 0.75 * cR, 0.0);
	glEnd();

	//读取鼠标所指处的颜色作为预览颜色
	glReadPixels(x_now, win_height - y_now, 1, 1,
		GL_RGB, GL_UNSIGNED_BYTE, colorBufferp);

	//画出颜色预览矩形
	glBegin(GL_POLYGON);
	glColor3ubv(colorBufferp);
	glVertex3d(0.7, 0.7, 0.0);
	glVertex3d(0.6, 0.7, 0.0);
	glVertex3d(0.6, 0.6, 0.0);
	glVertex3d(0.7, 0.6, 0.0);
	glEnd();

	glFlush();

	//读取点击的点的颜色作为实际画线的颜色
	glReadPixels(x_color, win_height - y_color, 1, 1,
		GL_RGB, GL_UNSIGNED_BYTE, penColorp);

}

//对应菜单的Size选项
void changeSize() {
	glColor3d(0.0, 0.0, 0.0);
	glRasterPos2d(-0.4, 0.2);
	glSetFont(35, ANSI_CHARSET, "Microsoft YaHei");
	glPrint("上下方向键调整线宽");

	//检测颜色是否被选择过，缺省为红色
	glLineWidth(penWidth);
	x_color ? glColor3ubv(penColorp) :
		glColor3d(1.0, 0.0, 0.0);

	glBegin(GL_LINES);
	glVertex3d(-0.8, 0.0, 0.0);
	glVertex3d(0.8, 0.0, 0.0);
	glEnd();
	glFlush();
}

//对应菜单的Type选项
void changeType() {
	glColor3d(0.0, 0.0, 0.0);
	glSetFont(35, ANSI_CHARSET, "Microsoft YaHei");
	glRasterPos2d(-0.4, 0.4);
	glPrint("上下方向键调整线形");
	glRasterPos2d(-0.4, 0.2);
	glPrint("左右方向键调整疏密");

	//设置线形
	glLineStipple(1.0 * lineType[1], lineType[lineType[0] % LINETYPE + 1]);
	x_color ? glColor3ubv(penColorp) : glColor3d(1.0, 0.0, 0.0);

	//画一条预览线
	glBegin(GL_LINES);
	glVertex3d(-0.8, 0.0, 0.0);
	glVertex3d(0.8, 0.0, 0.0);
	glEnd();
	glFlush();
}

//开始界面
void defaultDisplay() {
	glColor3d(0.0, 0.0, 0.0);
	glRasterPos2d(-0.3, 0.3);
	glSetFont(45, ANSI_CHARSET, "Microsoft YaHei");
	glPrint("橡皮条程序");
	glRasterPos2d(-0.35, 0.1);
	glPrint("右键设置属性");
	glRasterPos2d(-0.52, -0.1);
	glPrint("全局按空格开始画线");
	glRasterPos2d(-0.52, -0.3);
	glPrint("全局按回车返回本屏");
}

//把屏幕像素坐标映射到openGL的内部坐标，并把结果储存在全局变量x_adpt,y_adpt中
//因为没有考虑要复用这个函数，所以把存储的变量名直接写进去了
void CoorAdapt(GLint x, GLint y) {
	x_adpt = (2.0 * x / win_width - 1.0) * (1.0 * win_width / win_height);
	y_adpt = 2.0 * (win_height - 1.0 * y) / win_height - 1.0;
}

//画橡皮条
void drawScence() {
	x_color ? glColor3ubv(penColorp) : glColor3d(1.0, 0.0, 0.0);
	CoorAdapt(x_now, y_now);//坐标转换

	if (clickCnt) {
		if (clickCnt % 2) {
			//点击了奇数次时，有若干条线已经画完，新线起点已定，终点时刻跟随鼠标
			glBegin(GL_LINES);
			for (int i = 0; i < (clickCnt - 1) / 2; i++) {
				glVertex3d(linePos[i][0], linePos[i][1], 0.0);
				glVertex3d(linePos[i][2], linePos[i][3], 0.0);
			}
			glVertex3d(linePos[(clickCnt - 1) / 2][0],
				linePos[(clickCnt - 1) / 2][1], 0.0);//新线的起点
			glVertex3d(x_adpt, y_adpt, 0.0);//终点跟随鼠标
			glEnd();
			glFlush();
		}
		else {
			//偶数次计数，只需要画出存储的若干对
			glBegin(GL_LINES);
			for (int i = 0; i < clickCnt / 2; i++) {
				glVertex3d(linePos[i][0], linePos[i][1], 0.0);
				glVertex3d(linePos[i][2], linePos[i][3], 0.0);
			}
			glEnd();
			glFlush();
		}
	}
}

//显示回调函数
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	switch (DISPLAY_MODE) {
	case C_COLOR:
		changeColor();
		break;
	case C_SIZE:
		changeSize();
		break;
	case C_TYPE:
		changeType();
		break;
	case C_DRAW:
		drawScence();
		break;
	default:
		defaultDisplay();
		break;
	}

	glutPostRedisplay();
	glutSwapBuffers();
}

void reshape(int w, int h) {
	win_width = w;
	win_height = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1.0 * w / h, 1.73205084, 1000);
	gluLookAt(0.0, 0.0, 1.73205085, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void keybroad(unsigned char key, int x, int y) {
	switch (key) {
	case 27:										//ESC
		exit(0);
		break;
	case 13:										//回车返回主屏
		DISPLAY_MODE = C_BEGIN;
		break;
	case 32:										//空格开始画线
		DISPLAY_MODE = C_DRAW;
		break;
	case 'c':
		if (DISPLAY_MODE == C_DRAW)
			clickCnt = 0;							//画线时按c清屏(但是没有删除数据，只会重新写入)
	}
}

void special(int key, int x, int y) {
	if (DISPLAY_MODE == C_SIZE) {
		switch (key) {
		case GLUT_KEY_UP:
			if (penWidth < 10)penWidth += 1;
			break;
		case GLUT_KEY_DOWN:
			if (penWidth > 1) penWidth -= 1;
			break;

		}
	}
	if (DISPLAY_MODE == C_TYPE) {
		switch (key) {
		case GLUT_KEY_UP:
			lineType[0]++;
			break;
		case GLUT_KEY_DOWN:
			lineType[0] ? lineType[0]-- : lineType[0] = LINETYPE + 2;
			break;
		case GLUT_KEY_LEFT:
			if (lineType[1]) lineType[1]--;
			break;
		case GLUT_KEY_RIGHT:
			if (lineType[1] < 128) lineType[1]++;
			break;

		}
	}
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (DISPLAY_MODE == C_COLOR) {
			//当正在选择颜色时，保存鼠标点击的位置
			x_color = x;
			y_color = y;
		}
		if (DISPLAY_MODE == C_DRAW) {
			//当画橡皮条时，存储线的始末位置到相应的linePos数组位置
			CoorAdapt(x, y);
			if (!clickCnt) {
				linePos[0][0] = x_adpt;
				linePos[0][1] = y_adpt;
			}
			else if (clickCnt % 2) {
				linePos[(clickCnt - 1) / 2][2] = x_adpt;
				linePos[(clickCnt - 1) / 2][3] = y_adpt;
			}
			else {
				linePos[clickCnt / 2][0] = x_adpt;
				linePos[clickCnt / 2][1] = y_adpt;
			}
			clickCnt++;
		}
	}
}

void passiveMotion(int x, int y) {
	//时刻存储鼠标位置
	x_now = x;
	y_now = y;
}

void processMenuEvents(int id) {
	//当点击了菜单中的选项时，用选项的id决定display回调时使用哪个对应的函数重新绘制
	DISPLAY_MODE = id;
}

int createMenu(void) {
	int id = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Color", C_COLOR);
	glutAddMenuEntry("Size", C_SIZE);
	glutAddMenuEntry("Type", C_TYPE);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	return id;
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(800, 600);
	glutInitWindowPosition(200, 80);
	glutCreateWindow(argv[0]);

	init();
	createMenu();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keybroad);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(passiveMotion);

	glutMainLoop();

	return 0;
}
