/* Simple tetris clone using the terminal and ASCII characters */
#include <iostream>
#include <thread>
#include <Windows.h>
#include <vector>
using namespace std;

wstring tetris[7];
const int BOARD_WIDTH = 12;
const int BOARD_HEIGHT = 18;
const int SCREEN_WIDTH = 95;     /* Console window screen size X (coloums) */
const int SCREEN_HEIGHT = 95;    /* Console window screen size Y (rows)   */
unsigned char *pBoard = nullptr; /* pBoard stores the elements of the board */

/* Setup function prototypes */
int Rotate(int px, int py, int r);
bool PieceFits(int tp, int r, int px, int py);

int main()
{
    bool gameOver = false;
    bool keyHeld = false;               /* Flag if key is held         */
    bool bKey[4];                       /* Input keys for the game     */
    bool forceDown = false;             /* Flag if piece should be forced down */
    int currentPiece = 1;               /* What piece is falling       */
    int currentRotation = 0;            /* Rotation of current piece   */
    int currentX = BOARD_WIDTH / 2;     /* Current x-position of piece */
    int currentY = 0;                   /* Current y-position of piece */
    int pieceCount = 0;                 /* Count how far into the game */
    int gameSpeedCounter = 0;           /* Value to be incremented     */
    int gameSpeed = 20;                 /* Value to be reached before piece forced down */
    int score = 0;                      /* Player score                */
    vector<int> vLines;                 /* Stores lines of the game    */

    /* Create the tetris pieces */
    tetris[0].append(L"..X.");
    tetris[0].append(L"..X.");
    tetris[0].append(L"..X.");
    tetris[0].append(L"..X.");

    tetris[1].append(L"..X.");
    tetris[1].append(L".XX.");
    tetris[1].append(L".X..");
    tetris[1].append(L"....");

    tetris[2].append(L".X..");
    tetris[2].append(L".XX.");
    tetris[2].append(L"..X.");
    tetris[2].append(L"....");

    tetris[3].append(L"....");
    tetris[3].append(L".XX.");
    tetris[3].append(L".XX.");
    tetris[3].append(L"....");

    tetris[4].append(L"..X.");
    tetris[4].append(L".XX.");
    tetris[4].append(L"..X.");
    tetris[4].append(L"....");

    tetris[5].append(L"....");
    tetris[5].append(L".XX.");
    tetris[5].append(L"..X.");
    tetris[5].append(L"..X.");

    tetris[6].append(L"....");
    tetris[6].append(L".XX.");
    tetris[6].append(L".X..");
    tetris[6].append(L".X..");

    pBoard = new unsigned char[BOARD_WIDTH * BOARD_HEIGHT];
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            /* Creates the boarder boundaries, setting the value to nine
             * to repersent the border else the space must be free 
             * so set to zero */
            pBoard[y * BOARD_WIDTH + x] = (x == 0 || 
                x == BOARD_WIDTH - 1 || y == BOARD_HEIGHT - 1) ? 9 : 0;
        }
    }

    /* Draw the game to the window */
    /* Create an array of wchars for the screen width and height
     * filling it full of blank spaces */
    wchar_t *screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT];
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        screen[i] = L' ';
    }
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, 
                        NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    while (!gameOver) {
        /* Game timing setup */
        this_thread::sleep_for(50ms);
        gameSpeedCounter++;
        /* Checking if counter is equal to gameSpeed, if so set to true else false */
        forceDown = (gameSpeedCounter == gameSpeed);    

        /* Get user inputs */
        for (int k = 0; k < 4; k++) {
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
        }
        /* Game logic (scoring, collisions, etc.) */
            /* Check for collision (left, right, down) */
        currentX += (bKey[0] && PieceFits(currentPiece, currentRotation, currentX + 1, currentY)) ? 1 : 0;
        currentX -= (bKey[1] && PieceFits(currentPiece, currentRotation, currentX - 1, currentY)) ? 1 : 0;
        currentY += (bKey[2] && PieceFits(currentPiece, currentRotation, currentX, currentY + 1)) ? 1 : 0;
        if (bKey[3]) {
            currentRotation += (!keyHeld && PieceFits(currentPiece, currentRotation + 1, currentX, currentY)) ? 1 : 0;
            keyHeld = true;
        } else keyHeld = false;
            /* Force down the piece every 20 frames */
        if (forceDown) {
            if (PieceFits(currentPiece, currentRotation, currentX, currentY + 1)) {
                currentY++;
            } else {
                /* Lock current piece into field */
                for (int px = 0; px < 4; px++) {
                    for (int py = 0; py < 4; py++) {
                        if (tetris[currentPiece][Rotate(px, py, currentRotation)] == L'X') {
                            pBoard[(currentY + py) * BOARD_WIDTH + (currentX + px)] = currentPiece + 1;
                        }
                    }
                }
                pieceCount++;
                if (pieceCount % 10 == 0) {
                    if (gameSpeed >= 10) gameSpeed--;
                }
                /* Check for completed lines */
                for (int py = 0; py < 4; py++) {
                    if (currentY + py < BOARD_HEIGHT - 1) {
                        bool line = true; /* Assume line is completed */
                        for (int px = 1; px < BOARD_WIDTH - 1; px++) {
                            /* Check every x in the board for a row 
                             * for any empty area, if so set line to false */
                            line &= (pBoard[(currentY + py) * BOARD_WIDTH + px]) != 0;
                        }
                        if (line) {
                            for (int px = 1; px < BOARD_WIDTH - 1; px++) {
                                /* Draw the equal symbol to the screen on line competion */
                                pBoard[(currentY + py) * BOARD_WIDTH + px] = 8;
                                /* If line exists pushback current row */
                                vLines.push_back(currentY + py);
                            }
                        }
                        
                    }
                }
                score += 0;
                if (!vLines.empty()) score += (1 << vLines.size()) * 2;
                /* Get next piece */
                currentX = BOARD_WIDTH / 2;
                currentY = 0;
                currentRotation = 0;
                currentPiece = rand() % 7;

                /* Check if piece cannot fit and set gameOver */
                gameOver = !PieceFits(currentPiece, currentRotation, currentX, currentY);
            }
            gameSpeedCounter = 0;
        }

        /* Draw the field */
        for (int x = 0; x < BOARD_WIDTH; x++) {
            for (int y = 0; y < BOARD_HEIGHT; y++) {
                screen[(y + 2) * SCREEN_WIDTH + (x + 2)] = L" ABCDEFG=#"[pBoard[y * BOARD_WIDTH + x]];
            }
        }
        /* Draw current tetris piece to console window */
        for (int px = 0; px < 4; px++) {
            for (int py = 0; py < 4; py++) {
                if (tetris[currentPiece][Rotate(px, py, currentRotation)] == L'X') {
                    screen[(currentY + py + 2) * SCREEN_WIDTH + (currentX + px + 2)] = currentPiece + 65;
                }
            }
        }

        /* Draw player's score */
        swprintf_s(&screen[2 * SCREEN_WIDTH + BOARD_WIDTH + 6], 16, L"SCORE: %8d", score);

        if (!vLines.empty()) {
            WriteConsoleOutputCharacter(hConsole, screen, SCREEN_WIDTH* SCREEN_HEIGHT, { 0, 0 }, & dwBytesWritten);
            this_thread::sleep_for(400ms);  /* Delay the game for a moment */

            for (auto& v : vLines) {
                /* Check for lines in the vector, interating through the vector */
                for (int px = 1; px < BOARD_WIDTH - 1; px++) {
                    for(int py = v; py > 0; py--) {
                        pBoard[py * BOARD_WIDTH + px] = pBoard[(py - 1) * BOARD_WIDTH + px];
                    }
                    pBoard[px] = 0;
                }
            }
            vLines.clear();
        }
        /* Display current frame starting at position 0, 0 */
        WriteConsoleOutputCharacter(hConsole, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0, 0 }, &dwBytesWritten);
    }

    /* Display score and clean console */
    CloseHandle(hConsole);
    cout << "Game Over\nScore: " << score << endl;
    system("pause");
    return 0;
}

/*@desc Rotate the pieces when called
 *@parm px -- the pieces' x-value
 *@parm py -- the pieces' y-value
 *@parm r  -- the rotation based on cardinal direction 
 */
int Rotate(int px, int py, int r) {
    switch (r % 4) {
        case 0: /* 0 degrees */
            return py * 4 + px;
            break;
        case 1: /* 90 degrees */
            return 12 + py - (px * 4); 
            break;
        case 2: /* 180 degrees */
            return 15 - (py * 4) - px;
            break;
        case 3: /* 270 degrees */
            return 3 - py + (px * 4);
            break;
    }
    return 0;
}

/*@desc Checks to see if a piece can fit within the grid
 *@parm tp -- the tetris piece being checked
 *@parm r  -- the rotation value of the piece
 *@parm px -- the x position of the piece
 *@parm py -- the y position of the piece
 */
bool PieceFits(int tp, int r, int px, int py) {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            /* Index into piece */
            int pi = Rotate(x, y, r);
            /* Index into board */
            int fi = (py + y) * BOARD_WIDTH + (px + x);
            /* Check for range of bounds */
            if (px + x >= 0 && px + x < BOARD_WIDTH) {
                if (py + y >= 0 && py + y < BOARD_HEIGHT) {
                    if (tetris[tp][pi] == L'X' && pBoard[fi] != 0) {
                        return false;
                    }
                }
                // fi = fi * BOARD_WIDTH + (px + x);
            }
        }
    }
    return true;
}
