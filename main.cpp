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

Color DARKRED = { 128, 0, 0, 255 };  // Dark red color

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

    std::pair<int,int> start = {-1, -1},
                       end = {-1, -1}; // starting and ending points of our grid

    bool changeStarting = false;
    bool changeEnding = false;

    while(!WindowShouldClose()){
        BeginDrawing();

        if (IsKeyPressed(KEY_S))
            changeStarting = true;
        if (IsKeyPressed(KEY_E))
            changeEnding = true;
        
        if (!changeStarting && !changeEnding && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            // Fill a cell
            // All cells in a visited
            Vector2 pos = GetMousePosition();
            int X = pos.x / gridSize;
            int Y = pos.y / gridSize;

            if (pos.x < screenWidth && pos.y < screenHeight && ColorsEqual(gridColor[X][Y], LIGHTGRAY)){
                gridColor[X][Y] = DARKGRAY;
            }
        }   

        if (changeStarting == true){
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                if (start != std::make_pair(-1, -1)){
                    gridColor[start.first][start.second] = LIGHTGRAY;
                }
                Vector2 pos = GetMousePosition();
                int X = pos.x / gridSize;
                int Y = pos.y / gridSize;

                if (pos.x < screenWidth && pos.y < screenHeight){
                    if (!ColorsEqual(gridColor[X][Y], DARKRED)){
                        start = std::make_pair(X, Y);
                        gridColor[X][Y] = DARKGREEN;
                        changeStarting = false;
                    }
                }
            }
        } 

        if (changeEnding == true){
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                if (end != std::make_pair(-1, -1)){
                    gridColor[end.first][end.second] = LIGHTGRAY;
                }
                Vector2 pos = GetMousePosition();
                int X = pos.x / gridSize;
                int Y = pos.y / gridSize;

                if (pos.x < screenWidth && pos.y < screenHeight){
                    if (!ColorsEqual(gridColor[X][Y], DARKGREEN)){
                        end = std::make_pair(X, Y);
                        gridColor[X][Y] = DARKRED;
                        changeEnding = false;
                    }
                }
            }
        } 

        if (IsKeyDown(KEY_T)){
            // Clear a cell
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                Vector2 pos = GetMousePosition();
                int X = pos.x / gridSize;
                int Y = pos.y / gridSize;

                if (pos.x < screenWidth && pos.y < screenHeight &&
                    std::make_pair(X, Y) != start && std::make_pair(X, Y) != end){
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
