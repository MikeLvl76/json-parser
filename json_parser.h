#include "json_utils.h"
#include "json_types.h"

#define READ_TEST_FILE_PATH "test_parse.json"
#define MAX_BUFFER_SIZE 256
#define MAX_FILE_LINES 1024

char *getkey(char *pair)
{
    if (!pair || !strpbrk(pair, ":"))
        return NULL;

    char *copy = strdup(pair);
    char *key = strtok(copy, ":");
    if (!key)
        return NULL;
    key = sub(key, 1, strlen(key) - 1);

    return key;
}

char *getvalue(char *pair)
{
    if (!pair || !strpbrk(pair, ":"))
        return pair;

    char *copy = strdup(pair);
    char *value = strtok(copy, ":");
    value = strtok(NULL, ":");
    if (strchr(value, (unsigned char)','))
    {
        value = sub(value, 0, strlen(value) - 1);
    }
    trim(value);

    return value;
}

// TODO: retrieve value type
// Value type can be number, string, array (mixed values or not), object or boolean
JsonValue parse_value(char *pair)
{
    char *value = getvalue(pair);
    if (!value)
    {
        return (JsonValue){
            .type = NULL_VALUE};
    }

    /* Case 1: string */
    if (value[0] == '\"' && value[strlen(value) - 1] == '\"')
    {
        return (JsonValue){
            .type = STRING,
            .as.str = strdup(value)};
    }

    /* Case 2: integer */
    if (isint(value))
    {
        return (JsonValue){
            .type = INTEGER,
            .as.integer = atoi(value)};
    }

    /* Case 3: double */
    if (isdouble(value))
    {
        return (JsonValue){
            .type = DOUBLE,
            .as.double_value = atof(value)};
    }

    /* Case 4: array */

    /* Case 5: object */

    /* Case 6: boolean */
    if (strcmp(value, "true") == 0)
    {
        return (JsonValue){
            .type = BOOLEAN,
            .as.boolean = 1};
    }
    else if (strcmp(value, "false") == 0)
    {
        return (JsonValue){
            .type = BOOLEAN,
            .as.boolean = 0};
    }

    /* Case 7: null */
    if (strcmp(value, "null"))
    {
        return (JsonValue){
            .type = NULL_VALUE};
    }

    return (JsonValue){0};
}

void parse(char *str, char open_c, char closing_c)
{
    char *p = str;
    long long int obj_start = -1;
    int depth = 0, in_str = 0, escaped = 0, is_data_structure = 0;

    while (*p)
    {
        if (escaped)
        {
            escaped = 0;
            p++;
        }

        if (*p == '\n')
        {
            escaped = 1;
            p++;
        }

        if (*p == '"')
        {

            in_str = !in_str;

            if (!in_str && !is_data_structure)
            {
                if (*(p + 1) == ':')
                {
                    char *k = p, *v = p;
                    char key[128], value[1024];
                    int in_quotes = 0;

                    while (*k)
                    {
                        if ((*k == ',' || *k == '{') && !in_quotes)
                        {
                            key[p - k] = *k;
                            break;
                        }

                        if (*k == '"')
                        {
                            in_quotes = !in_quotes;
                        }

                        key[p - k] = *k;
                        k--;
                    }
                    key[p - k] = '\0';

                    printf("Key: %s\n", strrev(key));

                    size_t count = 0;
                    // Skip end of key ('"' character) and ":" character
                    v = p + 2;
                    while (*v)
                    {
                        if ((*v == '\n' || *v == '\0' || *v == ',' || *v == ']' || *v == '}') && !in_quotes)
                        {
                            break;
                        }

                        if ((*v == '{' || *v == '[') && !in_quotes)
                        {
                            is_data_structure = 1;
                            break;
                        }

                        if (*v == '"')
                        {
                            in_quotes = !in_quotes;
                        }

                        if (isspace(*v) && !in_quotes)
                        {
                            v++;
                            continue;
                        }

                        value[count++] = *v;
                        v++;
                    }
                    value[count] = '\0';
                    is_data_structure = 0;
                    printf("Value: %s\n", value);
                }
            }
        }

        if (!in_str)
        {
            if (*p == open_c)
            {
                if (depth == 0)
                {
                    obj_start = p - str;
                }
                depth++;
            }
            else if (*p == closing_c)
            {
                depth--;

                if (depth == 0 && obj_start > -1)
                {
                    char value[1024];
                    size_t count = 0;

                    for (long long i = obj_start; i < (p - str) + 1; ++i)
                    {
                        value[count++] = str[i];
                    }
                    value[count] = '\0';
                    printf("Structure: %s\n", value);
                }
            }
        }

        p++;
    }
}

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
    // printf("%s\n", str);

    if (*str != '{' && str[strlen(str) - 1] != '}')
    {
        printf("Missing object brackets in file\n");
        exit(1);
    }

    // parse_structure(str, '{', '}');
    parse(str, '[', ']');

    free(buffer);
    free(str);
    fclose(file);
    printf("--- %d line(s) ---\n", count);
}