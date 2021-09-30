#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

#include <stdbool.h>
#include <sys/types.h>

typedef struct 
{
    int handle;
    off_t current_position;
    int access; 
    bool eof;
} File;

extern bool f_ctor (File *file, const char *path, int acces);
extern char f_getchar (File *file);
extern char f_peek (File *file);
extern bool f_move (File *file, off_t off);
extern bool f_putchar (File *file, char ch);
extern bool f_write_str (File *file, const char *str);
extern void f_verify (File *file);
extern void f_dtor (File *file);
extern bool f_eof (File *file);

#endif 