#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "execute.h"

void execute_cmd(char **args) {

    int bg = 0; // Drapeau (flag) : 0 = normal, 1 = arrière-plan 
    int last_idx = 0;

    // 1. On cherche où se trouve le dernier mot tapé
    while (args[last_idx] != NULL) {
        last_idx++;
    }
    
    // 2. Si le dernier mot est un "&"
    if (last_idx > 0 && strcmp(args[last_idx - 1], "&") == 0) {
        bg = 1;                     // On lève le drapeau "arrière-plan"
        args[last_idx - 1] = NULL;  // On efface le "&" pour que la commande (ex: sleep) ne plante pas
    }

    char **cmds[100]; // Va stocker les sous-commandes séparées par '|'
    int num_cmds = 0;
    
    //  Découpage des commandes par rapport aux '|'
    cmds[num_cmds++] = args; // La première commande commence au début de args
    
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "|") == 0) {
            args[i] = NULL; // On remplace '|' par NULL pour terminer la commande précédente
            cmds[num_cmds++] = &args[i + 1]; // La commande suivante commence juste après
        }
    }
    
    pid_t pids[100];  //  Tableau pour stocker les PIDs précis
    int pipefds[2];
    int prev_fd = -1; // Pour mémoriser la sortie du pipe de la commande précédente

    //  Boucle pour exécuter chaque sous-commande
    for (int i = 0; i < num_cmds; i++) {
        
        // Si ce n'est pas la dernière commande, on crée un nouveau pipe
        if (i < num_cmds - 1) {
            if (pipe(pipefds) == -1) {
                perror("pipe");
                exit(1);
            }
        }

        pids[i] = fork(); 

        if (pids[i] == -1) {
            perror("fork");
        } 
        else if (pids[i] == 0) {
            // processus fils

            //  Branchement des PIPES, i on n'est pas la 1ère commande, on lit depuis le pipe précédent
            if (prev_fd != -1) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            // Si on n'est pas la dernière commande, on écrit dans le pipe actuel
            if (i < num_cmds - 1) {
                dup2(pipefds[1], STDOUT_FILENO);
                close(pipefds[0]); // Le fils ne lit pas dans son propre pipe
                close(pipefds[1]);
            }

            //  Application des REDIRECTIONS (<, >, 2>) sur la sous-commande cmds[i]
            char *file_in = NULL;
            char *file_out = NULL;
            char *file_err = NULL;
            int j = 0;
            int redir_index = -1;

            while (cmds[i][j] != NULL) {
                if (strcmp(cmds[i][j], "<") == 0) {
                    file_in = cmds[i][j+1];
                    if (redir_index == -1) redir_index = j;
                } else if (strcmp(cmds[i][j], ">") == 0) {
                    file_out = cmds[i][j+1];
                    if (redir_index == -1) redir_index = j;
                } else if (strcmp(cmds[i][j], "2>") == 0) {
                    file_err = cmds[i][j+1];
                    if (redir_index == -1) redir_index = j;
                }
                j++;
            }

            if (redir_index != -1) cmds[i][redir_index] = NULL; 

            if (file_in != NULL) {
                int fd = open(file_in, O_RDONLY);
                if (fd == -1) { perror("mishell: open in"); exit(1); }
                dup2(fd, STDIN_FILENO); close(fd);
            }
            if (file_out != NULL) {
                int fd = open(file_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) { perror("mishell: open out"); exit(1); }
                dup2(fd, STDOUT_FILENO); close(fd);
            }
            if (file_err != NULL) {
                int fd = open(file_err, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) { perror("mishell: open err"); exit(1); }
                dup2(fd, STDERR_FILENO); close(fd);
            }

            //  Exécution de la sous-commande
            if (execvp(cmds[i][0], cmds[i]) == -1) {
                perror("mishell");
                exit(1);
            }
        } 
        else {
            //processus pere 
            
            // Le père ferme l'ancienne lecture dont il n'a plus besoin
            if (prev_fd != -1) {
                close(prev_fd);
            }
            
            //  sauvegarde la lecture du pipe actuel pour le prochain fils
            if (i < num_cmds - 1) {
                prev_fd = pipefds[0];
                close(pipefds[1]); // Le père n'écrit jamais dans le pipe
            }
        }
    }
    if (bg == 0) {
        // Mode normal : on attend la fin de tous les processus fils
        for (int i = 0; i < num_cmds; i++) {
            waitpid(pids[i], NULL, 0);
        }
    } 
}