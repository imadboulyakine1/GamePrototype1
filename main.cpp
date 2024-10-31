#include "values.h"

#include <iostream>
#include <raylib.h>
#include <vector>

#include <random>
#include <algorithm>

using namespace std;

class Cell
{
public:
    int x, y;
    bool isWall, isVisited;

    Cell(int xCoord = 0, int yCoord = 0, bool wall = true, bool visited = false) : x(xCoord), y(yCoord), isWall(wall), isVisited(visited) {}

    void render() const
    {
        Color color = isWall ? BLACK : WHITE; // Black for walls, white for paths
        DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, color);
    }
};

class Maze
{
public:
    vector<vector<Cell>> maze;
    int mazeWidth = SCREEN_WIDTH;
    int mazeHeight = SCREEN_HEIGHT;

    Maze() : mazeWidth(SCREEN_WIDTH / CELL_SIZE), mazeHeight(SCREEN_HEIGHT / CELL_SIZE)
    {
        maze.resize(mazeHeight);

        for (int i = 0; i < mazeHeight; ++i)
        {
            maze[i].reserve(mazeWidth);
            for (int j = 0; j < mazeWidth; ++j)
            {
                maze[i].push_back(Cell(j * CELL_SIZE, i * CELL_SIZE));
            }
        }
    }

    int getRandomIndex(int size)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        uniform_int_distribution<> distrib(0, size - 1);
        return distrib(gen);
    }

    void Generate(bool visualize = false)
    {

        int startX = 1;
        int startY = 1;

        if (startX >= mazeWidth || startY >= mazeHeight)
            return;

        maze[startY][startX].isVisited = true;
        maze[startY][startX].isWall = false;

        std::vector<std::pair<int, int>> frontiers;

        auto add_frontier = [&](int x, int y)
        {
            if (x >= 1 && x < mazeWidth - 1 && y >= 1 && y < mazeHeight - 1 && maze[y][x].isWall)
            {
                frontiers.push_back({x, y});
            }
        };

        add_frontier(startX + 1, startY);add_frontier(startX - 1, startY);
        add_frontier(startX, startY + 1);add_frontier(startX, startY - 1);

        while (!frontiers.empty())
        {
            int randomIndex = getRandomIndex(frontiers.size());
            std::pair<int, int> wall = frontiers[randomIndex];
            frontiers.erase(frontiers.begin() + randomIndex);

            int x = wall.first;
            int y = wall.second;

            int visitedNeighbors = 0;
            int nx = -1, ny = -1;

            auto check_neighbor = [&](int cx, int cy)
            {
                if (cx >= 0 && cx < mazeWidth && cy >= 0 && cy < mazeHeight && maze[cy][cx].isVisited && !maze[cy][cx].isWall)
                {

                    visitedNeighbors++;
                    nx = cx;
                    ny = cy;
                }
            };

            check_neighbor(x + 1, y);check_neighbor(x - 1, y);
            check_neighbor(x, y + 1);check_neighbor(x, y - 1);

            if (visitedNeighbors == 1)
            {
                maze[y][x].isVisited = true;
                maze[y][x].isWall = false;

                if (visualize) { // Visualization block
                    BeginDrawing();
                    ClearBackground(RAYWHITE);
                    Render(); // Render the current maze state
                    DrawText("Generating Maze...", 10, 10, 20, RED); // Display progress message
                    EndDrawing();
                    WaitTime(0.005f); // Introduce a small delay (adjust as needed) 
                }

                add_frontier(x + 1, y);add_frontier(x - 1, y);
                add_frontier(x, y + 1);add_frontier(x, y - 1);
            }
        }

        if (visualize) { // Final render after generation
            BeginDrawing();
            ClearBackground(RAYWHITE);
            Render();
            DrawText("Maze Generated!", 10, 10, 20, GREEN);
            EndDrawing();
            WaitTime(2.0f); // Pause to show the finished maze
        }
    }

    void Render() const{
        for (const auto &row : maze)
    {
        for (const auto &cell : row)
        {
            cell.render();
        }
    }
    }


};

class Player
{
public:
    int x,y;
    float speed;

    Player(int startX, int startY, float playerSpeed) : x(startX), y(startY), speed(playerSpeed) {}

    void moveUp(const Maze& maze) {
        int nextY = y - speed;
        if (!isColliding(x, nextY, maze)) {
            y = nextY;
            cout << speed << endl;
        }
    }
    void moveDown(const Maze& maze) {
        int nextY = y + speed;
        if (!isColliding(x, nextY, maze)) {
            y = nextY;
        }
    }
    void moveLeft(const Maze& maze) {
        int nextX = x - speed;
        if (!isColliding(nextX, y, maze)) {
            x = nextX;
        }
    }
    void moveRight(const Maze& maze) {
        int nextX = x + speed;
        if (!isColliding(nextX, y, maze)) {
            x = nextX;
        }
    }

    bool isColliding(int nextX, int nextY, const Maze& maze) const {
        // Calculate the grid coordinates of the player's next position
        int gridX = nextX / CELL_SIZE;
        int gridY = nextY / CELL_SIZE;

        // Check bounds and if the next cell is a wall
        return (gridX < 0 || gridX >= maze.mazeWidth || 
                gridY < 0 || gridY >= maze.mazeHeight ||
                maze.maze[gridY][gridX].isWall);
    }

    void render(){
        DrawRectangle(x, y, CELL_SIZE/3, CELL_SIZE/3, PINK);
    }
};

class Game{
public:
    Maze maze;
    Player player;


    Game() : player(CELL_SIZE, CELL_SIZE, float(CELL_SIZE/15.0f)) {} 

    void run() {
        maze.Generate(); //imad: u can toggel visualisation mode here 

        while (!WindowShouldClose()) {
            update();
            render();
        }
    }

    void update() {
        if (IsKeyDown(KEY_UP)) player.moveUp(maze);
        if (IsKeyDown(KEY_DOWN)) player.moveDown(maze);
        if (IsKeyDown(KEY_LEFT)) player.moveLeft(maze);
        if (IsKeyDown(KEY_RIGHT)) player.moveRight(maze);
        
    }

    void render() {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        maze.Render();
        player.render();  // Render the player

        EndDrawing();
    }
};

int main()
{
    // Basic Window Setup

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
    SetTargetFPS(60);

    Game game; // Create a Game instance
    game.run();

    CloseWindow();

    return 0;
}