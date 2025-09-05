#pragma once
#include <vector>
#include "Edge.h"
using namespace std;
class Edge;
class Node
{
private:
	double x, y;
	double g; //sum of the costs along the shortest path from the start Node
	int color;
	Node* parent;
	vector<Edge*> outgoing;

public:
	Node();
	Node(double xx, double yy, int c, double gg, Node* p);
	void Draw();
	void SetColor(int c) { color = c; }
	int GetColor() { return color; }
	double GetX() { return x; }
	double GetY() { return y; }
	void setG(double value) { g = value; }
	double getG() { return g; }
	void SetParent(Node* p) { parent = p; }
	Node* GetParent() { return parent; }
	void AddEdge(Edge* p) { outgoing.push_back(p); }
	vector<Edge*> GetOutgoing() { return outgoing; }
};

