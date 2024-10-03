#include "raylib.h"
#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
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
Color DARKYELLOW = { 204, 204, 0, 255 };  // Dark yellow color

// implement bfs algorithm
int dr[4] = {-1, 0, 1, 0};
int dc[4] = {0, 1, 0, -1};
int bfs_checks = 0;

const int gridWidth = 1400;
const int screenWidth = gridWidth + 200;
const int screenHeight = 1200;
const int gridHeight = screenHeight;

int path_length = 0;

const int gridSize = 50; // Size of each grid cell;
float time_taken = 0.0f;    // Variable to store the time taken

void bfs(std::pair<int,int> &start, std::pair<int,int> &end, std::vector<std::vector<Color>> &gridColor){
    auto start_time = std::chrono::high_resolution_clock::now();
    std::queue<std::vector<int>> q;
    q.push({start.first, start.second, 0});

    int visited[gridWidth / gridSize][screenHeight / gridSize];
    std::pair<int,int> parent[gridWidth / gridSize][screenHeight / gridSize];

    for(int x = 0; x < gridWidth; x += gridSize){
        for(int y = 0; y < screenHeight; y += gridSize){
            visited[x / gridSize][y / gridSize] = 0;
            parent[x / gridSize][y / gridSize] = std::make_pair(-1, -1);
        }
    }

    bool path_found = false;
    while(!q.empty()){
        int r = q.front()[0];
        int c = q.front()[1];
        int d = q.front()[2];

        if (end == std::make_pair(r, c)){
            path_length = d;
            path_found = true;
            break;
        }

        q.pop();
        for(int i=0; i<4; i++){
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (nr < 0 || nr >= gridWidth / gridSize || nc < 0 || nc >= screenHeight / gridSize) continue;
            if (visited[nr][nc]) continue;
            if (visited[end.first][end.second]) continue;
            if (ColorsEqual(gridColor[nr][nc], DARKGRAY)) continue;

            visited[nr][nc] = 1;
            if (ColorsEqual(gridColor[nr][nc], LIGHTGRAY)){
                gridColor[nr][nc] = DARKBLUE;
                bfs_checks++;
            }
            parent[nr][nc] = std::make_pair(r, c);
            q.push({nr, nc, d + 1});
        }
    }
    
    if (path_found){
        gridColor[end.first][end.second] = DARKRED;
        std::pair<int, int> path = parent[end.first][end.second];
        while (path != start) {
            gridColor[path.first][path.second] = DARKYELLOW;
            path = parent[path.first][path.second];
        }
        gridColor[path.first][path.second] = DARKGREEN; // repainting start point
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end_time - start_time;
    time_taken = duration.count() * 1000;  // time in ms
}

void startBFS(std::pair<int,int> &start, std::pair<int,int> &end, 
              std::pair<int,int> &prev_start, std::pair<int,int> &prev_end,
              std::vector<std::vector<Color>> &gridColor,
              int flag){

    if (start != std::make_pair(-1, -1) && end != std::make_pair(-1, -1) && (start != prev_start || end != prev_end || flag)){
        prev_end = end;
        prev_start = start;
        for(int i = 0; i < gridWidth / gridSize; i += 1){
            for(int j = 0; j < screenHeight / gridSize; j += 1){
                if (!ColorsEqual(gridColor[i][j], DARKGRAY)
                    && !ColorsEqual(gridColor[i][j], DARKRED)
                    && !ColorsEqual(gridColor[i][j], DARKGREEN)){
                    gridColor[i][j] = LIGHTGRAY;
                }
            }
        }
        path_length = 0;
        bfs_checks = 0;
        time_taken = 0;
        bfs(start, end, gridColor);
    }

}

int main(void) {
    
    
    InitWindow(screenWidth, screenHeight, "Pathfinding Visualizer");
    
    SetTargetFPS(60);
    
    std::vector<std::vector<Color>> gridColor(gridWidth / gridSize, std::vector<Color>(screenHeight / gridSize));
    std::vector<std::vector<Color>> walls(gridWidth / gridSize, std::vector<Color>(screenHeight / gridSize));
    for(int i = 0; i < gridWidth / gridSize; i += 1){
        for(int j = 0; j < screenHeight / gridSize; j += 1){
            gridColor[i][j] = LIGHTGRAY;
        }
    }    

    std::pair<int,int> start = {-1, -1},
                       end = {-1, -1}; // starting and ending points of our grid

    bool changeStarting = false;
    bool changeEnding = false;
    std::pair<int,int> prev_start = start, prev_end = end;
    
    while(!WindowShouldClose()){

        BeginDrawing();

        if (IsKeyPressed(KEY_S)){
            changeStarting = true;
        }
        if (IsKeyPressed(KEY_E)){
            changeEnding = true;
        }
        
        if (!changeStarting && !changeEnding && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            // Fill a cell
            // All cells in a visited
            Vector2 pos = GetMousePosition();
            int X = pos.x / gridSize;
            int Y = pos.y / gridSize;

            if (pos.x < gridWidth && pos.y < screenHeight 
                && !ColorsEqual(gridColor[X][Y], DARKRED)
                && !ColorsEqual(gridColor[X][Y], DARKGREEN)){
                gridColor[X][Y] = DARKGRAY;
                startBFS(start, end, prev_start, prev_end, gridColor, 1);
            }
        }   

        if (changeStarting == true){
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                Vector2 pos = GetMousePosition();
                int X = pos.x / gridSize;
                int Y = pos.y / gridSize;

                if (pos.x < gridWidth && pos.y < screenHeight){
                    if (!ColorsEqual(gridColor[X][Y], DARKRED)){
                        if (start != std::make_pair(-1, -1)){
                            gridColor[start.first][start.second] = LIGHTGRAY;
                        }
                        prev_start = start;
                        start = std::make_pair(X, Y);
                        gridColor[X][Y] = DARKGREEN;
                        changeStarting = false;
                    }
                }
            }
        } 

        if (changeEnding == true){
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                
                Vector2 pos = GetMousePosition();
                int X = pos.x / gridSize;
                int Y = pos.y / gridSize;

                if (pos.x < gridWidth && pos.y < screenHeight){
                    if (!ColorsEqual(gridColor[X][Y], DARKGREEN)){
                        if (end != std::make_pair(-1, -1)){
                            gridColor[end.first][end.second] = LIGHTGRAY;
                        }
                        prev_end = end;
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

                if (pos.x < gridWidth && pos.y < screenHeight
                    && ColorsEqual(gridColor[X][Y], DARKGRAY)
                    ){
                    // startBFS = true;
                    // std::cout << X << " " << Y << " \n";
                    // std::cout << ColorsEqual(gridColor[X][Y], DARKGRAY) << "\n";
                    // std::cout << ColorsEqual(gridColor[X][Y], DARKBLUE) << "\n";
                    gridColor[X][Y] = LIGHTGRAY;
                    startBFS(start, end, prev_start, prev_end, gridColor, 1);
                
                    // std::cout << ColorsEqual(gridColor[X][Y], DARKBLUE) << "\n";
                    // std::cout << ColorsEqual(gridColor[X][Y], LIGHTGRAY) << "\n";

                    // walls[X][Y] = LIGHTGRAY;
                    // std::cout << gridColor[X][Y] << "\n";
                }
            }    
        }

        if (IsKeyPressed(KEY_C)){
            for(int x = 0; x < gridWidth; x += gridSize){
                for(int y = 0; y < screenHeight; y += gridSize){
                    gridColor[x / gridSize][y / gridSize] = LIGHTGRAY;
                }
            }

            // reset start and end points
            start = std::make_pair(-1, -1);
            end = std::make_pair(-1, -1);
        }  
        
        startBFS(start, end, prev_start, prev_end, gridColor, 0);

        ClearBackground(RAYWHITE);

        for(int x = 0; x < gridWidth; x += gridSize){
            for(int y = 0; y < screenHeight; y += gridSize){
                DrawRectangle(x, y, gridSize, gridSize, gridColor[x/gridSize][y/gridSize]);
                DrawRectangleLines(x, y, gridSize, gridSize, DARKGRAY);
            }
        }

        DrawRectangle(screenWidth, 0, 200, screenHeight, Fade(RAYWHITE, 0.5f)); // Sidebar background
        DrawText("BFS", screenWidth - 190, 20, 20, DARKGRAY);
        DrawText(TextFormat("Checks: %d", bfs_checks+1), screenWidth - 190, 60, 20, DARKGRAY);
        DrawText(TextFormat("Path Length: %d", path_length), screenWidth - 190, 100, 20, DARKGRAY);
        DrawText(TextFormat("Time Taken: %.4f ms", time_taken), screenWidth - 190, 140, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
