/*
    gmm2json: program that reads Gridmonger's GMM file and converts it into JSON
   format Copyright (C) 2025 Jagholin (github.com/Jagholin)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see
   <https://www.gnu.org/licenses/>
*/
#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <unistd.h>
// Simple result type for returning error information from functions
// negative codes are errors, 0 or positive are success.
typedef short int RESULT;
#define CHECKRESULT(...) CHECKERR(last_error < 0, __VA_ARGS__)
#define PROPAGATEERR()                                                         \
  if (last_error < 0) {                                                        \
    printf("Caught error %d on line %d in file " __FILE__ "\n", last_error,    \
           __LINE__);                                                          \
    goto onpropagate;                                                          \
  }
extern const RESULT RES_OK;
extern const RESULT RES_ERR;
extern const RESULT RES_BUFFER_TOO_SMALL;
extern const RESULT RES_BAD_INPUT;
extern RESULT last_error;

static const char oom_message[] = "Out of memory\n\r";
#define OOMERROR(ptr)                                                          \
  if (ptr == NULL) {                                                           \
    write(STDERR_FILENO, oom_message, sizeof(oom_message));                    \
    goto onoom;                                                                \
  }
#define PACKED_STRUCT struct __attribute__((__packed__))
#define CHECKERR(cond, ...)                                                    \
  if (cond) {                                                                  \
    printf(__VA_ARGS__);                                                       \
    if (last_error == RES_OK)                                                  \
      last_error = RES_ERR;                                                    \
    goto onerror;                                                              \
  }
#define CHECKEXPR(expr, cond, errstr, ...)                                     \
  if ((expr)cond) {                                                            \
    printf(errstr "\n", __VA_ARGS__);                                          \
    if (last_error == RES_OK)                                                  \
      last_error = RES_ERR;                                                    \
    goto onerror;                                                              \
  }

#endif // DEFS_H
