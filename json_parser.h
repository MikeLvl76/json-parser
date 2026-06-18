#include "json_functions.h"

#define READ_TEST_FILE_PATH "default_data.json"
#define MAX_BUFFER_SIZE 256
#define MAX_FILE_LINES 1024

void read_json(char *filepath)
{

    FILE *file = fopen(filepath, "rb");

    if (!file)
    {
        printf("Error: cannot open file at path %s\n", filepath);
        exit(1);
    }

    char *buffer = (char *)malloc(MAX_BUFFER_SIZE);
    char *str = (char *)malloc(MAX_BUFFER_SIZE * MAX_FILE_LINES);

    if (!buffer || !str)
    {
        printf("Error: cannot allocate memory\n");
        exit(1);
    }

    *buffer = '\0';
    *str = '\0';

    int count = 0;
    while (!feof(file))
    {
        if (ferror(file))
        {
            fprintf(stderr, "Error while reading file: code %d\n", errno);
            exit(1);
        }

        char *line = fgets(buffer, MAX_BUFFER_SIZE, file);
        if (!line)
        {
            count++;
            continue;
        }

        trim(buffer);
        strncat(str, buffer, strlen(buffer));

        count++;
    }

    if (*str != '{' && str[strlen(str) - 1] != '}')
    {
        printf("Missing object brackets in file\n");
        exit(1);
    }

    JsonValue *json = str_to_json_value(str);
    dump_json(json);

    free(buffer);
    free(str);
    free(json);
    fclose(file);
    printf("--- Read %d line(s) ---\n", count);
}