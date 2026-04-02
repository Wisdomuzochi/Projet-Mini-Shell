#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h> // Ajout pour waitpid
#include "parser.h"
#include "execute.h" 

#define MAX_CMD_LENGTH 4096
#define MAX_ARGS 100

int main(void) {
    char buffer[MAX_CMD_LENGTH];
    char cwd[MAX_CMD_LENGTH];
    char *args[MAX_ARGS];

    while (1) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s %% ", cwd);
        } else {
            printf("%% ");
        }
        fflush(stdout);

        if (fgets(buffer, MAX_CMD_LENGTH, stdin) == NULL) {
            printf("\n");
            break;
        }

        int argc = parse_cmd(buffer, args);
        if (argc == 0) continue;

        if (strcmp(args[0], "exit") == 0) {
            break;
        }
        if (strcmp(args[0], "cd") == 0) {
            char *target_dir = args[1];

            // Si aucun argument n'est tapé (juste "cd"), on récupère le HOME
            if (target_dir == NULL) {
                target_dir = getenv("HOME");
                if (target_dir == NULL) {
                    fprintf(stderr, "cd: variable HOME introuvable\n");
                    continue;
                }
            }

            // On change de dossier vers target_dir (qui est soit args[1], soit HOME)
            if (chdir(target_dir) != 0) {
                perror("cd");
            }
            continue; 
        }

        
        //  appelle la fonction du nouveau fichier execute.c, qui gere desormais fork/exec et les redirections
        execute_cmd(args);
    }

    return 0;
}