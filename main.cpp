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

int currentAlgo = 1;
int prev_currentAlgo = 0;

// implement bfs algorithm
int dr[4] = {-1, 0, 1, 0};
int dc[4] = {0, 1, 0, -1};
int checks_bfs = 0;
int checks_dijakstra = 0;
int checks_a_star = 0;

const int gridWidth = 1400;
const int screenWidth = gridWidth + 200;
const int screenHeight = 1200;
const int gridHeight = screenHeight;

int path_length_bfs = 0;
int path_length_dijakstra = 0;
int path_length_a_star = 0;

const int gridSize = 50; // Size of each grid cell;
float time_taken_bfs = 0.0f;    // Variable to store the time taken
float time_taken_dijakstra = 0.0f;    // Variable to store the time taken
float time_taken_a_star = 0.0f;    // Variable to store the time taken

void bfs(std::pair<int,int> &start, std::pair<int,int> &end, 
    std::vector<std::vector<Color>> &gridColor){
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
    auto start_time = std::chrono::high_resolution_clock::now();
    
    while(!q.empty()){
        int r = q.front()[0];
        int c = q.front()[1];
        int d = q.front()[2];

        if (end == std::make_pair(r, c)){
            path_length_bfs = d;
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
                checks_bfs++;
            }
            else if (ColorsEqual(gridColor[nr][nc], LIGHTGRAY)){
                checks_bfs++;
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
    time_taken_bfs = duration.count() * 1000;  // time in ms
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
        path_length_bfs = 0;
        checks_bfs = 0;
        time_taken_bfs = 0;
        prev_currentAlgo = currentAlgo;
        bfs(start, end, gridColor);
    }
}

void dijakstra(std::pair<int,int> &start, std::pair<int,int> &end, 
    std::vector<std::vector<Color>> &gridColor){
    std::priority_queue<std::vector<int>, std::vector<std::vector<int>>, std::greater<std::vector<int>>> q;
    q.push({0, start.first, start.second});

    std::pair<int,int> parent[gridWidth / gridSize][screenHeight / gridSize];
    int distance[gridWidth / gridSize][screenHeight / gridSize];
    for(int x = 0; x < gridWidth / gridSize; x++){
        for(int y = 0; y < screenHeight / gridSize; y++){
            distance[x][y] = 1e9;
            parent[x][y] = std::make_pair(-1, -1);
        }
    }
    
    distance[start.first][start.second] = 0;

    bool path_found = false;

    auto start_time = std::chrono::high_resolution_clock::now();

    while(!q.empty()){
        int d = q.top()[0];
        int r = q.top()[1];
        int c = q.top()[2];
        q.pop();

        // If the popped element is not the shortest known distance, skip it
        if (d > distance[r][c]) continue;

        // Check if we have reached the destination
        if (end == std::make_pair(r, c)){
            path_found = true;
            break;
        }

        for(int i=0; i<4; i++){
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (nr < 0 || nr >= gridWidth / gridSize || nc < 0 || nc >= screenHeight / gridSize) continue;
            if (ColorsEqual(gridColor[nr][nc], DARKGRAY)) continue; // Skip walls

            // Only consider this new path if it is better
            if (d + 1 < distance[nr][nc]) {
                distance[nr][nc] = d + 1;
                checks_dijakstra++;

                if (ColorsEqual(gridColor[nr][nc], LIGHTGRAY)){
                    gridColor[nr][nc] = DARKBLUE; // Mark as visited
                }
                
                parent[nr][nc] = std::make_pair(r, c);
                q.push({distance[nr][nc], nr, nc}); // Push the new distance
            }
        }
    }
    
    // Trace back the path if found
    if (path_found){
        gridColor[end.first][end.second] = DARKRED;
        std::pair<int, int> path = parent[end.first][end.second];
        while (path != start) {
            gridColor[path.first][path.second] = DARKYELLOW;
            path = parent[path.first][path.second];
        }
        gridColor[path.first][path.second] = DARKGREEN; // repainting start point
    }
    
    path_length_dijakstra = distance[end.first][end.second];
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end_time - start_time;
    time_taken_dijakstra = duration.count() * 1000;  // time in ms
}

void startDIJAKSTRA(std::pair<int,int> &start, std::pair<int,int> &end, 
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
        path_length_dijakstra = 0;
        checks_dijakstra = 0;
        time_taken_dijakstra = 0;
        prev_currentAlgo = currentAlgo;
        dijakstra(start, end, gridColor);
    }
}

void a_star(std::pair<int,int> &start, std::pair<int,int> &end, 
    std::vector<std::vector<Color>> &gridColor){
    std::priority_queue<std::vector<int>, std::vector<std::vector<int>>, std::greater<std::vector<int>>> q;

    auto heuristic = [&](std::pair<int,int> a, std::pair<int,int> b){
        return abs(a.first - b.first) + abs(a.second - b.second);
    };

    q.push({heuristic(start, end), 0, start.first, start.second}); // cost + heuristic, cost, row, col
    // cost = distance from start point
    // heuristic = some kind of heuristic we decide

    std::pair<int,int> parent[gridWidth / gridSize][screenHeight / gridSize];
    int distance[gridWidth / gridSize][screenHeight / gridSize];
    for(int x = 0; x < gridWidth / gridSize; x++){
        for(int y = 0; y < screenHeight / gridSize; y++){
            distance[x][y] = 1e9;
            parent[x][y] = std::make_pair(-1, -1);
        }
    }
    
    distance[start.first][start.second] = 0;

    bool path_found = false;
    auto start_time = std::chrono::high_resolution_clock::now();

    while(!q.empty()){
        int cost = q.top()[1];
        int r = q.top()[2];
        int c = q.top()[3];
        q.pop();

        // If the popped element is not the shortest known distance, skip it
        if (cost > distance[r][c]) continue;

        // Check if we have reached the destination
        if (end == std::make_pair(r, c)){
            path_found = true;
            break;
        }

        for(int i=0; i<4; i++){
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (nr < 0 || nr >= gridWidth / gridSize || nc < 0 || nc >= screenHeight / gridSize) continue;
            if (ColorsEqual(gridColor[nr][nc], DARKGRAY)) continue; // Skip walls

            // Only consider this new path if it is better
            if (cost + 1 < distance[nr][nc]) {
                distance[nr][nc] = cost + 1;
                checks_a_star++;

                if (ColorsEqual(gridColor[nr][nc], LIGHTGRAY)){
                    gridColor[nr][nc] = DARKBLUE; // Mark as visited
                }
                
                parent[nr][nc] = std::make_pair(r, c);
                q.push({cost + 1 + heuristic({nr, nc}, end), cost + 1, nr, nc}); // Push the new distance
            }
        }
    }
    
    // Trace back the path if found
    if (path_found){
        gridColor[end.first][end.second] = DARKRED;
        std::pair<int, int> path = parent[end.first][end.second];
        while (path != start) {
            gridColor[path.first][path.second] = DARKYELLOW;
            path = parent[path.first][path.second];
        }
        gridColor[path.first][path.second] = DARKGREEN; // repainting start point
    }
    
    path_length_a_star = distance[end.first][end.second];
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end_time - start_time;
    time_taken_a_star = duration.count() * 1000;  // time in ms
}

void startA_STAR(std::pair<int,int> &start, std::pair<int,int> &end, 
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
        path_length_a_star = 0;
        checks_a_star = 0;
        time_taken_a_star = 0;
        prev_currentAlgo = currentAlgo;
        a_star(start, end, gridColor);
    }
}


int main(void) {
    
    InitWindow(screenWidth, screenHeight, "Pathfinding Visualizer");
    
    SetTargetFPS(60);
    
    std::vector<std::vector<std::vector<Color>>> gridColor(4, std::vector<std::vector<Color>>(gridWidth / gridSize, std::vector<Color>(gridHeight / gridSize)));
    for(int k=1; k<4; k++){
        for(int i = 0; i < gridWidth / gridSize; i += 1){
            for(int j = 0; j < screenHeight / gridSize; j += 1){
                    gridColor[k][i][j] = LIGHTGRAY;
            }
        }    
    }

    std::pair<int,int> prev_start[4] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
                       prev_end[4] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}}; // prev starting and ending points of our grid

    bool changeStarting = false;
    bool changeEnding = false;
    std::pair<int,int> start[4] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
                       end[4] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}}; // starting and ending points of our grid
    
    while(!WindowShouldClose()){

        BeginDrawing();

        if (IsKeyPressed(KEY_S)){
            changeStarting = true;
        }
        if (IsKeyPressed(KEY_E)){
            changeEnding = true;
        }

        if (IsKeyPressed(KEY_ONE)){
            currentAlgo = 1;
            // A* algo
        }

        if (IsKeyPressed(KEY_TWO)){
            currentAlgo = 2;
            // Dijakstra
        }

        if (IsKeyPressed(KEY_THREE)){
            currentAlgo = 3;
            // BFS
        }
        
        if (!changeStarting && !changeEnding && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            // Fill a cell
            // All cells in a visited
            Vector2 pos = GetMousePosition();
            int X = pos.x / gridSize;
            int Y = pos.y / gridSize;
        
            for(int k=1; k<4; k++){
                if (pos.x < gridWidth && pos.y < screenHeight 
                    && !ColorsEqual(gridColor[k][X][Y], DARKRED)
                    && !ColorsEqual(gridColor[k][X][Y], DARKGREEN)){
                    gridColor[k][X][Y] = DARKGRAY;
                    startBFS(start[1] , end[1], prev_start[1], prev_end[1], gridColor[1], 1);
                    startDIJAKSTRA(start[2], end[2], prev_start[2], prev_end[2] , gridColor[2], 1);
                    startA_STAR(start[3], end[3], prev_start[3], prev_end[3], gridColor[3], 1);
                }
            }
        }   

        if (changeStarting == true){
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                Vector2 pos = GetMousePosition();
                int X = pos.x / gridSize;
                int Y = pos.y / gridSize;
            
                for(int k=1; k<4; k++){
                    if (pos.x < gridWidth && pos.y < screenHeight){
                        if (!ColorsEqual(gridColor[k][X][Y], DARKRED)){
                            if (start[k] != std::make_pair(-1, -1)){
                                gridColor[k][start[k].first][start[k].second] = LIGHTGRAY;
                            }
                            prev_start[k] = start[k];
                            start[k] = std::make_pair(X, Y);
                            gridColor[k][X][Y] = DARKGREEN;
                            changeStarting = false;
                            prev_currentAlgo = -1;
                        }
                    }
                }
            }
        } 

        if (changeEnding == true){
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                
                Vector2 pos = GetMousePosition();
                int X = pos.x / gridSize;
                int Y = pos.y / gridSize;
                
                for(int k=1; k<4; k++){
                    if (pos.x < gridWidth && pos.y < screenHeight){
                        if (!ColorsEqual(gridColor[k][X][Y], DARKGREEN)){
                            if (end[k] != std::make_pair(-1, -1)){
                                gridColor[k][end[k].first][end[k].second] = LIGHTGRAY;
                            }
                            prev_end[k] = end[k];
                            end[k] = std::make_pair(X, Y);
                            gridColor[k][X][Y] = DARKRED;
                            changeEnding = false;
                        }
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
                
                for(int k=1; k<4; k++){
                    if (pos.x < gridWidth && pos.y < screenHeight
                        && ColorsEqual(gridColor[k][X][Y], DARKGRAY)
                        ){
                        gridColor[k][X][Y] = LIGHTGRAY;
                        startBFS(start[1] , end[1], prev_start[1], prev_end[1], gridColor[1], 1);
                        startDIJAKSTRA(start[2], end[2], prev_start[2], prev_end[2] , gridColor[2], 1);
                        startA_STAR(start[3], end[3], prev_start[3], prev_end[3], gridColor[3], 1);
                    }
                }
            }    
        }

        if (IsKeyPressed(KEY_C)){
            for(int k=1; k<4; k++){
                for(int x = 0; x < gridWidth; x += gridSize){
                    for(int y = 0; y < screenHeight; y += gridSize){
                        gridColor[k][x / gridSize][y / gridSize] = LIGHTGRAY;
                    }
                }
            }

            // reset start and end points
            for(int k=1; k<4; k++){
                start[k] = std::make_pair(-1, -1);
                end[k] = std::make_pair(-1, -1);
            }
        }  
        
        startBFS(start[1] , end[1], prev_start[1], prev_end[1], gridColor[1], 0);
        startDIJAKSTRA(start[2], end[2], prev_start[2], prev_end[2] , gridColor[2], 0);
        startA_STAR(start[3], end[3], prev_start[3], prev_end[3], gridColor[3], 0);
        // for(int i=0; i<gridWidth/gridSize; i++){
        //     for(int j=0; j<gridHeight/gridSize; j++){
        //         gridColor[i][j] = gridColor2[i][j];
        //     }
        // }
        // startDIJAKSTRA(start, end, prev_start, prev_end, gridColor, 0, (currentAlgo == 2 ? 0 : 1));
        // for(int i=0; i<gridWidth/gridSize; i++){
        //     for(int j=0; j<gridHeight/gridSize; j++){
        //         gridColor[i][j] = gridColor2[i][j];
        //     }
        // }

        ClearBackground(RAYWHITE);

        for(int x = 0; x < gridWidth; x += gridSize){
            for(int y = 0; y < screenHeight; y += gridSize){
                DrawRectangle(x, y, gridSize, gridSize, gridColor[currentAlgo][x/gridSize][y/gridSize]);
                DrawRectangleLines(x, y, gridSize, gridSize, DARKGRAY);
            }
        }

        DrawRectangle(screenWidth, 0, 200, screenHeight, Fade(RAYWHITE, 0.5f)); // Sidebar background
        DrawText("BFS", screenWidth - 190, 20, 20, DARKGRAY);
        DrawText(TextFormat("Checks: %d", checks_bfs), screenWidth - 190, 60, 20, DARKGRAY);
        DrawText(TextFormat("Path Length: %d", path_length_bfs), screenWidth - 190, 100, 20, DARKGRAY);
        DrawText(TextFormat("Time Taken: %.4f ms", time_taken_bfs), screenWidth - 190, 140, 20, DARKGRAY);

        DrawText("Dijkstra", screenWidth - 190, 200, 20, DARKGRAY);
        DrawText(TextFormat("Checks: %d", checks_dijakstra), screenWidth - 190, 240, 20, DARKGRAY);
        DrawText(TextFormat("Path Length: %d", path_length_dijakstra), screenWidth - 190, 280, 20, DARKGRAY);
        DrawText(TextFormat("Time Taken: %.4f ms", time_taken_dijakstra), screenWidth - 190, 320, 20, DARKGRAY);

        DrawText("A star", screenWidth - 190, 380, 20, DARKGRAY);
        DrawText(TextFormat("Checks: %d", checks_a_star), screenWidth - 190, 420, 20, DARKGRAY);
        DrawText(TextFormat("Path Length: %d", path_length_a_star), screenWidth - 190, 460, 20, DARKGRAY);
        DrawText(TextFormat("Time Taken: %.4f ms", time_taken_a_star), screenWidth - 190, 500, 20, DARKGRAY);


        EndDrawing();
    }

    CloseWindow();

    return 0;
}
