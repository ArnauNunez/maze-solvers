# Escapa — Maze Solving Using Multiple Algorithms

**Author:** Arnau Núñez Martínez
**Date:** 25/04/2023  

This project implements a C program capable of finding the exit of any maze using several classical search algorithms. The program reads a maze from a text file and applies the selected algorithm to locate the exit, optionally displaying additional information about the search process.

---

## Features

The program includes four different search algorithms:

| Algorithm | Description |
|----------|-------------|
| borratxo | Random walk. Moves randomly until it finds the exit or reaches a maximum number of iterations. |
| backtrack | Recursive exhaustive search with backtracking. Guarantees a solution if one exists. |
| bfs | Breadth-First Search. Finds the shortest path in terms of number of steps. |
| A* | Informed search using the Manhattan heuristic. Prioritizes nodes with the lowest estimated cost. |

---

## Project Structure

escapa.c          — Main source code  
labyrinth.txt     — Example maze file (or any other maze file)  
README.md         — This document  

---

## Maze Format

The maze is read from a plain text file using ASCII characters.

- ' ' (space) represents an open path  
- Walls are represented by characters such as '#'  
- The starting position is marked with 'X'  
- The exit is any open space located on the boundary of the maze  

---

## Compilation and Execution

Compile the program with:

gcc escapa.c -o escapa

Run it with:

./escapa <algorithm> <maze_file> [option]

### Available Algorithms

- borratxo
- backtrack
- bfs
- A*

### Optional Flags

- -imprimeixlaberint — Prints the maze before and after the search (not valid for borratxo).  
- -casellesexplorades — Marks all visited cells during the search.  
- -info — Displays additional information (steps, iterations, path length, etc.).  

## Example Mazes

This repository includes seven example maze files:

- laberint1.txt  
- laberint2.txt  
- laberint3.txt  
- laberint4.txt  
- laberint5.txt  
- laberint6.txt  
- laberint7.txt  

You can test the program with any of them, for example:

./escapa bfs laberint3.txt -info

## Algorithm Overview

### Borratxo (Random Walk)
- Moves randomly at each step  
- Does not guarantee finding the exit  
- Demonstrates uninformed search  

### Backtracking
- Recursive depth-first exploration  
- Explores all possible paths  
- Guarantees a solution if one exists  

### BFS
- Breadth-first search  
- Always finds the shortest path  
- Uses a custom queue implementation  

### A*
- Informed search algorithm  
- Uses Manhattan distance as heuristic  
- Expands nodes based on f(n) = g(n) + h(n)  

---

## Implementation Details

The project includes:

- Dynamic reading and allocation of the maze (reservalaberint)  
- Conversion from ASCII maze to internal coordinates  
- Visited-cell matrix  
- Custom queue for BFS and A*  
- Priority insertion for A*  
- Path reconstruction using predecessor matrices  

---

## Errors and Exit Codes

The program detects:

- Invalid parameters  
- Invalid algorithm name  
- Missing or unreadable maze file  
- Mazes with no exit  

Return values:

- 0 → success (exit found)  
- 1 → error or no exit  

---

## License

All Rights Reserved. This project is publicly visible but may not be copied, modified, or redistributed.

