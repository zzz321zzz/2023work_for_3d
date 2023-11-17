#include <iostream>
#include <vector>
#include "HalfEdgeDataStructure.h"
#include "EulerOperation.h"
#include <GL/glut.h>
using namespace std;

Solid* EulerOperation::mvsf(double x, double y, double z) {//给定初始坐标创建面
	Solid* solid = new Solid();//创建一个新的体
	Face* face = new Face();//创建一个新的面
	Loop* loop = new Loop();//创建一个新的环
	Vertex* vertex = new Vertex(x, y, z);//创建一个新的点

	solid->sfaces.push_back(face);//体包含多个面，将创建的新的面压入数组中
	solid->sorigin = vertex;//体的起始点确定
	face->fsolid = solid;//面对应的体确定
	face->floops.push_back(loop);//面包含多个环，将创建的新的环压入数组中
	loop->lface = face;//环对应的面确定

	return solid;//返回指向体的指针
}

Edge* EulerOperation::mev(Vertex* startv, Vertex* newv, Loop* loop) {//在给定的环中创建一个新的顶点并将顶点并入环内
	Edge* edge = new Edge();//创建一个新的边
	HalfEdge* he1 = new HalfEdge(startv, newv, edge);//创建两个新的半边
	HalfEdge* he2 = new HalfEdge(newv, startv, edge);
	Solid* solid = loop->lface->fsolid;//环对应的面对应的体

	edge->he1 = he1;//使边与半边 产生联系
	edge->he2 = he2;

	if (loop->ledge == NULL) {//如果环对应的边不存在，即第一次操作空环
		he1->nxt = he2;//两个半边形成环
		he2->nxt = he1;
		he1->prv = he2;
		he2->prv = he1;
		loop->ledge = he1;//环对应的半边定义为第一个半边
	} else {
		HalfEdge* tmphe = loop->ledge;//环对应的半边做临时变量
		while (tmphe->endv != startv) tmphe = tmphe->nxt;//while循环，在环对应半边的终点没到起始点时反复，即找到最后一条边
		he1->nxt = he2;//he1和he2产生联系
		he2->prv = he1;
		he2->nxt = tmphe->nxt;//最后一条边的下一个半边赋值给he2的下一个半边
		tmphe->nxt->prv = he2;//最后一条边的下一个边的上一个半边为he2
		tmphe->nxt = he1;//he1为最后一条边的下一个半边
		he1->prv = tmphe;//形成环
	}
	he1->wloop = loop;//半边对应的环为环
	he2->wloop = loop;

	return edge;//返回边
}

Face* EulerOperation::mef(Vertex* startv, Vertex* endv, Face* face) {//在给定面上插入一条新的边，形成一个新的面
	Face* newface = new Face();// 创建新的面
	Loop* newloop = new Loop();// 创建新的环
	Loop* loop = face->floops[0];// 获取旧面的第一个环
	Solid* solid = face->fsolid;// 获取旧面所属的固体
	Edge* edge = new Edge();// 创建新的边
	HalfEdge* he1 = new HalfEdge(startv, endv, edge);// 创建两个半边
	HalfEdge* he2 = new HalfEdge(endv, startv, edge);// 创建两个半边
	edge->he1 = he1;// 设置边的两个半边
	edge->he2 = he2;
	solid->sfaces.push_back(newface);// 将新面加入到固体的面列表中
	newface->fsolid = solid;
	newface->floops.push_back(newloop);// 将新环加入到新面的环列表中
	newloop->lface = newface;

	HalfEdge* tmphe = loop->ledge;
	HalfEdge* tmphe1;
	HalfEdge* tmphe2;
	HalfEdge* tmphe3;
	while (tmphe->endv != startv) tmphe = tmphe->nxt;// 在旧面的环列表中找到包含起始顶点的半边
	tmphe1 = tmphe;
	while (tmphe->endv != endv) tmphe = tmphe->nxt;// 在找到的半边之后找到包含结束顶点的半边
	tmphe2 = tmphe;
	// 找到结束顶点相邻的半边
	tmphe3 = tmphe->nxt;
	while (tmphe3->endv != endv) tmphe3 = tmphe3->nxt;
	bool flag = false;// 判断是否为一个环
	if (tmphe2 == tmphe3)
		flag = true;
	// 将he1插入到tmphe2之后，将he2插入到tmphe1之后
	he1->nxt = tmphe2->nxt;
	tmphe2->nxt->prv = he1;
	he2->nxt = tmphe1->nxt;
	tmphe1->nxt->prv = he2;
	tmphe1->nxt = he1;
	he1->prv = tmphe1;
	tmphe2->nxt = he2;
	he2->prv = tmphe2;
	// 根据flag决定新环的起始半边
	if (flag) {
		newloop->ledge = he1;
		he1->wloop = newloop;
		loop->ledge = he2;
		he2->wloop = loop;
	} else {
		newloop->ledge = he2;
		he2->wloop = newloop;
		loop->ledge = he1;
		he1->wloop = loop;
	}
	// 将新环中的半边与新环建立关联
	for (HalfEdge* he = he1->nxt; he != he1; he = he->nxt) {
		he->wloop = he1->wloop;
	}
	for (HalfEdge* he = he2->nxt; he != he2; he = he->nxt) {
		he->wloop = he2->wloop;
	}
	return newface;
}

Loop* EulerOperation::kemr(Edge* edge) {//删除一条边，并将其连接的两个环合并成一个环
	HalfEdge* he1 = edge->he1;// 获取边的两个半边
	HalfEdge* he2 = edge->he2;
	Loop* newloop = new Loop();// 创建一个新的环
	Loop* loop = he1->wloop;// 获取边所在环
	Face* face = loop->lface;// 获取边所在环对应的面

	newloop->ledge = he1->nxt;// 将新环的起始半边设置为边1的下一条半边
	loop->ledge = he1->prv;// 将旧环的起始半边设置为边1的上一条半边
	face->floops.push_back(newloop);// 将新环加入到面的环列表中

	he1->prv->nxt = he2->nxt;// 调整相邻半边的关系，删除边、半边
	he2->nxt->prv = he1->prv;
	he2->prv->nxt = he1->nxt;
	he1->nxt->prv = he2->prv;
	delete he1;// 释放内存
	delete he2;
	delete edge;

	return newloop;// 返回新创建的环
}

void EulerOperation::kfmrh(Face* bigf, Face* smallf) {
	Loop* small_loop = smallf->floops[0];// 获取小面的第一个环
	bigf->floops.push_back(small_loop);// 将小面的环加入到大面的环列表中
	vector<Face*>::iterator it = bigf->fsolid->sfaces.begin();// 在大面所属的固体的面列表中查找小面并删除
	while (it != bigf->fsolid->sfaces.end()) {
		if (*it == smallf) {
			bigf->fsolid->sfaces.erase(it);
			break;
		}
		it++;
	}
	delete smallf;// 释放小面的内存
}
