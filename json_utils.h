#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int isint(char *str)
{
    if (*str == '\0')
        return 0;

    if (*str == '-')
    {
        str++;
    }

    while (*str)
    {
        if (!isdigit((unsigned char)*str))
        {
            return 0;
        }

        str++;
    }

    return 1;
}

int isdouble(const char *str)
{
    int has_digit = 0;
    int has_dot = 0;
    int has_exp = 0;

    if (!str || *str == '\0')
        return 0;

    // +/- leading
    if (*str == '-' || *str == '+')
        str++;

    while (*str)
    {
        if (isdigit((unsigned char)*str))
        {
            has_digit = 1;
        }
        else if (*str == '.')
        {
            if (has_dot || has_exp)
                return 0;
            has_dot = 1;
        }
        else if (*str == 'e' || *str == 'E')
        {
            if (has_exp || !has_digit)
                return 0;

            has_exp = 1;
            has_digit = 0; // must have digits after exponent

            // +/- after e
            if (*(str + 1) == '+' || *(str + 1) == '-')
                str++;

            if (!*(str + 1))
                return 0;
        }
        else
        {
            return 0;
        }

        str++;
    }

    return has_digit;
}

void trim(char *str)
{
    if (!str)
    {
        printf("Error: empty or null string\n");
        return;
    }

    char *start = str;

    while (*start && isspace((unsigned char)*start))
    {
        start++;
    }

    if (start != str)
    {
        memmove(str, start, strlen(start) + 1);
    }

    char *end = str + strlen(str);

    while (end > str && isspace((unsigned char)*(end - 1)))
    {
        end--;
    }

    *end = '\0';
}

char *sub(const char *str, size_t start, size_t end)
{
    if (!str || end < start)
        return NULL;

    size_t len = end - start;

    char *out = malloc(len + 1);
    if (!out)
        return NULL;

    memcpy(out, str + start, len);
    out[len] = '\0';

    return out;
}

void print_indent(int indent)
{
    for (int i = 0; i < indent; i++)
        putchar(' ');
}