#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct
{
    char *start;
    size_t len;
} Slice;

typedef struct
{
    char type; // '{' or '['
    char *start;
} Scope;

static void extract_key(char *end_quote, char *begin, char *out)
{
    char *k = end_quote - 1;
    size_t len = 0;

    while (k >= begin && *k != '"')
    {
        out[len++] = *k--;
    }

    out[len] = '\0';

    for (size_t i = 0; i < len / 2; ++i)
    {
        char tmp = out[i];
        out[i] = out[len - i - 1];
        out[len - i - 1] = tmp;
    }
}

size_t split_top_level(char *str, char sep, Slice *out, size_t max)
{
    size_t count = 0;

    int in_str = 0;
    int escaped = 0;
    int depth = 0;

    char *start = str;

    for (char *p = str; *p; p++)
    {
        if (escaped)
        {
            escaped = 0;
            continue;
        }

        if (*p == '\\')
        {
            escaped = 1;
            continue;
        }

        if (*p == '"')
            in_str = !in_str;

        if (!in_str)
        {
            if (*p == '{' || *p == '[')
                depth++;

            if (*p == '}' || *p == ']')
                depth--;

            if (depth == 0 && *p == sep)
            {
                if (count < max)
                {
                    out[count].start = start;
                    out[count].len = (size_t)(p - start);
                    count++;
                }

                start = p + 1;
            }
        }
    }

    /* last token */
    if (*start)
    {
        out[count].start = start;
        out[count].len = strlen(start);
        count++;
    }

    return count;
}

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