#ifndef GAME_H
#define GAME_H

void init_game();
void end_game();
void start_game();
void load_phase(const char *filename);
void print_menu();
int get_choice();
void show_stats();
void save_score();

#endif
