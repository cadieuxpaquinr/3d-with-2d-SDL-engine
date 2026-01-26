#include <SDL2/SDL.h>
#include <SDL2/SDL_assert.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>

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
  double x;
  double y;
  double z;
};

struct cube {
  struct vertice vertices[8];
};

struct vertice *project(struct vertice *v) {
  v->x = v->x / v->z;
  v->y = v->y / v->z;
  return v;
}

void screen_xy(struct vertice *v) {
  v->x = (v->x + 1) / 2 * 1000;
  v->y = (1 - (v->y + 1) / 2) * 1000;
}

void draw_cube(SDL_Renderer *renderer, struct cube cube) {
  // drawing cube lines from vertices logic

  // face 1
  SDL_RenderDrawLine(renderer, cube.vertices[0].x, cube.vertices[0].y,
                     cube.vertices[1].x, cube.vertices[1].y);
  SDL_RenderDrawLine(renderer, cube.vertices[2].x, cube.vertices[2].y,
                     cube.vertices[3].x, cube.vertices[3].y);
  SDL_RenderDrawLine(renderer, cube.vertices[0].x, cube.vertices[0].y,
                     cube.vertices[2].x, cube.vertices[2].y);
  SDL_RenderDrawLine(renderer, cube.vertices[1].x, cube.vertices[1].y,
                     cube.vertices[3].x, cube.vertices[3].y);
  // face 2
  SDL_RenderDrawLine(renderer, cube.vertices[4].x, cube.vertices[4].y,
                     cube.vertices[5].x, cube.vertices[5].y);
  SDL_RenderDrawLine(renderer, cube.vertices[6].x, cube.vertices[6].y,
                     cube.vertices[7].x, cube.vertices[7].y);
  SDL_RenderDrawLine(renderer, cube.vertices[4].x, cube.vertices[4].y,
                     cube.vertices[6].x, cube.vertices[6].y);
  SDL_RenderDrawLine(renderer, cube.vertices[5].x, cube.vertices[5].y,
                     cube.vertices[7].x, cube.vertices[7].y);

  // "face" 3
  SDL_RenderDrawLine(renderer, cube.vertices[0].x, cube.vertices[0].y,
                     cube.vertices[4].x, cube.vertices[4].y);
  SDL_RenderDrawLine(renderer, cube.vertices[2].x, cube.vertices[2].y,
                     cube.vertices[6].x, cube.vertices[6].y);

  // "face" 4
  SDL_RenderDrawLine(renderer, cube.vertices[1].x, cube.vertices[1].y,
                     cube.vertices[5].x, cube.vertices[5].y);
  SDL_RenderDrawLine(renderer, cube.vertices[3].x, cube.vertices[3].y,
                     cube.vertices[7].x, cube.vertices[7].y);
}

struct vertice *rotate_point(struct vertice *v, double angle) {
  double x_temp, z_temp;
  x_temp = v->x * cos(angle) - v->z * sin(angle);
  z_temp = v->x * sin(angle) + v->z * cos(angle);
  v->x = x_temp;
  v->z = z_temp;
  return v;
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
  SDL_Renderer *renderer = SDL_CreateRenderer(window, 0, NULL);
  if (!renderer)
    fprintf(stderr, "no renderer");
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE); // red

  struct cube cube = {.vertices = {
                          // x,y,z
                          {-0.1, 0.1, 1},
                          {0.1, 0.1, 1},
                          {-0.1, -0.1, 1},
                          {0.1, -0.1, 1},
                          {-0.1, 0.1, 2},
                          {0.1, 0.1, 2},
                          {-0.1, -0.1, 2},
                          {0.1, -0.1, 2},
                      }};

  for (int i = 0; i < 8; i++) {
    project(&cube.vertices[i]);
  }

  for (int i = 0; i < 8; i++) {
    screen_xy(&cube.vertices[i]);
  }

  draw_cube(renderer, cube);
  SDL_RenderPresent(renderer);

  signal(SIGINT, handle_sigint);
  time_t timer;
  time_t last_time;
  time(&last_time);
  double angle = 0;
  Uint64 NOW = SDL_GetPerformanceCounter();
  Uint64 LAST = 0;
  double deltaTime = 0;

  while (keep_running) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_KEYDOWN:
        printf("Key press detected\n");
        SDL_KeyboardEvent *key = &event.key;
        switch (key->keysym.sym) {
        case SDLK_LEFT:
          angle -= 1;
          fprintf(stdout, "lefty %f\n", angle);
          SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // black
          SDL_RenderClear(renderer);
          SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE); // red
          for (int i = 0; i < 8; i++) {
            cube.vertices[i].x-=1;
            // screen_xy(project(rotate_point(&cube.vertices[i], angle)));
            screen_xy(project(&cube.vertices[i]));
          }
          draw_cube(renderer, cube);
          SDL_RenderPresent(renderer);
          break;
        case SDLK_RIGHT:
          angle += 1;
          fprintf(stdout, "righty %f\n", angle);
          SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // black
          SDL_RenderClear(renderer);
          SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE); // red
          for (int i = 0; i < 8; i++) {
            cube.vertices[i].x+=1;
            // screen_xy(project(rotate_point(&cube.vertices[i], angle)));
            screen_xy(project(&cube.vertices[i]));
          }
          draw_cube(renderer, cube);
          SDL_RenderPresent(renderer);
          break;
        default:
          break;
        }
        fprintf(stdout, ", Name: %s", SDL_GetKeyName(key->keysym.sym));
        break;

      case SDL_KEYUP:
        // printf("Key release detected\n");
        break;

      default:
        break;
      }
    }

    // LAST = NOW;
    // NOW = SDL_GetPerformanceCounter();
    // deltaTime =
    //     (double)((NOW - LAST) * 1000 /
    //     (double)SDL_GetPerformanceFrequency());

    // fprintf(stdout, "deltaTime: %f\n", deltaTime);
    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // black
    // SDL_RenderClear(renderer);
    // SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE); // red
    // for (int i = 0; i < 8; i++) {
    //   angle += 2 * 3.1416 * (deltaTime/60);
    //   screen_xy(project(rotate_point(&cube.vertices[i], angle)));
    //   // project(&cube.vertices[i]);
    //   // screen_xy(&cube.vertices[i]);
    //   draw_cube(renderer, cube);
    // }
    // SDL_RenderPresent(renderer);
    // fprintf(stdout, "Frame updated\n");
  }
  SDL_Quit();
}
