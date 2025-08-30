#include "game.h"
#define SDL_MAIN_USE_CALLBACKS 1

#include "_gen/cmake_variables.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static TetrominoCollection *coll = NULL;

void stdoutLog(void *UNUSED(userdata), int UNUSED(category), SDL_LogPriority UNUSED(priority), const char *message) {
  printf("%s\n", message);
}

SDL_AppResult SDL_AppInit(void **UNUSED(appstate), int UNUSED(argc), char *UNUSED(argv[])) {
  SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
  SDL_SetLogOutputFunction(stdoutLog, NULL);

  SDL_SetAppMetadata(CMAKE_PROJECT_NAME, CMAKE_PROJECT_VERSION, "com.rse8.tetris");

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Init video: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Init Audio: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // TODO: initialize GameState

  if (SDL_CreateWindowAndRenderer(CMAKE_PROJECT_NAME, 100, 100,
                                  /* SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS, */
                                  0, &window, &renderer) != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Init window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  coll = TetrominoCollection_init(10);
  Tetromino *t = Tetromino_init(TETROMINO_SHAPE_I, 1, 1);
  TetrominoCollection_push(coll, t);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *UNUSED(appstate), SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *UNUSED(appstate)) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  if (coll && coll->cnt > 0) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    size_t *coords = TetrominoWell_coords(coll->arr[0]);
    for (size_t i = 0; i < MINO_COORDS_SIZE; i += 2) {
      SDL_Rect rect = {(int)(coords[i + 1] * 20), (int)(coords[i] * 20), 20, 20};
      SDL_RenderFillRect(renderer, &rect);
    }
    free(coords);
  }

  SDL_RenderPresent(renderer);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *UNUSED(appstate), SDL_AppResult UNUSED(result)) { TetrominoCollection_free(coll); }
