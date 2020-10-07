/*
  Parser for the shell designed to parse the following grammar
  ('+' = whitespace)
  expr: s / s + op + expr
  s: {expr} / cmd [... + arg] [ + &]
  op: && / || / | / ; / < / << / > / >>
  cmd: All supported commands (ls, mv, etc.) / Assignment statement
  arg: $(expr) / $NAMED_CONSTANT / LITERAL / WILDCARD
*/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#define BUFF_MAX 1024 /* Maximum number of characters in the character buffer */
#define INVALID_POS -1
#define FATAL_ERR -2 /* Return code for a fatal error used internally */
#define FAIL 1
#define SUCCESS 0

bool isOp(char *s);
bool isCmd(char *s);
bool matchBrace(char *s, int *pos, unsigned int maxPos);
char* evalArg(char *s);
int evalCmd(char *cmd, char *args, char *inBuff, char *outBuff);
int evalS(char *s, int pos1, int pos2, char *inBuff, char *outBuff);
int evalExpr(char *expr, int pos1, int pos2, char *inBuff, char *outBuff);

/* Check if the string is a valid operator */
bool isOp(char *s)
{
    if (strlen(s) > 2) /* No operators are more than 2 characters long */
        return false;
    if (strlen(s) == 1) /* Single character operator lets us use a switch statement */
    {
        switch (s[0])
        {
        case '|':
        case ';':
        case '<':
        case '>':
            return true;
        default:
            return false;
        }
    }
    else /* Compare the string to supported 2 character operators */
    {
        const unsigned int NUM_OPS = 4;
        char *ops[NUM_OPS] = { "&&", "||", "<<", ">>" }; /* All supported 2 character operators */
        for (unsigned int i = 0; i < NUM_OPS; ++i)
        {
            if (strcmp(s, ops[i]) == 0)
                return true;
        }
        return false;
    }
}

/* Check if the string is a valid command */
bool isCmd(char *s)
{
    const unsigned int NUM_CMD = 8;
    char *cmds[NUM_CMD] = { "cd", "pwd", "mkdir", "rmdir", "ls", "cp", "exit", "echo" };
    for (unsigned int i = 0; i < NUM_CMD; ++i)
    {
        if (strcmp(s, cmds[i]) == 0)
            return true;
    }
    return false;
}

/* Move i to matching closing brace */
bool matchBrace(char *s, int *pos, unsigned int maxPos)
{
    if (s[*pos] != '{')
    {
        fprintf(stderr, "matchBrace: position passed is not a brace\n");
        return false;
    }
    unsigned int count = 1; /* Number of opening braces encountered */
    int i = *pos;
    while (count > 0 && i < maxPos)
    {
        ++i;
        if (s[i] == '{')
            ++count;
        if (s[i]=='}')
            --count;
    }
    if (count != 0)
    {
        fprintf(stderr, "matchBrace: matching brace not found\n");
        return false;
    }
    *pos = i;
    return true;
}

/* Evaluate the argument */
char* evalArg(char *s)
{
    /* TODO: Implement this */
    return NULL; /* Placeholder */
}

/* Evaluate the command or run the executable */
int evalCmd(char *cmd, char *args, char *inBuff, char *outBuff)
{
    /* TODO: Implement this */
    return 0; /* Placeholder */
}

/* Evaluate the statement */
int evalS(char *s, int pos1, int pos2, char *inBuff, char *outBuff)
{
    /* TODO: Implement this */
    return 0; /* Placeholder */
}

/* Parse the given expression into a left statement, operator, and right expression */
int parseExpr(char *expr, char *s, char *op, char *e)
{
    /* TODO: Needs testing */
    int pos1 = 0;
    int pos2 = strlen(expr) - 1;
    int i = INVALID_POS;
    int sEnd = INVALID_POS; /* End position of statement */
    int opPos1 = INVALID_POS; /* Start and end positions for operator */
    int opPos2 = INVALID_POS;
    int expStart = INVALID_POS;
    char token[BUFF_MAX] = ""; /* The space delineated token we are considering */
    int tokPos1, tokPos2;
    /* Initialize return values */
    s[0] = '\0';
    op[0] = '\0';
    e[0] = '\0';
    if (strlen(expr) == 0) /* Empty expression passed */
        return SUCCESS;
    while (pos2 > pos1 && isspace(expr[pos2])) /* Remove trailing whitespace */
        --pos2;
    while (pos1 <= pos2 && isspace(expr[pos1])) /* Ignore leading whitespace */
        ++pos1;
    if (pos1 > pos2) /* Expression consisted of all white space */
        return SUCCESS; /* Do nothing */
    i = pos1;
    if (expr[i] == '{') /* Statement is a braced expression. Move i to the matching brace */
    {
        bool ok = matchBrace(expr, &i, pos2);
        if (!ok) /* Failed to match brace */
        {
            fprintf(stderr, "parse error: failed to match brace\n");
            return FAIL;
        }
        ++i; /* Move past the matched brace */
    }
    /* Move to the operator */
    while (i <= pos2)
    {
        while (i <= pos2 && isspace(expr[i])) /* Move until not whitespace */
            ++i;
        tokPos1 = i;
        while (i <= pos2 && !isspace(expr[i])) /* Move until whitespace */
            ++i;
        tokPos2 = i - 1;
        strncpy(token, expr + tokPos1, tokPos2 - tokPos1 + 1);
        token[tokPos2 - tokPos1 + 1] = '\0';
        printf("Checking token: %s\n", token);
        if (isOp(token)) /* Found the operator */
        {
            opPos1 = tokPos1;
            opPos2 = tokPos2;
            break;
        }
    }
    if (opPos1 == INVALID_POS) /* No operator, just a statement */
    {
        strncpy(s, expr + pos1, pos2 - pos1 + 1);
        s[pos2 - pos1 + 1] = '\0';
        return SUCCESS;
    }
    if (opPos1 == 0)
    {
        fprintf(stderr, "parse error: left statement is empty\n");
        return FAIL;
    }
    if (opPos2 == pos2) /* No second argument to operator */
    {
        fprintf(stderr, "parse error: expected argument after operator \'%s\'\n", op);
        return FAIL;
    }
    strncpy(op, expr + opPos1, opPos2 - opPos1 + 1); /* Store the operator */
    op[opPos2 - opPos1 + 1] = '\0';
    sEnd = opPos1 - 1;
    strncpy(s, expr + pos1, sEnd - pos1 + 1); /* Store the statement */
    s[sEnd - pos1 + 1] = '\0';
    while (i <= pos2 && isspace(expr[i])) /* Move past white space seperation */
        ++i;
    expStart = i; /* Mark the start of the right hand expression */
    strncpy(e, expr + expStart, pos2 - expStart + 1);
    e[pos2 - expStart + 1] = '\0';
    return SUCCESS;
}

/* TODO: Rewrite this to pipe properly */
/* Evaluate the expression. Assume that there are no trailing whitespaces */
int evalExpr(char *expr, int pos1, int pos2, char *inBuff, char *outBuff)
{
    return 0; /* Placeholder */
    /* TODO: Rewrite this to support proper piping */
    /* if (strcmp(op, "&&") == 0) /\* Logical AND *\/ */
    /* { */
    /*     r = evalS(expr, pos1, sEnd, inBuff, outBuff); */
    /*     if (r == FATAL_ERR) /\* Propogate fatal errors *\/ */
    /*         return FATAL_ERR; */
    /*     if (r == 0) /\* Left hand statement succeeded *\/ */
    /*         return evalExpr(expr, expStart, pos2, inBuff, outBuff); /\* Evaluate right hand expression *\/ */
    /*     else /\* Return false if first statement fails *\/ */
    /*         return 1; */
    /* } */
    /* else if (strcmp(op, "||") == 0) /\* Logical OR *\/ */
    /* { */
    /*     r = evalS(expr, pos1, sEnd, inBuff, outBuff); */
    /*     if (r == FATAL_ERR) */
    /*         return FATAL_ERR; */
    /*     if (r == 1) /\* Left hand statement failed *\/ */
    /*         return evalExpr(expr, expStart, pos2, inBuff, outBuff); */
    /*     else /\* Return true if first statement succeeds *\/ */
    /*         return 0; */
    /* } */
    /* else if (strcmp(op, "|") == 0) /\* Pipe *\/ */
    /* { */
    /*     char pipeBuff[BUFF_MAX] = ""; /\* Intermediate buffer to do the piping *\/ */
    /*     r = evalS(expr, pos1, sEnd, inBuff, pipeBuff); */
    /*     if (r == FATAL_ERR) */
    /*         return FATAL_ERR; */
    /*     return evalExpr(expr, expStart, pos2, pipeBuff, outBuff); */
    /* } */
    /* else if (strcmp(op, ";") == 0) /\* Evaluate sequentually. Idk if this should be treated as an operator tbh *\/ */
    /* { */
    /*     r = evalS(expr, pos1, sEnd, inBuff, outBuff); */
    /*     if (r == FATAL_ERR) */
    /*         return FATAL_ERR; */
    /*     return evalExpr(expr, expStart, pos2, inBuff, outBuff); */
    /* } */
    /* else if (strcmp(op, "<") == 0) /\* Input redirection *\/ */
    /* { */
    /*     /\* Right hand expression is a file name *\/ */
    /*     char fileName[BUFF_MAX] = {}; */
    /*     strncpy(fileName, expr + expStart, pos2 - expStart + 1); */
    /*     FILE *inFile = fopen(fileName, "r"); */
    /*     if (inFile == NULL) /\* Could not open file *\/ */
    /*     { */
    /*         fprintf(stderr, "Could not open file \'%s\'\n", fileName); */
    /*         return FATAL_ERR; */
    /*     } */
    /*     /\* Get the file size *\/ */
    /*     fseek(inFile, 0, SEEK_END); */
    /*     long fSize = ftell(inFile); */
    /*     rewind(inFile); */
    /*     char *newIn = (char*)malloc(sizeof(char) * fSize); /\* New input buffer that contains the contents of the file *\/ */
    /*     long readSize = fread(newIn, 1, fSize, inFile); /\* Read in contents of file into buffer *\/ */
    /*     if (readSize != fSize) */
    /*     { */
    /*         fprintf(stderr, "Could not read contents of file \'%s\'\n", fileName); */
    /*         return FATAL_ERR; */
    /*     } */
    /*     fclose(inFile); */
    /*     r = evalS(expr, pos1, sEnd, newIn, outBuff); */
    /*     free(newIn); */
    /*     return r; */
    /* } */
    /* else if (strcmp(op, "<<") == 0) /\* Here document *\/ */
    /* { */
    /*     char delim[BUFF_MAX] = {}; /\* Delimiter string signaling end of input *\/ */
    /*     char buffer[BUFF_MAX] = {}; */
    /*     char input[BUFF_MAX] = {}; */
    /*     strncpy(delim, expr + expStart, pos2 - expStart + 1); */
    /*     fgets(input, BUFF_MAX, stdin); */
    /*     while (strcmp(input, delim) != 0) /\* While we have not encountered the delim string *\/ */
    /*     { */
    /*         strcat(buffer, input); /\* Cat input into buffer *\/ */
    /*         fgets(input, BUFF_MAX, stdin); */
    /*     } */
    /*     return evalS(expr, pos1, sEnd, buffer, outBuff); */
    /* } */
    /* else if (strcmp(op, ">") == 0 || strcmp(op, ">>") == 0) /\* Output redirection *\/ */
    /* { */
    /*     char fileName[BUFF_MAX] = ""; */
    /*     strncpy(fileName, expr + expStart, pos2 - expStart + 1); */
    /*     char newOut[BUFF_MAX] = {}; */
    /*     r = evalS(expr, pos1, sEnd, inBuff, newOut); */
    /*     FILE *outFile = (strlen(op) == 1) ? fopen(fileName, "w") : fopen(fileName, "a"); */
    /*     if (outFile == NULL) */
    /*     { */
    /*         fprintf(stderr, "Could not open file \'%s\'", fileName); */
    /*         return FATAL_ERR; */
    /*     } */
    /*     fwrite(newOut, sizeof(char), sizeof(newOut), outFile); */
    /*     fclose(outFile); */
    /*     return r; */
    /* } */
    /* /\* Execution should never reach here *\/ */
    /* fprintf(stderr, "evalExpr: tried to evaluate invalid operator\n"); */
    /* return FATAL_ERR; */
}

/* Test driver */
int main()
{
    char expr[BUFF_MAX] = "{ test this && { and this && and some more of this } } || remaining expression > here"; /* Test expression */
    char s[BUFF_MAX];
    char op[BUFF_MAX];
    char e[BUFF_MAX];
    int r = parseExpr(expr, s, op, e);
    printf("Expression parsed: %s\n", expr);
    printf("Return code: %d\ns: %s\nop: %s\ne: %s\n", r, s, op, e);
}
