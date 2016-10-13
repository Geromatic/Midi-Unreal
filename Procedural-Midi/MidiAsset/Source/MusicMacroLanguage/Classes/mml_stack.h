/**
 * @file mml_stack.h
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

#ifndef MML_STACK_H
#define MML_STACK_H

#define MML_STACK_MAX_DEPTH (8)

typedef struct {
  char *pointer;
} mml_stack_data_t;

typedef struct {
  mml_stack_data_t array[MML_STACK_MAX_DEPTH];
  int count;
} mml_stack_t;

typedef enum {
  MML_STACK_RESULT_OK           = 0,
  MML_STACK_RESULT_ERROR_FULL   = 1,
  MML_STACK_RESULT_ERROR_EMPTY  = 2,
} MML_STACK_RESULT;

#ifdef __cplusplus
extern "C" {
#endif

MML_STACK_RESULT mml_stack_init(mml_stack_t *p);
MML_STACK_RESULT mml_stack_push(mml_stack_t *p, char *pointer);
MML_STACK_RESULT mml_stack_pop(mml_stack_t *p, char **pointer);

#ifdef __cplusplus
}
#endif

#endif

