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

static inline int next_bf(char* program);
static inline void interpret(char* program, int *jmptab);
static inline int mod(int a, int b);
static inline int valid_bf(char* program, int *jmptab);

#ifndef PROG_MAX_LEN
#define PROG_MAX_LEN 40
#endif
#ifndef MEMCELL
#define MEMCELL unsigned char
#endif
#ifndef MAXCELLS
#define MAXCELLS 256
#endif

static int solutions[256][MAXCELLS];
static int max_steps = 5000;

int main(int argc, char* argv[])
{
static int jmptab[PROG_MAX_LEN];
static char program[PROG_MAX_LEN];

  printf("  _                        \n");
  printf(" / `_   _  _  /__/|   _  /_\n");
  printf("/_,//_// //_ / // /_//_ /\\ \n");
  printf("\n");

  if(argc != 1) {
    printf("Usage: %s < last.log > this.log\n", argv[0]);
    exit(1);
  }

  *program = 0;

  printf("Reading a list of already known solutions for...\n");
  fflush(stdout);

  while(1) {
    int d;
    char buf1[BUFSIZ], buf2[BUFSIZ], buf3[BUFSIZ];
    char linebuf[BUFSIZ];

    if (fgets(linebuf, sizeof(linebuf), stdin) != 0) {
	if(sscanf(linebuf, "%s %d %s %s",buf1, &d, buf2, buf3) == 4 &&
	    strcmp(buf1, "Result:") == 0) {

	  if(valid_bf(buf3, jmptab)) interpret(buf3, jmptab);

	} else
	if(sscanf(linebuf, "%s %s %s",buf1, buf2, buf3) == 3 &&
	    strcmp(buf1, "Last") == 0) {
	    strcpy(program, buf3);
	} else
	if(sscanf(linebuf, "%s %s %s %s %s %s",buf1, buf2, buf2, buf2, buf2, buf3) == 6 &&
	    strcmp(buf1, "Starting") == 0) {
	    max_steps = atoi(buf3);
	}
	// else printf("%s", linebuf);
    }
    else break;
  }
  printf("Done reading...\n");
  printf("Starting from %s, steps = %d\n", program, max_steps);
  fflush(stdout);

  do {
    if(valid_bf(program, jmptab)) interpret(program, jmptab);
  } while(next_bf(program) <= 12);

  printf("Last program: %s\n", program);
  return 0;
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
int next_bf(char* program)
{
  int pos = 0;
  int done;

  switch(program[pos]) {
    case '\0':
      program[pos] = '+';
      return pos;
    case '+':
      program[pos] = '-';
      return pos;
    case '-':
      program[pos] = '>';
      return pos;
    case '>':
    case '<':
    case '[':
    case ']':
      program[pos] = '+';
      pos++;
      done = 0;
  }

  switch(program[pos]) {
    case '\0':
      program[pos] = '+';
      return pos;
    case '+':
      program[pos] = '-';
      return pos;
    case '-':
      program[pos] = '>';
      return pos;
    case '>':
    case '<':
      program[pos] = '[';
      return pos;
    case '[':
    case ']':
      program[pos] = '+';
      pos++;
      done = 0;
  }

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

  return pos;
}

static inline
void interpret(char* program, int *jmptab)
{
static MEMCELL memory[MAXCELLS];
static int mcptr = 0;
  int iptr = 0;
  int cptr = 0;
  int steps = 0;

  memset(memory, '\0', (mcptr+1)*sizeof*memory);
  mcptr = 0;

  for(;;) {
    if(steps > max_steps) {
	// fprintf(stderr, "Longrunner: %s\n", program);
	return;
    }
    steps++;

    //printf("%d %c (iptr := %d, cptr := %d)\n",steps, program[iptr], iptr, cptr);

    if (!program[iptr])
	break;
    else switch(program[iptr]) {
      case '+': memory[cptr]++; break;
      case '-': memory[cptr]--; break;
      case '>':
	    if (cptr == mcptr) {
		if (cptr == MAXCELLS-1) return;
		mcptr = ++cptr;
	    } else
		cptr++;
	    break;
      case '<': if (cptr == 0) return; cptr--; break;
      case '[': if(memory[cptr] == 0) iptr = jmptab[iptr]; break;
      case ']': if(memory[cptr] != 0) iptr = jmptab[iptr]; break;
    }

    iptr++;
  }

  if(memory[cptr] < 248 && memory[cptr] > 8) {
      int l = iptr;
      int val = memory[cptr];
      int cells = mcptr + 1;
      steps--; // The nul

      if(solutions[val][cells] == 0 || solutions[val][cells] >= l) {

	printf("Result: %u == %s (%d, %d, %d)\n",
		val, program, l, cells, steps);
	fflush(stdout);
	solutions[val][cells] = l;
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

#if 1
  for(pc=0; program[pc]; ) {
    char c = program[pc];
    switch(c) {
      case '[':
        if(program[pc+1] == ']') return 0;
        level++;
        break;
      case ']':
        if(program[pc+1] == '[') return 0;	// Comment loops waste space
        if(program[pc+1] == '\0') return 0;	// Not looking for zeros.

        level--;
	if(level < 0) return 0;
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
  if(level != 0) return 0;
#endif

  for(pc=0; program[pc]; ) {
    char c = program[pc];
    switch(c) {
      case '[':
        if(program[pc+1] == ']') return 0;

#if 0
	if((program[pc+1] == '+' || program[pc+1] == '-') && program[pc+2] != ']')
	{
	    // If the loop has just '+', '-' it's zero or infinite.
	    // The 3 byte zero has been seen already and we don't want infinite.
	    int po = 1, ok = 0;
	    while(program[pc+po] != ']')
	    {
		if (program[pc+po] != '+' && program[pc+po] != '-') {
		    ok = 1;
		    break;
		}
		po++;
	    }
	    if (!ok) return 0;
	}
#endif

#if 1
	if(program[pc+1] != '-' || program[pc+2] != ']')
	{
	    int po = 1, sc = 0, shift = 0, sum = 0;
	    while(program[pc+po] && program[pc+po] != ']' && program[pc+po] != '[') {
		if (shift == 0 && program[pc+po] == '+') sum++;
		if (shift == 0 && program[pc+po] == '-') sum--;
		if (program[pc+po] == '>') shift++, sc++;
		if (program[pc+po] == '<') shift--, sc++;
		po++;
	    }

	    /* Infinite loop or skipped loop. */
	    if (program[pc+po] == ']' && sum == 0 && shift == 0) return 0;
	    if (program[pc+po] == ']' && sc == 0) return 0;
	}
#endif

	stk[level] = pc;
        level++;
        break;
      case ']':
        if(program[pc+1] == '[') return 0;	// Comment loops waste space
        if(program[pc+1] == '\0') return 0;	// Not looking for zeros.

        level--;
	if(level < 0) return 0;

	jmptab[pc] = stk[level];
	jmptab[stk[level]] = pc;
        break;
#if 0
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
#endif
    }
    pc++;
  }

  if(level != 0)
    return 0;

  return 1;
}
