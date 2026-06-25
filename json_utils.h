#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int isint(const char *str, int show_error, int stop_on_error)
{
    if (!str || *str == '\0')
    {
        if (show_error)
            fprintf(stderr, "String is empty\n");

        if (stop_on_error)
            exit(1);

        return 0;
    }

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

int isdouble(const char *str, int show_error, int stop_on_error)
{
    int has_digit = 0;
    int has_dot = 0;
    int has_exp = 0;

    if (!str || *str == '\0')
    {
        if (show_error)
            fprintf(stderr, "String is empty\n");

        if (stop_on_error)
            exit(1);

        return 0;
    }

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

void trim(char *str, int show_error, int stop_on_error)
{
    if (!str)
    {
        if (show_error)
            fprintf(stderr, "Error: empty or null string\n");

        if (stop_on_error)
            exit(1);

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

char *sub(const char *str, size_t start, size_t end, int show_error, int stop_on_error)
{
    if (!str || end < start)
    {
        if (show_error)
            fprintf(stderr, "Check if indices (%zu, %zu) are valid or if string exists\n", start, end);

        if (stop_on_error)
            exit(1);

        return NULL;
    }

    size_t len = end - start;

    char *out = malloc(len + 1);
    if (!out)
    {
        if (show_error)
            fprintf(stderr, "Cannot allocate memory\n");

        if (stop_on_error)
            exit(1);

        return NULL;
    }

    memcpy(out, str + start, len);
    out[len] = '\0';

    return out;
}

void print_indent(int indent)
{
    for (int i = 0; i < indent; i++)
        putchar(' ');
}

void print_tree_prefix(int *has_next, int depth)
{
    for (int i = 0; i < depth; ++i)
    {
        printf(has_next[i] ? "│   " : "    ");
    }
}

int check_extension(char *path, int show_error, int stop_on_error)
{
    if (!path || strlen(path) < 6)
    {
        if (show_error)
            fprintf(stderr, "Path \"%s\" doesn't exist or doesn't have enough characters\n", path);

        if (stop_on_error)
            exit(1);

        return 0;
    }

    char *extension = sub(path, strlen(path) - strlen(".json"), strlen(path), show_error, stop_on_error);
    return strcmp(extension, ".json") == 0;
}

int starts_with(char *str, char c, int show_error, int stop_on_error)
{
    if (!str || *str == '\0')
    {
        if (show_error)
            fprintf(stderr, "String doesn't exist\n");

        if (stop_on_error)
            exit(1);

        return 0;
    }

    return str[0] == c;
}