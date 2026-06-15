#include "json_types.h"

void dump_json(JsonValue *json)
{
    if (!json)
    {
        printf("JSON value does not exist\n");
        return;
    }

    switch (json->type)
    {

    case INTEGER:
        printf("%d", json->as.integer);
        break;

    case DOUBLE:
        printf("%lf", json->as.double_value);
        break;

    case STRING:
        printf("%s", json->as.str);
        break;

    case ARRAY:
        printf("[");
        for (size_t i = 0; i < json->as.array.length; ++i)
        {
            JsonValue *item = &json->as.array.items[i];
            if (item)
                dump_json(item);
            if (i < json->as.array.length - 1)
            {
                printf(", ");
            }
        }
        printf("]");
        break;

    case OBJECT:
        printf("{");
        for (size_t i = 0; i < json->as.object.count; ++i)
        {
            JsonValueObjectEntry *entry = &json->as.object.entries[i];
            if (entry)
            {
                printf("  %s: ", entry->key);
                dump_json(&entry->value);
            }
            if (i < json->as.object.count - 1)
            {
                printf(",");
            }
        }
        printf("}");
        break;

    case BOOLEAN:
        printf(json->as.boolean ? "True" : "False");
        break;

    case NULL_VALUE:
        printf("NULL");
        break;

    default:
        break;
    }
}