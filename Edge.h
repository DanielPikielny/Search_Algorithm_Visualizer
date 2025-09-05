#pragma once
#include "Node.h"
class Node;
class Edge
{
private:
	Node* pn1;
	Node* pn2;
	double cost;
public:
	Edge(Node* p1, Node* p2, double c);
	void Draw();
	Node* GetNode1() { return pn1; }
	Node* GetNode2() { return pn2; }
	double GetCost() { return cost; }
};

