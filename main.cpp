#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;
int width = 20;
int height = 20;
int speed = 200;
int score = 0;
bool gameOver = false;
bool paused = false;

// Snake stuff
int headX, headY;
vector<int> tailX, tailY;
int tailLength = 0;
int dir = 0; //

// Fruit stuff
int fruitX, fruitY;

// Characters
char head = 'O';
char body = 'o';
char fruit = 'F';
char wall = '#';

void setup() {
    gameOver = false;
    paused = false;
    score = 0;
    speed = 200;
    tailLength = 0;
    dir = 0;
    
    // Start snake in middle
    headX = width / 2;
    headY = height / 2;
    
    // Clear tail
    tailX.clear();
    tailY.clear();
    
    // Place fruit
    fruitX = rand() % width;
    fruitY = rand() % height;
}

void clearScreen() {
    COORD pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void draw() {
    clearScreen();
    
    // Top wall
    for (int i = 0; i < width + 2; i++) {
        cout << wall;
    }
    cout << endl;
    
    // Game area
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) cout << wall;
            
            // Check what to draw
            if (i == headY && j == headX) {
                cout << head;
            }
            else if (i == fruitY && j == fruitX) {
                cout << fruit;
            }
            else {
                bool isTail = false;
                for (int k = 0; k < tailLength; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        cout << body;
                        isTail = true;
                        break;
                    }
                }
                if (!isTail) cout << " ";
            }
            
            if (j == width - 1) cout << wall;
        }
        cout << endl;
    }
    
    // Bottom wall
    for (int i = 0; i < width + 2; i++) {
        cout << wall;
    }
    cout << endl;
    
    cout << "Score: " << score;
    if (paused) cout << " [PAUSED]";
    cout << endl;
}

void input() {
    if (_kbhit()) {
        char key = _getch();
        
        switch (key) {
            case 'a':
                if (dir != 2) dir = 1; // left
                break;
            case 'd':
                if (dir != 1) dir = 2; // right
                break;
            case 'w':
                if (dir != 4) dir = 3; // up
                break;
            case 's':
                if (dir != 3) dir = 4; // down
                break;
            case 'p':
                paused = !paused;
                break;
            case 'x':
                gameOver = true;
                break;
        }
    }
}

void moveSnake() {
    if (dir == 0) return; // not moving
    
    // Store old head position
    int prevX = headX;
    int prevY = headY;
    
    // Move head
    switch (dir) {
        case 1: headX--; break; // left
        case 2: headX++; break; // right
        case 3: headY--; break; // up
        case 4: headY++; break; // down
    }
    
    // Move tail
    if (tailLength > 0) {
        tailX.erase(tailX.begin());
        tailY.erase(tailY.begin());
        tailX.push_back(prevX);
        tailY.push_back(prevY);
    }
}

void checkCollisions() {
    // Hit walls
    if (headX < 0 || headX >= width || headY < 0 || headY >= height) {
        gameOver = true;
        return;
    }
    
    // Hit self
    for (int i = 0; i < tailLength; i++) {
        if (tailX[i] == headX && tailY[i] == headY) {
            gameOver = true;
            return;
        }
    }
}

void checkFruit() {
    if (headX == fruitX && headY == fruitY) {
        score += 10;
        tailLength++;
        
        // Add new tail segment
        tailX.push_back(headX);
        tailY.push_back(headY);
        
        // New fruit position
        bool goodSpot = false;
        while (!goodSpot) {
            fruitX = rand() % width;
            fruitY = rand() % height;
            
            goodSpot = true;
            if (fruitX == headX && fruitY == headY) goodSpot = false;
            
            for (int i = 0; i < tailLength; i++) {
                if (tailX[i] == fruitX && tailY[i] == fruitY) {
                    goodSpot = false;
                    break;
                }
            }
        }
        
        // Speed up
        if (speed > 50) {
            speed -= 10;
        }
    }
}

void showMenu() {
    clearScreen();
    cout << "\n\n";
    cout << "  ================================" << endl;
    cout << "  |        SNAKE GAME           |" << endl;
    cout << "  ================================" << endl;
    cout << "  Controls:" << endl;
    cout << "  W/A/S/D - Move" << endl;
    cout << "  P - Pause" << endl;
    cout << "  X - Exit" << endl;
    cout << "  Press any key to start..." << endl;
    cout << "\n\n";
}

void showGameOver() {
    clearScreen();
    cout << "\n\n";
    cout << "  ================================" << endl;
    cout << "  |        GAME OVER!           |" << endl;
    cout << "  ================================" << endl;
    cout << "  Final Score: " << score << endl;
    cout << "  Press any key to exit..." << endl;
    cout << "\n\n";
}

int main() {
    srand(time(0));
    
    // Show menu
    showMenu();
    _getch();
    
    // Start game
    setup();
    
    // Game loop
    while (!gameOver) {
        draw();
        input();
        
        if (!paused) {
            moveSnake();
            checkCollisions();
            checkFruit();
        }
        
        Sleep(speed);
    }
    
    // Game over screen
    showGameOver();
    _getch();
    
    return 0;
}
