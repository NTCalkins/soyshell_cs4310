#include "Parser.h"

int main() {
   init();
    char d[PATH_MAX];
    char user[BUFF_MAX];
    int nread;
    size_t n = 0;

    unsigned int i = 0;
    int lastResult;

    if (getcwd(d, sizeof(d)) == NULL) {
        return 0;
    }
    getlogin_r(user, BUFF_MAX);
    if (strcmp(user, "") == 0) {
        strncpy(user,"anonymous",10);
    }
    
    char *expr = NULL;
    puts("Welcome to soyshell!");
    while (1) {
        if (getcwd(d, sizeof(d)) == NULL) {
            return 0;
        }
        i = strlen(d);
        while (i > 0 && d[i-1] != '/')
            i--;
        printf("%s@soyshell %s > ", user, d +i);
        if ((nread = getline(&expr,&n, stdin)) == -1) {
            printf("%s\n","Failed to read stdin");
            continue;
        }
        int len = strlen(expr);
        if (expr[len-1] == '\n')
            expr[len-1] = 0;
        if (strncmp(expr, "exit", 4) == 0) {
            return 0;
        }
        if (len == 0) /* Empty expression */
            continue;
        lastResult = evalExpr(expr);
        free(expr);
        expr = NULL;
    }
    finish();
    return 0;  
}
