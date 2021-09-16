#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
//#include <io.h>
#include <errno.h>

#include "File.h"

void BizzBuzz (File *input, File *output);
void skip_spaces (File *file);
off_t bizz_buzz_token_loop (File *file, bool *div_3, bool *div_5, bool *is_num);
int to_num (char);

int main(int argc, char **argv) 
{
    if (argc < 3)
    {
        fprintf (stderr, "Format error!\n"
        "Usage: ./bizzbuzz <input-file-name> <output-file-name>\n");
        return 1;
    }
    
    File input;
    File output;
    if (! f_ctor (&input, argv[1],  O_RDONLY))
    {
        fprintf (stderr, "Error: can't open \'%s\'\n", argv[1]);
        return 1;
    }
    if (! f_ctor (&output, argv[2], O_WRONLY))
    {
        fprintf (stderr, "Error: can't open \'%s\'\n", argv[2]);
        return 1;
    }

    BizzBuzz (&input, &output);

    return 0; 
}

void BizzBuzz (File *input, File *output)
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
    while (!f_eof (input))
    {
        skip_spaces (input);   

        tok_offset = bizz_buzz_token_loop (input, &has_divisibility_by_3, &has_divisibility_by_5, &is_number);
        printf ("tok_offset = %ld\n", tok_offset);
        printf ("input->curr_pos = %ld\n", input->current_position);
        printf ("has_divisibility_by_3 = %d\n", has_divisibility_by_3);
        printf ("has_divisibility_by_5 = %d\n", has_divisibility_by_5);
        printf ("is_number             = %d\n", is_number);
        printf ("eof = %d\n", f_eof (input));

        if (tok_offset > 0)
        {
            if (is_number && (has_divisibility_by_3 || has_divisibility_by_5))
            {
                printf ("printing bizz or buzz\n");
                if (has_divisibility_by_3) f_write_str (output, "Bizz");
                if (has_divisibility_by_5) f_write_str (output, "Buzz");
            }
            else 
            {
                f_move (input, -tok_offset);
                printf ("pos after move = %ld\n", input->current_position);
                printf ("eof = %d\n", input->eof);
                printf ("getchar = \'%c\'\n", f_getchar (input));
                //printf ("getchar = %c\n", f_getchar (input));
                f_move (input, -1);
                for (char curr_ch = f_getchar (input); (!isspace (curr_ch)) && curr_ch; curr_ch = f_getchar (input))
                {
                    f_putchar (output, curr_ch); 
                    printf ("putchar (%c)\n", curr_ch);
                }
            }
            f_putchar (output, ' ');
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
    printf ("is_deviding_by_3_or_5::curr_ch = %c\n", curr_ch);

    if (curr_ch == '+' || curr_ch == '-')
    {
        curr_ch = f_getchar (file);
    }

    for (; isdigit (curr_ch) && ! f_eof (file); 
        prev_ch = curr_ch, 
        curr_ch = f_getchar (file))
    {
        printf ("is_dev loop got \'%c\', prev = \'%c\'\n", curr_ch, prev_ch);
        mod_3_sum += to_num (curr_ch);
        mod_3_sum %= 3;
    }

    if (isdigit (prev_ch) && (isspace (curr_ch) || curr_ch == '\0')) *is_num = true;

    if (mod_3_sum == 0) 
        *div_3 = true;
    if (prev_ch == '5' || prev_ch == '0') 
        *div_5 = true;

    return file->current_position - old_pos;
}


void skip_spaces (File *file)
{
    char ch = 0;
    while (isspace (ch = f_getchar (file)))
        printf ("skipping \'%c\'\n", ch);

    if (ch != '\0')
        f_move (file, -1);
}

int to_num (char ch)
{
    assert (isdigit (ch));
    return ch - '0';
}

