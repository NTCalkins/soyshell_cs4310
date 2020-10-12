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

bool isOp(char *s);
bool matchBrace(char *s, int *pos, unsigned int maxPos);

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

/* Move pos to matching closing brace */
bool matchBrace(char *s, int *pos, const unsigned int maxPos)
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

/* Move i to matching closing closing */
bool matchQuote(char *s, int *pos, const unsigned int maxPos)
{
    if (s[*pos] != '\"')
    {
        fprintf(stderr, "matchBrace: position passed is not a quote\n");
        return false;
    }
    int i = *pos;
    while (i < maxPos)
    {
        ++i;
        if (s[i] == '\"')
            break;
    }
    if (s[i] != '\"')
        return false;
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

/*
 Parse the given expression into a left statement, operator, and right expression
 expr: Expression to be parsed
 s: String to store the resulting statement
 op: String to store the resulting operator
 e: String to store the resulting right expression
*/
bool parseExpr(char *expr, char *s, char *op, char *e)
{
    int pos1 = 0;
    int pos2 = strlen(expr) - 1;
    int i = INVALID_POS;
    int sEnd = INVALID_POS; /* End position of statement */
    int opPos1 = INVALID_POS; /* Start and end positions for operator */
    int opPos2 = INVALID_POS;
    int expStart = INVALID_POS;
    char token[BUFF_MAX] = ""; /* The space delineated token we are considering */
    int tokPos1 = INVALID_POS;
    int tokPos2 = INVALID_POS;
    /* Initialize return values */
    s[0] = '\0';
    op[0] = '\0';
    e[0] = '\0';
    if (strlen(expr) == 0) /* Empty expression passed */
        return true;
    while (pos2 > pos1 && isspace(expr[pos2])) /* Remove trailing whitespace */
        --pos2;
    while (pos1 <= pos2 && isspace(expr[pos1])) /* Ignore leading whitespace */
        ++pos1;
    if (pos1 > pos2) /* Expression consisted of all white space */
        return true; /* Do nothing */
    i = pos1;
    if (expr[i] == '{') /* Statement is a braced expression. Move i to the matching brace */
    {
        bool ok = matchBrace(expr, &i, pos2);
        if (!ok) /* Failed to match brace */
        {
            fprintf(stderr, "parser: failed to match brace\n");
            return false;
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
        return true;
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
    return true;
}

/*
 Parse a string into a command and list of args
 s: Statement to parse
 MAX_ARGS: The maximum number of arguments argv can store
 cmd: String to store the resulting command
 argv: String array to store the resulting arguments
 numArgs: Returns the number of arguments extracted
 isBg: Returns if a & was passed to indicate a background process
*/
bool parseCmd(char *s, const unsigned int MAX_ARGS, char *cmd, char **argv, unsigned int *numArgs, bool *isBg)
{
    int pos1 = 0;
    int pos2 = strlen(s) - 1;
    int tokPos1 = INVALID_POS;
    int tokPos2 = INVALID_POS;
    unsigned int i = 0;
    /* Initialize return values */
    cmd[0] = '\0';
    *numArgs = 0;
    *isBg = false;
    if (strlen(s) == 0) /* Empty expression passed */
        return true;
    while (pos2 > pos1 && isspace(s[pos2])) /* Remove trailing whitespace */
        --pos2;
    while (pos1 <= pos2 && isspace(s[pos1])) /* Ignore leading whitespace */
        ++pos1;
    if (pos1 > pos2) /* Expression consisted of all white space */
        return true; /* Do nothing */
    if (pos2 - pos1 > 0 && s[pos2] == '&' && s[pos2 - 1] == ' ') /* & was passed to run process in background */
    {
        *isBg = true;
        /* Remove & from the argument list */
        --pos2;
        while (pos2 > pos1 && isspace(s[pos2]))
            --pos2;
    }
    /* Extract the first space delineated token and store in cmd */
    tokPos1 = tokPos2 = pos1;
    while (tokPos2 < pos2 && !isspace(s[tokPos2]))
        ++tokPos2;
    strncpy(cmd, s + tokPos1, tokPos2 - tokPos1 + 1);
    while (i < MAX_ARGS && tokPos2 < pos2)
    {
        while (tokPos2 < pos2 && isspace(s[tokPos2]))
            ++tokPos2;
        tokPos1 = tokPos2;
        while (tokPos2 < pos2 && !isspace(s[tokPos2]))
            ++tokPos2;
        strncpy(argv[i], s + tokPos1, tokPos2 - tokPos1 + 1);
        ++i;
    }
    *numArgs = i;
    return true;
}

/*
  Parse the statement into either an expression or a command
  If s is an expression enclosed in braces, it will be stored in e and cmd will be the empty string
  If s is simply a command, it will be stored in cmd and e will be the empty string
  s: The statement to be parsed
  e: String to store the parsed expression
  cmd: String to store the parsed command
*/
bool parseS(char *s, char *e, char *cmd)
{
    int pos1 = 0;
    int pos2 = strlen(s) - 1;
    e[0] = '\0';
    cmd[0] = '\0';
    if (strlen(s) == 0) /* Empty expression passed */
        return true;
    while (pos2 > pos1 && isspace(s[pos2])) /* Remove trailing whitespace */
        --pos2;
    while (pos1 <= pos2 && isspace(s[pos1])) /* Ignore leading whitespace */
        ++pos1;
    if (s[pos1] == '{') /* Statement is an expression enclosed in braces */
    {
        if (s[pos2] != '}')
        {
            fprintf(stderr, "parseS: expression not properly enclosed in braces\n");
            return false;
        }
        --pos2;
        while (pos2 > pos1 && isspace(s[pos2]))
            --pos2;
        /* Return the expression */
        strncpy(e, s + pos1, pos2 - pos1 + 1);
        s[pos2 - pos1 + 1] = '\0';
        return true;
    }
    /* Statement is a command */
    strncpy(cmd, s + pos1, pos2 - pos1 + 1);
    s[pos2 - pos1 + 1] = '\0';
    return true;
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
    char *s = "ffmpeg    -i foo.mp4   bar.mkv  & ";
    char cmd[BUFF_MAX];
    char *args[3];
    for (unsigned int i = 0; i < 3; ++i)
    {
        args[i] = (char*) malloc(sizeof(char) * BUFF_MAX);
    }
    bool isBg;
    unsigned int numArgs;
    printf("Command parsed: %s\n", s);
    parseCmd(s, 3, cmd, args, &numArgs, &isBg);
    printf("Command: %s\n", cmd);
    printf("Background: %d\n", isBg);
    printf("Number of arguments: %d\n", numArgs);
    puts("Args...");
    for (int i = 0; i < 3; ++i)
    {
        puts(args[i]);
        free(args[i]);
    }
    return 0;
}
