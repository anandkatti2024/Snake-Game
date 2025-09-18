#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>

using namespace std;
int width = 20;
int height = 20;
int speed = 200;
int score = 0;
int highScore = 0;
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

// Console styling
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
WORD COLOR_DEFAULT = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
WORD COLOR_DIM = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
WORD COLOR_WALL = FOREGROUND_BLUE | FOREGROUND_GREEN; // cyan-ish
WORD COLOR_HEAD = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
WORD COLOR_BODY = FOREGROUND_GREEN;
WORD COLOR_FRUIT = FOREGROUND_RED | FOREGROUND_INTENSITY;
WORD COLOR_UI = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; // yellow
WORD COLOR_TITLE = FOREGROUND_GREEN | FOREGROUND_INTENSITY;

void setColor(WORD color) {
	SetConsoleTextAttribute(hConsole, color);
}

void resetColor() {
	SetConsoleTextAttribute(hConsole, COLOR_DEFAULT);
}

void setCursorVisible(bool visible) {
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 25;
	info.bVisible = visible;
	SetConsoleCursorInfo(hConsole, &info);
}

void initConsole() {
	SetConsoleTitleA("Snake Game");
	setCursorVisible(false);
}

int getConsoleWidth() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
		return csbi.srWindow.Right - csbi.srWindow.Left + 1;
	}
	return width + 6; // fallback
}

int getLeftPadding() {
	int consoleWidth = getConsoleWidth();
	int contentWidth = width + 2; // walls included
	int pad = (consoleWidth - contentWidth) / 2;
	if (pad < 0) pad = 0;
	return pad;
}

void printPadding(int count) {
	for (int i = 0; i < count; i++) cout << ' ';
}

void playEatSound() {
	Beep(900, 35);
}

void playGameOverSound() {
	Beep(400, 150);
	Beep(300, 220);
}

void loadHighScore() {
	ifstream in("highscore.dat", ios::in | ios::binary);
	if (in.good()) {
		in.read(reinterpret_cast<char*>(&highScore), sizeof(highScore));
	}
	in.close();
}

void saveHighScore() {
	ofstream out("highscore.dat", ios::out | ios::binary | ios::trunc);
	out.write(reinterpret_cast<const char*>(&highScore), sizeof(highScore));
	out.close();
}

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
    
    int pad = getLeftPadding();
    int level = (score / 50) + 1;

    // Header bar (centered)
    printPadding(max(0, pad - 6));
    setColor(COLOR_TITLE);
    cout << "=== SNAKE GAME === ";
    resetColor();
    setColor(COLOR_UI);
    cout << "Score: " << score << "  ";
    cout << "Level: " << level << "  ";
    cout << "High: " << highScore;
    if (paused) cout << "  [PAUSED]";
    resetColor();
    cout << endl;

    // Top wall
    printPadding(pad);
    setColor(COLOR_WALL);
    for (int i = 0; i < width + 2; i++) {
        cout << wall;
    }
    resetColor();
    cout << endl;
    
    // Game area
    for (int i = 0; i < height; i++) {
        printPadding(pad);
        for (int j = 0; j < width; j++) {
            if (j == 0) { setColor(COLOR_WALL); cout << wall; resetColor(); }
            
            // Check what to draw
            if (i == headY && j == headX) {
                setColor(COLOR_HEAD);
                cout << head;
                resetColor();
            }
            else if (i == fruitY && j == fruitX) {
                setColor(COLOR_FRUIT);
                cout << fruit;
                resetColor();
            }
            else {
                bool isTail = false;
                for (int k = 0; k < tailLength; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        setColor(COLOR_BODY);
                        cout << body;
                        resetColor();
                        isTail = true;
                        break;
                    }
                }
                if (!isTail) {
                    // Checkerboard hint background using '.' for contrast
                    if (((i + j) & 1) == 0) {
                        setColor(COLOR_DIM);
                        cout << '.';
                        resetColor();
                    } else {
                        cout << ' ';
                    }
                }
            }
            
            if (j == width - 1) { setColor(COLOR_WALL); cout << wall; resetColor(); }
        }
        cout << endl;
    }
    
    // Bottom wall
    printPadding(pad);
    setColor(COLOR_WALL);
    for (int i = 0; i < width + 2; i++) {
        cout << wall;
    }
    resetColor();
    cout << endl;

    // Controls hint line
    setColor(COLOR_DIM);
    printPadding(max(0, pad - 2));
    cout << "W/A/S/D: Move    P: Pause    X: Exit" << endl;
    resetColor();
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
        if (score > highScore) { highScore = score; saveHighScore(); }
        tailLength++;
        
        // Add new tail segment
        tailX.push_back(headX);
        tailY.push_back(headY);
        playEatSound();
        
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
    setColor(COLOR_TITLE);
    cout << "  ================================" << endl;
    cout << "  |          SNAKE GAME          |" << endl;
    cout << "  ================================" << endl;
    resetColor();
    setColor(COLOR_UI);
    cout << "  Controls:" << endl;
    resetColor();
    cout << "  W/A/S/D - Move" << endl;
    cout << "  P - Pause" << endl;
    cout << "  X - Exit" << endl;
    setColor(COLOR_UI);
    cout << "  High Score: "; resetColor(); cout << highScore << endl;
    setColor(COLOR_UI);
    cout << "\n  Press any key to start..." << endl;
    resetColor();
    cout << "\n\n";
}

void showGameOver() {
    clearScreen();
    cout << "\n\n";
    setColor(COLOR_FRUIT);
    cout << "  ================================" << endl;
    cout << "  |           GAME OVER!         |" << endl;
    cout << "  ================================" << endl;
    resetColor();
    setColor(COLOR_UI);
    cout << "  Final Score: ";
    resetColor();
    cout << score << endl;
    setColor(COLOR_UI);
    cout << "  High Score: "; resetColor(); cout << highScore << endl;
    setColor(COLOR_UI);
    cout << "\n  Press R to restart or any other key to exit..." << endl;
    resetColor();
    cout << "\n\n";
}

int main() {
    srand(time(0));
    initConsole();
    loadHighScore();
    
    bool restart = true;
    while (restart) {
        // Show menu
        showMenu();
        _getch();

        // Countdown
        clearScreen();
        for (int i = 3; i >= 1; --i) {
            setColor(COLOR_TITLE);
            cout << "\n\n            Starting in " << i << "...\n\n";
            resetColor();
            Sleep(600);
            clearScreen();
        }

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
        playGameOverSound();
        showGameOver();
        char ch = _getch();
        if (ch == 'r' || ch == 'R') {
            restart = true;
            gameOver = false;
        } else {
            restart = false;
        }
    }
    
    return 0;
}
