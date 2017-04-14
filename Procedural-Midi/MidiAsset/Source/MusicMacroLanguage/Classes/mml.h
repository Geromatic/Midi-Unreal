/**
 * @file mml.h
 * @author Shinichiro Nakamura
 * @copyright
 * ===============================================================
 * A tiny MML parser (Version 0.5.0)
 * ===============================================================
 * Copyright (c) 2014-2015 Shinichiro Nakamura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef MML_H
#define MML_H

#include "mml_stack.h"

#define MML_CONFIG_OCTAVE_REVERSE   (0)

#define MML_USER_EVENT_NAME_SIZE    (16)

#define MML_OCTAVE_MIN              (0)
#define MML_OCTAVE_MAX              (10)
#define MML_NOTE_NUMBER_MIN         (0)
#define MML_NOTE_NUMBER_MAX         (127)

#define MML_OPTION_DEFAULT_OCTAVE   (4)
#define MML_OPTION_DEFAULT_LENGTH   (8)
#define MML_OPTION_DEFAULT_BTICKS   (480)

#define MML_OPTION_INITIALIZER(P, OCTAVE, LENGTH, BTICKS) \
  do { \
    (P)->octave = (OCTAVE); \
    (P)->length = (LENGTH); \
    (P)->bticks = (BTICKS); \
  } while (0)

#define MML_OPTION_INITIALIZER_DEFAULT(P) \
  do { \
    (P)->octave = (MML_OPTION_DEFAULT_OCTAVE); \
    (P)->length = (MML_OPTION_DEFAULT_LENGTH); \
    (P)->bticks = (MML_OPTION_DEFAULT_BTICKS); \
  } while (0)

typedef enum {
  MML_TYPE_NOTE,
  MML_TYPE_REST,
  MML_TYPE_TEMPO,
  MML_TYPE_LENGTH,
  MML_TYPE_VOLUME,
  MML_TYPE_OCTAVE,
  MML_TYPE_OCTAVE_UP,
  MML_TYPE_OCTAVE_DOWN,
  MML_TYPE_USER_EVENT,
} MML_TYPE;

typedef struct {
  int number;
  int ticks;
} MML_ARGS_NOTE;

typedef struct {
  int ticks;
} MML_ARGS_REST;

typedef struct {
  int value;
} MML_ARGS_TEMPO;

typedef struct {
  int value;
} MML_ARGS_LENGTH;

typedef struct {
  int value;
} MML_ARGS_VOLUME;

typedef struct {
  int value;
} MML_ARGS_OCTAVE;

typedef struct {
  char name[MML_USER_EVENT_NAME_SIZE];
} MML_ARGS_USER_EVENT;

typedef struct {
  MML_TYPE type;
  union {
    MML_ARGS_NOTE note;
    MML_ARGS_REST rest;
    MML_ARGS_TEMPO tempo;
    MML_ARGS_LENGTH length;
    MML_ARGS_VOLUME volume;
    MML_ARGS_OCTAVE octave;
    MML_ARGS_OCTAVE octave_up;
    MML_ARGS_OCTAVE octave_down;
    MML_ARGS_USER_EVENT user_event;
  } args;
} MML_INFO;

typedef void (*MML_CALLBACK)(MML_INFO *p, void *extobj);

typedef struct {
  int octave;
  int length;
  int bticks;
} MML_OPTION;

typedef enum {
  MML_STATE_INIT,
  MML_STATE_WAIT_INIT,
  MML_STATE_WAIT_SETUP,
  MML_STATE_FETCH_READY,
} MML_STATE;

typedef struct {
  mml_stack_t left;
  mml_stack_t right;
} MML_REPEAT_STACK;

typedef struct {
  MML_CALLBACK callback;
  void *extobj;
  MML_OPTION option;
  char *tp;
  MML_STATE state;
  MML_REPEAT_STACK rs;
} MML;

typedef enum {
  MML_RESULT_OK,
  MML_RESULT_EOT,
  MML_RESULT_ILLEGAL_OCTAVE_VALUE,
  MML_RESULT_ILLEGAL_LENGTH_VALUE,
  MML_RESULT_ILLEGAL_TEMPO_VALUE,
  MML_RESULT_ILLEGAL_VOLUME_VALUE,
  MML_RESULT_ILLEGAL_NOTE_TEXT,
  MML_RESULT_ILLEGAL_SEQUENCE_FOUND,
  MML_RESULT_ILLEGAL_TOKEN_LENGTH,
  MML_RESULT_ILLEGAL_USER_EVENT_TOKEN,
  MML_RESULT_ILLEGAL_USER_EVENT_LENGTH,
  MML_RESULT_ILLEGAL_REPEAT_PAIR,
  MML_RESULT_NOT_READY_FOR_SETUP,
  MML_RESULT_NOT_READY_FOR_FETCH,
  MML_RESULT_STACK_FULL_REPEAT_LEFT,
  MML_RESULT_STACK_EMPTY_REPEAT_LEFT,
  MML_RESULT_STACK_FULL_REPEAT_RIGHT,
  MML_RESULT_STACK_EMPTY_REPEAT_RIGHT,
} MML_RESULT;

#ifdef __cplusplus
extern "C" {
#endif

MML_RESULT mml_init(MML *handle, MML_CALLBACK callback, void *extobj);
MML_RESULT mml_setup(MML *handle, MML_OPTION *option, char *text);
MML_RESULT mml_fetch(MML *handle);

#ifdef __cplusplus
}
#endif

#endif

