#include "values.h"
// slmimadjdid
#include <iostream>
#include <raylib.h>
#include <vector>
#include <fstream>
#include <algorithm>

#include <random>
#include <set>

using namespace std;

const int MAZE_WIDTH = 1000;
const int MAZE_HEIGHT = 1000;

Shader glowShader;
Texture2D sandTexture;
Texture2D rockTexture;
Texture2D wormTexture;
Texture2D playerTexture; // Add player texture
Music backgroundMusic;
Sound winSound;

const int MAX_HIGH_SCORES = 10; // Maximum number of high scores to keep

struct HighScore {
    std::string name;
    float time;
};

std::vector<HighScore> highScores;

void LoadHighScores() {
    std::ifstream file("highscores.txt");
    if (file.is_open()) {
        HighScore hs;
        while (file >> hs.name >> hs.time) {
            highScores.push_back(hs);
        }
        file.close();
    }
}

void SaveHighScores() {
    std::ofstream file("highscores.txt");
    if (file.is_open()) {
        for (const auto& hs : highScores) {
            file << hs.name << " " << hs.time << std::endl;
        }
        file.close();
    }
}

void AddHighScore(const std::string& name, float time) {
    highScores.push_back({name, time});
    std::sort(highScores.begin(), highScores.end(), [](const HighScore& a, const HighScore& b) {
        return a.time < b.time;
    });
    if (highScores.size() > MAX_HIGH_SCORES) {
        highScores.pop_back();
    }
    SaveHighScores();
}

void ShowHighScores() {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("High Scores", SCREEN_WIDTH / 2 - 50, 50, 20, WHITE);
    for (size_t i = 0; i < highScores.size(); ++i) {
        DrawText(TextFormat("%d. %s - %.2f", i + 1, highScores[i].name.c_str(), highScores[i].time), SCREEN_WIDTH / 2 - 100, 100 + i * 30, 20, WHITE);
    }
    DrawText("Press SPACE to play again", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT - 50, 20, WHITE);
    EndDrawing();
}

void LoadTextures() {
    sandTexture = LoadTexture("sand.png");
    rockTexture = LoadTexture("rock.jpg");
    wormTexture = LoadTexture("worm.png");
    playerTexture = LoadTexture("player.png"); // Load player texture
}

void UnloadTextures() {
    UnloadTexture(sandTexture);
    UnloadTexture(rockTexture);
    UnloadTexture(wormTexture);
    UnloadTexture(playerTexture); // Unload player texture
}

void LoadAudio() {
    InitAudioDevice();
    backgroundMusic = LoadMusicStream("dune_theme.mp3");
    winSound = LoadSound("win_sound.wav");
}

void UnloadAudio() {
    UnloadMusicStream(backgroundMusic);
    UnloadSound(winSound);
    CloseAudioDevice();
}

class Cell
{
public:
    int x, y;
    bool isWall, isVisited, isWater; // Add isWater flag

    Cell(int xCoord = 0, int yCoord = 0, bool wall = true, bool visited = false, bool water = false) : x(xCoord), y(yCoord), isWall(wall), isVisited(visited), isWater(water) {}

    void render(int cellSize) const
    {
        if (isWall) {
            DrawTexture(rockTexture, x, y, WHITE);
        } else if (isWater) {
            DrawRectangle(x, y, cellSize, cellSize, BLUE); // Render water cell
        } else {
            DrawTexture(sandTexture, x, y, WHITE);
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
    RenderTexture2D mazeTexture; // Add RenderTexture2D member
    int offsetX; // Add offsetX
    int offsetY; // Add offsetY

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
        mazeTexture = LoadRenderTexture(MAZE_WIDTH, MAZE_HEIGHT); // Initialize mazeTexture
        offsetX = (SCREEN_WIDTH - MAZE_WIDTH) / 2; // Calculate offsetX
        offsetY = (SCREEN_HEIGHT - MAZE_HEIGHT) / 2; // Calculate offsetY
    }

    ~Maze() {
        UnloadRenderTexture(mazeTexture); // Unload mazeTexture
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

        // Add random water cells
        for (int i = 0; i < mazeHeight; ++i) {
            for (int j = 0; j < mazeWidth; ++j) {
                if (!maze[i][j].isWall && !maze[i][j].isWater && getRandomIndex(100) < 5) { // 5% chance to be water
                    maze[i][j].isWater = true;
                }
            }
        }

        // Render the maze to the texture
        BeginTextureMode(mazeTexture);
        ClearBackground(BLANK); // Clear the texture background
        for (const auto &row : maze)
        {
            for (const auto &cell : row)
            {
                cell.render(cellSize);
            }
        }
        DrawRectangle(start.first * cellSize, start.second * cellSize, cellSize, cellSize, START_COLOR); // Start
        DrawRectangle(finish.first * cellSize, finish.second * cellSize, cellSize, cellSize, FINISH_COLOR); // Finish
        EndTextureMode();
    }

    void Render() const
    {
        DrawTextureRec(mazeTexture.texture, {0.0f, 0.0f, (float)mazeTexture.texture.width, (float)-mazeTexture.texture.height}, {(float)offsetX, (float)offsetY}, WHITE);
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

void ShowStory()
{
    const char* storyLines[] = {
        "In a world full of mazes...",
        "You are the chosen one...",
        "Navigate through the labyrinth...",
        "And find your way to freedom..."
    };
    int numLines = sizeof(storyLines) / sizeof(storyLines[0]);
    int lineIndex = 0;
    float alpha = 0.0f;
    float fadeSpeed = 0.02f;

    while (lineIndex < numLines)
    {
        BeginDrawing();
        ClearBackground(BLACK);

        if (alpha < 1.0f)
        {
            alpha += fadeSpeed;
        }
        else
        {
            alpha = 1.0f;
        }

        DrawText(storyLines[lineIndex], SCREEN_WIDTH / 2 - MeasureText(storyLines[lineIndex], 20) / 2, SCREEN_HEIGHT / 2, 20, Fade(WHITE, alpha));

        EndDrawing();

        if (alpha >= 1.0f && IsKeyPressed(KEY_SPACE))
        {
            alpha = 0.0f;
            lineIndex++;
        }
    }
}

class Player
{
public:
    int x, y;
    float speed;
    int cellSize;
    int thirst; // Add thirst level
    vector<pair<int, int>> trail; // Store the trail positions

    Player(int startX, int startY, float playerSpeed, int cellSize) : x(startX), y(startY), speed(playerSpeed), cellSize(cellSize), thirst(MAX_THIRST) {}

    void moveUp(Maze &maze)
    {
        int nextY = y - speed;
        if (!isColliding(x, nextY, maze))
        {
            trail.push_back({x, y});
            y = nextY;
            checkWater(maze);
        }
    }
    void moveDown(Maze &maze)
    {
        int nextY = y + speed;
        if (!isColliding(x, nextY, maze))
        {
            trail.push_back({x, y});
            y = nextY;
            checkWater(maze);
        }
    }
    void moveLeft(Maze &maze)
    {
        int nextX = x - speed;
        if (!isColliding(nextX, y, maze))
        {
            trail.push_back({x, y});
            x = nextX;
            checkWater(maze);
        }
    }
    void moveRight(Maze &maze)
    {
        int nextX = x + speed;
        if (!isColliding(nextX, y, maze))
        {
            trail.push_back({x, y});
            x = nextX;
            checkWater(maze);
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

    void checkWater(Maze &maze)
    {
        int cellX = x / cellSize;
        int cellY = y / cellSize;
        if (maze.maze[cellY][cellX].isWater) {
            thirst = min(thirst + THIRST_INCREASE_AMOUNT, MAX_THIRST); // Increase thirst level
            maze.maze[cellY][cellX].isWater = false; // Remove water cell after drinking
            
            // Re-render the entire maze texture to reflect the change
            BeginTextureMode(maze.mazeTexture);
            ClearBackground(BLANK); // Clear the texture background
            for (const auto &row : maze.maze)
            {
                for (const auto &cell : row)
                {
                    cell.render(cellSize);
                }
            }
            DrawRectangle(maze.start.first * cellSize, maze.start.second * cellSize, cellSize, cellSize, START_COLOR); // Start
            DrawRectangle(maze.finish.first * cellSize, maze.finish.second * cellSize, cellSize, cellSize, FINISH_COLOR); // Finish
            EndTextureMode();
        }
    }

    void render(int offsetX, int offsetY) const
    {
        // Draw the player using the texture, scaled to fit within a single cell
        DrawTexturePro(playerTexture, {0, 0, (float)playerTexture.width, (float)playerTexture.height}, {(float)(x + offsetX), (float)(y + offsetY), (float)cellSize, (float)cellSize}, {0, 0}, 0.0f, WHITE);
    }
};

class Worm
{
public:
    int x, y;
    float speed;
    int cellSize;
    float visionRadius;

    Worm(int startX, int startY, float wormSpeed, int cellSize, float visionRadius) : x(startX), y(startY), speed(wormSpeed), cellSize(cellSize), visionRadius(visionRadius) {}

    void moveTowardsPlayer(const Player &player)
    {
        if (distanceToPlayer(player) < visionRadius) {
            if (x < player.x)
                x += speed;
            else if (x > player.x)
                x -= speed;

            if (y < player.y)
                y += speed;
            else if (y > player.y)
                y -= speed;
        }
    }

    float distanceToPlayer(const Player &player) const
    {
        return sqrt(pow(x - player.x, 2) + pow(y - player.y, 2));
    }

    bool isCollidingWithPlayer(const Player &player) const
    {
        return (x < player.x + cellSize && x + cellSize > player.x &&
                y < player.y + cellSize && y + cellSize > player.y);
    }

    void render(int offsetX, int offsetY) const
    {
        // Draw the worm as a colored rectangle
        DrawRectangle(x + offsetX, y + offsetY, cellSize, cellSize, RED);
    }
};

class Game
{
public:
    Maze maze;
    Player player;
    Worm worm;
    float timer;
    int cellSize;
    bool gameWon;
    bool fullscreen;
    Texture2D gameBg;
    Texture2D winBg; // Add win background texture

    Game(int cellSize, bool fullscreen) : maze(cellSize), player(cellSize, cellSize, float(cellSize / 5.0f), cellSize), worm(0, 0, float(cellSize / 20.0f), cellSize, 200.0f), timer(0.0f), cellSize(cellSize), gameWon(false), fullscreen(fullscreen)
    {
        gameBg = LoadTexture("game.jpg");
        winBg = LoadTexture("bg.jpg"); // Load win background texture
    }

    ~Game()
    {
        UnloadTexture(gameBg);
        UnloadTexture(winBg); // Unload win background texture
    }

    void run()
    {
        LoadHighScores(); // Load high scores at the start
        maze.Generate(); // imad: u can toggel visualisation mode here

        player.x = maze.start.first * maze.cellSize;
        player.y = maze.start.second * maze.cellSize;

        // Place worm in a random position within the maze
        do {
            worm.x = maze.getRandomIndex(maze.mazeWidth) * maze.cellSize;
            worm.y = maze.getRandomIndex(maze.mazeHeight) * maze.cellSize;
        } while (maze.maze[worm.y / cellSize][worm.x / cellSize].isWall);

        PlayMusicStream(backgroundMusic); // Start playing music when the game starts

        while (!WindowShouldClose())
        {
            UpdateMusicStream(backgroundMusic);
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

            worm.moveTowardsPlayer(player);

            if (worm.isCollidingWithPlayer(player))
                reset();

            if (IsKeyPressed(KEY_R))
                reset();

            if (IsKeyPressed(KEY_F))
                toggleFullscreen();

            timer += GetFrameTime();
            player.thirst -= THIRST_DECREASE_RATE * GetFrameTime(); // Decrease thirst level

            if (player.thirst <= 0)
                reset(); // Reset game if thirst reaches 0

            // Check if player reached the finish point
            if (player.x / cellSize == maze.finish.first && player.y / cellSize == maze.finish.second)
            {
                gameWon = true;
                PlaySound(winSound);
            }
        }
        else
        {
            if (IsKeyPressed(KEY_SPACE)) {
                std::string playerName = "Player"; // Replace with actual player name input if needed
                AddHighScore(playerName, timer);
                reset();
            }
        }
    }

    void render()
    {
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        if (gameWon)
        {
            DrawTexture(winBg, 0, 0, WHITE); // Draw win background
            DrawText("You Win!", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 100, 40, START_COLOR);
            ShowHighScores(); // Show high scores on win screen
        }
        else
        {
            DrawTexture(gameBg, 0, 0, WHITE);
            maze.Render();
            player.render(maze.offsetX, maze.offsetY); // Pass offsets to player render
            worm.render(maze.offsetX, maze.offsetY); // Render the worm

            DrawText(TextFormat("Time: %.2f", timer), 10, 10, 20, TEXT_COLOR);
            DrawText("Press R to Reset", 10, 40, 20, TEXT_COLOR);
            DrawText("Press F to Toggle Fullscreen", 10, 70, 20, TEXT_COLOR);
            DrawFPS(SCREEN_WIDTH - 100, 10); // Display FPS at the top-right corner

            // Render thirst bar
            DrawRectangle(10, 100, 200, 20, GRAY);
            DrawRectangle(10, 100, (player.thirst * 200 / MAX_THIRST), 20, BLUE); // Scale thirst for display
            DrawText(TextFormat("Thirst: %d%%", (player.thirst * 100 / MAX_THIRST)), 10, 80, 20, TEXT_COLOR); // Display thirst percentage
        }

        EndDrawing();
    }

    void reset()
    {
        maze.Generate();
        player.x = maze.start.first * maze.cellSize;
        player.y = maze.start.second * maze.cellSize;
        player.thirst = MAX_THIRST; // Reset thirst level

        // Place worm in a random position within the maze
        do {
            worm.x = maze.getRandomIndex(maze.mazeWidth) * maze.cellSize;
            worm.y = maze.getRandomIndex(maze.mazeHeight) * maze.cellSize;
        } while (maze.maze[worm.y / cellSize][worm.x / cellSize].isWall);

        player.trail.clear(); // Clear the trail on reset
        timer = 0.0f;
        gameWon = false;
    }

    void returnToMenu()
    {
        int newCellSize;
        bool newFullscreen = IsWindowFullscreen();
        ShowDifficultyMenu(newCellSize, newFullscreen);
        if (newFullscreen != fullscreen)
        {
            ToggleFullscreen();
            fullscreen = newFullscreen;
        }
        cellSize = newCellSize;
        maze = Maze(cellSize);
        player = Player(maze.start.first * cellSize, maze.start.second * cellSize, float(cellSize / 5.0f), cellSize); // Corrected player initialization
        worm = Worm(0, 0, float(cellSize / 20.0f), cellSize, 200.0f); // Initialize worm
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
    LoadTextures(); // Load textures
    LoadAudio(); // Load audio

    ShowStory(); // Show the story before the difficulty menu

    int cellSize;
    bool fullscreen = true; // Always run in fullscreen mode
    ShowDifficultyMenu(cellSize, fullscreen);

    if (fullscreen)
    {
        ToggleFullscreen();
    }

    Game game(cellSize, fullscreen); // Pass cellSize and fullscreen to Game instance
    game.run();

    UnloadShader(glowShader); // Unload the glow shader
    UnloadTextures(); // Unload textures
    UnloadAudio(); // Unload audio
    CloseWindow();

    return 0;
}