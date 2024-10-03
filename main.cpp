#include "raylib.h"
#include<iostream>
// We cant compare two Color directly, since each has:
// r (red)
// g (green)
// b (blue)
// a (alpha) --> transparency level of the color. 255 means opaque and 0 transparent
bool ColorsEqual(Color color1, Color color2) {
    return (color1.r == color2.r) && (color1.g == color2.g) && 
           (color1.b == color2.b) && (color1.a == color2.a);
}

int main(void) {
    
    const int screenWidth = 1600;
    const int screenHeight = 1200;
    InitWindow(screenWidth, screenHeight, "Pathfinding Visualizer");
    
    SetTargetFPS(60);
    const int gridSize = 50; // Size of each grid cell;
    
    Color gridColor[screenWidth / gridSize][screenHeight / gridSize];
    for(int i = 0; i < screenWidth / gridSize; i += 1){
        for(int j = 0; j < screenHeight / gridSize; j += 1){
            gridColor[i][j] = LIGHTGRAY;
        }
    }    

    // bool visitedCells[screenWidth / gridSize][screenHeight / gridSize];

    // for(int i = 0; i < screenWidth / gridSize; i += 1){
    //     for(int j = 0; j < screenHeight / gridSize; j += 1){
    //         visitedCells[i][j] = 0;
    //     }
    // }

    while(!WindowShouldClose()){
        BeginDrawing();

        // bool mouseHeld = false;
        
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            // Fill a cell
            // All cells in a visited
            // mouseHeld = true;
            Vector2 pos = GetMousePosition();
            int X = pos.x / gridSize;
            int Y = pos.y / gridSize;

            if (pos.x < screenWidth && pos.y < screenHeight){
                // visitedCells[X][Y] = 1;
                // std::cout << X << " " << Y << " " << visitedCells[X][Y] << std::endl;
                // if (ColorsEqual(gridColor[X][Y], LIGHTGRAY)) 
                gridColor[X][Y] = DARKGRAY;
                // else
                //     gridColor[X][Y] = LIGHTGRAY;
            }
        }    

        // else if (mouseHeld && IsMouseButtonUp(MOUSE_BUTTON_LEFT)){
        //     mouseHeld = false;
        //     for(int i = 0; i < screenWidth / gridSize; i += 1){
        //         for(int j = 0; j < screenHeight / gridSize; j += 1){
        //             visitedCells[i][j] = 0;
        //         }
        //     }
        // }

        if (IsKeyDown(KEY_T)){
            // Clear a cell
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                Vector2 pos = GetMousePosition();
                int X = pos.x / gridSize;
                int Y = pos.y / gridSize;

                if (pos.x < screenWidth && pos.y < screenHeight){
                    gridColor[X][Y] = LIGHTGRAY;
                }
            }    
        }

        if (IsKeyPressed(KEY_C)){
            for(int x = 0; x < screenWidth; x += gridSize){
                for(int y = 0; y < screenHeight; y += gridSize){
                    gridColor[x / gridSize][y / gridSize] = LIGHTGRAY;
                }
            }
        }    

        ClearBackground(RAYWHITE);

        for(int x = 0; x < screenWidth; x += gridSize){
            for(int y = 0; y < screenHeight; y += gridSize){
                DrawRectangle(x, y, gridSize, gridSize, gridColor[x/gridSize][y/gridSize]);
                DrawRectangleLines(x, y, gridSize, gridSize, DARKGRAY);
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
