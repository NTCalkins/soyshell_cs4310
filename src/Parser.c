/*
  Parser for the shell designed to parse the following grammar
  ('+' = whitespace)
  expr: s / s + op + expr
  s: {expr} / invoke
  invoke: cmd [ + | + cmd ]...
  op: && / || / ; / =
  redir: < / > / >>
  cmd: EXECUTABLE [+ arg]... [+ redir + FILE_NAME/DELIM] [ + &]
  arg: $NAMED_CONSTANT / LITERAL

  IMPORTANT: Don't forget to call init() to intialize the array of user
  defined constants and finish() to cleanup the array
*/
#include "Parser.h"

char ***consts; /* Array of string pairs to store user defined constants. If we have more time, this should be replaced with a BST */
unsigned int numConsts; /* Current number of constants ie. next free index */
unsigned int maxConsts; /* Current maximum number of user defined constants */

/* Initialize the global variables */
void init()
{
    maxConsts = INIT_CONSTS;
    numConsts = 0;
    consts = (char***) malloc(maxConsts * sizeof(char**));
    /* Reserve the 0th index for the PATH variable */
    consts[numConsts] = (char**) malloc(2 * sizeof(char*));
    consts[numConsts][0] = (char*) malloc(BUFF_MAX * sizeof(char));
    consts[numConsts][1] = (char*) malloc(BUFF_MAX * sizeof(char));
    strcpy(consts[numConsts][0], "PATH");
    /* For the purpose of the assignment, we will make the assumption that the executable is called in the root of the
       repo and the default path will be the repo's bin folder */
    getcwd(consts[numConsts][1], BUFF_MAX);
    strcat(consts[numConsts][1], "/bin");
    ++numConsts;
}

/* Clean up global variables */
void finish()
{
    for (unsigned int i = 0; i < numConsts; ++i)
    {
        free(consts[i][0]);
        free(consts[i][1]);
        free(consts[i]);
    }
    free(consts);
}

/* Define a constant with the specified key and value */
bool addConst(char *key, char *val)
{
    if (strlen(key) > BUFF_MAX - 1)
    {
        fprintf(stderr, "addConst: length of key exceeds BUFF_MAX\n");
        return false;
    }
    if (strlen(val) > BUFF_MAX - 1)
    {
        fprintf(stderr, "addConst: length of val exceeds BUFF_MAX\n");
        return false;
    }
    if (!isalpha(key[0]))
    {
        fprintf(stderr, "addConst: key must start with an alphabetical character\n");
        return false;
    }
    for (unsigned int i = 1; i < strlen(key); ++i)
    {
        if (!isalnum(key[i]))
        {
            fprintf(stderr, "addConst: key must be alpha-numeric\n");
            return false;
        }
    }
    for (unsigned int i = 0; i < numConsts; ++i)
    {
        if (strcmp(consts[i][0], key) == 0) /* Key already exists */
        {
            /* Just update the value */
            strcpy(consts[i][1], val);
            return true;
        }
    }
    consts[numConsts] = (char**) malloc(2 * sizeof(char*));
    consts[numConsts][0] = (char*) malloc(BUFF_MAX * sizeof(char));
    consts[numConsts][1] = (char*) malloc(BUFF_MAX * sizeof(char));
    strcpy(consts[numConsts][0], key);
    strcpy(consts[numConsts][1], val);
    ++numConsts;
    if (numConsts == maxConsts) /* Need to expand the array */
    {
        maxConsts *= 2;
        consts = (char***) realloc(consts, maxConsts * sizeof(char***));
    }
    return true;
}

/*
  Get the string associated with the key
  Returns blank string on failure
*/
char* getConst(char *key)
{
    /* Due to time constraints, this will just be a linear search for now */
    for (unsigned int i = 0; i < numConsts; ++i)
    {
        if (strcmp(key, consts[i][0]) == 0) /* Found it */
            return consts[i][1];
    }
    return "";
}

/* Check if the string is a valid operator */
bool isOp(char *s)
{
    if (strlen(s) > 2) /* No operators are more than 2 characters long */
        return false;
    if (strlen(s) == 1) /* Single character operator lets us use a switch statement */
    {
        switch (s[0])
        {
        case ';':
        case '=':
            return true;
        default:
            return false;
        }
    }
    else /* Compare the string to supported 2 character operators */
    {
        if (strcmp(s, "&&") == 0)
            return true;
        if (strcmp(s, "||") == 0)
            return true;
        return false;
    }
}

/* Check if the string is a pipe */
bool isPipe(char *s)
{ return (strlen(s) == 1 && s[0] == '|'); }

/* Check if the string is a redirection operator */
bool isRedir(char *s)
{
    
    if (strlen(s) == 1)
    {
        switch (s[0])
        {
        case '<':
        case '>':
            return true;
        default:
            return false;
        }
    }
    if (strcmp(s, ">>") == 0)
        return true;
    return false;
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

/* Move pos to matching closing closing */
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
            fprintf(stderr, "parseExpr: failed to match brace\n");
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
    while (sEnd > pos1 && isspace(expr[sEnd]))
        --sEnd;
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
 maxArgs: The maximum number of arguments argv can store. This includes the NULL terminator
 cmd: String to store the resulting command
 argv: Array of unallocated character pointers to store the resulting arguments
 numArgs: Returns the number of arguments extracted
 isBg: Returns if a & was passed to indicate a background process
*/
bool parseCmd(char *s, const unsigned int maxArgs, char *cmd, char **argv, char **redirs, char **filenames, unsigned int *numArgs, unsigned int *numRedirs, unsigned int *numFilenames,  bool *isBg)
{
    int pos1 = 0;
    int pos2 = strlen(s) - 1;
    int tokPos1 = INVALID_POS;
    int tokPos2 = INVALID_POS;
    char token[BUFF_MAX] = "";
    /* Initialize return values */
    cmd[0] = '\0';
    *numArgs = 0;
    *numRedirs = 0;
    *numFilenames = 0;
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
    while (tokPos2 <= pos2 && !isspace(s[tokPos2]))
        ++tokPos2;
    strncpy(cmd, s + tokPos1, tokPos2 - tokPos1);
    cmd[tokPos2 - tokPos1] = '\0';
    /* First element of argv is always the name of the command */
    argv[*numArgs] = (char*) malloc(BUFF_MAX * sizeof(char));
    strcpy(argv[*numArgs], cmd);
    ++(*numArgs);
    /* Parse the argument list */
    while (*numArgs < maxArgs - 1 && tokPos2 <= pos2)
    {
        while (tokPos2 <= pos2 && isspace(s[tokPos2]))
            ++tokPos2;
        tokPos1 = tokPos2;
        if (s[tokPos1] == '\"') /* Quoted argument */
        {
            bool ok = matchQuote(s, &tokPos2, pos2);
            if (!ok) /* Could not match quote */
            {
                fprintf(stderr, "parseCmd: could not parse all arguments\n");
                /* Terminate argv early */
                argv[*numArgs] = NULL;
                free(argv);
                return false;
            }
            argv[*numArgs] = (char*) malloc(BUFF_MAX * sizeof(char));
            /* Take away the quotes and copy into argv */
            strncpy(argv[*numArgs], s + tokPos1 + 1, tokPos2 - tokPos1 - 1);
            argv[*numArgs][tokPos2 - tokPos1 - 1] = '\0';
            ++(*numArgs);
            ++tokPos2; /* Move past end quote */
        }
        else
        {
            while (tokPos2 <= pos2 && !isspace(s[tokPos2]))
                ++tokPos2;
            strncpy(token, s + tokPos1, tokPos2 - tokPos1);
            token[tokPos2 - tokPos1] = '\0';
            if (isRedir(token)) /* Hit the end of argument list and encountered a redirection operator */
            {
                redirs[*numRedirs] = (char*) malloc(BUFF_MAX * sizeof(char));
                strcpy(redirs[*numRedirs], token);
                ++(*numRedirs);
                break;
            }
            argv[*numArgs] = (char*) malloc(BUFF_MAX * sizeof(char));
            strcpy(argv[*numArgs], token);
            evalArg(argv[*numArgs]); /* Expand any user defined constants in the arg */
            ++(*numArgs);
        }
    }
    argv[*numArgs] = NULL; /* Terminate list of args with NULL */
    if (*numRedirs > 0) /* Command has redirection arguments */
    {
        unsigned int itr = 0; /* Track the number of iterations */
        /* Parse the filenames and redirection */
        while (*numRedirs < maxArgs && *numFilenames < maxArgs && tokPos2 <= pos2)
        {
            while (tokPos2 <= pos2 && isspace(s[tokPos2]))
                ++tokPos2;
            tokPos1 = tokPos2;
            while (tokPos2 <= pos2 && !isspace(s[tokPos2]))
                ++tokPos2;
            strncpy(token, s + tokPos1, tokPos2 - tokPos1);
            token[tokPos2 - tokPos1] = '\0';
            if (itr % 2 == 0 && isRedir(token)) /* Expect filenames on even iterations */
            {
                fprintf(stderr, "parseCmd: expected filename near \'%s\'\n", token);
                free(argv);
                return false;
            }
            if (itr % 2 == 1 && !isRedir(token)) /* Expect redirection operator on odd iterations */
            {
                fprintf(stderr, "parseCmd: expected redirection operator near \'%s\'\n", token);
                free(argv);
                return false;
            }
            if (itr % 2 == 0) /* Even iteration */
            {
                filenames[*numFilenames] = (char*) malloc(BUFF_MAX * sizeof(char));
                strcpy(filenames[*numFilenames], token);
                ++(*numFilenames);
            }
            else
            {
                redirs[*numRedirs] = (char*) malloc(BUFF_MAX * sizeof(char));
                strcpy(redirs[*numRedirs], token);
                ++(*numRedirs);
            }
            ++itr;
        }
    }
    return true;
}

/*
  Parse the statement into either an expression or a invocation
  If s is an expression enclosed in braces, it will be stored in e and inv will be the empty string
  If s is simply a invokation, it will be stored in inv and e will be the empty string
  s: The statement to be parsed
  e: String to store the parsed expression
  inv: String to store the parsed command
*/
bool parseS(char *s, char *e, char *inv)
{
    int pos1 = 0;
    int pos2 = strlen(s) - 1;
    e[0] = '\0';
    inv[0] = '\0';
    if (strlen(s) == 0) /* Empty expression passed */
        return false;
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
        ++pos1;
        --pos2;
        while (pos2 > pos1 && isspace(s[pos2]))
            --pos2;
        /* Return the expression */
        strncpy(e, s + pos1, pos2 - pos1 + 1);
        s[pos2 - pos1 + 1] = '\0';
        return true;
    }
    /* Statement is a invocation */
    strncpy(inv, s + pos1, pos2 - pos1 + 1);
    s[pos2 - pos1 + 1] = '\0';
    return true;
}

/*
  Parse an invocation consisting of a series of commands and redirections
  s: String to be parsed
  cmds: Array of unallocated char pointers to store the parsed commands
  numCmds: Int to store the number of parsed commands
  numRedirs: Int to store the number of parsed redirection operators
*/
bool parseInvoke(char *s, char **cmds, unsigned int *numCmds, unsigned int *numPipes)
{
    int pos1 = 0;
    int pos2 = strlen(s) - 1;
    int tokPos1 = INVALID_POS;
    int tokPos2 = INVALID_POS;
    char token[BUFF_MAX] = {};
    int i = 0;
    /* Initialize return values */
    *numCmds = 0;
    if (strlen(s) == 0) /* Empty expression passed */
        return true;
    while (pos2 > pos1 && isspace(s[pos2])) /* Remove trailing whitespace */
        --pos2;
    while (pos1 <= pos2 && isspace(s[pos1])) /* Ignore leading whitespace */
        ++pos1;
    if (pos1 > pos2) /* Expression consisted of all white space */
        return true; /* Do nothing */
    i = pos1;
    while (i <= pos2)
    {
        while (i <= pos2 && isspace(s[i]))
            ++i;
        tokPos1 = i;
        while (i <= pos2 && !isspace(s[i]))
            ++i;
        tokPos2 = i;
        strncpy(token, s + tokPos1, tokPos2 - tokPos1);
        token[tokPos2 - tokPos1] = '\0';
        if (isPipe(token)) /* Found a redirection operator */
        {
            i = tokPos1 - 1;
            if (i == -1)
            {
                fprintf(stderr, "parseInvoke: expected left command for redirection operator\n");
                return false;
            }
            while (i > 0 && isspace(s[i]))
                --i;
            /* Save the command */
            cmds[*numCmds] = (char*) malloc(BUFF_MAX * sizeof(char));
            strncpy(cmds[*numCmds], s + pos1, i - pos1 + 1);
            cmds[*numCmds][i - pos1 + 1] = '\0';
            ++(*numCmds);
            ++(*numPipes);
            pos1 = tokPos2;
            while (pos1 <= pos2 && isspace(s[pos1]))
                ++pos1;
        }
        i = tokPos2;
        while (i <= pos2 && isspace(s[i]))
            ++i;
    }
    /* Add the remaining cmd */
    cmds[*numCmds] = (char*) malloc(BUFF_MAX * sizeof(char));
    strcpy(cmds[*numCmds], s + pos1);
    ++(*numCmds);
    /* Terminate with NULL */
    cmds[*numCmds] = NULL;
    return true;
}

/* Evaluate the invocation */
int evalInvoke(char *s)
{
    /* For parseInvoke */
    char *cmds[MAX_ARGS]; /* Array to store the commands parsed */
    unsigned int numCmds = 0; /* Number of commands extracted */
    unsigned int numPipes = 0; /* Number of pipes extracted */
    /* File descriptors to handle forking */
    int in = 0;
    int fd[2];
    bool ok;
    int r = 0;
    if (strlen(s) == 0) /* Empty string passed */
        return 0;
    ok = parseInvoke(s, cmds, &numCmds, &numPipes);
    if (!ok) /* Failed to parse */
        return 1;
    if (numPipes == 0) /* No pipes, just a single command */
        r = evalCmd(0, 1, cmds[0]);
    /* Process pipes */
    else
    {
        for (unsigned int i = 0; i < numCmds - 1; ++i)
        {
            pipe(fd);
            evalCmd(in, fd[1], cmds[i]);
            close(fd[1]); /* No longer need write end of pipe */
            in = fd[0]; /* Keep read end of pipe */
        }
        /* Handle last stage of pipe */
        r = evalCmd(in, 1, cmds[numCmds - 1]);
    }
    /* Free cmds */
    for (unsigned int i = 0; i < numCmds; ++i)
        free(cmds[i]);
    return r;
}

bool getExecPath(char *cmd, char *execPath)
{
    char path[BUFF_MAX]; /* String to store the current value of PATH */
    strcpy(path, consts[0][1]); /* Get the current PATH value */
    char *tok = NULL;
    bool isPath = false; /* Is the given command already a path to an executable */
    for (unsigned int i = 0; i < strlen(cmd); ++i) /* Look for a '/' to signal that cmd is already a path */
    {
        if (cmd[i] == '/')
        {
            isPath = true;
            break;
        }
    }
    if (isPath)
        strcpy(execPath, cmd);
    else
    {
        tok = strtok(path, ":");
        while (tok != NULL)
        {
            /* Generate possible executable path using value in PATH and cmd */
            strcpy(execPath, tok);
            strcat(execPath, "/");
            strcat(execPath, cmd);
            if (access(execPath, X_OK) != -1) /* Found an appropriate executable */
                break;
            tok = strtok(NULL, ":");
        }
    }
    if (access(execPath, X_OK) != -1)
        return true;
    return false;
}

/*
  Evaluate the argument
  This just expands any user defined constants preceeded by a $
*/
char* evalArg(char *arg)
{
    int pos1 = 0;
    int pos2 = strlen(arg);
    int keyPos1 = INVALID_POS;
    int keyPos2 = INVALID_POS;
    int i = 0;
    char key[BUFF_MAX] = "";
    char *val = NULL;
    char temp[BUFF_MAX] = "";
    temp[0] = '\0';
    while (pos1 < pos2)
    {
        while (i < pos2 && arg[i] != '$')
            ++i;
        if (i == pos2) /* No $ in arg */
            break;
        /* Else found a $ */
        keyPos1 = keyPos2 = i + 1;
        while (keyPos2 <= pos2 && isalnum(arg[keyPos2])) /* Read key until we hit a non-alnum character or end of string */
            ++keyPos2;
        if (keyPos2 - keyPos1 > BUFF_MAX - 1)
        {
            fprintf(stderr, "evalArg: key length exceeds BUFF_MAX\n");
            return arg;
        }
        strncpy(key, arg + keyPos1, keyPos2 - keyPos1);
        key[keyPos2 - keyPos1] = '\0';
        val = getConst(key);
        strncpy(temp + pos1, arg + pos1, i - pos1);
        temp[i] = '\0';
        strcat(temp, val);
        pos1 = i = keyPos2;
    }
    if (pos1 < pos2)
        strcat(temp, arg + pos1); /* Add the remainder of the arg */
    strcpy(arg, temp);
    return arg;
}

/* Evaluate the command and run the executable */
int evalCmd(int in, int out, char* s)
{
    char cmd[BUFF_MAX]; /* Command name */
    char *argv[MAX_ARGS]; /* Argument list */
    char *redirs[MAX_ARGS]; /* List of redirection operators */
    char *filenames[MAX_ARGS]; /* List of filenames associated with redirection operators */
    char exec[BUFF_MAX]; /* Path to executable associated with command name */
    unsigned int numArgs;
    unsigned int numRedirs;
    unsigned int numFilenames;
    bool isBg; /* Was a & passed to indicate a background process */
    bool ok;
    int fd; /* File descriptor returned by open */
    pid_t pid;
    ok = parseCmd(s, MAX_ARGS, cmd, argv, redirs, filenames, &numArgs, &numRedirs, &numFilenames, &isBg);
    if (!ok) /* Failed to parse */
        return 1;
    if (numRedirs != numFilenames)
    {
        fprintf(stderr, "evalCmd: expected filename after \'%s\'\n", redirs[numRedirs - 1]);
        /* Clean up */
        for (unsigned int i = 0; i < numArgs; ++i)
            free(argv[i]);
        for (unsigned int i = 0; i < numRedirs; ++i)
            free(redirs[i]);
        for (unsigned int i = 0; i < numFilenames; ++i)
            free(filenames[i]);
        return 1;
    }
    if (strcmp(cmd, "cd") == 0) /* Special case for cd */
    {
        int retVal = 0;
        if (numArgs != 2)
        {
            fprintf(stderr, "cd: invalid number of arguments\n");
            retVal = 1;
        }
        else if (chdir(argv[1]) == -1)
        {
            fprintf(stderr, "cd: failed to change directory\n");
            retVal = 1;
        }
        /* Clean up */
        for (unsigned int i = 0; i < numArgs; ++i)
            free(argv[i]);
        for (unsigned int i = 0; i < numRedirs; ++i)
            free(redirs[i]);
        for (unsigned int i = 0; i < numFilenames; ++i)
            free(filenames[i]);
        return retVal;
    }
    ok = getExecPath(cmd, exec);
    if (!ok) /* Failed to get valid path to executable */
    {
        fprintf(stderr, "\'%s\' is not a valid command\n", cmd);
        /* Clean up */
        for (unsigned int i = 0; i < numArgs; ++i)
            free(argv[i]);
        for (unsigned int i = 0; i < numRedirs; ++i)
            free(redirs[i]);
        for (unsigned int i = 0; i < numFilenames; ++i)
            free(filenames[i]);
        return 1;
    }
    pid = fork();
    if (pid == 0) /* Child process */
    {
        for (unsigned int i = 0; i < numRedirs; ++i)
        {
            if (strcmp(redirs[i], "<") == 0) /* Input redirection */
            {
                fd = open(filenames[i], O_RDONLY);
                if (fd == -1)
                {
                    
                    fprintf(stderr, "evalCmd: could not open file \'%s\' for reading\n", filenames[i]);
                    dup2(fd, 0);
                    close(fd);
                        for (unsigned int i = 0; i < numArgs; ++i)
                            free(argv[i]);
                        for (unsigned int i = 0; i < numRedirs; ++i)
                            free(redirs[i]);
                        for (unsigned int i = 0; i < numFilenames; ++i)
                            free(filenames[i]);
                    return 1;
                }
                dup2(fd, 0);
                close(fd);
            }
            if (strcmp(redirs[i], ">") == 0) /* Output redirection */
            {
                fd = open(filenames[i], O_WRONLY | O_CREAT, 0666);
                if (fd == -1)
                {
                    fprintf(stderr, "evalCmd: could not open file \'%s\' for writing\n", filenames[i]);
                    dup2(fd, 1);
                    close(fd);
                    for (unsigned int i = 0; i < numArgs; ++i)
                        free(argv[i]);
                    for (unsigned int i = 0; i < numRedirs; ++i)
                        free(redirs[i]);
                    for (unsigned int i = 0; i < numFilenames; ++i)
                        free(filenames[i]);
                    return 1;
                }
                dup2(fd, 1);
                close(fd);
            }
            if (strcmp(redirs[i], ">>") == 0) /* Output with append */
            {
                fd = open(filenames[i], O_WRONLY | O_APPEND | O_CREAT, 0666);
                if (fd == -1)
                {
                    fprintf(stderr, "evalCmd: could not open file \'%s\' for writing\n", filenames[i]);
                    dup2(fd, 1);
                    close(fd);
                    for (unsigned int i = 0; i < numArgs; ++i)
                        free(argv[i]);
                    for (unsigned int i = 0; i < numRedirs; ++i)
                        free(redirs[i]);
                    for (unsigned int i = 0; i < numFilenames; ++i)
                        free(filenames[i]);
                    return 1;
                }
                dup2(fd, 1);
                close(fd);
            }
        }
        /* Deal with specified piping */
        if (in != 0) /* in is not stdin */
        {
            dup2(in, 0); /* Use it as stdin */
            close(in);
        }
        if (out != 1) /* out is not stdout */
        {
            dup2(out, 1); /* Use it as stdout */
            close(out);
        }
        execvp(exec, argv);
    }
    /* Parent process */
    pid_t r = 0;
    /* Clean up */
    for (unsigned int i = 0; i < numArgs; ++i)
        free(argv[i]);
    for (unsigned int i = 0; i < numRedirs; ++i)
        free(redirs[i]);
    for (unsigned int i = 0; i < numFilenames; ++i)
        free(filenames[i]);
    if (isBg) /* Don't wait for background process */
        return 0;
    r = waitpid(pid, 0, 0);
    if (r == -1)
        return 1;
    return 0;
}

/* Evaluate the statement */
int evalS(char *s)
{
    char e[BUFF_MAX] = "";
    char inv[BUFF_MAX] = "";
    bool ok = parseS(s, e, inv);
    if (!ok)
        return 1;
    if (strlen(inv) == 0) /* Statement is a braced expression */
        return evalExpr(e);
    /* Else statement is an invocation */
    return evalInvoke(inv);
}

/* Evaluate the expression. Assume that there are no trailing whitespaces */
int evalExpr(char *expr)
{
    char left[BUFF_MAX];
    char op[BUFF_MAX];
    char right[BUFF_MAX];
    int l_code;
    if (strlen(expr) == 0) /* Empty expression */
        return 0;
    parseExpr(expr,left,op,right);
    if (strlen(op) == 0) /* No operator, just a statement */
        return evalS(expr);
    if (strlen(right) == 0) /* No right expression found */
    {
        fprintf(stderr, "evalExpr: expected right hand expression for operator\n");
        return 1;
    }
    if (strcmp(op, "=") == 0) /* Assignment operator */
    {
        /* Left is the key, right is the val */
        evalArg(right);
        bool ok = addConst(left, right);
        if (!ok)
            return 1;
        return 0;
    }
    if (strcmp(op, "&&") == 0) /* AND operator */
    {
        l_code = evalS(left);
        if (l_code == 0)
            return evalExpr(right);
    }
    else if (strcmp(op, "||") == 0) /* OR operator */
    {
        l_code = evalS(left);
        if (l_code == 0)
            return 0;
        return evalExpr(right);
    }
    else if (strcmp(op, ";") == 0) /* Evaluate sequentially */
    {
        evalS(left);
        return evalExpr(right);
    }
    else
    {
        fprintf(stderr, "\'%s\' is not an operator\n", op);
        return 1;
    }
    return 0;
}
