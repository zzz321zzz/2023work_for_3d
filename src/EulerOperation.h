#pragma once

#include "HalfEdgeDataStructure.h"
#include <GL/glut.h>
using namespace std;

class EulerOperation {

private:

public:
	static Solid* mvsf(double x, double y, double z);//创建一个具有给定坐标的实体（可能是立方体、球体等）并返回指向该实体的指针。
	static Edge* mev(Vertex* startv, Vertex* newv, Loop* loop);//在给定环路中创建一个新顶点，并将其与起始顶点通过一条边连接起来。返回指向该边的指针。
	static Face* mef(Vertex* startv, Vertex* endv, Face* face);//在给定面上创建一条连接起始顶点和结束顶点的边，并返回指向面的指针。
	static Loop* kemr(Edge* edge);//从给定边开始，移除与之相关的环路，并返回指向新环路（如果有的话）的指针。
	static void kfmrh(Face* bigf, Face* smallf);//将一个小面（smallf）合并到一个大面（bigf）中。
};
