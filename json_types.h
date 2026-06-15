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