#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define READ_TEST_FILE_PATH "test_parse.json"
#define MAX_BUFFER_SIZE 256

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

void read_json(char *filepath)
{

    FILE *file = fopen(filepath, "r");

    if (!file)
    {
        printf("Error: cannot open file at path %s\n", filepath);
        exit(1);
    }

    char *buffer = (char *)malloc(MAX_BUFFER_SIZE);

    if (!buffer)
    {
        printf("Error: cannot allocate memory\n");
        exit(1);
    }

    int line_count = 0;
    while (!feof(file))
    {
        if (ferror(file))
        {
            fprintf(stderr, "Error while reading file: code %d\n", errno);
            exit(1);
        }

        fgets(buffer, MAX_BUFFER_SIZE, file);
        if (line_count == 0 && *buffer != '{')
        {
            printf("Missing expected open bracked '{', got value: %s", buffer);
            exit(1);
        }

        trim(buffer);
        printf("%s\n", buffer);
        line_count++;
    }

    if (*buffer != '}')
    {
        printf("Missing expected closing bracked '}', got value: %s", buffer);
        exit(1);
    }

    free(buffer);
    fclose(file);
}