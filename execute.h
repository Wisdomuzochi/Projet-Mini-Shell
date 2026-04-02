#ifndef EXECUTE_H
#define EXECUTE_H

// Prend le tableau d'arguments et gère fork, execvp, et les redirections
void execute_cmd(char **args);

#endif