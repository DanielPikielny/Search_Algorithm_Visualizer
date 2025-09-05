#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include "Cell.h"
#include "Node.h"
#include "Definitions.h"
#include "CompareNodes.h"
#include <queue>
#include <iostream>
#include <vector>

using namespace std;

const int MSZ = 100;

const double MAX_G = 10000;

const int NUM_NODES = 300;
const int NUM_EDGES = 900;

Node* nodes[NUM_NODES];
Edge* edges[NUM_EDGES];

int maze[MSZ][MSZ] = { 0 };
bool runBFS = false;
bool runDFS = false;
bool runBiBFS = false;
bool runUCS = false;
bool runIDS = false;
queue<Cell*> grays;
vector<Cell*> graysStack;
queue<Cell*> graysStart;
queue<Cell*> graysTarget;
priority_queue<Node*, vector<Node*>, CompareNodes> pq;
// for IDS
int currentDepthLimit = 0;
int maxDepthLimit = MSZ * 2;
bool depthLimitReached = false;
bool solutionFound = false;
vector<Cell*> idsStack;

void InitMaze()
{
	int i, j;
	// set frame of WALLS
	for (i = 0; i < MSZ; i++)
	{
		maze[0][i] = WALL;  // first line
		maze[MSZ - 1][i] = WALL; // last line
		maze[i][0] = WALL; // first column
		maze[i][MSZ - 1] = WALL; // last collumn
	}

	for (i = 1; i < MSZ - 1; i++)
		for (j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 1) // odd line => motly spaces
			{
				if (rand() % 10 < 2) // 20% are WALLS
					maze[i][j] = WALL;
				else // 80% are spaces
					maze[i][j] = SPACE;
			}
			else // even lines are mostly WALLs
			{
				if (rand() % 10 < 6) // 60%
					maze[i][j] = WALL;
				else
					maze[i][j] = SPACE;
			}
		}
	maze[MSZ / 2][MSZ / 2] = START;
	Cell* targetCell = new Cell(rand() % MSZ, rand() % MSZ, nullptr);
	maze[targetCell->getRow()][targetCell->getCol()] = TARGET;

	//save the START cell in grays (the queue that was defined at the top)
	Cell* pc = new Cell(MSZ / 2, MSZ / 2, nullptr);
	grays.push(pc);//queue for BFS
	graysStack.push_back(pc);//stack for DFS
	//Cell* startCell = new Cell(MSZ / 2, MSZ / 2, nullptr);
	graysStart.push(pc);
	graysTarget.push(targetCell);

	currentDepthLimit = 0;
	depthLimitReached = false;
	solutionFound = false;
	idsStack.clear();
	idsStack.push_back(pc);
}

double Distance(Node* pn1, Node* pn2) {
	double x1, y1, x2, y2;
	x1 = pn1->GetX();
	y1 = pn1->GetY();
	x2 = pn2->GetX();
	y2 = pn2->GetY();
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void InitGraph()
{
	int i, index1, index2;
	double x, y;
	double distance;


	for (i = 0; i < NUM_NODES; i++)
	{
		x = rand() % MSZ;
		y = rand() % MSZ;
		nodes[i] = new Node(x, y, WHITE, MAX_G, nullptr);
	}

	// now we can define edges
	for (i = 0; i < NUM_EDGES; i += 2)
	{
		do {
			index1 = rand() % NUM_NODES;
			index2 = rand() % NUM_NODES;
			distance = Distance(nodes[index1], nodes[index2]);
		} while (distance > 15);
		edges[i] = new Edge(nodes[index1], nodes[index2], distance);
		nodes[index1]->AddEdge(edges[i]);
		edges[i + 1] = new Edge(nodes[index2], nodes[index1], distance); // edge in opposite direction
		nodes[index2]->AddEdge(edges[i + 1]);

	}

	// pick random Node as START and another Node as TARGET
	nodes[0]->SetColor(START);
	pq.push(nodes[0]); // prepare pq for UCS
	nodes[1]->SetColor(WHITE_TARGET);

}

void init()
{
	srand(time(0));

	glClearColor(0.5, 0.5, 0.5, 0);// color of window background
	glOrtho(0, MSZ, 0, MSZ, -1, 1); // set the coordinates system

	InitGraph();
	InitMaze();
}

void DrawMaze()
{
	int i, j;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			// set color
			switch (maze[i][j])
			{
			case WALL:
				glColor3d(0.4, 0, 0); // dark red
				break;
			case SPACE:
				glColor3d(1, 1, 1); // white
				break;
			case START:
				glColor3d(0.2, 0.5, 0.5); // light blue
				break;
			case TARGET:
				glColor3d(1, 0, 0); // red
				break;
			case BLACK:
				glColor3d(0.5, 0.5, 0.5); // black
				break;
			case GRAY:
				glColor3d(0, 1, 0); // green
				break;
			case GRAY_TARGET:
				glColor3d(0, 0.2, 1); // deep blue
				break;
			case PATH:
				glColor3d(1, 0.5, 1); // magenta
				break;
			}
			// show cell (i,j)
			glBegin(GL_POLYGON);
			glVertex2d(j, i);
			glVertex2d(j, i + 1);
			glVertex2d(j + 1, i + 1);
			glVertex2d(j + 1, i);
			glEnd();
		}
}

void RestorePath(Cell* pc) {
	while (pc->getParent() != nullptr) {
		maze[pc->getRow()][pc->getCol()] = PATH;
		pc = pc->getParent();
	}
}

bool CheckNeighbour(int row, int col, Cell* pCurrent, bool isBFS) {
	if (maze[row][col] == TARGET) {
		RestorePath(pCurrent);
		if (isBFS)
			runBFS = false;
		else
			runDFS = false;
		return false;
	}
	else {// the neighbour is WHITE so add it to grays
		Cell* pn = new Cell(row, col, pCurrent);
		if (isBFS)
			grays.push(pn);
		else
			graysStack.push_back(pn);
		maze[row][col] = GRAY;
		return true;
	}
}

bool CheckNeighbourIDS(int row, int col, Cell* pCurrent, int depth) {
	if (maze[row][col] == TARGET) {
		RestorePath(pCurrent);
		runIDS = false;
		solutionFound = true;
		return false;
	}
	if (maze[row][col] == SPACE) {
		if (depth < currentDepthLimit) {
			Cell* pn = new Cell(row, col, pCurrent);
			idsStack.push_back(pn);
			maze[row][col] = GRAY;
		}
		else {
			depthLimitReached = true;
		}
	}
	return true;
}

bool CheckNeighbourBiBFS(int row, int col, Cell* pCurrent, queue<Cell*>& myQueue, queue<Cell*>& otherQueue, bool isStartSearch)
{
	// Don't process walls
	if (maze[row][col] == WALL) return true;

	// Found the other search front - this is where we need to connect the paths
	if ((isStartSearch && (maze[row][col] == GRAY_TARGET || maze[row][col] == TARGET)) ||
		(!isStartSearch && (maze[row][col] == GRAY_START || maze[row][col] == START))) {

		// First, find the cell from the other search front
		Cell* otherCell = nullptr;
		queue<Cell*> tempQueue = otherQueue;

		while (!tempQueue.empty()) {
			Cell* current = tempQueue.front();
			tempQueue.pop();

			if (current->getRow() == row && current->getCol() == col) {
				otherCell = current;
				break;
			}
		}

		// If we're at the start search meeting the target search
		if (isStartSearch) {
			// Restore path from current (start side) to start point
			RestorePath(pCurrent);

			// If we found the matching cell from target side
			if (otherCell) {
				// Restore path from the other cell to target
				RestorePath(otherCell);
			}
			// If we directly found TARGET
			else if (maze[row][col] == TARGET) {
				// Mark the connection point
				maze[row][col] = PATH;
			}
		}
		// If we're at the target search meeting the start search
		else {
			// Restore path from current (target side) to target point
			RestorePath(pCurrent);

			// If we found the matching cell from start side
			if (otherCell) {
				// Restore path from the other cell to start
				RestorePath(otherCell);
			}
			// If we directly found START
			else if (maze[row][col] == START) {
				// Mark the connection point
				maze[row][col] = PATH;
			}
		}

		runBiBFS = false;
		return false;  // Stop the search
	}

	// Normal cell processing
	if (maze[row][col] == SPACE) {
		// Mark cells differently depending on search direction
		maze[row][col] = isStartSearch ? GRAY_START : GRAY_TARGET;

		Cell* pn = new Cell(row, col, pCurrent);
		myQueue.push(pn);
	}

	return true;
}

void RunBFSIteration() {
	Cell* pCurrent;
	int row, col;
	bool goOn = true;

	if (grays.empty()) {
		cout << "Grays are empty: There is no solution\n";
		runBFS = false;
		return;
	}
	else {
		pCurrent = grays.front();
		grays.pop();
		maze[pCurrent->getRow()][pCurrent->getCol()] = BLACK;
		//check the neighbours of pCurrent
		row = pCurrent->getRow();
		col = pCurrent->getCol();
		//try going up
		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == TARGET) {
			goOn = CheckNeighbour(row + 1, col, pCurrent, true);
		}
		//try going down
		if (goOn && (maze[row - 1][col] == SPACE || maze[row - 1][col] == TARGET)) {
			goOn = CheckNeighbour(row - 1, col, pCurrent, true);
		}
		//try going right
		if (goOn && (maze[row][col + 1] == SPACE || maze[row][col + 1] == TARGET)) {
			goOn = CheckNeighbour(row, col + 1, pCurrent, true);
		}
		//try going left
		if (goOn && (maze[row][col - 1] == SPACE || maze[row][col - 1] == TARGET)) {
			goOn = CheckNeighbour(row, col - 1, pCurrent, true);
		}
	}
}

void RunDFSIteration() {
	Cell* pCurrent;
	int row, col;
	bool goOn = true;

	if (graysStack.empty()) {
		cout << "Grays stack is empty: There is no solution\n";
		runDFS = false;
		return;
	}
	else {
		pCurrent = graysStack.back();//pick the last element 
		graysStack.pop_back();//remove the last element from the stack

		row = pCurrent->getRow();
		col = pCurrent->getCol();
		if (maze[row][col] != START) {
			maze[row][col] = BLACK;
		}

		int directions[4] = { -1, -1, -1, -1 };
		int index;
		for (int dir = 0; dir < 4; dir++) {
			do {
				index = rand() % 4;

			} while (directions[index] != -1);
			directions[index] = dir;
		}

		for (index = 0; goOn && index < 4; index++) {
			switch (directions[index])
			{
			case 0://up
				if (maze[row + 1][col] == SPACE || maze[row + 1][col] == TARGET)
					goOn = CheckNeighbour(row + 1, col, pCurrent, false);
				break;
			case 1://down
				if (maze[row - 1][col] == SPACE || maze[row - 1][col] == TARGET)
					goOn = CheckNeighbour(row - 1, col, pCurrent, false);
				break;
			case 2://right
				if (maze[row][col + 1] == SPACE || maze[row][col + 1] == TARGET)
					goOn = CheckNeighbour(row, col + 1, pCurrent, false);
				break;
			case 3://right
				if (maze[row][col - 1] == SPACE || maze[row][col - 1] == TARGET)
					goOn = CheckNeighbour(row, col - 1, pCurrent, false);
				break;
			}
		}


	}
}

void RunBiBFSIteration()
{
	if (graysStart.empty() || graysTarget.empty()) {
		cout << "No solution found!\n";
		runBiBFS = false;
		return;
	}

	// Process one step from the start side
	Cell* pCurrentStart = graysStart.front();
	graysStart.pop();

	// Don't mark START as BLACK
	if (maze[pCurrentStart->getRow()][pCurrentStart->getCol()] != START) {
		maze[pCurrentStart->getRow()][pCurrentStart->getCol()] = BLACK;
	}

	// Process one step from the target side
	Cell* pCurrentTarget = graysTarget.front();
	graysTarget.pop();

	// Don't mark TARGET as BLACK
	if (maze[pCurrentTarget->getRow()][pCurrentTarget->getCol()] != TARGET) {
		maze[pCurrentTarget->getRow()][pCurrentTarget->getCol()] = BLACK;
	}

	int directions[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
	bool goOn = true;

	// Expand start search
	for (int i = 0; i < 4 && goOn; i++) {
		int newRow = pCurrentStart->getRow() + directions[i][0];
		int newCol = pCurrentStart->getCol() + directions[i][1];

		// Check if in bounds
		if (newRow >= 0 && newRow < MSZ && newCol >= 0 && newCol < MSZ) {
			goOn = CheckNeighbourBiBFS(newRow, newCol, pCurrentStart, graysStart, graysTarget, true);
		}
	}

	// If we've already found a path, don't continue
	if (!runBiBFS) return;

	goOn = true;
	// Expand target search
	for (int i = 0; i < 4 && goOn; i++) {
		int newRow = pCurrentTarget->getRow() + directions[i][0];
		int newCol = pCurrentTarget->getCol() + directions[i][1];

		// Check if in bounds
		if (newRow >= 0 && newRow < MSZ && newCol >= 0 && newCol < MSZ) {
			goOn = CheckNeighbourBiBFS(newRow, newCol, pCurrentTarget, graysTarget, graysStart, false);
		}
	}
}

void RunIDSIteration() {
	// Check if we've exceeded the maximum depth limit
	if (currentDepthLimit > maxDepthLimit) {
		cout << "IDS reached maximum depth limit. No solution found.\n";
		runIDS = false;
		return;
	}

	// Check if the stack is empty
	if (idsStack.empty()) {
		if (solutionFound) {
			// Solution was found at current depth limit
			cout << "Solution found at depth " << currentDepthLimit << endl;
			runIDS = false;
			return;
		}

		if (depthLimitReached) {
			// Need to increase depth limit and restart
			cout << "Increasing depth limit to " << currentDepthLimit + 1 << endl;
			currentDepthLimit++;
			depthLimitReached = false;

			// Reset maze (but preserve walls, start, and target)
			for (int i = 0; i < MSZ; i++) {
				for (int j = 0; j < MSZ; j++) {
					if (maze[i][j] == BLACK || maze[i][j] == GRAY) {
						maze[i][j] = SPACE;
					}
				}
			}

			// Clear stack and add start node
			idsStack.clear();
			idsStack.push_back(new Cell(MSZ / 2, MSZ / 2, nullptr));
		}
		else {
			// Empty stack without reaching depth limit or finding solution means no solution exists
			cout << "IDS completed all paths without finding solution.\n";
			runIDS = false;
		}
		return;
	}

	// Get current cell from stack
	Cell* pCurrent = idsStack.back();
	idsStack.pop_back();

	int row = pCurrent->getRow();
	int col = pCurrent->getCol();

	// Calculate current depth by counting steps from start
	int depth = 0;
	Cell* temp = pCurrent;
	while (temp->getParent() != nullptr) {
		depth++;
		temp = temp->getParent();
	}

	// Mark cell as visited (except START)
	if (maze[row][col] != START) {
		maze[row][col] = BLACK;
	}

	// If we've reached the target, stop
	if (maze[row][col] == TARGET) {
		RestorePath(pCurrent);
		runIDS = false;
		solutionFound = true;
		return;
	}

	// Only expand neighbors if we haven't reached depth limit
	if (depth < currentDepthLimit) {
		bool goOn = true;

		// Direction vectors: up, down, right, left
		int directions[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

		// Randomize direction order
		int dirOrder[4] = { -1, -1, -1, -1 };
		for (int dir = 0; dir < 4; dir++) {
			int index;
			do {
				index = rand() % 4;
			} while (dirOrder[index] != -1);
			dirOrder[index] = dir;
		}

		// Try each direction
		for (int i = 0; i < 4 && goOn; i++) {
			int dirIndex = dirOrder[i];
			int newRow = row + directions[dirIndex][0];
			int newCol = col + directions[dirIndex][1];

			// Check bounds
			if (newRow >= 0 && newRow < MSZ && newCol >= 0 && newCol < MSZ) {
				// Check if it's a space or target
				if (maze[newRow][newCol] == SPACE || maze[newRow][newCol] == TARGET) {
					goOn = CheckNeighbourIDS(newRow, newCol, pCurrent, depth + 1);
				}
			}
		}
	}
	else {
		// Mark that we reached the depth limit
		depthLimitReached = true;
	}
}

void UpdatePQ(Node* pNeighbour, double newG, Node* pCurrent, priority_queue<Node*, vector<Node*>, CompareNodes>& pq) {
	vector<Node*> tmp;
	pNeighbour->setG(newG);
	pNeighbour->SetParent(pCurrent);
	while (!pq.empty() && pq.top() != pNeighbour) {
		tmp.push_back(pq.top());
		pq.pop();
	}
	if (pq.empty()) {
		cout << "Internal error: we didn't find pNeighbour in the priority queue";
		exit(2);
	}
	//remove top and insert pNeighbour and all the nodes from tmp
	pq.pop();
	pq.push(pNeighbour);
	while (!tmp.empty()) {
		pq.push(tmp.back());
		tmp.pop_back();
	}
}

void RestorePathInGraph(Node* pCurrent) {
	while (pCurrent->GetParent() != nullptr) {
		pCurrent->SetColor(PATH);
		pCurrent = pCurrent->GetParent();
	}
}

void runUCSIteration() {
	Node* pCurrent;
	Node* pNeighbour;
	double newG;
	if (pq.empty()) {
		cout << "PQ is empty: there is no solution\n";
		runUCS = false;
		return;
	}
	else {
		pCurrent = pq.top();
		//check for success
		if (pCurrent->GetColor() == GRAY_TARGET) {
			cout << "The target has been found\n";
			runUCS = false;
			RestorePathInGraph(pCurrent);
			return;
		}
		else {
			//remove pCurrent from pq and paint it black (if it isn't START) and check its neighbours
			pq.pop();
			if (pCurrent->GetColor() != START) pCurrent->SetColor(BLACK);
			for (auto it : pCurrent->GetOutgoing()) {//defines iterator according to what you get back from outgoing
				pNeighbour = it->GetNode2();
				newG = pCurrent->getG() + it->GetCost();
				switch (pNeighbour->GetColor()) {
				case WHITE:// add pNeighbour to pq = paint gray
					pNeighbour->SetColor(GRAY);
					pNeighbour->setG(newG);
					pNeighbour->SetParent(pCurrent);
					pq.push(pNeighbour);
					break;
				case GRAY:
				case GRAY_TARGET:
					if (newG < pNeighbour->getG()) {
						UpdatePQ(pNeighbour, newG, pCurrent, pq);
					}
					break;
				case WHITE_TARGET:
					pNeighbour->SetColor(GRAY_TARGET);
					pNeighbour->setG(newG);
					pNeighbour->SetParent(pCurrent);
					pq.push(pNeighbour);
					break;
				}
			}
		}
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer
	DrawMaze();

	glutSwapBuffers(); // show all
}

void DrawGraph() {
	int i;
	for (i = 0; i < NUM_EDGES; i++) {
		edges[i]->Draw();
	}
	for (i = 0; i < NUM_NODES; i++) {
		nodes[i]->Draw();
	}
}

void displayGraph()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer
	DrawGraph();

	glutSwapBuffers(); // show all
}

void displayStart()
{
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 0.9f, 0.0f);  // White color for title
	const char* title = "Maze Search Algorithms";
	glRasterPos2f(40, 85);  // Adjusted position to fit screen
	for (int i = 0; i < strlen(title); ++i)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, title[i]);
	glColor3f(1.0f, 0.9f, 0.0f);
	const char* message = "Right-click to open the menu";
	glRasterPos2f(35, 75); // Adjusted position
	for (int i = 0; i < strlen(message); ++i)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);

	// Algorithm descriptions
	glColor3f(1.0f, 0.9f, 0.0f); 
	const char* algorithms[] = {
		"BFS - Breadth-First Search: Finds shortest path",
		"DFS - Depth-First Search: Explores as far as possible",
		"BiBFS - Bidirectional BFS: Searches from both ends",
		"IDS - Iterative Deepening Search: Gradually increases depth"
	};

	float startY = 60; // Adjusted spacing
	for (int i = 0; i < 4; ++i) {
		glRasterPos2f(25, startY);
		for (int j = 0; j < strlen(algorithms[i]); ++j)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, algorithms[i][j]);
		startY -= 7;  // Space out lines for better visibility
	}

	// Reset background color for later scenes
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

	glutSwapBuffers();
}

void idle()
{
	if (runBFS) {
		RunBFSIteration();
	}
	if (runDFS) {
		RunDFSIteration();
	}
	if (runBiBFS) {
		RunBiBFSIteration();
	}
	if (runUCS) {
		//@@@@@@@@@@@@@@@@@@@@@@@@@@@@2@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	}
	if (runIDS) {
		RunIDSIteration();
	}
	glutPostRedisplay();
}

void menu(int option) {
	switch (option)
	{
	case 1://BFS
		glutDisplayFunc(display);
		runBFS = true;
		break;
	case 2://DFS
		glutDisplayFunc(display);
		runDFS = true;
		break;
	case 3://Bidirectional BFS
		glutDisplayFunc(display);
		runBiBFS = true;
		break;
	case 4://UCS
		glutDisplayFunc(displayGraph);
		runUCS = true;
		break;
	case 5:
		glutDisplayFunc(display);
		runIDS = true;
	}
}

void keyboard(unsigned char key, int x, int y) {
	if (runUCS && key == ' ')
		runUCSIteration();
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("First Example");

	glutDisplayFunc(displayStart);//defines the function "display" as drawing function

	glutIdleFunc(idle);//defines the function "idle" as background function
	glutKeyboardFunc(keyboard);
	glutCreateMenu(menu);//create menu
	glutAddMenuEntry("BFS", 1);
	glutAddMenuEntry("DFS", 2);
	glutAddMenuEntry("Bidirectional BFS", 3);
	glutAddMenuEntry("Uniform Cost Search", 4);
	glutAddMenuEntry("IDS", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	init();

	glutMainLoop();
}