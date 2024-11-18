#include "game.h"
#include "snake.h"
#include "board.h"
#include "food.h"
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

static Snake *snake;
static Food *food;
static Board *board;
static int score;

void init_game() {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);

    snake = create_snake();
    food = create_food();
    board = create_board();
    score = 0;
}

void end_game() {
    free(snake);
    free(food);
    free(board);
    endwin();
}

void start_game() {
    load_phase("phase1.txt");

    int ch;
    while (1) {
        ch = getch();
        if (ch == 'q') break;

        move_snake(ch);
        check_collision();
        print_board();
    }

    save_score();
}

void load_phase(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Falha ao carregar fase");
        exit(1);
    }

    char line[50];
    int y = 0;

    while (fgets(line, sizeof(line), file)) {
        for (int x = 0; line[x] != '\0'; x++) {
            if (line[x] == 'S') {
                snake->head->x = x;
                snake->head->y = y;
            }
            if (line[x] == 'F') {
                food->x = x;
                food->y = y;
            }
        }
        y++;
    }
    fclose(file);
}

void print_menu() {
    clear();
    printw("1. Iniciar Jogo\n");
    printw("2. Estatísticas\n");
    printw("3. Sair\n");
    refresh();
}

int get_choice() {
    int choice = getch() - '0';
    return choice;
}

void show_stats() {
    clear();
    printw("Estatísticas do Jogo\n");
    printw("Pontuação: %d\n", score);
    refresh();
    getch();
}

void save_score() {
    FILE *file = fopen("score.txt", "w");
    if (file) {
        fprintf(file, "Score: %d\n", score);
        fclose(file);
    }
}
