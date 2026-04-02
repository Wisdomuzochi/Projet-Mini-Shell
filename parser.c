#include <stdio.h>
#include <string.h>
#include "parser.h"

int parse_cmd(char *buffer, char *args[]) {
    int argc = 0;
    char *ptr = buffer;

    while (*ptr != '\0') {
        // On ignore les espaces (ou tabulations/retours à la ligne) avant le mot
        while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n') {
            *ptr = '\0'; // On remplace par le caractère de fin de chaîne
            ptr++;
        }

        // Si on est arrivé à la fin de la phrase, on s'arrête
        if (*ptr == '\0') break;

        //  GESTION DES GUILLEMETS 
        if (*ptr == '"') {
            ptr++;              // On saute le premier guillemet
            args[argc] = ptr;   // Le mot commence juste après
            argc++;
            
            // On avance jusqu'à trouver le guillemet de fermeture
            while (*ptr != '\0' && *ptr != '"') {
                ptr++;
            }
            
            // Si on a trouvé le guillemet de fin, on le coupe 
            if (*ptr == '"') {
                *ptr = '\0';
                ptr++;
            }
        } 
        // GESTION D'UN MOT NORMAL (sans guillemets)
        else {
            args[argc] = ptr; // Le mot commence ici
            argc++;
            
            // On avance jusqu'au prochain espace 
            while (*ptr != '\0' && *ptr != ' ' && *ptr != '\t' && *ptr != '\n') {
                ptr++;
            }
        }
    }
    
    args[argc] = NULL; // Très important pour execvp
    return argc;
}