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

static inline char* next_bf(char* program);
static inline void interpret(char* program, int* solutions);
static inline int mod(int a, int b);
static inline int valid_bf(char* program, int *jmptab);

#ifndef PROG_MAX_LEN
#define PROG_MAX_LEN 40
#endif
#ifndef PROG_MAX_STEPS
#define PROG_MAX_STEPS 5000
#endif

int main(int argc, char* argv[])
{
  printf("  _                        \n");
  printf(" / `_   _  _  /__/|   _  /_\n");
  printf("/_,//_// //_ / // /_//_ /\\ \n");
  printf("\n");


  char* program;
  int* solutions;

  if(argc != 2) {
    printf("Usage: %s prog n\n", argv[0]);
    printf("\tprog - A start program (example: ++++++++)\n");
    printf("\tn - How many (next) programs you want to check\n");
    exit(1);
  }

  fflush(stdout);

  program = calloc(PROG_MAX_LEN, sizeof(char));
  assert(program != NULL);

  solutions = calloc(256, sizeof(int));
  assert(solutions != NULL);

  // strncpy(program, argv[1], PROG_MAX_LEN - 1);
  strcpy(program, "+");

  unsigned long long int limit = strtoull(argv[1], NULL, 10), i = 0;
  if (limit <= 0) limit = 4294967295LL;

  printf("Reading a list of already known solutions for...\n");
  while(1) {
    int d;
    char buf1[256], buf2[256], buf3[256];
    char linebuf[2048];

    if (fgets(linebuf, sizeof(linebuf), stdin) != 0) {
	if(sscanf(linebuf, "%s %d %s %s",buf1, &d, buf2, buf3) == 4 &&
	    strcmp(buf1, "Result:") == 0) {
	  solutions[d] = strlen(buf3);
	  printf("Result: %d == %s\n", d, buf3);
	}
	if(sscanf(linebuf, "%s %s %s",buf1, buf2, buf3) == 3 &&
	    strcmp(buf1, "Last") == 0) {
	    strcpy(program, buf3);
	}
    }
    else break;
  }
  printf("Done reading...\n");
  printf("Starting from %s, steps = %d\n", program, PROG_MAX_STEPS);
  fflush(stdout);

  for(;i < limit;i++) {
    interpret(program, solutions);
    program = next_bf(program);
  }

  printf("Last program: %s\n", program);
}

static inline
int mod(int a, int b) {
  if(a > b)
    return a - b;
  if(a < 0)
    return a + b;
  return a;
}

static inline
char* next_bf(char* program)
{
  int pos = 0;
  int done;

  do {
      done = 1;

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
	  pos++;
	  done = 0;
      }
  } while(!done);

  return program;
}

static inline
void interpret(char* program, int* solutions)
{
  unsigned char  memory[256] = {0};
  int jmptab[PROG_MAX_LEN] = {0};
  int iptr = 0;
  unsigned char cptr = 0;
  int steps = 0;

  if(!valid_bf(program, jmptab)) return;

  for(;;) {
    if(steps > PROG_MAX_STEPS) return;
    steps++;

    //printf("%d %c (iptr := %d, cptr := %d)\n",steps, program[iptr], iptr, cptr);

    if (!program[iptr])
	break;
    else switch(program[iptr]) {
      case '+': memory[cptr]++; break;
      case '-': memory[cptr]--; break;
      case '>': if (cptr == 255) return; cptr++; break;
      case '<': if (cptr == 0) return; cptr--; break;
      case '[': if(memory[cptr] == 0) iptr = jmptab[iptr]; break;
      case ']': if(memory[cptr] != 0) iptr = jmptab[iptr]; break;
    }

    iptr++;
  }

  if(memory[cptr] < 248 && memory[cptr] > 8) {
      int l = iptr;

      if(solutions[memory[cptr]] == 0 || solutions[memory[cptr]] == l) {

	printf("Result: %u == %s\n", memory[cptr], program);
	fflush(stdout);
	solutions[memory[cptr]] = l;
      }
  }
}

static inline
int valid_bf(char* program, int *jmptab)
{
  int level = 0;
  int stk[PROG_MAX_LEN];
  int pc = 0;
  if (program[0] == '<') return 0;
  if (program[0] == '[') return 0;
  if (program[0] == ']') return 0;
  while(program[pc]) {
    char c = program[pc];
    switch(c) {
      case '[':
        if(program[pc+1] == ']') return 0;

	stk[level] = pc;
        level++;
	if (program[pc+1] == ']') return 0;
        break;
      case ']':
        if(program[pc+1] == '[') return 0;	// Comment loops waste space
        if(program[pc+1] == '\0') return 0;	// Not looking for zeros.

        level--;
	if(level < 0) return 0;

	jmptab[pc] = stk[level];
	jmptab[stk[level]] = pc;
        break;
      case '+':
        if(program[pc+1] == '-') return 0;
	break;
      case '-':
        if(program[pc+1] == '+') return 0;
	break;
      case '>':
        if(program[pc+1] == '<') return 0;
	break;
      case '<':
        if(program[pc+1] == '>') return 0;
	break;
      default: break;
    }
    pc++;
  }

  if(level != 0)
    return 0;

  return 1;
}
