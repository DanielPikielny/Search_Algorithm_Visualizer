#include "Node.h"
#include "glut.h"
#include "Definitions.h"
Node::Node() {

}
Node::Node(double xx, double yy, int c, double gg, Node* p) {
	x = xx;
	y = yy;
	color = c;
	g = gg;
	parent = p;
}

void Node::Draw()
{
	switch (color)
	{
	case WHITE:
		glColor3d(1, 1, 1); // white
		break;
	case START:
		glColor3d(0, 0, 1); // blue
		break;
	case GRAY_TARGET:
		glColor3d(1, 1, 0); // yellow
		break;
	case WHITE_TARGET:
		glColor3d(1, 0, 0); // red
		break;
	case GRAY:
		glColor3d(0, 1, 0); // green
		break;
	case BLACK:
		glColor3d(0, 0, 0); // black
		break;
	case PATH:
		glColor3d(1, 0, 1); // magenta
		break;

	}

	glBegin(GL_POLYGON);
	glVertex2d(x, y + 0.5);
	glVertex2d(x + 0.5, y);
	glVertex2d(x, y - 0.5);
	glVertex2d(x - 0.5, y);
	glEnd();

	glColor3d(0, 0, 0); // black
	glBegin(GL_LINE_LOOP);
	glVertex2d(x, y + 0.5);
	glVertex2d(x + 0.5, y);
	glVertex2d(x, y - 0.5);
	glVertex2d(x - 0.5, y);
	glEnd();
}
