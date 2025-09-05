# Search_Algorithm_Visualizer
This project is a C++ application that visualizes several classic search algorithms on both a maze and a graph. The program uses the OpenGL Utility Toolkit (GLUT) to provide a graphical user interface where users can select and observe the execution of different pathfinding algorithms.

The program visualizes the following search algorithms:

    Breadth-First Search (BFS): An algorithm for finding the shortest path in an unweighted graph or maze.

    Depth-First Search (DFS): An algorithm that explores as far as possible along each branch before backtracking.

    Bidirectional BFS (Bi-BFS): A search algorithm that simultaneously searches from the start node and the target node, meeting in the middle.

    Iterative Deepening Search (IDS): A search strategy that performs a series of depth-limited searches with increasing depth limits.

    Uniform Cost Search (UCS): An algorithm that finds the least-cost path from a single source node to a target node in a weighted graph.

Prerequisites

To compile and run this program, you will need:

    A C++ compiler (e.g., g++).

    The GLUT library for OpenGL.

    The project's header files: Cell.h, Node.h, Definitions.h, and CompareNodes.h.

Compilation

Since the project uses GLUT, you'll need to link the appropriate libraries during compilation. Here is an example of a typical compilation command using g++ on a Linux-based system:
Bash

g++ main.cpp -o search_visualizer -lglut -lGL -lGLU -lm

On other systems, the command may vary depending on how GLUT is installed.

How to Use

    Compile the main.cpp file as shown above.

    Run the executable from your terminal:
    Bash

    ./search_visualizer

    A window titled "First Example" will appear, displaying a welcome message.

    Right-click anywhere in the window to open a menu.

    Select a search algorithm from the menu.

        BFS, DFS, Bi-BFS, and IDS will run automatically on a dynamically generated maze.

        Uniform Cost Search (UCS) will run on a dynamically generated graph. To advance the UCS algorithm one step at a time, press the spacebar on your keyboard.

Code Overview

The main.cpp file contains the core logic for the application:

    main(): Initializes the GLUT window and sets up the display and idle functions.

    init(): Initializes both the maze and the graph data structures, including setting up the start and target points.

    display() and DrawMaze(): Functions for rendering the maze visualization.

    displayGraph() and DrawGraph(): Functions for rendering the graph visualization.

    idle(): A callback function that runs the next iteration of the selected search algorithm.

    Run*Iteration() functions (RunBFSIteration, RunDFSIteration, etc.): Implement the core logic for each search algorithm, exploring the maze or graph one step at a time.

    menu() and keyboard(): Handle user input for selecting algorithms and controlling the UCS process.

