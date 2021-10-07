#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "File.h"

void buzz_buzz_loop (File *input, File *output);
void print_spaces (File *intput, File *output);
off_t bizz_buzz_token_loop (File *file, bool *div_3, bool *div_5, bool *is_num);
File file_ctor_handeled (const char *path, int mode);
int to_num (char);

int main(int argc, char **argv) 
{
    if (argc < 3)
    {
        fprintf (stderr, "Usage: %s <input-file-name> <output-file-name>\n", argv[0]);
        return 1;
    }

    if (strcmp (argv[1], argv[2]) == 0)
    {
        fprintf (stderr, "%s and %s are the same file\n", argv[1], argv[2]);
        return 1;    
    }
    
    File input  = file_ctor_handeled (argv[1], O_RDONLY);
    File output = file_ctor_handeled (argv[2], O_WRONLY | O_CREAT | O_TRUNC);

    buzz_buzz_loop (&input, &output);

    return 0; 
}

void buzz_buzz_loop (File *input, File *output)
{
    assert (input != output);
    f_verify (input);
    f_verify (output);
    assert (input->access  != O_WRONLY);
    assert (output->access != O_RDONLY);

    off_t tok_offset = 0;
    bool has_divisibility_by_3 = false;
    bool has_divisibility_by_5 = false;
    bool is_number             = false;
    char curr_ch = 0;
    while (!f_eof (input))
    {
        if ((curr_ch = f_getchar (input)) == '\0') 
            break;
        else if (isspace (curr_ch))
        {
            f_putchar (output, curr_ch);  
            continue;
        }
        else 
            f_move (input, -1);

        tok_offset = bizz_buzz_token_loop (input, &has_divisibility_by_3, &has_divisibility_by_5, &is_number);

        if (tok_offset >= 0)
        {
            if (is_number && (has_divisibility_by_3 || has_divisibility_by_5))
            {
                if (has_divisibility_by_3) f_write_str (output, "Bizz");
                if (has_divisibility_by_5) f_write_str (output, "Buzz");               
            }
            else 
            {
                f_move (input, -tok_offset);
                for (curr_ch = f_getchar (input); (!isspace (curr_ch)) && curr_ch; curr_ch = f_getchar (input))
                {
                    f_putchar (output, curr_ch); 
                }
                if (isspace (curr_ch)) f_move (input, -1);
            }
        }
    } 
}

off_t bizz_buzz_token_loop (File *file, bool *div_3, bool *div_5, bool *is_num)
{
    assert (div_3);
    assert (div_5);
    assert (is_num);
    f_verify (file);

    *div_3  = false;
    *div_5  = false;
    *is_num = false;
    const off_t old_pos = file->current_position;
    int mod_3_sum = 0;
    char prev_ch = '\0';
    char curr_ch = f_getchar (file);
    
    if (curr_ch == '+' || curr_ch == '-')
    {
        curr_ch = f_getchar (file);
    }

    const char first_ch = curr_ch;
    for (; isdigit (curr_ch); 
        prev_ch = curr_ch, 
        curr_ch = f_getchar (file))
    {
        mod_3_sum += to_num (curr_ch);
        mod_3_sum %= 3;
    }

    if (file->current_position - old_pos > 1 && first_ch == '0') return file->current_position - old_pos;

    bool is_inteter = true;
    if (curr_ch == '.')
        for (curr_ch = f_getchar (file); isdigit (curr_ch) && is_inteter;
             prev_ch = curr_ch, curr_ch = f_getchar (file))
            if (curr_ch != '0')
                is_inteter = false;

    if (isdigit (prev_ch) && (isspace (curr_ch) || curr_ch == '\0')) *is_num = true;

    if (is_inteter && mod_3_sum == 0) 
        *div_3 = true;
    if (is_inteter && (prev_ch == '5' || prev_ch == '0')) 
        *div_5 = true;

    if (!f_eof (file)) f_move (file, -1);
    return file->current_position - old_pos;
}

File file_ctor_handeled (const char *path, int mode)
{
    assert (path);

    File file;
    if (! f_ctor (&file, path,  mode))
    {
        fprintf (stderr, "Error: can't open \'%s\'\n", path);
        exit (1);
    }
    if (is_common_file (&file) == false)
    {
        fprintf (stderr, "Error: %s is not a regular file.\n", path);
        exit (1);
    }  

    return file;
}

void print_spaces (File *intput, File *output)
{
    char ch = 0;
    while (isspace (ch = f_getchar (intput)))
        f_putchar (output, ch);

    if (ch != '\0')
        f_move (intput, -1);
}

int to_num (char ch)
{
    assert (isdigit (ch));
    return ch - '0';
}

