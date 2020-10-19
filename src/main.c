#include "Parser.c"

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
    
    char *userInput = NULL;

    while (1) {
        if (getcwd(d, sizeof(d)) == NULL) {
            return 0;
        }
        i = strlen(d);
        while (i > 0 && d[i-1] != '/')
            i--;
        printf("%s@soyshell %s> ", user, d +i);
        if ((nread = getline(&userInput,&n, stdin)) == -1) {
            printf("%s\n","Failed to read stdin");
            continue;
        }
        int len = strlen(userInput);
        if (userInput[len-1] == '\n')
            userInput[len-1] = 0;
        if (strcmp(userInput,"exit") == 0) {
            return 0;
        }if (strcmp(userInput, "") == 0)
        {
            continue;
        }
        evalS(userInput);
    }
    return 0;  
}
