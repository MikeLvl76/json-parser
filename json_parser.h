#include "json_utils.h"
#include "json_types.h"

#define READ_TEST_FILE_PATH "test_parse.json"
#define MAX_BUFFER_SIZE 256

char *getkey(char *pair)
{
    char *copy = strdup(pair);
    if (!copy || strlen(copy) < 6 || !strpbrk(copy, ":"))
        return NULL;

    char *key = strtok(copy, ":");
    key = sub(key, 1, strlen(key) - 1);

    return key;
}

// TODO: retrieve value type
// Value type can be number, string, array (mixed values or not), object or boolean
JsonValue getvalue(char *pair)
{
    char *copy = strdup(pair);
    if (!copy || strlen(copy) < 6 || !strpbrk(copy, ":"))
    {
        return (JsonValue){
            .type = NULL_VALUE};
    }

    char *value = strtok(copy, ":");
    value = strtok(NULL, ":");
    if (strchr(value, (unsigned char)','))
    {
        value = sub(value, 0, strlen(value) - 1);
    }
    trim(value);

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
        // printf("%s\n", buffer);

        char *key = getkey(buffer);
        JsonValue value = getvalue(buffer);
        printf("%s: ", key);
        dump_json_value(&value);
        line_count++;
    }

    if (*buffer != '}')
    {
        printf("Missing expected closing bracked '}', got value: %s", buffer);
        exit(1);
    }

    free(buffer);
    fclose(file);
    printf("--- %d line(s) ---\n", line_count);
}