#include <stdlib.h>
#include <stdio.h>

typedef struct JsonValue JsonValue;
typedef struct JsonValueObjectEntry JsonValueObjectEntry;
typedef struct JsonValueObject JsonValueObject;

typedef enum
{
    NULL_VALUE,
    INTEGER,
    DOUBLE,
    STRING,
    BOOLEAN,
    ARRAY,
    OBJECT
} JsonValueType;

struct JsonValueObject
{
    JsonValueObjectEntry *entries;
    size_t count;
    size_t capacity;
};

struct JsonValue
{
    JsonValueType type;
    union
    {
        int integer;
        double double_value;
        char *str;
        int boolean;
        struct
        {
            JsonValue *items;
            size_t length;
            size_t capacity;
        } array;

        JsonValueObject object;
    } as;
};

struct JsonValueObjectEntry
{
    char *key;
    JsonValue value;
};

void dump_json_value(JsonValue *value)
{
    if (!value)
        printf("No json value\n");

    switch (value->type)
    {
    case NULL_VALUE:
        printf("null\n");
        break;

    case INTEGER:
        printf("%d\n", value->as.integer);
        break;

    case DOUBLE:
        printf("%lf\n", value->as.double_value);
        break;

    case STRING:
        printf("%s\n", value->as.str);
        break;

    case BOOLEAN:
        printf(value->as.boolean ? "true\n" : "false\n");
        break;

    case ARRAY:
        printf("TODO: display array values\n");
        break;

    case OBJECT:
        printf("TODO: display key/value pairs\n");
        break;

    default:
        printf("Nothing to display\n");
        break;
    }
}