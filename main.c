#include <stdio.h>

#include <time.h>
#include <rand.h>

#include <gb/gb.h>
#include <gb/console.h>
#include <gb/drawing.h>

#define WORLD_W 32
#define WORLD_H 32
#define CELL_BITS 8
#define CELLS_PER_ROW WORLD_W / CELL_BITS
#define POPULATION_MEM_SIZE WORLD_H * CELLS_PER_ROW
#define COLOR_WHITE 0x00U
#define COLOR_BLACK 0xFFU
#define DEAD_STATE 0
#define ALIVE_STATE 0


void clear_screen()
{
  BGP_REG = COLOR_WHITE;
  color(COLOR_BLACK, COLOR_WHITE, SOLID);
}

void print_bits(int num) {
  int x;
  int buffer[8];

  for(x = 0; x < 8; x++) {
    buffer[x] = (num >> x) & 0x01;
  }

  for(x = 7; x >= 0; x--) {
    printf("%d", buffer[x]);
  }

  printf("\n");
}

int cell_offset(int x, int y) {
  return y * CELLS_PER_ROW + x / CELL_BITS;
}

void population_write(UINT8 population[POPULATION_MEM_SIZE], int x, int y, int value)
{
  int offset = cell_offset(x, y);
  UINT8 cell = population[offset];

  if (value) {
    population[offset] = cell | (0x0001 << (x % CELL_BITS));
  } else {
    population[offset] = cell & ~(0x0001 << (x % CELL_BITS));
  }
}

int population_read(UINT8 population[POPULATION_MEM_SIZE], int x, int y) {
  return (population[cell_offset(x, y)] >> (x % CELL_BITS)) & 1;
}

static UBYTE tick; //global tick counter;
static UINT8 next_population[POPULATION_MEM_SIZE];

/*static UINT8 alive_table = {0, 1, 1, 0, 0, 0, 0, 0};*/
/*static UINT8 dead_table = {0, 0, 0, 1, 0, 0, 0, 0};*/

void plot_population(UINT8 population[POPULATION_MEM_SIZE])
{
  int x, y;

  for(y = 0; y < WORLD_H; y++){
    for(x = 0; x < WORLD_W; x++){
      plot(x, y, LTGREY, SOLID);
      if(population_read(population, x, y) == 1) {
        plot(x, y, BLACK, SOLID);
      }
    }
  }
}

void init_population(UINT8 population[POPULATION_MEM_SIZE])
{
  int i;

  for(i = 0; i < POPULATION_MEM_SIZE; i++){
    population[i] = 0;
  }
}

int number_of_neighbours_at(int at_x, int at_y, UINT8 population[POPULATION_MEM_SIZE])
{
  int x, y, dx, dy, count;

  count = 0;

  for(dy = -1; dy <= 1; dy++){
    for(dx = -1; dx <= 1; dx++){
      if (dx == 0 && dy == 0) {
        continue;
      }

      x = at_x + dx;
      y = at_y + dy;

      if (x < 0 || x >= WORLD_W) {
        continue;
      }

      if (y < 0 || y >= WORLD_H) {
        continue;
      }

      if (population_read(population, x, y) == 1) {
        count++;
      }
    }
  }

  return count;
}

void copy_population(UINT8 src[POPULATION_MEM_SIZE], UINT8 dst[POPULATION_MEM_SIZE])
{
  int i;

  for(i = 0; i < POPULATION_MEM_SIZE; i++) {
    dst[i] = src[i];
  }
}

void computate_population(UINT8 population[POPULATION_MEM_SIZE])
{
  int neighbours;
  int is_alive;
  int will_survive; // at first I was afraid, I was petrified ...
  int x, y;

  for(y = 0; y < WORLD_H; y++){
    for(x = 0; x < WORLD_W; x++){
      neighbours = number_of_neighbours_at(x, y, population);
      is_alive = population_read(population, x, y);
      will_survive = (is_alive && (neighbours >= 2 && neighbours <= 3)) || (!is_alive && neighbours == 3);
      population_write(next_population, x, y, will_survive);
    }
  }

  copy_population(next_population, population);
}

void main(void)
{
  UINT8 population[POPULATION_MEM_SIZE];
  int neighbours;
  int is_alive;
  int will_survive; // at first I was afraid, I was petrified ...

  tick = 0;

  init_population(population);
  init_population(next_population);


  population_write(population, 1, 6, 1);
  population_write(population, 10, 11, 1);
  population_write(population, 10, 16, 1);
  population_write(population, 10, 2, 1);
  population_write(population, 10, 7, 1);
  population_write(population, 10, 9, 1);
  population_write(population, 13, 5, 1);
  population_write(population, 5, 11, 1);
  population_write(population, 5, 12, 1);
  population_write(population, 6, 1, 1);
  population_write(population, 6, 11, 1);
  population_write(population, 7, 11, 1);

  while (1) {
    tick++;

    if ((tick & 0xFF) == 0) {
      computate_population(population);
      plot_population(population);
    }
  }
}
