#include "game.h"
#define SDL_MAIN_USE_CALLBACKS 1

#include "_gen/cmake_variables.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static Tetromino *active_tetromino = NULL;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define BLOCK_SIZE_PIXELS 32

void stdoutLog(void *UNUSED(userdata), int UNUSED(category), SDL_LogPriority UNUSED(priority), const char *message) {
  printf("%s\n", message);
}

SDL_AppResult SDL_AppInit(void **UNUSED(appstate), int UNUSED(argc), char *UNUSED(argv[])) {
  SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
  SDL_SetLogOutputFunction(stdoutLog, NULL);

  SDL_SetAppMetadata(CMAKE_PROJECT_NAME, CMAKE_PROJECT_VERSION, "com.rse8.tetris");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Init video: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_Init(SDL_INIT_AUDIO)) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Init Audio: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // TODO: initialize GameState

  if (!SDL_CreateWindowAndRenderer(CMAKE_PROJECT_NAME, WINDOW_WIDTH, WINDOW_HEIGHT,
                                   /* SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS, */
                                   0, &window, &renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Init window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  active_tetromino = Tetromino_init(TETROMINO_SHAPE_I, 1, WINDOW_WIDTH / (2 * BLOCK_SIZE_PIXELS));

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *UNUSED(appstate), SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
  ;
}

SDL_AppResult SDL_AppIterate(void *UNUSED(appstate)) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  size_t *coords = TetrominoWell_coords(active_tetromino);
  SDL_SetRenderDrawColor(renderer, 0, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_FRect rect = {.w = BLOCK_SIZE_PIXELS, .h = BLOCK_SIZE_PIXELS};
  for (size_t i = 0; i < MINO_COORDS_SIZE; i += 2) {
    rect.y = coords[i] * BLOCK_SIZE_PIXELS;
    rect.x = coords[i + 1] * BLOCK_SIZE_PIXELS;
    SDL_RenderFillRect(renderer, &rect);
  }
  free(coords);

  SDL_RenderPresent(renderer);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *UNUSED(appstate), SDL_AppResult UNUSED(result)) {
  Tetromino_free(active_tetromino);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
