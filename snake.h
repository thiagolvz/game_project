#ifndef SNAKE_H
#define SNAKE_H

#define BOARD_WIDTH 20
#define BOARD_HEIGHT 20

typedef struct SnakePart {
    int x, y;
    struct SnakePart *next;
} SnakePart;

typedef struct {
    int length;
    SnakePart *head;
} Snake;

Snake* create_snake();
void move_snake(int direction);
void check_collision();

#endif
