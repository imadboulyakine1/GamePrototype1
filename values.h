#define CONSTANTS_H
#include "raylib.h"
// VideoScreen Values
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 1000;
const char* GAME_TITLE = "Dune Maze Game";

// Color Palette
const Color BACKGROUND_COLOR = DARKBROWN;
const Color WALL_COLOR = BROWN;
const Color WALL_GLOW_COLOR = GOLD;
const Color PATH_COLOR = BLANK;
const Color PLAYER_COLOR = ORANGE;
const Color START_COLOR = DARKGREEN;
const Color FINISH_COLOR = RED;
const Color TEXT_COLOR = WHITE;

// Thirst Mechanics
const int MAX_THIRST = 100000;
const float THIRST_DECREASE_RATE = 1000;  // Changed from 1 to 0.001f for slower decrease
const int THIRST_INCREASE_AMOUNT = 20000; // Adjusted for the new MAX_THIRST

// Texture Definitions
extern Texture2D sandTexture;
extern Texture2D rockTexture;
extern Texture2D wormTexture;
extern Texture2D playerTexture; // Add player texture definition

// Sound Definitions
extern Music backgroundMusic;
extern Sound winSound;

// Dificulty
// const int CELL_SIZE = 30; // Removed CELL_SIZE definition

