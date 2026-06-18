#include <stdlib.h>

typedef struct JsonValue JsonValue;
typedef struct JsonArray JsonArray;
typedef struct JsonEntry JsonEntry;
typedef struct JsonObject JsonObject;

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

struct JsonArray
{
    JsonValue **items;
    size_t length;
    size_t capacity;
};

struct JsonObject
{
    JsonEntry **entries;
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
        JsonArray array;
        JsonObject object;
    } as;
};

struct JsonEntry
{
    char *key;
    JsonValue *value;
};
