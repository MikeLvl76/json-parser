#include <stdlib.h>

typedef struct JsonValue JsonValue;
typedef struct JsonArray JsonArray;
typedef struct JsonEntry JsonEntry;
typedef struct JsonObject JsonObject;
typedef struct JsonIterator JsonIterator;

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

typedef union
{
    int integer;
    double double_value;
    char *str;
    int boolean;
    JsonArray array;
    JsonObject object;
} JsonValueAs;

struct JsonValue
{
    JsonValueType type;
    JsonValueAs as;
};

struct JsonEntry
{
    char *key;
    JsonValue *value;
};

struct JsonIterator
{
    JsonValue *value;
    size_t pos;
    union
    {
        JsonValue *item;
        JsonEntry *entry;
    } current;
};