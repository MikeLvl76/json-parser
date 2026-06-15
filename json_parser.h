#include "json_utils.h"
#include "json_functions.h"
#include <stdlib.h>

#define READ_TEST_FILE_PATH "default_data.json"
#define MAX_BUFFER_SIZE 256
#define MAX_FILE_LINES 1024
#define MAX_SCOPE_DEPTH 128
#define MAX_VALUE_LENGTH 4096

JsonValue parse_array(char *str);
JsonValue parse_object(char *str);
JsonValue parse_value(char *str);
JsonValue *parse(char *str);
void read_json(char *filepath);

JsonValue parse_array(char *str)
{
    JsonValue v = (JsonValue){
        .type = ARRAY,
        .as.array.capacity = 256,
        .as.array.length = 0,
        .as.array.items = malloc(sizeof(JsonValue) * 256)};

    if (!v.as.array.items)
    {
        fprintf(stderr, "Failed to parse: %s\n", str);
        return (JsonValue){0};
    }

    char *inner = sub(str, 1, strlen(str) - 1);

    Slice parts[256];
    size_t n = split_top_level(inner, ',', parts, 256);

    for (size_t i = 0; i < n; ++i)
    {
        char buf[1024];
        memcpy(buf, parts[i].start, parts[i].len);
        buf[parts[i].len] = '\0';

        trim(buf);

        if (v.as.array.length == v.as.array.capacity)
        {
            v.as.array.capacity *= 2;

            v.as.array.items =
                realloc(
                    v.as.array.items,
                    sizeof(JsonValue) *
                        v.as.array.capacity);

            if (!v.as.array.items)
            {
                fprintf(stderr, "Failed to parse: %s\n", str);
                return (JsonValue){0};
            }
        }

        v.as.array.items[v.as.array.length++] =
            parse_value(buf);
    }

    free(inner);
    return v;
}

JsonValue parse_object(char *str)
{
    JsonValue v = (JsonValue){
        .type = OBJECT,
        .as.object.capacity = 256,
        .as.object.count = 0,
        .as.object.entries = malloc(sizeof(JsonValueObjectEntry) * 256)};

    if (!v.as.object.entries)
    {
        fprintf(stderr, "Failed to parse: %s\n", str);
        return (JsonValue){0};
    }

    char *inner = sub(str, 1, strlen(str) - 1);

    Slice parts[256];
    size_t n = split_top_level(inner, ',', parts, 256);

    for (size_t i = 0; i < n; ++i)
    {
        char buf[1024];
        if (parts[i].len >= sizeof(buf))
        {
            fprintf(stderr, "Token too large\n");
            continue;
        }
        memcpy(buf, parts[i].start, parts[i].len);
        buf[parts[i].len] = '\0';

        char *colon = strchr(buf, ':');
        if (!colon)
            continue;

        *colon = '\0';

        char *key = buf;
        char *val = colon + 1;

        trim(key);
        trim(val);

        if (!key || !val || *key == '\0' || *val == '\0')
            continue;

        if (v.as.object.count == v.as.object.capacity)
        {
            v.as.object.capacity *= 2;

            v.as.object.entries =
                realloc(
                    v.as.object.entries,
                    sizeof(JsonValueObjectEntry) *
                        v.as.object.capacity);

            if (!v.as.object.entries)
            {
                fprintf(stderr, "Failed to parse: %s\n", str);
                return (JsonValue){0};
            }
        }

        v.as.object.entries[v.as.object.count++] =
            (JsonValueObjectEntry){
                .key = strdup(key),
                .value = parse_value(val)};
    }

    free(inner);
    return v;
}

JsonValue parse_value(char *str)
{
    JsonValue value = {0};

    if (!str)
    {
        value.type = NULL_VALUE;
        return value;
    }

    trim(str);

    size_t len = strlen(str);

    /* STRING */
    if (len >= 2 && str[0] == '"' && str[len - 1] == '"')
    {
        str[len - 1] = '\0';

        value.type = STRING;
        value.as.str = strdup(str + 1);

        return value;
    }

    /* INTEGER */
    if (isint(str))
    {
        value.type = INTEGER;
        value.as.integer = atoi(str);
        return value;
    }

    /* DOUBLE */
    if (isdouble(str))
    {
        value.type = DOUBLE;
        value.as.double_value = atof(str);
        return value;
    }

    /* BOOLEAN */
    if (strcmp(str, "true") == 0 || strcmp(str, "false") == 0)
    {
        value.type = BOOLEAN;
        value.as.boolean = strcmp(str, "true") == 0;
        return value;
    }

    /* NULL */
    if (strcmp(str, "null") == 0)
    {
        value.type = NULL_VALUE;
        return value;
    }

    /* ARRAY */
    if (len >= 2 && str[0] == '[' && str[len - 1] == ']')
        return parse_array(str);

    /* OBJECT */
    if (len >= 2 && str[0] == '{' && str[len - 1] == '}')
        return parse_object(str);

    return value;
}

JsonValue *parse(char *str)
{
    JsonValue *json = malloc(sizeof(JsonValue));
    if (!json)
    {
        return NULL;
    }

    json->type = OBJECT;
    json->as.object.capacity = 256;
    json->as.object.count = 0;
    json->as.object.entries = malloc(sizeof(JsonValueObjectEntry) * json->as.object.capacity);

    char *p = str;

    int in_str = 0;
    int escaped = 0;

    Scope stack[MAX_SCOPE_DEPTH];
    int top = -1;

    while (*p)
    {
        if (json->as.object.count == json->as.object.capacity)
        {
            json->as.object.capacity *= 2;
            json->as.object.entries = realloc(json->as.object.entries, sizeof(JsonValueObjectEntry) * json->as.object.capacity);
            if (!json->as.object.entries)
            {
                printf("An error has occurred\n");
                break;
            }
        }

        JsonValueObjectEntry *entry = &json->as.object.entries[json->as.object.count++];

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

                // printf("Key: %s\n", key);
                entry->key = strdup(key);

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

                    entry->value = parse_value(value);

                    // printf("Value: %s\n", value);
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

                        entry->value = parse_value(value);

                        // printf("Structure (%c): %s\n", scope.type, value);
                    }
                }
            }
        }
        p++;
    }

    return json;
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

    if (*str != '{' && str[strlen(str) - 1] != '}')
    {
        printf("Missing object brackets in file\n");
        exit(1);
    }

    JsonValue *json = parse(str);
    if (!json)
    {
        printf("Error: cannot parse file\n");
        return;
    }
    dump_json(json);

    free(buffer);
    free(str);
    free(json);
    fclose(file);
    printf("--- Read %d line(s) ---\n", count);
}