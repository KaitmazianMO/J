#include "File.h"

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>

static off_t file_size (int fd);

__attribute__((unused))
bool f_ctor (File *file, const char *path, int acces)
{
    assert (file);

    if (acces & O_CREAT)
        file->handle = open (path, acces, 0777);
    else
        file->handle = open (path, acces);
    if (file->handle == -1) 
    {
        file->current_position = -1;
        return false;
    }

    file->access = acces;
    file->current_position = 0;
    file->eof = false;
    return true;
}

__attribute__((unused))
void f_dtor (File *file)
{
    f_verify (file);

    close (file->handle);
    file->current_position = -1;
    file->handle = -1;
    file->eof = false;
}

__attribute__((unused))
char f_getchar (File *file)
{
    f_verify (file);   
    if (file->access == O_WRONLY) return '\0'; 

    char ch = '\0';
    if (!file->eof)
    {
        ssize_t n = read (file->handle, &ch, sizeof (ch));

        if (n == 0)
            file->eof = true;
        else 
            ++file->current_position;

        if (n == -1)
            ch = '\0';
    }

    return ch;
}

__attribute__((unused))
char f_peek (File *file)
{
    f_verify (file);

    char ch = 0;
    if (f_move (file, -1))
        ch = f_getchar (file);

    return ch;
}

__attribute__((unused))
bool f_move (File *file, off_t offset)
{
    f_verify (file);

    if (file->current_position + offset < 0) return false;
    
    if (lseek (file->handle, offset, SEEK_CUR) == -1L) return false;
    
    if (file->eof && offset < 0) file->eof = false;

    file->current_position += offset;
    return true;
}

__attribute__((unused))
bool f_putchar (File *file, char ch)
{
    f_verify (file);
    if (file->access == O_RDONLY) return false;
    
    if (write (file->handle, &ch, sizeof (ch)) == -1) return false;

    ++file->current_position;
    return true;
}

__attribute__((unused))
bool f_write_str (File *file, const char *str)
{
    f_verify (file);
    assert (str);
    if (file->access == O_RDONLY) return false;

    const size_t len = strlen (str);
    bool no_error = true;
    for (int i = 0; i < len; ++i)
        no_error |= f_putchar (file, str[i]);

    return no_error; 
}

__attribute__((unused))
void f_verify (File *file)
{
    assert (file != NULL);
    assert (file->current_position >= 0);
    assert (file->handle != -1);
} 

__attribute__((unused))
extern bool f_eof (File *file)
{
    f_verify (file);

    return file->eof;
}

__attribute__((unused))
static off_t file_size (int fd)
{
    assert (fd != -1);

    struct stat st = {};
    if (fstat (fd, &st) == -1)
    {
        return -1;
    }

    return st.st_size;
}