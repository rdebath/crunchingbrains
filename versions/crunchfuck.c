/*
Copyright (c) 2012, Roman Muentener, fmnssun _at_ gmail [dot] com

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

int valid_bf(char* program);
char* next_bf(char* program, int pos);
void interpret(char* program, int* solutions);
inline int mod(int a, int b);

#define PROG_MAX_LEN 40
#define PROG_MAX_STEPS 5000

int main(int argc, char* argv[])
{
  printf("  _                        \n");
  printf(" / `_   _  _  /__/|   _  /_\n");
  printf("/_,//_// //_ / // /_//_ /\\ \n");
  printf("\n");


  char* program;
  int* solutions;

  if(argc != 3) {
    printf("Usage: %s prog n\n", argv[0]);
    printf("\tprog - A start program (example: ++++++++)\n");
    printf("\tn - How many (next) programs you want to check\n");
    exit(1);
  }

  fflush(stdout);

  program = calloc(PROG_MAX_LEN, sizeof(char));
  assert(program != NULL);

  solutions = calloc(265, sizeof(int));
  assert(solutions != NULL);

  strncpy(program, argv[1], PROG_MAX_LEN - 1);

  unsigned long int limit = strtoul(argv[2], NULL, 10), i = 0;
  assert(limit > 0);

  printf("Reading a list of already known solutions for...\n");
  while(1) {
    int d;
    if(scanf("%d",&d) == 1) {
      solutions[d] = 1;
    }
    else break;
  }
  printf("Done reading...\n");


  for(;i < limit;i++) {
    if(valid_bf(program))
      interpret(program, solutions);
    program = next_bf(program, 0);
  }

  printf("Last program: %s\n", program);
}

inline int mod(int a, int b) {
  if(a > b)
    return a - b;
  if(a < 0)
    return a + b;
  return a;
}

char* next_bf(char* program, int pos)
{
  if(pos >= PROG_MAX_LEN) {
    fputs("Presumed maximum length exceeded!\n", stderr);
    exit(1);
  }

  /* Next instruction */
  switch(program[pos]) {
    case '\0':
      program[pos] = '+';
      break;
    case '+':
      program[pos] = '-';
      break;
    case '-':
      program[pos] = '>';
      break;
    case '>':
      program[pos] = '<';
      break;
    case '<':
      program[pos] = '[';
      break;
    case '[':
      program[pos] = ']';
      break;
    case ']':
      program[pos] = '+';
      return next_bf(program, pos+1);
  }

  return program;
}

void interpret(char* program, int* solutions)
{
  unsigned char memory[256] = {0};
  short iptr = 0;
  unsigned char cptr = 0;
  short steps = 0;
  short len = strlen(program);

  while(iptr < len) {
    if(steps > PROG_MAX_STEPS) return;
    steps++;

    //printf("%d %c (iptr := %d, cptr := %d)\n",steps, program[iptr], iptr, cptr);

    switch(program[iptr]) {
      case '+':
        memory[cptr]++;
        break;
      case '-':
        memory[cptr]--;
        break;
      case '>':
        cptr++;
        break;
      case '<':
        cptr--;
        break;
      case '[':
        if(memory[cptr] == 0) {
          int lvl = 0;
          iptr++;
          while(iptr < len) {
            if(program[iptr] == ']')
              if(lvl < 1) break;
              else lvl--;
            if(program[iptr] == '[')
              lvl++;
            iptr++;
          }
        }
        break;
      case ']':
        if(memory[cptr] != 0) {
          int lvl = 0;
          iptr--;
          while(iptr >= 0) {
            if(program[iptr] == '[')
              if(lvl < 1) break;
              else lvl--;
            if(program[iptr] == ']')
              lvl++;
            iptr--;
          }
        }
        break;
    }

    iptr++;
  }

  if(!solutions[memory[cptr]]) {
    printf("Result: %u -> %s\n", memory[cptr], program);
    fflush(stdout);
    solutions[memory[cptr]] = 1;
  }
}

int valid_bf(char* program)
{
  int level = 0;
  while(*program) {
    char c = *program;
    switch(c) {
      case '[':
        level++;
        break;
      case ']':
        level--;
        break;
      default: break;
    }

    if(level < 0)
      return 0;

    program++;
  }

  if(level != 0)
    return 0;

  return 1;
}
