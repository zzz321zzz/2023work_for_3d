#pragma once
#include <string>
#include <vector>
#include <GL/glut.h>
#include <iterator>
#include <stdlib.h> // 包含 srand 和 rand 函数
#include <time.h>	// 包含 time 函数
using namespace std;

struct Vertex;
struct HalfEdge;
struct Edge;
struct Face;
struct Loop;
struct Solid;

struct Vertex
{								 //定义顶点
	double x;					 //顶点x
	double y;					 //顶点y
	double z;					 //顶点z
	vector<HalfEdge *> start_he; //多个starthe组成的数组
	vector<HalfEdge *> end_he;	 //多个end组成的数组

	Vertex *prevv; //前一个点
	Vertex *nextv; //后一个点

	Vertex() {}

	Vertex(double x, double y, double z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	bool operator==(const Vertex &v)
	{
		return (this->x == v.x && this->y == v.y && this->z == v.z);
	}

	bool operator!=(const Vertex &v)
	{
		return !(this->x == v.x && this->y == v.y && this->z == v.z);
	}
};

struct HalfEdge
{					//定义半边
	Vertex *startv; //半边起始点
	Vertex *endv;	//半边终点
	Loop *wloop;	//半边对应的环
	Edge *edg;		//半边对应的边

	HalfEdge *prv; //前一个半边
	HalfEdge *nxt; //后一个半边

	HalfEdge() {}

	HalfEdge(Vertex *startv, Vertex *endv, Edge *edg)
	{
		this->startv = startv;
		this->endv = endv;
		this->edg = edg;
	}
};

struct Edge
{				   //定义边
	HalfEdge *he1; //每条边由两个半边组成
	HalfEdge *he2;

	Edge *preve; //前一个边
	Edge *nexte; //后一个边
};

struct Loop
{					 //定义环
	Face *lface;	 //环对应的面
	HalfEdge *ledge; //环对应的半边

	Loop *nextl; //下一个环
	Loop *prevl; //上一个环
};

struct Face
{						   //定义面
	vector<Loop *> floops; //面由多个环组成
	Solid *fsolid;		   //面对应的体

	Face *prevf; //前一个面
	Face *nextf; //后一个面
};

struct Solid
{						   //定义体
	vector<Face *> sfaces; //体由多个面组成
	Vertex *sorigin;	   //面的起始点

	Solid *prevs; //前一个体
	Solid *nexts; //后一个体

	void wirepaint(Solid *solid)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(1.0, 0.0, 0.0);				   // 将线框颜色设置为白色
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 设置多边形模式为线框模式

		// 遍历每个面
		for (Face *face : solid->sfaces)
		{
			// 遍历每个环
			for (Loop *loop : face->floops)
			{
				// 遍历每个半边
				HalfEdge *he = loop->ledge;
				do
				{
					glBegin(GL_LINES);
					glVertex3d(he->startv->x, he->startv->y, he->startv->z);
					glVertex3d(he->endv->x, he->endv->y, he->endv->z);
					glEnd();

					he = he->nxt;
				} while (he != loop->ledge);
			}
		}

		glFlush();
	}

	void solidpaint(Solid *solid)
{
    glClear(GL_COLOR_BUFFER_BIT);

    // 绘制线框
    glColor3f(0.0, 0.0, 0.0);                   // 将线框颜色设置为红色
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 将多边形模式设置为线框模式

    for (Face *face : solid->sfaces)
    {
        for (Loop *loop : face->floops)
        {
            HalfEdge *he = loop->ledge;
            glBegin(GL_LINES);
            do
            {
                glVertex3d(he->startv->x, he->startv->y, he->startv->z);
                glVertex3d(he->endv->x, he->endv->y, he->endv->z);
                he = he->nxt;
            } while (he != loop->ledge);
            glEnd();
        }
    }

    // 绘制实体多边形
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    srand(time(NULL));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLfloat colors[][3] = {{1.0, 0.0, 0.0},{0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 1.0, 1.0}, {1.0, 0.0, 1.0}, {1.0, 1.0, 0.0},{1.0, 1.0, 1.0},{0.0, 0.0, 0.0}};
    int Coptions = 0;

    for (Face *face : solid->sfaces)
    {
        bool firstLoopColored = false; // 表示是否为该面的第一个环设置了颜色

        for (Loop *loop : face->floops)
        {
            glBegin(GL_POLYGON);

            if (!firstLoopColored)
            {
                Coptions = Coptions % 8;
                float a = colors[Coptions][0];
                float b = colors[Coptions][1];
                float c = colors[Coptions][2];
                glColor4f(a, b, c, 0.9f);
                firstLoopColored = true;
            }
            else
            {
                // 使用透明颜色
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f); 
            }

            HalfEdge *he = loop->ledge;
            do
            {
                glVertex3d(he->startv->x, he->startv->y, he->startv->z);
                he = he->nxt;
            } while (he != loop->ledge);

            glEnd();
        }

        Coptions++;
    }

    glFlush();
}

};