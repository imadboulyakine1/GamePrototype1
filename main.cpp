#include "values.h"
// slmimadjdid
#include <iostream>
#include <raylib.h>
#include <vector>

#include <random>
#include <set>

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

    std::pair<int, int> start;
    std::pair<int, int> finish;

    Maze() : mazeWidth(SCREEN_WIDTH / CELL_SIZE), mazeHeight(SCREEN_HEIGHT / CELL_SIZE)
    {
        // Come la fct de un matrix dans c
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

bool hasNeighbors(int x, int y) const {
    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    for (int i = 0; i < 4; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (nx >= 0 && nx < mazeWidth && ny >= 0 && ny < mazeHeight && !maze[ny][nx].isWall && (nx != start.first || ny != start.second) ) { // Exclude start itself
            return true;
        }
    }
    return false;
}

void Generate(bool visualize = false)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> distribWidth(1, mazeWidth - 2); // Exclude corners
    std::uniform_int_distribution<> distribHeight(1, mazeHeight - 2);

    std::set<std::pair<int, int>> frontier; // Store frontier cells (x, y)

    int startX = 1;
    int startY = 1;

    maze[startY][startX].isVisited = true;
    maze[startY][startX].isWall = false;

    auto add_to_frontier = [&](int x, int y)
    {
        if (x >= 1 && x < mazeWidth - 1 && y >= 1 && y < mazeHeight - 1 && !maze[y][x].isVisited)
        {
            frontier.insert({x, y});
        }
    };

    add_to_frontier(startX + 2, startY);
    add_to_frontier(startX - 2, startY);
    add_to_frontier(startX, startY + 2);
    add_to_frontier(startX, startY - 2);

    while (!frontier.empty())
    {
        // Choose a random frontier cell
        int randomIndex = getRandomIndex(frontier.size());
        auto it = frontier.begin();
        std::advance(it, randomIndex); // Move iterator to the random position

        int x = it->first;
        int y = it->second;
        frontier.erase(it);

        maze[y][x].isVisited = true;
        maze[y][x].isWall = false;

        // Connect to a visited neighbor
        int nx = x, ny = y;
        while (true)
        {
            int dx[] = {0, 0, 2, -2};
            int dy[] = {2, -2, 0, 0};
            int r = getRandomIndex(4);
            nx = x + dx[r];
            ny = y + dy[r];

            if (nx >= 0 && nx < mazeWidth && ny >= 0 && ny < mazeHeight && maze[ny][nx].isVisited)
            {
                maze[(y + ny) / 2][(x + nx) / 2].isWall = false; // Remove wall between
                break;
            }
        }

        // Add unvisited neighbors to the frontier
        add_to_frontier(x + 2, y);
        add_to_frontier(x - 2, y);
        add_to_frontier(x, y + 2);
        add_to_frontier(x, y - 2);

        if (visualize)
        {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            Render();
            EndDrawing();
            WaitTime(0.01f);
        }
    }

    do {
        for (int i = 0; i < mazeHeight; ++i) {
        maze[i][0].isWall = true;             // Left border
        maze[i][mazeWidth - 1].isWall = true;  // Right border
    }
    for (int j = 0; j < mazeWidth; ++j) {
        maze[0][j].isWall = true;             // Top border
        maze[mazeHeight - 1][j].isWall = true; // Bottom border
    }
    
    int startSide = getRandomIndex(4); // 0: Top, 1: Right, 2: Bottom, 3: Left
    int finishSide = getRandomIndex(4);

    while (startSide == finishSide)
        finishSide = getRandomIndex(4);

    switch (startSide)
    {
    case 0:
        start = {distribWidth(gen), 0};
        break; // Top
    case 1:
        start = {mazeWidth - 1, distribHeight(gen)};
        break; // Right
    case 2:
        start = {distribWidth(gen), mazeHeight - 1};
        break; // Bottom
    case 3:
        start = {0, distribHeight(gen)};
        break; // Left
    }

    switch (finishSide)
    {
    case 0:
        finish = {distribWidth(gen), 0};
        break; // Top
    case 1:
        finish = {mazeWidth - 1, distribHeight(gen)};
        break; // Right
    case 2:
        finish = {distribWidth(gen), mazeHeight - 1};
        break; // Bottom
    case 3:
        finish = {0, distribHeight(gen)};
        break; // Left
    }

    // Carve out the start and end points
    maze[start.second][start.first].isWall = false;
    maze[finish.second][finish.first].isWall = false;
    } while (!hasNeighbors(start.first, start.second) || !hasNeighbors(finish.first, finish.second)); 

    
}

    void Render() const
    {
        for (const auto &row : maze)
        {
            for (const auto &cell : row)
            {
                cell.render();
            }
        }
        DrawRectangle(start.first * CELL_SIZE, start.second * CELL_SIZE, CELL_SIZE, CELL_SIZE, GREEN); // Start
        DrawRectangle(finish.first * CELL_SIZE, finish.second * CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);
    }
};

class Player
{
public:
    int x, y;
    float speed;

    Player(int startX, int startY, float playerSpeed) : x(startX), y(startY), speed(playerSpeed) {}

    void moveUp(const Maze &maze)
    {
        int nextY = y - speed;
        if (!isColliding(x, nextY, maze))
        {
            y = nextY;
        }
    }
    void moveDown(const Maze &maze)
    {
        int nextY = y + speed;
        if (!isColliding(x, nextY, maze))
        {
            y = nextY;
        }
    }
    void moveLeft(const Maze &maze)
    {
        int nextX = x - speed;
        if (!isColliding(nextX, y, maze))
        {
            x = nextX;
        }
    }
    void moveRight(const Maze &maze)
    {
        int nextX = x + speed;
        if (!isColliding(nextX, y, maze))
        {
            x = nextX;
        }
    }

    bool isColliding(int nextX, int nextY, const Maze &maze) const
    {
        // Calculate the grid coordinates of the player's bounding box corners
        int topLeftX = nextX / CELL_SIZE;
        int topLeftY = nextY / CELL_SIZE;
        int topRightX = (nextX + CELL_SIZE - 1) / CELL_SIZE;
        int topRightY = nextY / CELL_SIZE;
        int bottomLeftX = nextX / CELL_SIZE;
        int bottomLeftY = (nextY + CELL_SIZE - 1) / CELL_SIZE;
        int bottomRightX = (nextX + CELL_SIZE - 1) / CELL_SIZE;
        int bottomRightY = (nextY + CELL_SIZE - 1) / CELL_SIZE;

        // Check bounds and if any of the next cells are walls
        return (topLeftX < 0 || topLeftX >= maze.mazeWidth ||
                topLeftY < 0 || topLeftY >= maze.mazeHeight ||
                topRightX < 0 || topRightX >= maze.mazeWidth ||
                topRightY < 0 || topRightY >= maze.mazeHeight ||
                bottomLeftX < 0 || bottomLeftX >= maze.mazeWidth ||
                bottomLeftY < 0 || bottomLeftY >= maze.mazeHeight ||
                bottomRightX < 0 || bottomRightX >= maze.mazeWidth ||
                bottomRightY < 0 || bottomRightY >= maze.mazeHeight ||
                maze.maze[topLeftY][topLeftX].isWall ||
                maze.maze[topRightY][topRightX].isWall ||
                maze.maze[bottomLeftY][bottomLeftX].isWall ||
                maze.maze[bottomRightY][bottomRightX].isWall);
    }

    void render()
    {
        DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, PINK);
    }
};

class Game
{
public:
    Maze maze;
    Player player;

    Game() : player(CELL_SIZE, CELL_SIZE, float(CELL_SIZE / 15.0f)) {}

    void run()
    {
        maze.Generate(); // imad: u can toggel visualisation mode here

        player.x = maze.start.first * CELL_SIZE;
        player.y = maze.start.second * CELL_SIZE;

        while (!WindowShouldClose())
        {
            update();
            render();
        }
    }

    void update()
    {
        if (IsKeyDown(KEY_UP))
            player.moveUp(maze);
        if (IsKeyDown(KEY_DOWN))
            player.moveDown(maze);
        if (IsKeyDown(KEY_LEFT))
            player.moveLeft(maze);
        if (IsKeyDown(KEY_RIGHT))
            player.moveRight(maze);
    }

    void render()
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        maze.Render();
        player.render(); // Render the player

        EndDrawing();
    }
};

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
    SetTargetFPS(60);

    Game game; // Create a Game instance
    game.run();

    CloseWindow();

    return 0;
}