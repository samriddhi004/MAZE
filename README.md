# Maze Solver Using A* Algorithm

This project implements a maze solver using the A* algorithm with visualization using SFML (Simple and Fast Multimedia Library).
It generates a maze using DFS and then finds the optimal path from the start to the end of the maze using the A* algorithm.

# Controls

  Press M to generate a new maze.
  Press S to solve the current maze.
  Close the window to exit the program.

## Dependencies

- SFML: The project uses SFML for graphics and window management. Make sure to have SFML installed before running the program.

## Installation
1. Install SFML library if not already installed. You can install SFML using the package manager of your operating system or by downloading the SFML binaries from the official website.
  For example, on Debian-based Linux distributions, you can use the following command to install SFML:

```bash
    sudo apt-get install libsfml-dev
```
  
2. Clone the repository to your local machine:

```bash
   git clone https://github.com/samriddhi004/MAZE.git
```


## How to Run

  Navigate to the project directory. Compile the source code using a C++ compiler. Make sure to link SFML libraries:

```bash
    g++ -o maze_gen_and_solver.cpp -lsfml-graphics -lsfml-window -lsfml-system
```

  Run the executable:

```bash
    ./prog
```

