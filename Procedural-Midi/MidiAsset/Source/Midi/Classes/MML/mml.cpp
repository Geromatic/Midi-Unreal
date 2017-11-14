/**
 * @file mml.c
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

#include "mml.h"

#include "ntlibc.h"

#define MML_WORK_BUFFER_SIZE    (8)

#define LIBC_TOUPPER            ntlibc_toupper
#define LIBC_STRCHR             ntlibc_strchr
#define LIBC_ATOI               ntlibc_atoi


#define TEXT_READ(HANDLE)            ((char)LIBC_TOUPPER(*(HANDLE)->tp))
#define TEXT_NEXT(HANDLE)            do { ((HANDLE)->tp)++; } while (0)
#define TEXT_SET_POINTER(HANDLE, P)  do { ((HANDLE)->tp) = (P); } while (0)
#define TEXT_GET_POINTER(HANDLE, PP) do { (*PP) = ((HANDLE)->tp); } while (0)

typedef enum {
  MML_CHAR_TYPE_NOTE,
  MML_CHAR_TYPE_SHARP,
  MML_CHAR_TYPE_FLAT,
  MML_CHAR_TYPE_REST,
  MML_CHAR_TYPE_OCTAVE,
  MML_CHAR_TYPE_LENGTH,
  MML_CHAR_TYPE_VOLUME,
  MML_CHAR_TYPE_TEMPO,
  MML_CHAR_TYPE_OCTAVE_UP,
  MML_CHAR_TYPE_OCTAVE_DOWN,
  MML_CHAR_TYPE_NUMBER,
  MML_CHAR_TYPE_REPEAT_LEFT,
  MML_CHAR_TYPE_REPEAT_RIGHT,
  MML_CHAR_TYPE_USER_EVENT_MARKER,
  MML_CHAR_TYPE_UNKNOWN,
} MML_CHAR_TYPE;

static MML_RESULT skip_whitespace(MML *handle)
{
  while (((TEXT_READ(handle)) == ' ') || ((TEXT_READ(handle)) == '\r') || ((TEXT_READ(handle)) == '\n') || ((TEXT_READ(handle)) == '\t')) {
    TEXT_NEXT(handle);
  }
  return MML_RESULT_OK;
}

static MML_RESULT get_note_number(MML *handle, char *text, int *number)
{
  char *note_distance = (char *)"C D EF G A B";
  char *p;

  p = LIBC_STRCHR(note_distance, text[0]);
  if (p == 0) {
    return MML_RESULT_ILLEGAL_NOTE_TEXT;
  }
  *number = (int)(p - note_distance);

  p = &text[1];
  while (*p) {
    if (*p == '+') {
      *number = *number + 1;
    }
    if (*p == '-') {
      *number = *number - 1;
    }
    p++;
  }

  *number += (handle->option.octave * 12);

  return MML_RESULT_OK;
}

static MML_RESULT convert_note_length_to_ticks(MML *handle, int note_length, int *ticks)
{
  int bticks = handle->option.bticks;

  switch (note_length) {
    case 0:
      *ticks = 0;
      break;
    case 1:
      *ticks = (bticks * 4);
      break;
    case 2:
      *ticks = (bticks * 2);
      break;
    case 4:
      *ticks = (bticks * 1);
      break;
    case 8:
      *ticks = (bticks / 2);
      break;
    case 16:
      *ticks = (bticks / 4);
      break;
    case 32:
      *ticks = (bticks / 8);
      break;
    case 64:
      *ticks = (bticks / 16);
      break;
    case 23:
      /*
       * 1/2 note triplet.
       */
      *ticks = (bticks * 4) / 3;
      break;
    case 43:
      /*
       * 1/4 note triplet.
       */
      *ticks = (bticks * 2) / 3;
      break;
    case 83:
      /*
       * 1/8 note triplet.
       */
      *ticks = (bticks * 1) / 3;
      break;
    case 163:
      /*
       * 1/16 note triplet.
       */
      *ticks = (bticks / 2) / 3;
      break;
    case 323:
      /*
       * 1/32 note triplet.
       */
      *ticks = (bticks / 4) / 3;
      break;
    case 643:
      /*
       * 1/64 note triplet.
       */
      *ticks = (bticks / 8) / 3;
      break;
    default:
      *ticks = 0;
      return MML_RESULT_ILLEGAL_LENGTH_VALUE;
  }

  return MML_RESULT_OK;
}

static MML_RESULT get_note_ticks(MML *handle, char *text, int *ticks)
{
  int note_length = LIBC_ATOI(text);
  char *p;
  int val;
  MML_RESULT mr;
  mr = convert_note_length_to_ticks(handle, note_length, &val);
  if (mr != MML_RESULT_OK) {
    return mr;
  }
  *ticks = val;
  p = &text[0];
  while (*p) {
    if (*p == '.') {
      note_length *= 2;
      mr = convert_note_length_to_ticks(handle, note_length, &val);
      if (mr != MML_RESULT_OK) {
        return mr;
      }
      *ticks += val;
    }
    p++;
  }
  return MML_RESULT_OK;
}

static MML_RESULT get_note_ticks_default(MML *handle, int *ticks)
{
  int val;
  MML_RESULT mr;
  mr = convert_note_length_to_ticks(handle, handle->option.length, &val);
  *ticks = val;
  return mr;
}

static MML_CHAR_TYPE get_char_type(char c)
{
  switch (c) {
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'A':
    case 'B':
      return MML_CHAR_TYPE_NOTE;
    case '+':
      return MML_CHAR_TYPE_SHARP;
    case '-':
      return MML_CHAR_TYPE_FLAT;
    case 'R':
      return MML_CHAR_TYPE_REST;
    case 'O':
      return MML_CHAR_TYPE_OCTAVE;
    case 'L':
      return MML_CHAR_TYPE_LENGTH;
    case 'V':
      return MML_CHAR_TYPE_VOLUME;
    case 'T':
      return MML_CHAR_TYPE_TEMPO;
#if (MML_CONFIG_OCTAVE_REVERSE)
    case '<':
      return MML_CHAR_TYPE_OCTAVE_UP;
    case '>':
      return MML_CHAR_TYPE_OCTAVE_DOWN;
#else
    case '>':
      return MML_CHAR_TYPE_OCTAVE_UP;
    case '<':
      return MML_CHAR_TYPE_OCTAVE_DOWN;
#endif
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '.':
      return MML_CHAR_TYPE_NUMBER;
    case '[':
      return MML_CHAR_TYPE_REPEAT_LEFT;
    case ']':
      return MML_CHAR_TYPE_REPEAT_RIGHT;
    case '@':
      return MML_CHAR_TYPE_USER_EVENT_MARKER;
  }
  return MML_CHAR_TYPE_UNKNOWN;
}

static MML_RESULT get_token(MML *handle, char *buf, int siz)
{
  MML_CHAR_TYPE ct_head, ct_next;
  int i;
  int cnt = 0;

  for (i = 0; i < siz; i++) {
    buf[i] = '\0';
  }

  skip_whitespace(handle);
  if (TEXT_READ(handle) == '\0') {
    char *p;
    if (mml_stack_pop(&(handle->rs.left), &p) == MML_STACK_RESULT_OK) {
      return MML_RESULT_ILLEGAL_REPEAT_PAIR;
    }
    if (mml_stack_pop(&(handle->rs.right), &p) == MML_STACK_RESULT_OK) {
      return MML_RESULT_ILLEGAL_REPEAT_PAIR;
    }
    return MML_RESULT_EOT;
  }

  ct_head = get_char_type(TEXT_READ(handle));
  while ((ct_next = get_char_type(TEXT_READ(handle))) == ct_head) {
    *buf++ = TEXT_READ(handle);
    TEXT_NEXT(handle);
    if ((MML_CHAR_TYPE_NOTE == ct_head) && (MML_CHAR_TYPE_NOTE == ct_next)) {
      break;
    }
    if ((MML_CHAR_TYPE_REPEAT_LEFT == ct_head) && (MML_CHAR_TYPE_REPEAT_LEFT == ct_next)) {
      break;
    }
    if ((MML_CHAR_TYPE_REPEAT_RIGHT == ct_head) && (MML_CHAR_TYPE_REPEAT_RIGHT == ct_next)) {
      break;
    }
    cnt++;
    if (siz <= (cnt + 1)) {
      *buf++ = 0;
      return MML_RESULT_ILLEGAL_TOKEN_LENGTH;
    }
  }

  while (1) {
    MML_CHAR_TYPE ct_work;
    ct_work = get_char_type(TEXT_READ(handle));
    if ((ct_work != MML_CHAR_TYPE_SHARP) && (ct_work != MML_CHAR_TYPE_FLAT)) {
      break;
    }
    *buf++ = TEXT_READ(handle);
    TEXT_NEXT(handle);
  }

  *buf++ = 0;

  return MML_RESULT_OK;
}

MML_RESULT mml_init(MML *handle, MML_CALLBACK callback, void *extobj)
{
  handle->callback = callback;
  handle->extobj = extobj;

  TEXT_SET_POINTER(handle, 0);
  mml_stack_init(&(handle->rs.left));
  mml_stack_init(&(handle->rs.right));

  handle->state = MML_STATE_WAIT_SETUP;

  return MML_RESULT_OK;
}

MML_RESULT mml_setup(MML *handle, MML_OPTION *option, char *text)
{
  if ((handle->state != MML_STATE_WAIT_SETUP) && (handle->state != MML_STATE_FETCH_READY)) {
    return MML_RESULT_NOT_READY_FOR_SETUP;
  }

  if (option != 0) {
    handle->option.octave = option->octave;
    handle->option.length = option->length;
    handle->option.bticks = option->bticks;
  } else {
    handle->option.octave = MML_OPTION_DEFAULT_OCTAVE;
    handle->option.length = MML_OPTION_DEFAULT_LENGTH;
    handle->option.bticks = MML_OPTION_DEFAULT_BTICKS;
  }

  TEXT_SET_POINTER(handle, text);
  mml_stack_init(&(handle->rs.left));
  mml_stack_init(&(handle->rs.right));

  handle->state = MML_STATE_FETCH_READY;

  return MML_RESULT_OK;
}

static MML_RESULT get_user_event_token(MML *handle, char *buf, int siz)
{
  int i;
  int cnt = 0;

  for (i = 0; i < siz; i++) {
    buf[i] = '\0';
  }

  if (TEXT_READ(handle) != '{') {
    return MML_RESULT_ILLEGAL_USER_EVENT_TOKEN;
  }
  TEXT_NEXT(handle);

  while (TEXT_READ(handle) != '}') {
    *buf++ = TEXT_READ(handle);
    TEXT_NEXT(handle);
    if (TEXT_READ(handle) == '\0') {
      return MML_RESULT_ILLEGAL_SEQUENCE_FOUND;
    }
    cnt++;
    if (siz <= (cnt + 1)) {
      *buf++ = 0;
      return MML_RESULT_ILLEGAL_USER_EVENT_LENGTH;
    }
  }
  *buf++ = 0;
  TEXT_NEXT(handle);
  return MML_RESULT_OK;
}

MML_RESULT mml_fetch(MML *handle)
{
  MML_RESULT mr;
  char buf1[MML_WORK_BUFFER_SIZE];
  char buf2[MML_WORK_BUFFER_SIZE];

  if (handle->state != MML_STATE_FETCH_READY) {
    return MML_RESULT_NOT_READY_FOR_FETCH;
  }

  mr = get_token(handle, buf1, sizeof(buf1));
  if (mr != MML_RESULT_OK) {
    return mr;
  }

  switch (get_char_type(buf1[0])) {
    case MML_CHAR_TYPE_NOTE:
      {
        char *p;
        TEXT_GET_POINTER(handle, &p);
        mr = get_token(handle, buf2, sizeof(buf2));
        if (get_char_type(buf2[0]) == MML_CHAR_TYPE_NUMBER) {
          MML_INFO info;
          info.type = MML_TYPE_NOTE;
          mr = get_note_number(handle, buf1, &(info.args.note.number));
          if (mr != MML_RESULT_OK) {
            return mr;
          }
          mr = get_note_ticks(handle, buf2, &(info.args.note.ticks));
          if (mr != MML_RESULT_OK) {
            return mr;
          }
          handle->callback(&info, handle->extobj);
        } else {
          MML_INFO info;
          info.type = MML_TYPE_NOTE;
          mr = get_note_number(handle, buf1, &(info.args.note.number));
          if (mr != MML_RESULT_OK) {
            return mr;
          }
          mr = get_note_ticks_default(handle, &(info.args.note.ticks));
          if (mr != MML_RESULT_OK) {
            return mr;
          }
          handle->callback(&info, handle->extobj);
          TEXT_SET_POINTER(handle, p);
        }
      }
      break;
    case MML_CHAR_TYPE_REST:
      {
        char *p;
        TEXT_GET_POINTER(handle, &p);
        mr = get_token(handle, buf2, sizeof(buf2));
        if (get_char_type(buf2[0]) == MML_CHAR_TYPE_NUMBER) {
          MML_INFO info;
          info.type = MML_TYPE_REST;
          mr = get_note_ticks(handle, buf2, &(info.args.rest.ticks));
          if (mr != MML_RESULT_OK) {
            return mr;
          }
          handle->callback(&info, handle->extobj);
        } else {
          MML_INFO info;
          info.type = MML_TYPE_REST;
          mr = get_note_ticks_default(handle, &(info.args.rest.ticks));
          if (mr != MML_RESULT_OK) {
            return mr;
          }
          handle->callback(&info, handle->extobj);
          TEXT_SET_POINTER(handle, p);
        }
      }
      break;
    case MML_CHAR_TYPE_TEMPO:
      {
        mr = get_token(handle, buf2, sizeof(buf2));
        if (get_char_type(buf2[0]) == MML_CHAR_TYPE_NUMBER) {
          MML_INFO info;
          info.type = MML_TYPE_TEMPO;
          info.args.tempo.value = LIBC_ATOI(buf2);
          handle->callback(&info, handle->extobj);
        } else {
          return MML_RESULT_ILLEGAL_TEMPO_VALUE;
        }
      }
      break;
    case MML_CHAR_TYPE_VOLUME:
      {
        mr = get_token(handle, buf2, sizeof(buf2));
        if (get_char_type(buf2[0]) == MML_CHAR_TYPE_NUMBER) {
          MML_INFO info;
          info.type = MML_TYPE_VOLUME;
          info.args.volume.value = LIBC_ATOI(buf2);
          handle->callback(&info, handle->extobj);
        } else {
          return MML_RESULT_ILLEGAL_VOLUME_VALUE;
        }
      }
      break;
    case MML_CHAR_TYPE_LENGTH:
      {
        mr = get_token(handle, buf2, sizeof(buf2));
        if (get_char_type(buf2[0]) == MML_CHAR_TYPE_NUMBER) {
          MML_INFO info;
          info.type = MML_TYPE_LENGTH;
          info.args.length.value = LIBC_ATOI(buf2);
          handle->option.length = info.args.length.value;
          handle->callback(&info, handle->extobj);
        } else {
          return MML_RESULT_ILLEGAL_LENGTH_VALUE;
        }
      }
      break;
    case MML_CHAR_TYPE_OCTAVE:
      {
        mr = get_token(handle, buf2, sizeof(buf2));
        if (get_char_type(buf2[0]) == MML_CHAR_TYPE_NUMBER) {
          MML_INFO info;
          info.type = MML_TYPE_OCTAVE;
          info.args.octave.value = LIBC_ATOI(buf2);
          handle->option.octave = info.args.octave.value;
          handle->callback(&info, handle->extobj);
        } else {
          return MML_RESULT_ILLEGAL_OCTAVE_VALUE;
        }
      }
      break;
    case MML_CHAR_TYPE_OCTAVE_UP:
      {
        MML_INFO info;
        char *p = buf1;
        while (*p++) {
          if (handle->option.octave < MML_OCTAVE_MAX) {
            handle->option.octave++;
          }
        }
        info.type = MML_TYPE_OCTAVE_UP;
        info.args.octave_up.value = handle->option.octave;
        handle->callback(&info, handle->extobj);
      }
      break;
    case MML_CHAR_TYPE_OCTAVE_DOWN:
      {
        MML_INFO info;
        char *p = buf1;
        while (*p++) {
          if (MML_OCTAVE_MIN < handle->option.octave) {
            handle->option.octave--;
          }
        }
        info.type = MML_TYPE_OCTAVE_DOWN;
        info.args.octave_down.value = handle->option.octave;
        handle->callback(&info, handle->extobj);
      }
      break;
    case MML_CHAR_TYPE_REPEAT_LEFT:
      {
        char *rsp;
        TEXT_GET_POINTER(handle, &rsp);
        rsp--;
        if (mml_stack_push(&(handle->rs.left), rsp) != MML_STACK_RESULT_OK) {
          return MML_RESULT_STACK_FULL_REPEAT_LEFT;
        }
      }
      break;
    case MML_CHAR_TYPE_REPEAT_RIGHT:
      {
        char *rsp;
        char *p;
        TEXT_GET_POINTER(handle, &rsp);
        rsp--;
        if (mml_stack_pop(&(handle->rs.right), &p) == MML_STACK_RESULT_OK) {
          if (rsp == p) {
            /*
             * Nothing to do.
             */
          } else {
            if (mml_stack_push(&(handle->rs.right), p) != MML_STACK_RESULT_OK) {
              return MML_RESULT_STACK_FULL_REPEAT_RIGHT;
            }
            if (mml_stack_push(&(handle->rs.right), rsp) != MML_STACK_RESULT_OK) {
              return MML_RESULT_STACK_FULL_REPEAT_RIGHT;
            }
            if (mml_stack_pop(&(handle->rs.left), &p) != MML_STACK_RESULT_OK) {
              return MML_RESULT_STACK_EMPTY_REPEAT_LEFT;
            }
            TEXT_SET_POINTER(handle, p);
            TEXT_NEXT(handle);
          }
        } else {
          if (mml_stack_push(&(handle->rs.right), rsp) != MML_STACK_RESULT_OK) {
            return MML_RESULT_STACK_FULL_REPEAT_RIGHT;
          }
          if (mml_stack_pop(&(handle->rs.left), &p) != MML_STACK_RESULT_OK) {
            return MML_RESULT_STACK_EMPTY_REPEAT_LEFT;
          }
          TEXT_SET_POINTER(handle, p);
          TEXT_NEXT(handle);
        }
      }
      break;
    case MML_CHAR_TYPE_USER_EVENT_MARKER:
      {
        MML_INFO info;
        info.type = MML_TYPE_USER_EVENT;
        mr = get_user_event_token(handle, info.args.user_event.name, sizeof(info.args.user_event.name));
        if (mr == MML_RESULT_OK) {
          handle->callback(&info, handle->extobj);
        }
      }
      break;
    case MML_CHAR_TYPE_NUMBER:
    case MML_CHAR_TYPE_SHARP:
    case MML_CHAR_TYPE_FLAT:
    case MML_CHAR_TYPE_UNKNOWN:
      {
        /*
         * This is a illegal case of the parsing because these characters are needed a command.
         */
        return MML_RESULT_ILLEGAL_SEQUENCE_FOUND;
      }
      break;
  }

  return mr;
}

