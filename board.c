#include "board.h"
#include "snake.h"
#include "food.h"
#include <ncurses.h>

void print_board() {
    clear();
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (x == snake->head->x && y == snake->head->y) {
                printw("S");
            } else if (x == food->x && y == food->y) {
                printw("F");
            } else {
                printw(".");
            }
        }
        printw("\n");
    }
    refresh();
}
