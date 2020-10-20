/*
  Parser for the shell designed to parse the following grammar
  ('+' = whitespace)
  expr: s / s + op + expr
  s: {expr} / invoke
  invoke: cmd [ + | + cmd ]...
  op: && / || / ; / =
  redir: < / > / >>
  cmd: EXECUTABLE [+ arg]... [ + &] [+ redir + FILE_NAME/DELIM]
  arg: $NAMED_CONSTANT / LITERAL

  IMPORTANT: Don't forget to call init() to intialize the array of user
  defined constants and finish() to cleanup the array
*/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>

#define BUFF_MAX 1024 /* Maximum number of characters in the character buffer */
#define INVALID_POS -1
#define INIT_CONSTS 8 /* Initial number of constants to allocate memory for */
#define MAX_ARGS 1024 /* Maximum number of arguments in argv */

char ***consts; /* Array of string pairs to store user defined constants. If we have more time, this should be replaced with a BST */
unsigned int numConsts; /* Current number of constants ie. next free index */
unsigned int maxConsts; /* Current maximum number of user defined constants */

void init();
void finish();
bool addConst(char*, char*);
char* getConst(char*);
bool isOp(char*);
bool isPipe(char*);
bool isRedir(char*);
bool matchBrace(char*, int*, const unsigned int);
bool matchQuote(char*, int*, const unsigned int);
bool parseExpr(char*, char*, char*, char*);
bool parseCmd(char*, const unsigned int, char*, char**, char**, char**, unsigned int*, unsigned int*, unsigned int*, bool*);
bool parseInvoke(char*, char**, unsigned int*, unsigned int*);
bool parseS(char*, char*, char*);
char* evalArg(char*);
bool getExecPath(char*, char*);
int evalCmd(int, int, char*);
int evalInvoke(char*);
int evalS(char*);
int evalExpr(char*);
char *read_command(void);
