#include <stdlib.h>

typedef struct json_value_t json_value_t;
typedef struct json_array_t json_array_t;
typedef struct json_entry_t json_entry_t;
typedef struct json_object_t json_object_t;
typedef struct json_iterator_t json_iterator_t;

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

struct json_array_t
{
    json_value_t **items;
    size_t length;
    size_t capacity;
};

struct json_object_t
{
    json_entry_t **entries;
    size_t count;
    size_t capacity;
};

typedef union
{
    int integer;
    double double_value;
    char *str;
    int boolean;
    json_array_t array;
    json_object_t object;
} JsonValueAs;

struct json_value_t
{
    JsonValueType type;
    JsonValueAs as;
};

struct json_entry_t
{
    char *key;
    json_value_t *value;
};

struct json_iterator_t
{
    json_value_t *value;
    size_t pos;
    union
    {
        json_value_t *item;
        json_entry_t *entry;
    } current;
};