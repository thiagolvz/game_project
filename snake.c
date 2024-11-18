#include "snake.h"
#include <stdlib.h>
#include <stdio.h>

Snake* create_snake() {
    Snake *snake = malloc(sizeof(Snake));
    snake->length = 1;
    snake->head = malloc(sizeof(SnakePart));
    snake->head->x = 10;
    snake->head->y = 10;
    snake->head->next = NULL;
    return snake;
}

void move_snake(int direction) {
    SnakePart *new_head = malloc(sizeof(SnakePart));
    new_head->next = snake->head;

    switch (direction) {
        case KEY_UP:
            new_head->x = snake->head->x;
            new_head->y = snake->head->y - 1;
            break;
        case KEY_DOWN:
            new_head->x = snake->head->x;
            new_head->y = snake->head->y + 1;
            break;
        case KEY_LEFT:
            new_head->x = snake->head->x - 1;
            new_head->y = snake->head->y;
            break;
        case KEY_RIGHT:
            new_head->x = snake->head->x + 1;
            new_head->y = snake->head->y;
            break;
    }

    snake->head = new_head;
}

void check_collision() {
    // Checa se a cobra bateu nas bordas ou se comeu a si mesma
    if (snake->head->x < 0 || snake->head->x >= BOARD_WIDTH ||
        snake->head->y < 0 || snake->head->y >= BOARD_HEIGHT) {
        // Fim de jogo
        end_game();
    }
}
