#include "json_types.h"
#include "json_utils.h"

void dump_json(JsonValue *json);

/* Parse JSON file */
JsonValue *str_to_json_value(char *str);
JsonValue *str_to_json_int(char *str);
JsonValue *str_to_json_double(char *str);
JsonValue *str_to_json_str(char *str);
JsonValue *str_to_json_bool(char *str);
JsonValue *str_to_json_array(char *str_array);
JsonValue *str_to_json_object(char *str_object);

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
            JsonValue *item = json->as.array.items[i];
            if (item)
            {
                dump_json(item);
            }
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
            JsonEntry *entry = json->as.object.entries[i];
            if (entry)
            {
                printf("%s: ", entry->key);
                dump_json(entry->value);
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
        printf("null");
        break;

    default:
        break;
    }
}

JsonValue *str_to_json_value(char *str)
{
    if (!str)
    {
        return NULL;
    }

    trim(str);
    size_t len = strlen(str);

    // INTEGER
    if (isint(str))
        return str_to_json_int(str);

    // DOUBLE
    if (isdouble(str))
        return str_to_json_double(str);

    // STRING
    if (len >= 2 && str[0] == '"' && str[len - 1] == '"')
    {
        return str_to_json_str(str);
    }

    // BOOLEAN
    if (strcmp(str, "true") == 0 || strcmp(str, "false") == 0)
        return str_to_json_bool(str);

    // NULL
    if (strcmp(str, "null") == 0)
    {
        JsonValue *v = malloc(sizeof(JsonValue));
        if (!v)
        {
            printf("An error has occurred");
            return NULL;
        }
        v->type = NULL_VALUE;
        return v;
    }

    // ARRAY
    if (len >= 2 && str[0] == '[' && str[len - 1] == ']')
        return str_to_json_array(str);

    // OBJECT
    if (len >= 2 && str[0] == '{' && str[len - 1] == '}')
        return str_to_json_object(str);

    return NULL;
}

JsonValue *str_to_json_int(char *str)
{
    JsonValue *value = malloc(sizeof(JsonValue));
    if (!value)
    {
        return NULL;
    }

    value->type = INTEGER;
    value->as.integer = atoi(str);

    return value;
}

JsonValue *str_to_json_double(char *str)
{
    JsonValue *value = malloc(sizeof(JsonValue));
    if (!value)
    {
        return NULL;
    }

    value->type = DOUBLE;
    value->as.double_value = atof(str);

    return value;
}

JsonValue *str_to_json_str(char *str)
{
    JsonValue *value = malloc(sizeof(JsonValue));
    if (!value)
    {
        return NULL;
    }

    value->type = STRING;
    value->as.str = str ? strdup(str) : NULL;

    return value;
}

JsonValue *str_to_json_bool(char *str)
{
    JsonValue *value = malloc(sizeof(JsonValue));
    if (!value)
    {
        return NULL;
    }

    value->type = BOOLEAN;
    value->as.boolean = strcmp(str, "true") == 0;

    return value;
}

/*
 * Returns JsonValue with ARRAY as type
 * Returns a JsonValue with NULL as type if error
 *
 */
JsonValue *str_to_json_array(char *str_array)
{
    JsonValue *v = malloc(sizeof(JsonValue));
    if (!v)
    {
        return NULL;
    }

    v->type = ARRAY;
    v->as.array.capacity = 256;
    v->as.array.length = 0;
    v->as.array.items = malloc(sizeof(JsonValue) * 256);

    if (!v->as.array.items)
    {
        fprintf(stderr, "Failed to parse: %s\n", str_array);
        return NULL;
    }

    // Skip '['
    char *p = str_array + 1;

    int depth = 0, in_str = 0;
    char buffer[2048];
    size_t id = 0;

    while (*p)
    {
        if (v->as.array.length == v->as.array.capacity)
        {
            v->as.array.capacity *= 2;
            v->as.array.items = realloc(v->as.array.items, sizeof(JsonValue) * v->as.array.capacity);
            if (!v->as.array.items)
            {
                printf("An error has occurred\n");
                return v;
            }
        }

        if (*p == '"' && (p == str_array + 1 || *(p - 1) != '\\'))
            in_str = !in_str;

        if (!in_str)
        {
            if (*p == '{' || *p == '[')
            {
                depth++;
            }

            if (*p == '}' || *p == ']')
            {
                if (depth > 0)
                    depth--;
                else if (*p == ']')
                {
                    buffer[id] = '\0';

                    trim(buffer);

                    if (id > 0)
                    {
                        v->as.array.items[v->as.array.length++] = str_to_json_value(buffer);
                    }

                    break;
                }
            }
        }

        if (!in_str && *p == ',' && depth == 0)
        {
            buffer[id] = '\0';

            trim(buffer);

            if (id > 0)
            {
                v->as.array.items[v->as.array.length++] = str_to_json_value(buffer);
            }

            id = 0;

            p++;
            continue;
        }

        buffer[id++] = *p++;
    }

    return v;
}

/*
 * Returns JsonValue with OBJECT as type
 * Returns a JsonValue with NULL as type if error
 *
 */
JsonValue *str_to_json_object(char *str_object)
{
    JsonValue *v = malloc(sizeof(JsonValue));
    if (!v)
    {
        return NULL;
    }

    v->type = OBJECT;
    v->as.object.capacity = 256;
    v->as.object.count = 0;
    v->as.object.entries = malloc(sizeof(JsonEntry) * 256);

    if (!v->as.object.entries)
    {
        fprintf(stderr, "Failed to parse: %s\n", str_object);
        return NULL;
    }

    int in_str = 0, depth = 0;
    char entry_buf[2048];
    size_t idx = 0;

    // Skip '{'
    char *p = str_object + 1;

    while (*p)
    {
        JsonEntry *entry = malloc(sizeof(JsonEntry));
        if (!entry)
        {
            return NULL;
        }

        if (*p == '"' && (p == str_object + 1 || *(p - 1) != '\\'))
            in_str = !in_str;

        if (!in_str)
        {
            if (*p == '{' || *p == '[')
                depth++;

            if (*p == '}' || *p == ']')
                depth--;

            if ((*p == ',' && depth == 0) ||
                (*p == '}' && depth == -1))
            {
                entry_buf[idx] = '\0';

                trim(entry_buf);

                if (*entry_buf)
                {
                    char *colon = strchr(entry_buf, ':');

                    if (colon)
                    {
                        *colon = '\0';

                        char *key = entry_buf;
                        char *value = colon + 1;

                        trim(key);
                        trim(value);

                        entry->key = strdup(key);
                        entry->value = str_to_json_value(value);

                        v->as.object.entries[v->as.object.count++] = entry;
                    }
                }

                idx = 0;

                if (*p == '}')
                    break;

                p++;
                continue;
            }
        }

        entry_buf[idx++] = *p;
        free(entry);
        p++;
    }

    return v;
}

/* Traverse struct */
int has_next(JsonIterator *iterator);
JsonValue *next(JsonIterator *iterator);
void reset(JsonIterator *iterator);

/* Manipulate struct */
size_t count_items(JsonValue json);
JsonValue *getitem(JsonValue *json, char *key);
JsonValue *setitem(JsonValue *dest, char *key, JsonValue *value);
JsonValue *remitem(JsonValue *dest, char *key);

int has_next(JsonIterator *iterator)
{
    if (!iterator || !iterator->value)
        return 0;

    if (iterator->value->type == ARRAY)
    {
        if (iterator->value->as.array.length == 0 || iterator->pos >= iterator->value->as.array.length)
            return 0;

        if (iterator->value->as.array.items[iterator->pos])
            return 1;
    }

    if (iterator->value->type == OBJECT)
    {
        if (iterator->value->as.object.count == 0 || iterator->pos >= iterator->value->as.object.count)
            return 0;

        if (iterator->value->as.object.entries[iterator->pos])
            return 1;
    }

    return 0;
}

// TODO: handle entry
JsonValue *next(JsonIterator *iterator)
{
    if (!has_next(iterator))
        return NULL;

    if (iterator->value->type == ARRAY)
    {
        if (iterator->value->as.array.length == 0 || iterator->pos >= iterator->value->as.array.length)
            return NULL;

        JsonValue *item = iterator->value->as.array.items[iterator->pos];
        if (item)
        {
            iterator->pos++;
            return item;
        }

        return NULL;
    }

    if (iterator->value->type == OBJECT)
    {
        if (iterator->value->as.object.count == 0 || iterator->pos >= iterator->value->as.object.count)
            return NULL;

        JsonEntry *entry = iterator->value->as.object.entries[iterator->pos];
        if (entry && entry->value)
        {
            iterator->pos++;
            return entry->value;
        }
    }

    return NULL;
}