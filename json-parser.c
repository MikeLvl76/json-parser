#include "json_parser.h"

int main()
{
    JsonValue *json = read_json(READ_TEST_FILE_PATH);

    size_t count = count_elements(*json);
    printf("Count: %zu\n", count);

    JsonEntry *entry = getentry(*json, "\"array_mixed\"");
    printf("key: %s\n", entry->key);
    dump_json(entry->value);
    printf("\n");

    JsonValue *new_value = alloc_value(STRING, (JsonValueAs){.str = "\"Modified string\""});

    setentry(json, "\"string\"", new_value);
    dump_json(json);
    printf("\n");

    free(new_value);

    JsonValue *v = alloc_value(INTEGER, (JsonValueAs){.integer = 67});
    JsonEntry *added = alloc_entry("\"Added integer\"", v);

    addentry(json, added, 1);
    dump_json(json);

    free(v);
    free(added);

    size_t new_count = count_elements(*json);
    printf("New count: %zu\n", new_count);

    rementry(json, "\"string\"");
    dump_json(json);
    printf("\n");

    printf("New count: %zu\n", count_elements(*json));

    free(json);
    return 0;
}