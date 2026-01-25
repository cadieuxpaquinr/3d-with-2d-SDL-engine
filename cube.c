#include <SDL2/SDL.h>
#include <SDL2/SDL_assert.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <signal.h>
#include <stdio.h>
#include <wchar.h>

/**
 * x' = x/z
 * y' = y/z
 */

static volatile sig_atomic_t keep_running = 1;

static void handle_sigint(int signum) {
  (void)signum;
  keep_running = 0;
}

int main(int argc, char **argv) {
  if (0 != SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
    fprintf(stderr, "SDL error: %s\n", SDL_GetError());
    return 1;
  }
  fprintf(stdout, "Creating window\n");
  SDL_Window *window =
      SDL_CreateWindow("main_window", 0, 0, 1900, 1000, SDL_WINDOW_RESIZABLE);
  (window != NULL) ? fprintf(stdout, "Window context created\n")
                   : fprintf(stderr, "Window context failed\n");
  
  SDL_Renderer *renderer = SDL_CreateRenderer(window, 0, NULL);
  if (!renderer)
    fprintf(stderr, "no renderer");
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(renderer, 100, 100, 50, 50);
  SDL_RenderPresent(renderer);
  
  signal(SIGINT, handle_sigint);
  while (keep_running)
    SDL_GetTicks64();

  SDL_Quit();
}
