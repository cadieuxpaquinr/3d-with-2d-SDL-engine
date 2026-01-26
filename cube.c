#include <SDL2/SDL.h>
#include <SDL2/SDL_assert.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <signal.h>
#include <stdio.h>

/**
 * x' = x/z
 * y' = y/z
 */

static volatile sig_atomic_t keep_running = 1;

static void handle_sigint(int signum) {
  (void)signum;
  keep_running = 0;
}
struct vertice {
  float x;
  float y;
  float z;
};

struct cube {
  struct vertice vertices[8];
  SDL_Point points[8];
};

void fancy_op(struct vertice *v) {
  v->x = v->x / v->z;
  v->y = v->y / v->z;
}

void screen_xy(struct vertice *v) {
  v->x = (v->x + 1) / 2 * 1000;
  v->y = (1 - (v->y + 1) / 2) * 1000;
}

void convert_to_points(struct vertice vertices[8], SDL_Point *points) {
  points[0].x = vertices[0].x;
  points[0].y = vertices[0].y;
  points[1].x = vertices[1].x;
  points[1].y = vertices[1].y;
  points[2].x = vertices[2].x;
  points[2].y = vertices[2].y;
  points[3].x = vertices[3].x;
  points[3].y = vertices[3].y;
  points[4].x = vertices[4].x;
  points[4].y = vertices[4].y;
  points[5].x = vertices[5].x;
  points[5].y = vertices[5].y;
  points[6].x = vertices[6].x;
  points[6].y = vertices[6].y;
  points[7].x = vertices[7].x;
  points[7].y = vertices[7].y;
}

int main(int argc, char **argv) {
  if (0 != SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS)) {
    fprintf(stderr, "SDL error: %s\n", SDL_GetError());
    return 1;
  }
  fprintf(stdout, "Creating window\n");
  SDL_Window *window =
      SDL_CreateWindow("main_window", 0, 0, 1000, 1000, SDL_WINDOW_RESIZABLE);
  (window != NULL) ? fprintf(stdout, "Window context created\n")
                   : fprintf(stderr, "Window context failed\n");
  int *top = (int *)malloc(sizeof(int) * 1);
  int *left = (int *)malloc(sizeof(int) * 1);
  int *bottom = (int *)malloc(sizeof(int) * 1);
  int *right = (int *)malloc(sizeof(int) * 1);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, 0, NULL);
  if (!renderer)
    fprintf(stderr, "no renderer");
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);

  SDL_GetWindowBordersSize(window, top, left, bottom, right);
  fprintf(stdout, "top: %d, left: %d, bottom: %d, right: %d", *top, *left,
          *bottom, *right);

  struct cube cube = {.vertices = {
                          // x,y,z
                          {0.1, 0.1, 1},
                          {0.1, -0.1, 1},
                          {-0.1, -0.1, 1},
                          {-0.1, 0.1, 1},
                          {0.1, 0.1, 2},
                          {0.1, -0.1, 2},
                          {-0.1, -0.1, 2},
                          {-0.1, 0.1, 2},
                      }};

  for (int i = 0; i < 8; i++) {
    fancy_op(&cube.vertices[i]);
  }

  for (int i = 0; i < 8; i++) {
    screen_xy(&cube.vertices[i]);
  }
  convert_to_points(cube.vertices, cube.points);
  SDL_RenderDrawPoints(renderer, cube.points, 8);
  SDL_RenderPresent(renderer);

  signal(SIGINT, handle_sigint);
  while (keep_running) {
    SDL_GetTicks64();
  }
  SDL_Quit();
}
