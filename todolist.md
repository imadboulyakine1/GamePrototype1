**I. Project Setup and Core Classes (Steps 1-20) - Detailed Instructions**

1. **Create Project Directory:** Create a new folder on your computer where all the files for your maze game will be stored.  Give it a descriptive name like "MazeGame." This helps keep your project organized.

2. **Create Main.cpp:** Inside your project folder, create a new text file named `main.cpp`. This will be the main entry point for your C++ program.

3. **Include Raylib:** At the top of your `main.cpp` file, add the line `#include <raylib.h>`. This line tells the compiler to include the Raylib library, which provides functions for graphics, sound, and input.

4. **Basic Window Setup:** Inside the `main()` function of `main.cpp`, add the code to initialize a window using `InitWindow()`. Set the window title and dimensions.  Also, set the target frames per second (FPS) using `SetTargetFPS()`.

5. **Create `Cell` Class (Header):**  Create a new header file named `Cell.h`. This file will contain the *declaration* of the `Cell` class.  Inside this file, define the class `Cell`.  Think about the data a single cell in your maze needs:  `x` coordinate, `y` coordinate, `isWall` (true if it's a wall, false if it's a path), and `isVisited` (used for maze generation algorithms). These will be the member variables (attributes) of your `Cell` class. Also, declare (but don't define yet) a `render()` function that will draw the cell on the screen.

6. **Cell Implementation (`Cell.cpp`):** Create a new C++ source file named `Cell.cpp`. This file will contain the *definitions* of the functions you declared in `Cell.h`. Implement the constructor(s) for the `Cell` class to initialize its member variables. Implement the `render()` function to draw a rectangle using Raylib's drawing functions (`DrawRectangle()`, etc.), depending on whether the cell is a wall or a path.

7. **Create `Maze` Class (Header `Maze.h`):**  Create a new header file named `Maze.h`.  Declare the `Maze` class.  It needs to store the `width` and `height` of the maze, and a 2D array (or vector of vectors) of `Cell` objects to represent the maze grid.  Declare methods (functions) like `generate()`, `render()`, and any others you'll need.

8. **Maze Constructor (`Maze.cpp`):** Create `Maze.cpp` to implement the `Maze` class's methods.  In the constructor, initialize the `width` and `height` and allocate memory for the 2D array of `Cell` objects. Initialize each `Cell` in the grid with its appropriate coordinates.

9. **Create `Player` Class (Header `Player.h`):** Create a header file `Player.h`.  Declare the `Player` class with member variables for the player's `x` and `y` position and `speed`.  Declare functions for movement (e.g., `moveUp()`, `moveDown()`, `moveLeft()`, `moveRight()`).

10. **Player Implementation (`Player.cpp`):**  Create `Player.cpp`.  Implement the `Player` class's methods. The movement functions should update the player's `x` and `y` position based on the `speed`.

11. **Integrate Player and Maze:** In your `main.cpp`, create instances (objects) of the `Maze` and `Player` classes.

12. **Basic Rendering:** In the game loop of `main()`, call the `render()` functions of the `Maze` and `Player` objects to draw them on the screen.

13. **Movement Test:** In `main()`, use Raylib's input functions (`IsKeyDown()`, etc.) to detect keyboard input.  Call the appropriate player movement functions based on the keys pressed.

14. **Collision Detection (Simple):**  In the player's movement functions, add basic collision detection. Check if the player's next position would be inside a wall. If so, prevent the movement.

15. **Refactor Rendering:** Move the rendering code from `main()` into the `render()` functions of the `Maze` and `Player` classes. This improves code organization.

16. **Game Loop Structure:**  Organize your `main()` function's game loop into three distinct sections: initialization (creating window, objects, etc.), update (handling input, game logic, etc.), and drawing (rendering the game).

17. **Create `Game` Class (Header `Game.h`):** Create a header file `Game.h`.  Declare a `Game` class that will manage the overall game state, difficulty level, timer, and other game-related variables and functions.

18. **Game Implementation (`Game.cpp`):** Create `Game.cpp`. Implement the `Game` class's methods.

19. **Integrate `Game` Class:** Modify your `main()` function to create a `Game` object. Use the `Game` object to control the game loop and other game logic.

20. **Clean Up Main:** After integrating the `Game` class, your `main()` function should primarily be responsible for setting up the window and running the game using the `Game` object.  This keeps your `main()` function concise and focused.