#include "values.h"

#include <iostream>
#include <raylib.h>
#include <vector>
#include <stack> // For the Recursive Backtracker algorithm
#include <random> // For random number generation


using namespace std;

class Cell {
public:
    int x,y;
    bool isWall,isVisited;

    Cell(int xCoord = 0, int yCoord = 0, bool wall = true, bool visited = false) : x(xCoord), y(yCoord), isWall(wall), isVisited(visited) {} 

    void render() const {
        Color color = isWall ? BLACK : WHITE;  // Black for walls, white for paths
        DrawRectangle(x , y, CELL_SIZE, CELL_SIZE, color);
    }
};

class Maze {
public:
    vector<vector<Cell>> maze;
    int mazeWidth = SCREEN_WIDTH;
    int mazeHeight = SCREEN_HEIGHT;

    Maze() : mazeWidth(SCREEN_WIDTH / CELL_SIZE), mazeHeight(SCREEN_HEIGHT / CELL_SIZE){ 
        maze.resize(mazeHeight); 

        for (int i = 0; i < mazeHeight; ++i) {
            maze[i].reserve(mazeWidth); 
            for (int j = 0; j < mazeWidth; ++j) {
                maze[i].push_back(Cell(j * CELL_SIZE, i * CELL_SIZE));
            }
        }
    }

    void Generate() {
        std::random_device rd;
        std::mt19937 gen(rd());  // Standard Mersenne Twister engine for better randomness
        std::uniform_int_distribution<> distrib(0, 3); // Distribution for directions

        std::stack<std::pair<int, int>> stack;

        // 1. Start at a random cell
        int startX = 1; // gen() % width;
        int startY = 1; // gen() % height;

        stack.push({startX, startY});
        maze[startY][startX].isWall = false;
        maze[startY][startX].isVisited = true;

        while (!stack.empty()) {
            int x = stack.top().first;
            int y = stack.top().second;
            stack.pop();

            // 2. Get unvisited neighbors
            std::vector<std::pair<int, int>> neighbors;
            int dx[] = {0, 0, 2, -2}; // Possible x movements
            int dy[] = {2, -2, 0, 0}; // Possible y movements

            for (int i = 0; i < 4; ++i) {
                int nx = x + dx[i];
                int ny = y + dy[i];

                if (nx > 0 && nx < mazeWidth - 1 && ny > 0 && ny < mazeHeight - 1 && !maze[ny][nx].isVisited) {
                    neighbors.push_back({nx, ny});
                }
            }

            // 3. If there are unvisited neighbors, choose one randomly
            if (!neighbors.empty()) {
                stack.push({x, y}); // Push current cell back onto the stack

                int randomIndex = distrib(gen) % neighbors.size() ;
                int nx = neighbors[randomIndex].first;
                int ny = neighbors[randomIndex].second;

                // 4. Remove wall between current cell and chosen neighbor
                maze[(y + ny) / 2][(x + nx) / 2].isWall = false;


                maze[ny][nx].isWall = false;
                maze[ny][nx].isVisited = true;

                
                stack.push({nx, ny});
            }
        }

    }

    void Render() const {
        for (const auto& row : maze) {
            for (const auto& cell : row) {
                cell.render();
            }
        }
    }
};

int main() {
    //Basic Window Setup
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE); 
    SetTargetFPS(60); 


    Maze maze;
    maze.Generate();
    // Main game loop
    while (!WindowShouldClose()){
        BeginDrawing();
            ClearBackground(RAYWHITE);

            maze.Render();
        EndDrawing();
    }

    CloseWindow();  

    return 0;
}