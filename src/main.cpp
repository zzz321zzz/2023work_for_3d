#include <iostream>
#include <fstream>

#include "HalfEdgeDataStructure.h"
#include "EulerOperation.h"
#include "Sweep.h"

#include <GL/glut.h>


#define PI 3.1415926536

using namespace std;

double xrot = 0.0;
double yrot = 0.0;
double xrotr = 0.0;
double yrotr = 0.0;
double sweep_x, sweep_y, sweep_z;
Solid* solid;
bool displaymode = 1;
int xp, yp;
bool bdown = false;


Face* build_face() {
	fstream in;// 打开文件
	in.open("D:\\3dCAD\\zyj3dCAD\\src\\ZYJ.txt");
	int loop_cnt;
	in >> loop_cnt;// 读取环的数量
	int vtx_cnt;
	in >> vtx_cnt;// 读取顶点的数量
	double x, y, z;
	in >> x >> y >> z;// 读取第一个顶点的坐标
	Solid* solid = EulerOperation::mvsf(x, y, z);// 创建一个固体，并在其中创建一个面和第一个环
	Face* face = solid->sfaces[0];
	Loop* loop = face->floops[0];
	Vertex* prvv = solid->sorigin;
	
	for (int i = 1; i < vtx_cnt; i++) {// 逐个读取顶点，使用欧拉操作创建半边和新顶点，构建第一个环
		in >> x >> y >> z;
		Vertex* newv = new Vertex(x, y, z);
		EulerOperation::mev(prvv, newv, loop);
		prvv = newv;
	}
	EulerOperation::mef(prvv, solid->sorigin, face);// 使用欧拉操作创建最后一条边，连接最后一个顶点和起始顶点，形成环
    
	for (int i = 1; i < loop_cnt; i++) {// 逐个读取环，使用欧拉操作创建边和新顶点，构建面
		in >> vtx_cnt;// 读取环中顶点的数量
		prvv = solid->sorigin;
		in >> x >> y >> z;// 读取新的起始顶点的坐标
		Vertex* newv = new Vertex(x, y, z);
		Edge* edge = EulerOperation::mev(prvv, newv, loop);// 创建一条新的边，并在其中插入新顶点
		prvv = edge->he1->endv;
		for (int j = 1; j < vtx_cnt; j++) {// 逐个读取环中的顶点，使用欧拉操作创建边和新顶点
			in >> x >> y >> z;
			Vertex* newv = new Vertex(x, y, z);
			EulerOperation::mev(prvv, newv, loop);
			prvv = newv;
		}
		EulerOperation::mef(prvv, edge->he1->endv, face);// 使用欧拉操作创建最后一条边，连接最后一个顶点和起始顶点，形成新的环
		EulerOperation::kemr(edge);// 删除创建的边，以完成环的构建
	}

	in >> sweep_x >> sweep_y >> sweep_z;// 读取拉伸方向的坐标

	in.close();// 关闭文件
	return face;// 返回构建的面
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);	
	glShadeModel(GL_FLAT);	
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);	
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);
}



void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -20.0);
    glRotatef(xrot + xrotr, 1.0, 0.0, 0.0);
    glRotatef(yrot + yrotr, 0.0, 1.0, 0.0);
	glScalef(0.3, 0.3, 0.3);  // 缩小物体的大小
    Solid* temp=solid;
    // 在这里调用你的绘制函数，根据需要修改
    if (!displaymode)
        solid->wirepaint(temp);  // 用你的 wireframe 绘制函数替换
    else
        solid->solidpaint(temp);  // 用你的实体绘制函数替换
    
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);             // 设置视口位置和大小，通常与窗口大小一致
    glMatrixMode(GL_PROJECTION);        // 设置当前矩阵为投影矩阵
    glLoadIdentity();                   // 将当前矩阵重置为单位矩阵

    gluPerspective(45, (double)w / (double)h, 0.1, 100.0);
    // 设置透视投影，参数依次为视场角度、宽高比、近裁剪面和远裁剪面

    glMatrixMode(GL_MODELVIEW);         // 设置当前矩阵为模型视图矩阵
    glLoadIdentity();                   // 将当前矩阵重置为单位矩阵
}

void mouse(int button, int state, int x, int y)
{
	// 如果鼠标左键被按下
	if (button == GLUT_LEFT_BUTTON)
	{
        // 如果左键状态为按下
		if (state == GLUT_DOWN)
		{
            // 设置全局变量 bdown 为 true，表示鼠标左键正在被按下
			bdown = true;
            // 记录当前鼠标位置到全局变量 xp 和 yp
			xp = x;
			yp = y;
		}
		// 如果左键状态为释放
		else
		{
            // 设置全局变量 bdown 为 false，表示鼠标左键已释放
			bdown = false;
            // 更新全局旋转角度 xrot 和 yrot，将之前的旋转角度 xrotr 和 yrotr 加到全局旋转角度上
			xrot += xrotr;
			yrot += yrotr;
		}
	}
}

void move(int x, int y)
{
	int deltx, delty;
	if (bdown)
	{
		deltx = x - xp;
		delty = y - yp;
		yrotr = ((double)deltx / 10.0 * 180.0 / PI * 0.04);
		xrotr = ((double)delty / 10.0 * 180.0 / PI * 0.04);
	}
	glutPostRedisplay();
}

void processMenuEvent(int option)
{
	switch (option)
	{
	case 1:
		displaymode = false;
		glutPostRedisplay();
		break;
	case 2:
		displaymode = true;
		glutPostRedisplay();
		break;
	}
}

void glCreateMenu(void)
{
	int menu;
	menu = glutCreateMenu(processMenuEvent);
	glutAddMenuEntry("wire cube", 1);
	glutAddMenuEntry("solid cube", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(1000, 750);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Solid Viewer");

    init();

	Face* face = build_face();// 创建一个面
	solid = Sweep::sweep(face, sweep_x, sweep_y, sweep_z);// 使用 Sweep 类的 sweep 方法进行拉伸，生成一个固体
	//你需要在这里接着开始进行着色并显示

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(move);
	glCreateMenu();
    glutMainLoop();

    return 0;
}