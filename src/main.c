#include <SDL3/SDL_oldnames.h>
#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS 1

#include "_gen/cmake_variables.h"
#include "game.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

static GameApp_t *app = NULL;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SpriteSheet_t tmp_sheet = {0};

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

  app = GameApp_init();

  if (!SDL_CreateWindowAndRenderer(CMAKE_PROJECT_NAME, app->board->cols * BLOCK_SIZE_PIXELS,
                                   app->board->rows * BLOCK_SIZE_PIXELS,
                                   /* SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS */ 0, &window, &renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Init window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SpriteSheet_init(renderer, &tmp_sheet, "tetrominos.bmp");
  SpriteSheet_tetrominos(&tmp_sheet);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *UNUSED(appstate), SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  if (event->type == SDL_EVENT_KEY_DOWN) {
    switch (event->key.scancode) {
    case SDL_SCANCODE_UP:
      app->input = USER_INPUT_ROTATE_RIGHT;
      break;
    case SDL_SCANCODE_K:
      app->input = USER_INPUT_ROTATE_RIGHT;
      break;
    case SDL_SCANCODE_J:
      app->input = USER_INPUT_ROTATE_LEFT;
      break;
    case SDL_SCANCODE_LEFT:
      app->input = USER_INPUT_MOVE_LEFT;
      break;
    case SDL_SCANCODE_RIGHT:
      app->input = USER_INPUT_MOVE_RIGHT;
      break;
    case SDL_SCANCODE_DOWN:
      app->input = USER_INPUT_SOFT_DROP;
      break;
    case SDL_SCANCODE_SPACE:
      app->input = USER_INPUT_HARD_DROP;
      break;
    case SDL_SCANCODE_D:
      app->input = USER_INPUT_SHOW_DEBUG;
      break;
    default:
      app->input = USER_INPUT_NONE;
      break;
    }
  } else {
    app->input = USER_INPUT_NONE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *UNUSED(appstate)) {
  GameApp_update(app);

  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0);
  SDL_RenderClear(renderer);

  TetrominoCollection_render(app->tetrominos, renderer);

  SDL_RenderPresent(renderer);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *UNUSED(appstate), SDL_AppResult UNUSED(result)) {}
