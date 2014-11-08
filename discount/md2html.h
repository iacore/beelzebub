#ifndef MD2HTML_H
#define MD2HTML_H
#include <stdio.h>

int convert_file(const char *path_in, const char *path_out);
int convert_move_file(const char *file_in, const char *path_out);
int open_files(FILE **fin, FILE **fout, const char *pin, const char *pout);
int convert_dir(const char *dirin, const char *dirout);

#endif
