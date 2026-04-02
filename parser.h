#ifndef PARSER_H
#define PARSER_H

/* Découpe la ligne, stocke les mots dans args et retourne le nombre de mots */
int parse_cmd(char *buffer, char *args[]);

#endif