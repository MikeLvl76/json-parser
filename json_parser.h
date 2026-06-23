#include "json_functions.h"

JsonValue *read_json(char *filepath, int show_error, int stop_on_error)
{

    FILE *file = fopen(filepath, "rb");

    if (!file)
    {
        if (show_error)
            fprintf(stderr, "Error: cannot open file at path %s\n", filepath);
        
        if (stop_on_error)
            exit(1);
    }

    size_t buffer_capacity = 256, str_capacity = 1024;

    char *buffer = malloc(buffer_capacity);
    char *str = malloc(str_capacity);

    if (!buffer || !str)
    {
        if (show_error)
            fprintf(stderr, "Error: cannot allocate memory\n");

        if (stop_on_error)
            exit(1);
    }

    size_t str_len = 0;

    *str = '\0';

    while (fgets(buffer, (int)buffer_capacity, file))
    {
        if (ferror(file))
        {
            if (show_error)
                fprintf(stderr, "Error while reading file: code %d\n", errno);

            free(buffer);
            free(str);

            if (stop_on_error)
                exit(1);
        }

        trim(buffer, show_error, stop_on_error);

        size_t buf_len = strlen(buffer);

        if (str_len + buf_len + 1 > str_capacity)
        {
            // Reallocate until having enough capacity
            while (str_len + buf_len + 1 > str_capacity)
                str_capacity *= 2;

            char *tmp = realloc(str, str_capacity);
            if (!tmp)
            {
                if (show_error)
                    fprintf(stderr, "Reallocation failed\n");

                free(buffer);
                free(str);

                if (stop_on_error)
                    exit(1);
            }
            str = tmp;
        }

        memcpy(str + str_len, buffer, buf_len);
        str_len += buf_len;
        str[str_len] = '\0';
    }

    free(buffer);
    fclose(file);

    char open = *str;

    if (open == '{')
    {
        if (str[str_len - 1] != '}')
        {
            if (show_error)
                fprintf(stderr, "Missing object brackets in file\n");

            if (stop_on_error)
                exit(1);
        }
    }
    else if (open == '[')
    {
        if (str[str_len - 1] != ']')
        {
            if (show_error)
                fprintf(stderr, "Missing array brackets in file\n");

            if (stop_on_error)
                exit(1);
        }
    }

    JsonValue *json = str_to_json_value(str, show_error, stop_on_error);

    free(str);

    return json;
}