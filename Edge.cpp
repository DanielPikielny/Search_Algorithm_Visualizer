#include "Edge.h"
#include "glut.h"
Edge::Edge(Node* p1, Node* p2, double c)
{
	pn1 = p1;
	pn2 = p2;
	cost = c;
}

void Edge::Draw()
{
	glColor3d(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(pn1->GetX(), pn1->GetY());
	glVertex2d(pn2->GetX(), pn2->GetY());

	glEnd();
}
