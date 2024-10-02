#include "raylib.h"

int main(void) {
    
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Pathfinding Visualizer");
    
    SetTargetFPS(60);
    const int gridSize = 50; // Size of each grid cell;

    while(!WindowShouldClose()){
        BeginDrawing();

        ClearBackground(RAYWHITE);
        
        // (0, 0) is origin (top-left corner)
        for(int x = 0; x < screenWidth; x += gridSize){
            DrawLine(x, 0, x, screenHeight, LIGHTGRAY); // vertical line
        }

        for(int y = 0; y < screenHeight; y += gridSize){
            DrawLine(0, y, screenWidth, y, LIGHTGRAY); // horizontall lines
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
