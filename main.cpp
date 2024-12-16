#include "values.h"
// slmimadjdid
#include <iostream>
#include <raylib.h>
#include <vector>

#include <random>
#include <set>

using namespace std;

const int MAZE_WIDTH = 1000;
const int MAZE_HEIGHT = 1000;

Shader glowShader;

class Cell
{
public:
    int x, y;
    bool isWall, isVisited;

    Cell(int xCoord = 0, int yCoord = 0, bool wall = true, bool visited = false) : x(xCoord), y(yCoord), isWall(wall), isVisited(visited) {}

    void render(int cellSize) const
    {
        if (isWall) {
            // Draw the glow effect
            DrawRectangle(x - 2, y - 2, cellSize + 4, cellSize + 4, WALL_GLOW_COLOR);
            // Draw the actual wall
            DrawRectangle(x, y, cellSize, cellSize, WALL_COLOR);
        } else {
            DrawRectangle(x, y, cellSize, cellSize, PATH_COLOR);
        }
    }
};

class Maze
{
public:
    vector<vector<Cell>> maze;
    int mazeWidth;
    int mazeHeight;
    int cellSize;

    std::pair<int, int> start;
    std::pair<int, int> finish;

    Maze(int cellSize) : cellSize(cellSize), mazeWidth(MAZE_WIDTH / cellSize), mazeHeight(MAZE_HEIGHT / cellSize)
    {
        // Come la fct de un matrix dans c
        maze.resize(mazeHeight);
        for (int i = 0; i < mazeHeight; ++i)
        {
            maze[i].reserve(mazeWidth);
            for (int j = 0; j < mazeWidth; ++j)
            {
                maze[i].push_back(Cell(j * cellSize, i * cellSize));
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
                cell.render(cellSize);
            }
        }
        DrawRectangle(start.first * cellSize, start.second * cellSize, cellSize, cellSize, START_COLOR); // Start
        DrawRectangle(finish.first * cellSize, finish.second * cellSize, cellSize, cellSize, FINISH_COLOR);
    }
};

void ShowDifficultyMenu(int &cellSize, bool &fullscreen)
{
    Texture2D bg = LoadTexture("bg.jpg");
    while (true)
    {
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);
        DrawTexture(bg, 0, 0, WHITE);
        DrawText("Select Difficulty", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 100, 20, TEXT_COLOR);
        DrawText("1. Easy", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50, 20, TEXT_COLOR);
        DrawText("2. Medium", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2, 20, TEXT_COLOR);
        DrawText("3. Hard", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + 50, 20, TEXT_COLOR);
        DrawText("Press F for Fullscreen", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 100, 20, TEXT_COLOR);
        EndDrawing();

        if (IsKeyPressed(KEY_ONE))
        {
            cellSize = 60;
            break;
        }
        if (IsKeyPressed(KEY_TWO))
        {
            cellSize = 30;
            break;
        }
        if (IsKeyPressed(KEY_THREE))
        {
            cellSize = 15;
            break;
        }
        if (IsKeyPressed(KEY_F))
        {
            fullscreen = !fullscreen;
            break;
        }
    }
    UnloadTexture(bg);
}

class Player
{
public:
    int x, y;
    float speed;
    int cellSize;
    vector<pair<int, int>> trail; // Store the trail positions

    Player(int startX, int startY, float playerSpeed, int cellSize) : x(startX), y(startY), speed(playerSpeed), cellSize(cellSize) {}

    void moveUp(const Maze &maze)
    {
        int nextY = y - speed;
        if (!isColliding(x, nextY, maze))
        {
            trail.push_back({x, y});
            y = nextY;
        }
    }
    void moveDown(const Maze &maze)
    {
        int nextY = y + speed;
        if (!isColliding(x, nextY, maze))
        {
            trail.push_back({x, y});
            y = nextY;
        }
    }
    void moveLeft(const Maze &maze)
    {
        int nextX = x - speed;
        if (!isColliding(nextX, y, maze))
        {
            trail.push_back({x, y});
            x = nextX;
        }
    }
    void moveRight(const Maze &maze)
    {
        int nextX = x + speed;
        if (!isColliding(nextX, y, maze))
        {
            trail.push_back({x, y});
            x = nextX;
        }
    }

    bool isColliding(int nextX, int nextY, const Maze &maze) const
    {
        // Calculate the grid coordinates of the player's bounding box corners
        int topLeftX = nextX / cellSize;
        int topLeftY = nextY / cellSize;
        int topRightX = (nextX + cellSize - 1) / cellSize;
        int topRightY = nextY / cellSize;
        int bottomLeftX = nextX / cellSize;
        int bottomLeftY = (nextY + cellSize - 1) / cellSize;
        int bottomRightX = (nextX + cellSize - 1) / cellSize;
        int bottomRightY = (nextY + cellSize - 1) / cellSize;

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
        // Draw the trail with glow effect using shader
        BeginShaderMode(glowShader);
        for (size_t i = 1; i < trail.size(); ++i)
        {
            DrawLineEx({(float)trail[i-1].first + cellSize / 2, (float)trail[i-1].second + cellSize / 2},
                       {(float)trail[i].first + cellSize / 2, (float)trail[i].second + cellSize / 2},
                       cellSize / 4, WALL_GLOW_COLOR);
        }
        EndShaderMode();
        // Draw the player
        DrawRectangle(x, y, cellSize, cellSize, PLAYER_COLOR);
    }
};

class Game
{
public:
    Maze maze;
    Player player;
    float timer;
    int cellSize;
    bool gameWon;
    bool fullscreen;

    Game(int cellSize, bool fullscreen) : maze(cellSize), player(cellSize, cellSize, float(cellSize / 5.0f), cellSize), timer(0.0f), cellSize(cellSize), gameWon(false), fullscreen(fullscreen) {}

    void run()
    {
        maze.Generate(); // imad: u can toggel visualisation mode here

        player.x = maze.start.first * maze.cellSize;
        player.y = maze.start.second * maze.cellSize;

        while (!WindowShouldClose())
        {
            update();
            render();
        }
    }

    void update()
    {
        if (!gameWon)
        {
            if (IsKeyDown(KEY_UP))
                player.moveUp(maze);
            if (IsKeyDown(KEY_DOWN))
                player.moveDown(maze);
            if (IsKeyDown(KEY_LEFT))
                player.moveLeft(maze);
            if (IsKeyDown(KEY_RIGHT))
                player.moveRight(maze);

            if (IsKeyPressed(KEY_R))
                reset();

            if (IsKeyPressed(KEY_F))
                toggleFullscreen();

            timer += GetFrameTime();

            // Check if player reached the finish point
            if (player.x / cellSize == maze.finish.first && player.y / cellSize == maze.finish.second)
            {
                gameWon = true;
            }
        }
        else
        {
            if (IsKeyPressed(KEY_SPACE))
                reset();
            if (IsKeyPressed(KEY_M))
                returnToMenu();
        }
    }

    void render()
    {
        Texture2D gameBg = LoadTexture("game.jpg");
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);
        DrawTexture(gameBg, 0, 0, WHITE);

        maze.Render();
        player.render(); // Render the player

        DrawText(TextFormat("Time: %.2f", timer), 10, 10, 20, TEXT_COLOR);
        DrawText("Press R to Reset", 10, 40, 20, TEXT_COLOR);
        DrawText("Press F to Toggle Fullscreen", 10, 70, 20, TEXT_COLOR);
        DrawFPS(SCREEN_WIDTH - 100, 10); // Display FPS at the top-right corner

        if (gameWon)
        {
            DrawText("You Win!", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50, 40, START_COLOR);
            DrawText("Press SPACE to Replay", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 20, TEXT_COLOR);
            DrawText("Press M to go to Menu", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 30, 20, TEXT_COLOR);
        }

        EndDrawing();
        UnloadTexture(gameBg);
    }

    void reset()
    {
        maze.Generate();
        player.x = maze.start.first * maze.cellSize;
        player.y = maze.start.second * maze.cellSize;
        player.trail.clear(); // Clear the trail on reset
        timer = 0.0f;
        gameWon = false;
    }

    void returnToMenu()
    {
        int newCellSize;
        bool fullscreen = IsWindowFullscreen();
        ShowDifficultyMenu(newCellSize, fullscreen);
        if (fullscreen != IsWindowFullscreen())
        {
            ToggleFullscreen();
        }
        cellSize = newCellSize;
        maze = Maze(cellSize);
        player = Player(cellSize, cellSize, float(cellSize / 15.0f), cellSize);
        reset();
    }

    void toggleFullscreen()
    {
        fullscreen = !fullscreen;
        ToggleFullscreen();
    }
};

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
    SetTargetFPS(60);

    glowShader = LoadShader(0, "glow.fs"); // Load the glow shader

    int cellSize;
    bool fullscreen = false;
    ShowDifficultyMenu(cellSize, fullscreen);

    if (fullscreen)
    {
        ToggleFullscreen();
    }

    Game game(cellSize, fullscreen); // Pass cellSize and fullscreen to Game instance
    game.run();

    UnloadShader(glowShader); // Unload the glow shader
    CloseWindow();

    return 0;
}