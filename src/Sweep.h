#pragma once
#include "HalfEdgeDataStructure.h"
using namespace std;

class Sweep {
private:
	static Loop* opposite_loop(Loop* loop);

public:
	static Solid* sweep(Face* face, double x, double y, double z);
};
