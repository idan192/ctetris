#ifndef GAME_H
#define GAME_H

#define BLOCK_SIZE_PIXELS 32
#define COORDS_SIZE 10
#define TETROMINO_MAP_SIZE 8

#include <SDL3/SDL_render.h>

typedef struct {
  SDL_Texture *sheet;
  int h;
  int w;
} SpriteSheet_t;

typedef enum TranslationDirection {
  TRANSLATION_DIRECTION_LEFT = -1,
  TRANSLATION_DIRECTION_RIGHT = 1,
} TranslationDirection;

typedef enum {
  USER_INPUT_NONE,
  USER_INPUT_ROTATE_LEFT,
  USER_INPUT_ROTATE_RIGHT,
  USER_INPUT_MOVE_LEFT,
  USER_INPUT_MOVE_RIGHT,
  USER_INPUT_SOFT_DROP,
  USER_INPUT_HARD_DROP,
  USER_INPUT_PAUSE,
  USER_INPUT_SHOW_DEBUG,
} UserInput_t;

typedef enum {
  TETROMINO_SHAPE_TAG_I,
  TETROMINO_SHAPE_TAG_O,
  TETROMINO_SHAPE_TAG_T,
  TETROMINO_SHAPE_TAG_S,
  TETROMINO_SHAPE_TAG_Z,
  TETROMINO_SHAPE_TAG_J,
  TETROMINO_SHAPE_TAG_L
} TetrominoShapeTag;

typedef enum {
  TETROMINO_STATE_ACTIVE,
  TETROMINO_STATE_LOCKED,
  TETROMINO_STATE_EXPLODED,
  TETROMINO_STATE_INTERSECTED,
  TETROMINO_STATE_PENDING,
} TetrominoState_t;

// TODO: This should be called TetrominoShape imo, and it should include deg rotated.
typedef struct {
  int row0, col0;
  size_t size;
  TetrominoShapeTag t;
  int const *map;
} TetrominoMatrix;

// TODO: deg_rot shoud be part of the Matrix
// Matrix should be called shape
// If the Tetromino "explodes" or otherwise "distorts"
// we should expand the area of the shape matrix so that
// those pieces can be rotated in an "exploded form"
// And "wall kicks" for exploded minos could end up inside a single gap...
typedef struct {
  SpriteSheet_t *sheet;
  SDL_FRect *clip;
  int deg_rot;
  TetrominoState_t state;
  TetrominoMatrix *mat;
  // Whether a block should be hidden
  uint8_t hide_mask;
} Tetromino_t;

typedef struct {
  Tetromino_t **tetrominos;
  size_t size;
  size_t cnt;
} TetrominoCollection_t;

typedef struct {
  size_t cols;
  size_t rows;
  Tetromino_t ***arr;
} GameBoard_t;

typedef struct {
  int display_w;
  int display_h;
  double tick_rate;
  GameBoard_t *board;
  GameBoard_t *previous_board;
  UserInput_t input;
  TetrominoCollection_t *tetrominos;
} GameApp_t;

Tetromino_t *Tetromino_init(TetrominoShapeTag const shape, int row, int col);
void Tetromino_free(Tetromino_t *t);
void Tetromino_rotate(Tetromino_t *const t, int const deg);

// TODO: maybe instead just use an Enum with PI, 2PI, 3PI
void TetrominoMatrix_free(TetrominoMatrix *o);
int *TetrominoMatrix_rotate(TetrominoMatrix const *const mat, int const dega);

TetrominoCollection_t *TetrominoCollection_init(size_t const size);
void TetrominoCollection_free(TetrominoCollection_t *col);
void TetrominoCollection_push(TetrominoCollection_t *col, Tetromino_t *const new);
bool TetrominoCollection_contains_active(TetrominoCollection_t const *const col);
void TetrominoCollection_render(TetrominoCollection_t *col, SDL_Renderer *render);

// TODO: this shoul return a sprite sheet
int SpriteSheet_init(SDL_Renderer *renderer, SpriteSheet_t *const sheet, char const *const sheet_path);
void SpriteSheet_free(SpriteSheet_t *o);

GameBoard_t *GameBoard_init(int cols, int rows);
void GameBoard_free(GameBoard_t *o);
bool GameBoard_collision(GameBoard_t const *const board, Tetromino_t *const t, size_t const rows, size_t const cols);
int *GameBoard_get_tetromino_coords(Tetromino_t const *const tetromino);
void GameBoard_clear_full_rows(GameBoard_t *board);
void GameBoard_copy(GameBoard_t *const dest, GameBoard_t const *const src);
void GameBoard_print_debug(GameBoard_t const *const board);
void GameBoard_rotate_pi(GameBoard_t *const board, Tetromino_t *const tetromino);
void GameBoard_insert_tetromino(GameBoard_t *const board, Tetromino_t *const new_tetromino);
void GameBoard_translate_down(GameBoard_t *const board, Tetromino_t *const tetromino);
void GameBoard_translate_left(GameBoard_t *const board, Tetromino_t *const tetromino);
void GameBoard_translate_right(GameBoard_t *const board, Tetromino_t *const tetromino);
void GameBoard_update(GameBoard_t *board, GameApp_t *app_state);
void GameBoard_destroy_mino(Tetromino_t *const tetromino, int const row, int const col);

GameApp_t *GameApp_init(void);
void GameApp_handle_input(GameApp_t *state);
void GameApp_update(GameApp_t *state);

void SpriteSheet_tetrominos(SpriteSheet_t *sheet);

#endif
