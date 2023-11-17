#include "HalfEdgeDataStructure.h"
#include "EulerOperation.h"
#include "Sweep.h"
#include <GL/glut.h>

#include <vector>

using namespace std;

Loop* Sweep::opposite_loop(Loop* loop) {
	HalfEdge* he = loop->ledge;// 获取环的第一条半边
	Edge* edg = he->edg;// 获取半边所属的边
	return edg->he1 == he ? edg->he2->wloop : edg->he1->wloop;// 返回相邻环，根据半边所属的边判断
}

Solid* Sweep::sweep(Face* bottom_face, double x, double y, double z) {
	Solid* solid = bottom_face->fsolid;// 获取底部面所属的固体
	vector<Loop*> loops = bottom_face->floops;// 获取底部面的所有环
	Face* up_face = NULL;// 初始化上部面为空

	for (int i = 0; i < loops.size(); i++) {// 遍历底部面的所有环
		Loop* loop = opposite_loop(loops[i]);// 获取当前环的相邻环
		Face* face = loop->lface;// 获取相邻环对应的面
		HalfEdge* he = loop->ledge;// 获取相邻环的第一条半边
		Vertex* src = he->startv;// 获取相邻环的第一个顶点
		Vertex* srcup = new Vertex(src->x + x, src->y + y, src->z + z);// 创建上部面对应的新顶点
		EulerOperation::mev(src, srcup, loop);// 在相邻环中插入新顶点
		Vertex* prvup = srcup;// 初始化上一个顶点为新顶点
		he = he->nxt;// 移动到下一条半边
		Vertex* vtx = he->startv;// 获取相邻环的下一个顶点
		while (vtx != src) {// 遍历相邻环的所有顶点
			Vertex* up = new Vertex(vtx->x + x, vtx->y + y, vtx->z + z);// 创建上部面对应的新顶点
			EulerOperation::mev(vtx, up, loop);// 在相邻环中插入新顶点
			EulerOperation::mef(up, prvup, face);// 在相邻面中插入边，连接新顶点和上一个顶点
			he = he->nxt;// 移动到下一条半边
			vtx = he->startv;// 获取相邻环的下一个顶点
			prvup = up;// 更新上一个顶点为当前新顶点
		}
		EulerOperation::mef(srcup, prvup, face);// 在相邻面中插入边，连接最后一个新顶点和第一个新顶点
		if (i != 0) {// 如果不是第一个环，合并上部面和相邻面
			EulerOperation::kfmrh(up_face, face);
		} else {
			up_face = face;// 如果是第一个环，更新上部面为当前相邻面
		}
	}
	
	return solid;// 返回生成的固体
}
