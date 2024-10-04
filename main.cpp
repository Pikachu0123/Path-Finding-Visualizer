#include "raylib.h"
#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <cmath>
// We cant compare two Color directly, since each has:
// r (red)
// g (green)
// b (blue)
// a (alpha) --> transparency level of the color. 255 means opaque and 0 transparent
bool ColorsEqual(Color color1, Color color2) {
    return (color1.r == color2.r) && (color1.g == color2.g) && 
           (color1.b == color2.b) && (color1.a == color2.a);
}

struct Cell {
    int x, y;
    bool isWall;
};

int found_bfs_path = 0;
int found_dijakstra_path = 0;
int found_a_star_path = 0;
int path_length_bfs = 0;
int path_length_dijakstra = 0;
int path_length_a_star = 0;

Color DARKRED = { 128, 0, 0, 255 };  // Dark red color
Color DARKYELLOW = { 204, 204, 0, 255 };  // Dark yellow color

int currentAlgo = 1;
// by default, A*

int dr[8] = {-1, 0, 1, 0, -1, 1, -1, 1};
int dc[8] = {0, 1, 0, -1, -1, -1, 1, 1};

const int gridWidth = 1400;
const int screenWidth = gridWidth + 200;
const int screenHeight = 1200;
const int gridHeight = screenHeight;
const int gridSize = 50; // Size of each grid cell;

struct Node{
    double total_cost; // heuristic + cost
    double cost; // cost = distance from source
    int r;
    int c;

    Node(double _total_cost, double _cost, int _r, int _c) : total_cost(_total_cost), cost(_cost), r(_r), c(_c) {}
};

auto dist = [](std::pair<int,int> a, std::pair<int,int> b) ->double{
    return sqrt((a.first-b.first)*(a.first-b.first)+(a.second-b.second)*(a.second-b.second));
};

auto Compare = [](Node a, Node b) {
    if (a.total_cost == b.total_cost) return a.cost > b.cost;
    return a.total_cost > b.total_cost;
};

auto heuristic = [](std::pair<int,int> a, std::pair<int,int> b) {
    return dist(a, b);
    // return abs(a.first - b.first) + abs(a.second - b.second);
};

std::priority_queue<Node, std::vector<Node>, decltype(Compare)> q_dijakstra(Compare);
std::priority_queue<Node, std::vector<Node>, decltype(Compare)> q_a_star(Compare);


void bfsStep(std::pair<int,int> &start, 
    std::pair<int,int> &end, 
    std::vector<std::vector<Color>> &gridColor,
    std::queue<std::vector<int>> &q,
    std::vector<std::vector<int>> &vis,
    std::vector<std::vector<std::pair<int,int>>> &par,
    std::pair<int,int> &path){

    if (q.empty() || found_bfs_path) return;

    int r = q.front()[0];
    int c = q.front()[1];
    int cost = q.front()[2];

    if (end == std::make_pair(r, c)){
        found_bfs_path = 1;
    }

    q.pop();
    if (!found_bfs_path){
        for(int i=0; i<8; i++){
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (nr < 0 || nr >= gridWidth / gridSize || nc < 0 || nc >= screenHeight / gridSize) continue;
            if (vis[nr][nc]) continue;
            if (vis[end.first][end.second]) continue;
            if (ColorsEqual(gridColor[nr][nc], DARKGRAY)) continue;
            if (i>3 && ColorsEqual(gridColor[nr][c], DARKGRAY) && ColorsEqual(gridColor[r][nc], DARKGRAY)) continue;

            vis[nr][nc] = 1;
            if (ColorsEqual(gridColor[nr][nc], LIGHTGRAY)){
                gridColor[nr][nc] = DARKBLUE;
            }
            par[nr][nc] = std::make_pair(r, c);
            q.push({nr, nc, cost + 1});
        }
    }

    else{
        gridColor[end.first][end.second] = DARKRED;
        path = par[path.first][path.second];
        path_length_bfs++;
        while (path != start) {
            path_length_bfs++;
            gridColor[path.first][path.second] = DARKYELLOW;
            path = par[path.first][path.second];
        }
        gridColor[path.first][path.second] = DARKGREEN; // repainting start point
        found_bfs_path = 2;
    }
}

void dijakstraStep(std::pair<int,int> &start, 
    std::pair<int,int> &end, 
    std::vector<std::vector<Color>> &gridColor,
    std::vector<std::vector<double>> &distance,
    std::vector<std::vector<std::pair<int,int>>> &par,
    std::pair<int,int> &path){

    if (q_dijakstra.empty() || found_dijakstra_path) return;

    Node node = q_dijakstra.top();
    q_dijakstra.pop();

    double cost = node.cost;
    int r = node.r;
    int c = node.c;
    // If the popped element is not the shortest known distance, skip it
    if (cost != distance[r][c]) return;

    // Check if we have reached the destination
    if (end == std::make_pair(r, c)){
        found_dijakstra_path = 1;
    }
    
    if (!found_dijakstra_path){
        for(int i=0; i<8; i++){
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (nr < 0 || nr >= gridWidth / gridSize || nc < 0 || nc >= screenHeight / gridSize) continue;
            if (ColorsEqual(gridColor[nr][nc], DARKGRAY)) continue; // Skip walls
            if (i>3 && ColorsEqual(gridColor[nr][c], DARKGRAY) && ColorsEqual(gridColor[r][nc], DARKGRAY)) continue;

            // Only consider this new path if it is better
            double new_cost = cost + dist({nr, nc}, {r, c});
            if (new_cost < distance[nr][nc]) {
                distance[nr][nc] = new_cost;

                if (ColorsEqual(gridColor[nr][nc], LIGHTGRAY)){
                    gridColor[nr][nc] = DARKBLUE; // Mark as visited
                }
                
                par[nr][nc] = std::make_pair(r, c);
                q_dijakstra.push(Node(new_cost, new_cost, nr, nc));

            }
        }
    }

    else{
        path_length_dijakstra++;
        gridColor[end.first][end.second] = DARKRED;
        path = par[path.first][path.second];
        while (path != start) {
            path_length_dijakstra++;
            gridColor[path.first][path.second] = DARKYELLOW;
            path = par[path.first][path.second];
        }
        gridColor[path.first][path.second] = DARKGREEN; // repainting start point
        found_dijakstra_path = 2;
    }
}

void aStarStep(std::pair<int,int> &start, 
    std::pair<int,int> &end, 
    std::vector<std::vector<Color>> &gridColor,
    std::vector<std::vector<double>> &distance,
    std::vector<std::vector<std::pair<int,int>>> &par,
    std::pair<int,int> &path){

    if (q_a_star.empty() || found_a_star_path) return;

    Node node = q_a_star.top();
    q_a_star.pop();

    double cost = node.cost;
    int r = node.r;
    int c = node.c;
    // If the popped element is not the shortest known distance, skip it
    if (cost != distance[r][c]) return;

    // Check if we have reached the destination
    if (end == std::make_pair(r, c)){
        found_a_star_path = 1;
    }
    
    if (!found_a_star_path){
        for(int i=0; i<8; i++){
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (nr < 0 || nr >= gridWidth / gridSize || nc < 0 || nc >= screenHeight / gridSize) continue;
            if (ColorsEqual(gridColor[nr][nc], DARKGRAY)) continue; // Skip walls
            if (i>3 && ColorsEqual(gridColor[nr][c], DARKGRAY) && ColorsEqual(gridColor[r][nc], DARKGRAY)) continue;

            // Only consider this new path if it is better
            double new_cost = cost + dist({nr, nc}, {r, c});
            if (new_cost < distance[nr][nc]) {
                distance[nr][nc] = new_cost;

                if (ColorsEqual(gridColor[nr][nc], LIGHTGRAY)){
                    gridColor[nr][nc] = DARKBLUE; // Mark as visited
                }
                
                par[nr][nc] = std::make_pair(r, c);
                q_a_star.push(Node(new_cost + heuristic({nr, nc}, end), new_cost, nr, nc));
            }
        }
    }

    else{
        path_length_a_star++;
        gridColor[end.first][end.second] = DARKRED;
        path = par[path.first][path.second];
        while (path != start) {
            path_length_a_star++;
            gridColor[path.first][path.second] = DARKYELLOW;
            path = par[path.first][path.second];
        }
        gridColor[path.first][path.second] = DARKGREEN; // repainting start point
        found_a_star_path = 2;
    }
}


void generateMaze(std::vector<std::vector<Color>> &gridColor) {
    static std::vector<Cell> walls; // Stores walls
    static bool initialized = false;
    
    if (!initialized) {
        // Initialize the grid
        for (int x = 0; x < gridWidth / gridSize; x++) {
            for (int y = 0; y < screenHeight / gridSize; y++) {
                gridColor[x][y] = DARKGRAY; // Set all cells as walls
            }
        }

        // Randomly select a starting cell
        int startX = rand() % (gridWidth / gridSize);
        int startY = rand() % (screenHeight / gridSize);
        gridColor[startX][startY] = LIGHTGRAY; // Start cell
        walls.push_back({startX, startY, false}); // Mark it as a cell

        initialized = true;
    }

    if (!walls.empty()) {
        // Randomly select a wall
        int randomIndex = rand() % walls.size();
        Cell currentWall = walls[randomIndex];

        // Check if the wall can be turned into a path
        int directions[4][2] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} }; // Down, Right, Up, Left
        std::vector<Cell> newCells;

        for (auto& dir : directions) {
            int newX = currentWall.x + dir[0];
            int newY = currentWall.y + dir[1];

            if (newX >= 0 && newX < gridWidth / gridSize && newY >= 0 && newY < screenHeight / gridSize) {
                if (ColorsEqual(gridColor[newX][newY], DARKGRAY)) {
                    int countAdjacentPaths = 0;

                    for (auto& d : directions) {
                        int checkX = newX + d[0];
                        int checkY = newY + d[1];
                        if (checkX >= 0 && checkX < gridWidth / gridSize && checkY >= 0 && checkY < screenHeight / gridSize) {
                            if (ColorsEqual(gridColor[checkX][checkY], LIGHTGRAY)) {
                                countAdjacentPaths++;
                            }
                        }
                    }

                    if (countAdjacentPaths == 1) { // Only one adjacent path
                        gridColor[newX][newY] = LIGHTGRAY; // Create a path
                        newCells.push_back({newX, newY, false}); // Add it to new cells
                    }
                }
            }
        }

        // Remove the wall from the list
        walls.erase(walls.begin() + randomIndex);

        // Add the new walls to the list
        walls.insert(walls.end(), newCells.begin(), newCells.end());
    }
}

void resetMaze(std::vector<std::vector<Color>> &gridColor) {
    for (int x = 0; x < gridWidth / gridSize; x++) {
        for (int y = 0; y < screenHeight / gridSize; y++) {
            gridColor[x][y] = DARKGRAY; // Reset all cells to walls
        }
    }
}

int main(void) {
    
    InitWindow(screenWidth, screenHeight, "Pathfinding Visualizer");
    
    SetTargetFPS(60);

    srand(static_cast<unsigned int>(time(nullptr))); // Seed for random number generation

    std::vector<std::vector<std::vector<Color>>> gridColor(4, std::vector<std::vector<Color>>(gridWidth / gridSize, std::vector<Color>(gridHeight / gridSize)));
    for(int k=1; k<4; k++){
        for(int i = 0; i < gridWidth / gridSize; i += 1){
            for(int j = 0; j < screenHeight / gridSize; j += 1){
                    gridColor[k][i][j] = LIGHTGRAY;
            }
        }    
    }

    bool changeStarting = false;
    bool changeEnding = false;

    std::pair<int,int> start[4];
    std::pair<int,int> end[4];

    for(int i=0; i<4; i++){
        start[i] = std::make_pair(-1, -1);
        end[i] = std::make_pair(-1, -1);
    }

    // Rectangle generateMaze = {screenWidth - 50, screenHeight - 50, 50, 50};

    const Rectangle generateMazeButton = { screenWidth - 200, 500, 130, 40 }; // Button position and size
    bool buttonPressed = false;
    bool running_bfs = false;
    bool running_dijakstra = false;
    bool running_a_star = false;
    const double stepDelay = 1.0 / 100.0; // Time between steps (1 FPS for visualization)
    double timeSinceLastStep = 0.0;

    std::queue<std::vector<int>> q_bfs; // queue for bfs
    std::vector<std::vector<int>> vis_bfs(gridWidth/gridSize, std::vector<int>(gridHeight/gridSize));
    std::vector<std::vector<std::pair<int,int>>> parent_bfs(gridWidth/gridSize, std::vector<std::pair<int,int>>(gridHeight/gridSize));

    auto compare = [](Node a, Node b){
        if (a.total_cost == b.total_cost) return a.cost > b.cost;
        return a.total_cost > b.total_cost;
    };

    std::vector<std::vector<double>> dis_dijakstra(gridWidth/gridSize, std::vector<double>(gridHeight/gridSize));
    std::vector<std::vector<std::pair<int,int>>> parent_dijakstra(gridWidth/gridSize, std::vector<std::pair<int,int>>(gridHeight/gridSize));

    std::vector<std::vector<double>> dis_a_star(gridWidth/gridSize, std::vector<double>(gridHeight/gridSize));
    std::vector<std::vector<std::pair<int,int>>> parent_a_star(gridWidth/gridSize, std::vector<std::pair<int,int>>(gridHeight/gridSize));

    bool reset = false;

    while(!WindowShouldClose()){

        if (CheckCollisionPointRec(GetMousePosition(), generateMazeButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            for(int k=1; k<4; k++)
                resetMaze(gridColor[k]); // Reset maze
            buttonPressed = true; // Indicate button was pressed
        }

        if (buttonPressed) {
            // Generate the maze
            generateMaze(gridColor[1]);
            for(int k=2; k<4; k++){
                for(int i=0; i<gridWidth/gridSize;i++){
                    for(int j=0; j<gridHeight/gridSize;j++){
                        gridColor[k][i][j] = gridColor[1][i][j];
                    }
                }
            }
        }

        if (IsKeyPressed(KEY_S)){
            changeStarting = true;
        }
        if (IsKeyPressed(KEY_E)){
            changeEnding = true;
        }

        if (IsKeyPressed(KEY_R)){
            reset = true;
        }

        if (IsKeyPressed(KEY_ONE)){
            currentAlgo = 1;
            // BFS algo
        }

        if (IsKeyPressed(KEY_TWO)){
            currentAlgo = 2;
            // Dijakstra
        }

        if (IsKeyPressed(KEY_THREE)){
            currentAlgo = 3;
            // A* algo
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
                            start[k] = std::make_pair(X, Y);
                            gridColor[k][X][Y] = DARKGREEN;
                            changeStarting = false;
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

        if (currentAlgo == 1 && !running_bfs && reset){
            if (start[1] != std::make_pair(-1, -1) && end[1] != std::make_pair(-1, -1)){
                running_bfs = true;
                for(int i = 0; i < gridWidth / gridSize; i += 1){
                    for(int j = 0; j < screenHeight / gridSize; j += 1){
                        if (!ColorsEqual(gridColor[1][i][j], DARKGRAY)
                            && !ColorsEqual(gridColor[1][i][j], DARKRED)
                            && !ColorsEqual(gridColor[1][i][j], DARKGREEN)){
                            gridColor[1][i][j] = LIGHTGRAY;
                        }

                        vis_bfs[i][j] = 0;
                        parent_bfs[i][j] = std::make_pair(-1, -1);
                    }
                }
                while(q_bfs.size() > 0)
                    q_bfs.pop();
                q_bfs.push({start[1].first, start[1].second, 0});
                vis_bfs[start[1].first][start[1].second] = 1;
            }
        }

        if (running_bfs){
            timeSinceLastStep += GetFrameTime();
            if (timeSinceLastStep >= stepDelay) {
                timeSinceLastStep -= stepDelay; // Reset time
                bfsStep(start[1], end[1], gridColor[1], q_bfs, vis_bfs, parent_bfs, end[1]); // Run BFS step
                if (found_bfs_path == 2){
                    reset = false;
                    found_bfs_path = false;
                    running_bfs = false; // Stop BFS if end is reached
                }
            }
        }

        if (currentAlgo == 2 && !running_dijakstra && reset){
            if (start[2] != std::make_pair(-1, -1) && end[2] != std::make_pair(-1, -1)){
                running_dijakstra = true;
                for(int i = 0; i < gridWidth / gridSize; i += 1){
                    for(int j = 0; j < screenHeight / gridSize; j += 1){
                        if (!ColorsEqual(gridColor[2][i][j], DARKGRAY)
                            && !ColorsEqual(gridColor[2][i][j], DARKRED)
                            && !ColorsEqual(gridColor[2][i][j], DARKGREEN)){
                            gridColor[2][i][j] = LIGHTGRAY;
                        }
                        
                        dis_dijakstra[i][j] = 1e9;
                        parent_dijakstra[i][j] = std::make_pair(-1, -1);
                    }
                }
                while(q_dijakstra.size() > 0)
                    q_dijakstra.pop();
                dis_dijakstra[start[2].first][start[2].second] = 0; 
                q_dijakstra.push(Node(0, 0, start[2].first, start[2].second));
            }
        }

        if (running_dijakstra){
            timeSinceLastStep += GetFrameTime();
            if (timeSinceLastStep >= stepDelay) {
                timeSinceLastStep -= stepDelay;
                dijakstraStep(start[2], end[2], gridColor[2], dis_dijakstra, parent_dijakstra, end[2]); // Run BFS step
                if (found_dijakstra_path == 2){
                    reset = false;
                    found_dijakstra_path = 0;
                    running_dijakstra = false; // Stop BFS if end is reached
                }
            }
        }

        if (currentAlgo == 3 && !running_a_star && reset){
            if (start[3] != std::make_pair(-1, -1) && end[3] != std::make_pair(-1, -1)){
                running_a_star = true;
                for(int i = 0; i < gridWidth / gridSize; i += 1){
                    for(int j = 0; j < screenHeight / gridSize; j += 1){
                        if (!ColorsEqual(gridColor[3][i][j], DARKGRAY)
                            && !ColorsEqual(gridColor[3][i][j], DARKRED)
                            && !ColorsEqual(gridColor[3][i][j], DARKGREEN)){
                            gridColor[3][i][j] = LIGHTGRAY;
                        }
                        
                        dis_a_star[i][j] = 1e9;
                        parent_a_star[i][j] = std::make_pair(-1, -1);
                    }
                }
                while(q_a_star.size() > 0)
                    q_a_star.pop();
                dis_a_star[start[3].first][start[3].second] = 0; 
                q_a_star.push(Node(heuristic(start[3], end[3]), 0, start[3].first, start[3].second));
            }
        }

        if (running_a_star){
            timeSinceLastStep += GetFrameTime();
            if (timeSinceLastStep >= stepDelay) {
                timeSinceLastStep -= stepDelay;
                aStarStep(start[3], end[3], gridColor[3], dis_a_star, parent_a_star, end[3]); // Run BFS step
                if (found_a_star_path == 2){
                    reset = false;
                    found_a_star_path = 0;
                    running_a_star = false; // Stop BFS if end is reached
                }
            }
        }

        ClearBackground(RAYWHITE);

        BeginDrawing();

        DrawRectangleRec(generateMazeButton, DARKGRAY);
        DrawText("Generate Maze", generateMazeButton.x + 10, generateMazeButton.y + 5, 20, WHITE);

        for(int x = 0; x < gridWidth; x += gridSize){
            for(int y = 0; y < screenHeight; y += gridSize){
                DrawRectangle(x, y, gridSize, gridSize, gridColor[currentAlgo][x/gridSize][y/gridSize]);
                DrawRectangleLines(x, y, gridSize, gridSize, DARKGRAY);
            }
        }

        DrawRectangle(screenWidth, 0, 200, screenHeight, Fade(RAYWHITE, 0.5f)); // Sidebar background
        if (currentAlgo == 1){
            DrawText("BFS", screenWidth - 190, 20, 20, DARKGRAY);
            DrawText(TextFormat("Path Length: %d", path_length_bfs), screenWidth - 190, 60, 20, DARKGRAY);
        }
        
        else if (currentAlgo == 2){
            DrawText("Dijakstra", screenWidth - 190, 20, 20, DARKGRAY);
            DrawText(TextFormat("Path Length: %d", path_length_dijakstra), screenWidth - 190, 60, 20, DARKGRAY);
        }
        
        else{
            DrawText("A* Algo", screenWidth - 190, 20, 20, DARKGRAY);
            DrawText(TextFormat("Path Length: %d", path_length_a_star), screenWidth - 190, 60, 20, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
