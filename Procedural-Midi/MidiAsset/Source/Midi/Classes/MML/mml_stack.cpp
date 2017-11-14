/**
 * @file mml_stack.c
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

#include "mml_stack.h"

MML_STACK_RESULT mml_stack_init(mml_stack_t *p)
{
  p->count = 0;
  return MML_STACK_RESULT_OK;
}

MML_STACK_RESULT mml_stack_push(mml_stack_t *p, char *pointer)
{
  if (p->count == MML_STACK_MAX_DEPTH) {
    return MML_STACK_RESULT_ERROR_FULL;
  }
  p->array[p->count].pointer = pointer;
  p->count++;
  return MML_STACK_RESULT_OK;
}

MML_STACK_RESULT mml_stack_pop(mml_stack_t *p, char **pointer)
{
  if (p->count == 0) {
    return MML_STACK_RESULT_ERROR_EMPTY;
  }
  p->count--;
  *pointer = p->array[p->count].pointer;
  return MML_STACK_RESULT_OK;
}

