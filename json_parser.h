#include "json_utils.h"
#include "json_types.h"

#define READ_TEST_FILE_PATH "default_data.json"
#define MAX_BUFFER_SIZE 256
#define MAX_FILE_LINES 1024
#define MAX_SCOPE_DEPTH 128
#define MAX_VALUE_LENGTH 4096

// TODO: retrieve value type
// Value type can be number, string, array (mixed values or not), object or boolean
// JsonValue parse_value(char *pair)
// {
//     char *value = getvalue(pair);
//     if (!value)
//     {
//         return (JsonValue){
//             .type = NULL_VALUE};
//     }

//     /* Case 1: string */
//     if (value[0] == '\"' && value[strlen(value) - 1] == '\"')
//     {
//         return (JsonValue){
//             .type = STRING,
//             .as.str = strdup(value)};
//     }

//     /* Case 2: integer */
//     if (isint(value))
//     {
//         return (JsonValue){
//             .type = INTEGER,
//             .as.integer = atoi(value)};
//     }

//     /* Case 3: double */
//     if (isdouble(value))
//     {
//         return (JsonValue){
//             .type = DOUBLE,
//             .as.double_value = atof(value)};
//     }

//     /* Case 4: array */

//     /* Case 5: object */

//     /* Case 6: boolean */
//     if (strcmp(value, "true") == 0)
//     {
//         return (JsonValue){
//             .type = BOOLEAN,
//             .as.boolean = 1};
//     }
//     else if (strcmp(value, "false") == 0)
//     {
//         return (JsonValue){
//             .type = BOOLEAN,
//             .as.boolean = 0};
//     }

//     /* Case 7: null */
//     if (strcmp(value, "null"))
//     {
//         return (JsonValue){
//             .type = NULL_VALUE};
//     }

//     return (JsonValue){0};
// }


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

void parse(char *str)
{
    char *p = str;

    int in_str = 0;
    int escaped = 0;

    Scope stack[MAX_SCOPE_DEPTH];
    int top = -1;

    while (*p)
    {
        if (escaped)
        {
            escaped = 0;
            p++;
            continue;
        }

        if (*p == '\n')
        {
            escaped = 1;
            p++;
            continue;
        }

        if (*p == '"')
        {
            in_str = !in_str;

            if (!in_str && *(p + 1) == ':')
            {
                char key[128];

                extract_key(p, str, key);

                printf("Key: %s\n", key);

                char *v = p + 2;

                while (*v && isspace((unsigned char)*v))
                    v++;

                // Primitive value
                if (*v != '{' && *v != '[')
                {
                    char value[1024];
                    size_t count = 0;
                    int value_in_quotes = 0;

                    while (*v)
                    {
                        if (*v == '"')
                            value_in_quotes = !value_in_quotes;

                        if (!value_in_quotes &&
                            (*v == ',' || *v == '}' || *v == ']'))
                        {
                            break;
                        }

                        value[count++] = *v++;
                    }

                    value[count] = '\0';

                    printf("Value: %s\n", value);
                }
            }
        }

        if (!in_str)
        {
            if (*p == '{' || *p == '[')
            {
                if (top + 1 < MAX_SCOPE_DEPTH)
                {
                    stack[++top].type = *p;
                    stack[top].start = p;
                }
            }
            else if (*p == '}' || *p == ']')
            {
                if (top >= 0)
                {
                    char expected =
                        stack[top].type == '{'
                            ? '}'
                            : ']';

                    if (*p == expected)
                    {
                        Scope scope = stack[top--];

                        size_t len =
                            (size_t)(p - scope.start + 1);

                        char value[MAX_VALUE_LENGTH];

                        if (len >= sizeof(value))
                            len = sizeof(value) - 1;

                        memcpy(value, scope.start, len);
                        value[len] = '\0';

                        printf("Structure (%c): %s\n",
                               scope.type,
                               value);
                    }
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
    parse(str);

    free(buffer);
    free(str);
    fclose(file);
    printf("--- Read %d line(s) ---\n", count);
}