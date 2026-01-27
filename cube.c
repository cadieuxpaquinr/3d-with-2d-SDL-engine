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

#define NUM_VERTICES 8
#define ANGLE_VAL 3

static volatile sig_atomic_t keep_running = 1;

static void handle_sigint(int signum)
{
   (void)signum;
   keep_running = 0;
}
struct vertice
{
   double x;
   double y;
   double z;
};

struct cube
{
   struct vertice vertices[NUM_VERTICES];
};

struct vertice projection(struct vertice v)
{
   struct vertice temp;
   temp.x = v.x / v.z;
   temp.y = v.y / v.z;
   return temp;
}

struct vertice screen_xy(struct vertice v)
{
   struct vertice temp;
   temp.x = (v.x + 1) / 2 * 1000;
   temp.y = (1 - (v.y + 1) / 2) * 1000;
   return temp;
}

#define DEG2RAD 57.295780

struct vertice rotate_x(struct vertice v, double angle, double z_avg)
{
   struct vertice temp;
   temp.x = v.x * cos(angle / DEG2RAD) - (v.z - z_avg) * sin(angle / DEG2RAD);
   temp.z = v.x * sin(angle / DEG2RAD) + (v.z - z_avg) * cos(angle / DEG2RAD);
   temp.z += z_avg;
   temp.y = v.y;
   return temp;
}

struct vertice rotate_y(struct vertice v, double angle, double z_avg)
{
   struct vertice temp;
   temp.y = v.y * cos(angle / DEG2RAD) - (v.z - z_avg) * sin(angle / DEG2RAD);
   temp.z = v.y * sin(angle / DEG2RAD) + (v.z - z_avg) * cos(angle / DEG2RAD);
   temp.z += z_avg;
   temp.x = v.x;
   return temp;
}

void draw_cube(SDL_Renderer *renderer, struct cube cube)
{
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

void clear_screen(SDL_Renderer *renderer)
{
   SDL_SetRenderDrawColor(
         renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // black
   SDL_RenderClear(renderer);
   SDL_SetRenderDrawColor(
         renderer, 255, 0, 0, SDL_ALPHA_OPAQUE); // red
}

/**
   Debugging helper
*/
void cube_print_infos(struct cube c)
{
   for (int i = 0; i < NUM_VERTICES; i++)
   {
      fprintf(stdout, "x: %f y: %f z: %f\n", c.vertices[i].x, c.vertices[i].y,
              c.vertices[i].z);
   }
}

int main(int argc, char **argv)
{
   // cube initial position
   struct cube cube = {
      .vertices = {
         // x,y,z
         {-0.3, 0.3, 1},
         {0.3, 0.3, 1},
         {-0.3, -0.3, 1},
         {0.3, -0.3, 1},
         {-0.3, 0.3, 1.6},
         {0.3, 0.3, 1.6},
         {-0.3, -0.3, 1.6},
         {0.3, -0.3, 1.6},
      }};
   // separate projected cube to keep original values
   struct cube projected_cube;

   //sdl initialisation for 2D engine
   if (0 != SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS))
   {
      fprintf(stderr, "SDL error: %s\n", SDL_GetError());
      return 1;
   }
   fprintf(stdout, "Creating window\n");
   SDL_Window *window =
       SDL_CreateWindow("main_window", 0, 0, 1000, 1000, SDL_WINDOW_RESIZABLE);
   (window != NULL) ? fprintf(stdout, "Window context created\n") : fprintf(stderr, "Window context failed\n");
   SDL_Renderer *renderer = SDL_CreateRenderer(window, 0, NULL);
   if (!renderer)
      fprintf(stderr, "no renderer");
   
   SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE); // red
   // initial cube drawn on screen
   for (int i = 0; i < NUM_VERTICES; i++)
   {
      projected_cube.vertices[i] = screen_xy(projection(cube.vertices[i]));
   }
   draw_cube(renderer, projected_cube);
   SDL_RenderPresent(renderer);

   // compute Z average for later rotation
   // rotation is around 0,0,0 and we want the cube to rotate on itself
   double z_total = 0;
   for (int i = 0; i < NUM_VERTICES; i++)
   {
      z_total += cube.vertices[i].z;
   }
   double z_avg = z_total/NUM_VERTICES;

   signal(SIGINT, handle_sigint);
   double angle = 0;
   SDL_KeyboardEvent *key;
   while (keep_running)
   {
      SDL_Event event;
      if (SDL_PollEvent(&event))
      {
         switch (event.type)
         {
         case SDL_KEYDOWN:
            key = &event.key;
            switch (key->keysym.sym)
            {
            case SDLK_RIGHT:
               angle = -ANGLE_VAL;
               clear_screen(renderer);
               for (int i = 0; i < NUM_VERTICES; i++)
               {
                  cube.vertices[i] = rotate_x(cube.vertices[i], angle, z_avg);
                  projected_cube.vertices[i] =
                      screen_xy(projection(cube.vertices[i]));
               }
               draw_cube(renderer, projected_cube);
               SDL_RenderPresent(renderer);
               break;
            case SDLK_LEFT:
               angle = ANGLE_VAL;
               clear_screen(renderer);
               for (int i = 0; i < NUM_VERTICES; i++)
               {
                  cube.vertices[i] = rotate_x(cube.vertices[i], angle, z_avg);
                  projected_cube.vertices[i] =
                      screen_xy(projection(cube.vertices[i]));
               }
               draw_cube(renderer, projected_cube);
               SDL_RenderPresent(renderer);
               break;
            case SDLK_UP:
               angle = ANGLE_VAL;
               clear_screen(renderer);
               for (int i = 0; i < NUM_VERTICES; i++)
               {
                  cube.vertices[i] = rotate_y(cube.vertices[i], angle, z_avg);
                  projected_cube.vertices[i] =
                      screen_xy(projection(cube.vertices[i]));
               }
               draw_cube(renderer, projected_cube);
               SDL_RenderPresent(renderer);
               break;
            case SDLK_DOWN:
               angle = -ANGLE_VAL;
               clear_screen(renderer);
               for (int i = 0; i < NUM_VERTICES; i++)
               {
                  cube.vertices[i] = rotate_y(cube.vertices[i], angle, z_avg);
                  projected_cube.vertices[i] =
                      screen_xy(projection(cube.vertices[i]));
               }
               draw_cube(renderer, projected_cube);
               SDL_RenderPresent(renderer);
               break;
            default:
               break;
            }
            break;
         case SDL_KEYUP:
            break;
         default:
            break;
         }
      }
   }
   SDL_Quit();
}
