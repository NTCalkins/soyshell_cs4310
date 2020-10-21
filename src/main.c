#include "Parser.h"

int main() {
   init();
    char d[PATH_MAX] = "";
    char user[BUFF_MAX] = "";
    int nread = 0;
    size_t n = 0;
    int len = 0;

    unsigned int i = 0;
    int lastResult = 0;

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
        len = strlen(expr);
        if (expr[len-1] == '\n')
            expr[len-1] = 0;
        if (strncmp(expr, "exit", 4) == 0) {
            break;
        }
        if (len == 0) /* Empty expression */
            continue;
        lastResult = evalExpr(expr);
    }
    free(expr);
    finish();
    return 0;  
}
