#include "game.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static const size_t TETROMINO_COORDS_I[MINO_COORDS_SIZE] = {1, 0, 1, 1, 1, 2, 1, 3};
static const size_t TETROMINO_COORDS_J[MINO_COORDS_SIZE] = {0, 0, 1, 0, 1, 1, 1, 2};
static const size_t TETROMINO_COORDS_L[MINO_COORDS_SIZE] = {0, 2, 1, 0, 1, 1, 1, 2};
static const size_t TETROMINO_COORDS_O[MINO_COORDS_SIZE] = {0, 0, 0, 1, 1, 0, 1, 1};
static const size_t TETROMINO_COORDS_S[MINO_COORDS_SIZE] = {0, 1, 0, 2, 1, 0, 1, 1};
static const size_t TETROMINO_COORDS_T[MINO_COORDS_SIZE] = {0, 1, 1, 0, 1, 1, 1, 2};
static const size_t TETROMINO_COORDS_Z[MINO_COORDS_SIZE] = {0, 0, 0, 1, 1, 1, 1, 2};

Tetromino *Tetromino_init(ETetrominoShape const shape, size_t const row, size_t const col) {
  Tetromino *new = calloc(1, sizeof(Tetromino));
  new->shape = shape;
  new->deg = 0;
  new->mino_mask = 0;
  memset(new->mino_shift, 0, sizeof(size_t) * MINO_COORDS_SIZE);

  switch (shape) {
  case TETROMINO_SHAPE_I:
    new->row0 = row - 1;
    new->col0 = col;
    new->mino_coords = TETROMINO_COORDS_I;
    new->bound_size = 4;
    break;
  case TETROMINO_SHAPE_J:
    new->row0 = row;
    new->col0 = col;
    new->mino_coords = TETROMINO_COORDS_J;
    new->bound_size = 3;
    break;
  case TETROMINO_SHAPE_L:
    new->row0 = row;
    new->col0 = col - 2;
    new->mino_coords = TETROMINO_COORDS_L;
    new->bound_size = 3;
    break;
  case TETROMINO_SHAPE_O:
    new->row0 = row;
    new->col0 = col;
    new->mino_coords = TETROMINO_COORDS_O;
    new->bound_size = 2;
    break;
  case TETROMINO_SHAPE_S:
    new->row0 = row;
    new->col0 = col - 1;
    new->mino_coords = TETROMINO_COORDS_S;
    new->bound_size = 3;
    break;
  case TETROMINO_SHAPE_T:
    new->row0 = row;
    new->col0 = col - 1;
    new->mino_coords = TETROMINO_COORDS_T;
    new->bound_size = 3;
    break;
  case TETROMINO_SHAPE_Z:
    new->row0 = row;
    new->col0 = col;
    new->mino_coords = TETROMINO_COORDS_Z;
    new->bound_size = 3;
    break;
  }

  return new;
}

static size_t *_Tetromino_rotated_coords(Tetromino const *const t) {
  assert(t->deg == 0 || t->deg == 90 || t->deg == 180 || t->deg == 270 && "invalid tetromino rotation");

  size_t *arr = calloc(1, sizeof(size_t) * MINO_COORDS_SIZE);

  if (t->deg == 0) {
    for (size_t e = 0; e < MINO_COORDS_SIZE; e += 2) {
      arr[e] = t->mino_coords[e];
      arr[e + 1] = t->mino_coords[e + 1];
    }
  } else if (t->deg == 90) {
    for (size_t e = 0; e < MINO_COORDS_SIZE; e = e + 2) {
      // Rotating pi radians: a[i,j] = a[j][n-i-1]
      arr[e] = t->mino_coords[e + 1];
      arr[e + 1] = t->bound_size - t->mino_coords[e] - 1;
    }
  } else if (t->deg == 180) {
    for (size_t e = 0; e < MINO_COORDS_SIZE; e = e + 2) {
      // Rotating 2pi radians: a[i,j] = a[n-i-1][n-j-1]
      arr[e] = t->bound_size - t->mino_coords[e] - 1;
      arr[e + 1] = t->bound_size - t->mino_coords[e + 1] - 1;
    }
  } else if (t->deg == 270) {
    for (size_t e = 0; e < MINO_COORDS_SIZE; e = e + 2) {
      // Rotating 3pi radians: a[i,j] = a[n-j-1][i]
      arr[e] = t->bound_size - t->mino_coords[e + 1] - 1;
      arr[e + 1] = t->mino_coords[e];
    }
  } else {
    assert(false && "invalid termino rotation");
  }

  return arr;
}

void Tetromino_free(Tetromino *t) {
  if (t == NULL) {
    return;
  }

  free(t);
}

TetrominoCollection *TetrominoCollection_init(size_t const cap) {
  TetrominoCollection *new = calloc(1, sizeof(TetrominoCollection));
  new->arr = calloc(1, sizeof(Tetromino) * 100);
  new->cap = cap;
  new->cnt = 0;

  return new;
}

void TetrominoCollection_free(TetrominoCollection *coll) {
  if (coll == NULL) {
    return;
  }

  for (size_t i = 0; i < coll->cnt; i++) {
    Tetromino_free(coll->arr[i]);
  }

  free(coll->arr);
  free(coll);
}

void TetrominoCollection_push(TetrominoCollection *const coll, Tetromino *const t) {
  if (coll->cnt + 1 > coll->cap) {
    // TODO: resize and add
    TetrominoCollection_resize(coll);
    assert(false && "resize TetrominoCollection not implemented");
    return;
  }

  coll->arr[coll->cnt++] = t;
}

void TetrominoCollection_resize(TetrominoCollection *const coll) {
  assert(false && "resize TetrominoCollection not implemented");
}

size_t *TetrominoWell_coords(Tetromino const *const t) {
  size_t *coords = _Tetromino_rotated_coords(t);

  for (size_t i = 0; i < MINO_COORDS_SIZE; i += 2) {
    coords[i] = t->row0 + coords[i] + t->mino_shift[i];
    coords[i + 1] = t->col0 + coords[i + 1] + t->mino_shift[i + 1];
  }

  return coords;
}
