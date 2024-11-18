#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <unistd.h>

#define WIDTH 20
#define HEIGHT 10

int x, y, foodX, foodY, score;
int tailX[100], tailY[100];
int nTail;
int gameOver;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN};
enum eDirection dir;

void setup() {
    gameOver = 0;
    dir = STOP;
    x = WIDTH / 2;
    y = HEIGHT / 2;
    foodX = rand() % WIDTH;
    foodY = rand() % HEIGHT;
    score = 0;
    nTail = 0;
}

void draw() {
    system("cls");  // Limpa a tela
    for (int i = 0; i < WIDTH+2; i++) printf("#");
    printf("\n");

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (j == 0) printf("#");
            if (i == y && j == x) printf("O");  // Cabeça da cobra
            else if (i == foodY && j == foodX) printf("F");  // Comida
            else {
                int isprint = 0;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        printf("o");  // Corpo da cobra
                        isprint = 1;
                    }
                }
                if (!isprint) printf(" ");
            }
            if (j == WIDTH - 1) printf("#");
        }
        printf("\n");
    }

    for (int i = 0; i < WIDTH+2; i++) printf("#");
    printf("\n");
    printf("Score: %d\n", score);
}

void input() {
    if (_kbhit()) {
        switch (_getch()) {
            case 'a': dir = LEFT; break;
            case 'd': dir = RIGHT; break;
            case 'w': dir = UP; break;
            case 's': dir = DOWN; break;
            case 'x': gameOver = 1; break;
        }
    }
}

void logic() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    switch (dir) {
        case LEFT: x--; break;
        case RIGHT: x++; break;
        case UP: y--; break;
        case DOWN: y++; break;
        default: break;
    }

    if (x >= WIDTH) x = 0; else if (x < 0) x = WIDTH - 1;
    if (y >= HEIGHT) y = 0; else if (y < 0) y = HEIGHT - 1;

    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == x && tailY[i] == y) gameOver = 1;
    }

    if (x == foodX && y == foodY) {
        score += 10;
        foodX = rand() % WIDTH;
        foodY = rand() % HEIGHT;
        nTail++;
    }
}

int main() {
    setup();
    while (!gameOver) {
        draw();
        input();
        logic();
        usleep(100000); // Controle da velocidade do jogo
    }
    return 0;
}
