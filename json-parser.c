#include "json_parser.h"

int main()
{
    read_json(READ_TEST_FILE_PATH);

    /*

    * Test struct with array

    char *str = "[Hi, Hello, Here, How, High]";

    char *items = sub(str, 1, strlen(str) - 1);
    if (!items)
    {
        return 1;
    }

    JsonValue *value = malloc(sizeof(JsonValue));
    value->type = ARRAY;
    value->as.array.capacity = 256;
    value->as.array.length = 0;
    value->as.array.items = malloc(sizeof(JsonValue) * value->as.array.capacity);

    char *delimiter = ",";
    char *token = strtok(items, delimiter);
    while (token)
    {
        value->as.array.items[value->as.array.length++] = (JsonValue){
        .type = STRING,
        .as = {
            .str = token}};
            token = strtok(NULL, delimiter);
        }

        printf("Total count: %zu\n", value->as.array.length);
        for (size_t i = 0; i < value->as.array.length; ++i)
        {
            printf("Items %zu: %s\n", i, value->as.array.items[i].as.str);
        }

        free(items);
        free(value);
    }
    */

    /*

    * Test struct with object

    char *str = "{\"firstname\" : \"John\", \"lastname\" : \"Doe\", \"username\" : \"john_doe\"}";

    char *entries = sub(str, 1, strlen(str) - 1);
    if (!entries)
    {
        return 1;
    }

    JsonValue *value = malloc(sizeof(JsonValue));
    value->type = OBJECT;
    value->as.object.capacity = 256;
    value->as.object.count = 0;
    value->as.object.entries = malloc(sizeof(JsonValueObjectEntry) * value->as.object.capacity);

    char *delimiter = ",";
    char *token = strtok(entries, delimiter);
    while (token)
    {
        char copy[256];
        strcpy(copy, token);

        char *colon = strchr(copy, ':');
        if (colon)
        {
            *colon = '\0';

            char *key = copy;
            char *v = colon + 1;

            trim(key);
            trim(v);
            value->as.object.entries[value->as.object.count++] = (JsonValueObjectEntry){
                .key = strdup(key),
                .value = (JsonValue){
                    .type = STRING,
                    .as = {
                        .str = strdup(v)}}};
                    }

        token = strtok(NULL, delimiter);
    }

    printf("Total count: %zu\n", value->as.object.count);
    printf("{\n");
    for (size_t i = 0; i < value->as.object.count; ++i)
    {
        printf("  %s: %s", value->as.object.entries[i].key, value->as.object.entries[i].value.as.str);
        if (i < value->as.object.count - 1)
        {
            printf(",");
        }
        printf("\n");
    }
    printf("}\n");

    free(entries);
    free(value);
    */

    return 0;
}