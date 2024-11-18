#include "food.h"
#include <stdlib.h>
#include <time.h>

Food* create_food() {
    Food *food = malloc(sizeof(Food));
    food->x = rand() % BOARD_WIDTH;
    food->y = rand() % BOARD_HEIGHT;
    return food;
}
