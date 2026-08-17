#include "json-structs.h"
#include "json-utils.h"

/* Alloc/free memory */
json_value_t *init_json_value(JSON_VALUE_TYPE type, json_value_as_union_t as);
json_entry_t *init_json_entry(char *key, json_value_t *value);
void free_json_value(json_value_t *value);
void free_json_entry(json_entry_t *entry);

/* Construct JSON with stringified data from file */
void dump_json(json_value_t *json, int indent, int prettify);
void tree(json_value_t *root, int show_values);
void paths(json_value_t *root);
json_value_t *str_to_json_value(char *str, int show_error, int stop_on_error);
json_value_t *str_to_json_int(char *str, int show_error, int stop_on_error);
json_value_t *str_to_json_double(char *str, int show_error, int stop_on_error);
json_value_t *str_to_json_str(char *str, int show_error, int stop_on_error);
json_value_t *str_to_json_bool(char *str, int show_error, int stop_on_error);
json_value_t *str_to_json_array(char *str_array, int show_error, int stop_on_error);
json_value_t *str_to_json_object(char *str_object, int show_error, int stop_on_error);

/* Manipulate struct */
size_t count_elements(json_value_t json);
size_t idxentry(json_value_t json, char *key, int show_error, int stop_on_error);
json_entry_t *getentry(json_value_t json, char *key, int show_error);
json_entry_t *entry_at(json_value_t json, size_t index, int show_error, int stop_on_error);
int addentry(json_value_t *dest, json_entry_t *entry, size_t position, int show_error, int stop_on_error);
int setentry(json_value_t *dest, char *key, json_value_t *value, int show_error, int stop_on_error);
int rementry(json_value_t *dest, char *key, int show_error, int stop_on_error);
char **getkeys(json_value_t json, int show_error, int stop_on_error);
json_value_t **getvalues(json_value_t json, int show_error, int stop_on_error);

/* Alloc/free memory */
json_value_t *init_json_value(JSON_VALUE_TYPE type, json_value_as_union_t as)
{
    json_value_t *value = malloc(sizeof(json_value_t));
    if (!value)
    {
        printf("Cannot allocate memory for json_value_t\n");
        return NULL;
    }

    switch (type)
    {
    case INTEGER:
        value->type = INTEGER;
        value->as.integer = as.integer;
        return value;

    case DOUBLE:
        value->type = DOUBLE;
        value->as.double_value = as.double_value;
        return value;

    case STRING:
        value->type = STRING;
        value->as.str = as.str;
        return value;

    case ARRAY:
        value->type = ARRAY;
        value->as.array = as.array;
        return value;

    case OBJECT:
        value->type = OBJECT;
        value->as.object = as.object;
        return value;

    case BOOLEAN:
        value->type = BOOLEAN;
        value->as.boolean = as.boolean;
        return value;

    case NULL_VALUE:
        value->type = NULL_VALUE;
        break;

    default:
        return NULL;
        break;
    }

    return NULL;
}

json_entry_t *init_json_entry(char *key, json_value_t *value)
{
    if (!key)
    {
        printf("No key set\n");
        return NULL;
    }

    if (!value)
    {
        printf("No value set\n");
        return NULL;
    }

    json_entry_t *entry = malloc(sizeof(json_entry_t));
    if (!entry)
    {
        printf("Cannot allocate json_entry_t\n");
        return NULL;
    }

    entry->key = strdup(key);
    entry->value = value;

    return entry;
}

void free_json_value(json_value_t *value)
{
    if (value)
    {
        if (value->type == ARRAY && value->as.array.items)
        {
            size_t length = value->as.array.length;
            for (size_t i = 0; i < length; ++i)
            {
                free_json_value(value->as.array.items[i]);
            }
            free(value->as.array.items);
        }
        else if (value->type == OBJECT && value->as.object.entries)
        {
            size_t count = value->as.object.count;
            for (size_t i = 0; i < count; ++i)
            {
                free_json_entry(value->as.object.entries[i]);
            }
            free(value->as.object.entries);
        }
        else if (value->type == STRING && value->as.str)
        {
            free(value->as.str);
        }
        free(value);
    }
}

void free_json_entry(json_entry_t *entry)
{

    if (entry)
    {
        if (entry->key)
        {
            free(entry->key);
        }

        if (entry->value)
        {
            free_json_value(entry->value);
        }
        free(entry);
    }
}

/* Construct JSON with stringified data from file */

void dump_json(json_value_t *json, int indent, int prettify)
{
    if (!json)
    {
        printf("No value");
        return;
    }

    switch (json->type)
    {

    case INTEGER:
        printf("%d", json->as.integer);
        break;

    case DOUBLE:
        printf("%.3lf", json->as.double_value);
        break;

    case STRING:
        printf("%s", json->as.str);
        break;

    case ARRAY:
        if (json->as.array.length == 0)
        {
            printf("[]");
            break;
        }

        printf(prettify > 0 ? "[\n" : "[");

        for (size_t i = 0; i < json->as.array.length; ++i)
        {
            if (prettify)
                print_indent(indent + 4);

            dump_json(json->as.array.items[i], prettify > 0 ? indent + 4 : indent, prettify);
            if (i < json->as.array.length - 1)
                printf(", ");

            if (prettify)
                printf("\n");
        }

        if (prettify)
            print_indent(indent);
        printf("]");
        break;

    case OBJECT:
        if (json->as.object.count == 0)
        {
            printf("{}");
            break;
        }

        printf(prettify > 0 ? "{\n" : "{");

        for (size_t i = 0; i < json->as.object.count; ++i)
        {
            if (prettify)
                print_indent(indent + 4);
            printf("%s: ", json->as.object.entries[i]->key);
            dump_json(json->as.object.entries[i]->value, prettify > 0 ? indent + 4 : indent, prettify);
            if (i < json->as.object.count - 1)
                printf(",");

            if (prettify)
                printf("\n");
        }

        if (prettify)
            print_indent(indent);
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

char *type_name(JSON_VALUE_TYPE type)
{
    switch (type)
    {
    case INTEGER:
        return "integer";
    case DOUBLE:
        return "double";
    case STRING:
        return "string";
    case ARRAY:
        return "array";
    case OBJECT:
        return "object";
    case BOOLEAN:
        return "boolean";
    case NULL_VALUE:
        return "null";
    default:
        return "unknown";
    }
}

char *type_value(json_value_t *value)
{
    char *str = malloc(64);

    switch (value->type)
    {
    case INTEGER:
        snprintf(str, 64, "%d", value->as.integer);
        break;

    case DOUBLE:
        snprintf(str, 64, "%.3f", value->as.double_value);
        break;

    case BOOLEAN:
        strcpy(str, value->as.boolean ? "true" : "false");
        break;

    case STRING:
        strcpy(str, value->as.str);
        break;

    case NULL_VALUE:
        strcpy(str, "null");
        break;

    case ARRAY:
        if (value->as.array.length == 0)
        {
            strcpy(str, "[]");
        }
        else
        {
            strcpy(str, "array");
        }
        break;

    case OBJECT:
        if (value->as.object.count == 0)
        {
            strcpy(str, "{}");
        }
        else
        {
            strcpy(str, "object");
        }
        break;

    default:
        strcpy(str, "");
        break;
    }

    return str;
}

void tree_impl(json_value_t *root, int depth, int *has_next, int is_last, char *label, int show_values)
{
    if (!root)
    {
        print_tree_prefix(has_next, depth);
        printf("%s invalid node\n", is_last ? "└──" : "├──");
        return;
    }

    if (depth == 0)
    {
        printf("(%s)\n", type_name(root->type));
    }
    else
    {
        print_tree_prefix(has_next, depth - 1);

        printf("%s ", is_last ? "└──" : "├──");

        if (label)
        {
            char *name = sub(label, 1, strlen(label) - 1, 1, 1);
            if (show_values)
            {
                char *value = type_value(root);
                if (!value)
                {
                    fprintf(stderr, "Cannot stringify value\n");
                    exit(1);
                }
                printf("%s = %s\n", name, value);
                free(value);
            }
            else
                printf("%s (%s)\n", name, type_name(root->type));
            free(name);
        }
        else
        {
            if (show_values)
            {
                char *value = type_value(root);
                if (!value)
                {
                    fprintf(stderr, "Cannot stringify value\n");
                    exit(1);
                }
                printf("%s\n", value);
                free(value);
            }
            else
                printf("(%s)\n", type_name(root->type));
        }
    }

    switch (root->type)
    {
    case ARRAY:
    {
        for (size_t i = 0; i < root->as.array.length; ++i)
        {
            char index[32];
            snprintf(index, sizeof(index), "[%zu]", i);

            has_next[depth] = !is_last;

            tree_impl(root->as.array.items[i], depth + 1, has_next, i == root->as.array.length - 1, index, show_values);
        }
        break;
    }

    case OBJECT:
    {
        for (size_t i = 0; i < root->as.object.count; ++i)
        {
            json_entry_t *entry = root->as.object.entries[i];

            if (!entry)
                continue;

            has_next[depth] = !is_last;

            tree_impl(entry->value, depth + 1, has_next, i == root->as.object.count - 1, entry->key, show_values);
        }
        break;
    }

    default:
        break;
    }
}

void tree(json_value_t *root, int show_values)
{
    int has_next[128] = {0};
    tree_impl(root, 0, has_next, 1, NULL, show_values);
}

json_value_t *str_to_json_value(char *str, int show_error, int stop_on_error)
{
    if (!str)
    {
        if (show_error)
            fprintf(stderr, "No string to parse\n");

        if (stop_on_error)
            exit(1);

        return NULL;
    }

    trim(str, show_error, stop_on_error);
    size_t len = strlen(str);

    // INTEGER
    if (isint(str, show_error, stop_on_error))
        return str_to_json_int(str, show_error, stop_on_error);

    // DOUBLE
    if (isdouble(str, show_error, stop_on_error))
        return str_to_json_double(str, show_error, stop_on_error);

    // STRING
    if (len >= 2 && str[0] == '"' && str[len - 1] == '"')
    {
        return str_to_json_str(str, show_error, stop_on_error);
    }

    // BOOLEAN
    if (strcmp(str, "true") == 0 || strcmp(str, "false") == 0)
        return str_to_json_bool(str, show_error, stop_on_error);

    // NULL
    if (strcmp(str, "null") == 0)
    {
        json_value_t *v = malloc(sizeof(json_value_t));
        if (!v)
        {
            if (show_error)
                fprintf(stderr, "An error has occurred");

            if (stop_on_error)
                exit(1);

            return NULL;
        }
        v->type = NULL_VALUE;
        return v;
    }

    // ARRAY
    if (len >= 2 && str[0] == '[' && str[len - 1] == ']')
        return str_to_json_array(str, show_error, stop_on_error);

    // OBJECT
    if (len >= 2 && str[0] == '{' && str[len - 1] == '}')
        return str_to_json_object(str, show_error, stop_on_error);

    return NULL;
}

json_value_t *str_to_json_int(char *str, int show_error, int stop_on_error)
{
    json_value_t *value = malloc(sizeof(json_value_t));
    if (!value)
    {
        if (show_error)
            fprintf(stderr, "Cannot allocate memory\n");

        if (stop_on_error)
            exit(1);

        return NULL;
    }

    value->type = INTEGER;
    value->as.integer = atoi(str);

    return value;
}

json_value_t *str_to_json_double(char *str, int show_error, int stop_on_error)
{
    json_value_t *value = malloc(sizeof(json_value_t));
    if (!value)
    {
        if (show_error)
            fprintf(stderr, "Cannot allocate memory\n");

        if (stop_on_error)
            exit(1);

        return NULL;
    }

    value->type = DOUBLE;
    value->as.double_value = atof(str);

    return value;
}

json_value_t *str_to_json_str(char *str, int show_error, int stop_on_error)
{
    json_value_t *value = malloc(sizeof(json_value_t));
    if (!value)
    {
        if (show_error)
            fprintf(stderr, "Cannot allocate memory\n");

        if (stop_on_error)
            exit(1);

        return NULL;
    }

    value->type = STRING;
    value->as.str = str ? strdup(str) : NULL;

    return value;
}

json_value_t *str_to_json_bool(char *str, int show_error, int stop_on_error)
{
    json_value_t *value = malloc(sizeof(json_value_t));
    if (!value)
    {
        if (show_error)
            fprintf(stderr, "Cannot allocate memory\n");

        if (stop_on_error)
            exit(1);

        return NULL;
    }

    value->type = BOOLEAN;
    value->as.boolean = strcmp(str, "true") == 0;

    return value;
}

/*
 * Returns json_value_t with ARRAY as type
 * Returns a json_value_t with NULL as type if error
 *
 */
json_value_t *str_to_json_array(char *str_array, int show_error, int stop_on_error)
{
    json_value_t *v = malloc(sizeof(json_value_t));
    if (!v)
    {
        if (show_error)
            fprintf(stderr, "Cannot allocate memory\n");

        if (stop_on_error)
            exit(1);

        return NULL;
    }

    v->type = ARRAY;
    v->as.array.capacity = 256;
    v->as.array.length = 0;
    v->as.array.items = malloc(sizeof(json_value_t) * 256);

    if (!v->as.array.items)
    {
        if (show_error)
            fprintf(stderr, "Failed to parse: %s\n", str_array);

        free(v);

        if (stop_on_error)
            exit(1);

        return NULL;
    }

    // Skip '['
    char *p = str_array + 1;

    size_t buf_capacity = 2048;
    char *buffer = malloc(buf_capacity);
    if (!buffer)
    {
        if (show_error)
            fprintf(stderr, "Cannot allocate memory to buffer\n");

        if (stop_on_error)
            exit(1);

        return v;
    }

    int depth = 0, in_str = 0;
    size_t id = 0;

    while (*p)
    {
        if (id >= buf_capacity)
        {
            while (id >= buf_capacity)
                buf_capacity *= 2;

            char *tmp = realloc(buffer, buf_capacity);
            if (!tmp)
            {
                if (show_error)
                    fprintf(stderr, "Reallocation failed\n");

                free(buffer);

                if (stop_on_error)
                    exit(1);

                return v;
            }
            buffer = tmp;
        }

        if (v->as.array.length == v->as.array.capacity)
        {

            v->as.array.capacity *= 2;
            json_value_t **tmp = realloc(v->as.array.items, sizeof(json_value_t) * v->as.array.capacity);
            if (!tmp)
            {
                if (show_error)
                    fprintf(stderr, "Reallocation failed\n");

                free(v->as.array.items);

                if (stop_on_error)
                    exit(1);

                return v;
            }
            v->as.array.items = tmp;
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

                    trim(buffer, show_error, stop_on_error);

                    if (id > 0)
                    {
                        v->as.array.items[v->as.array.length++] = str_to_json_value(buffer, show_error, stop_on_error);
                    }

                    break;
                }
            }
        }

        if (!in_str && *p == ',' && depth == 0)
        {
            buffer[id] = '\0';

            trim(buffer, show_error, stop_on_error);

            if (id > 0)
            {
                v->as.array.items[v->as.array.length++] = str_to_json_value(buffer, show_error, stop_on_error);
            }

            id = 0;

            p++;
            continue;
        }

        buffer[id++] = *p++;
    }

    free(buffer);
    return v;
}

/*
 * Returns json_value_t with OBJECT as type
 * Returns a json_value_t with NULL as type if error
 *
 */
json_value_t *str_to_json_object(char *str_object, int show_error, int stop_on_error)
{
    json_value_t *v = malloc(sizeof(json_value_t));
    if (!v)
    {
        if (show_error)
            fprintf(stderr, "Cannot allocate memory\n");

        if (stop_on_error)
            exit(1);

        return NULL;
    }

    v->type = OBJECT;
    v->as.object.capacity = 256;
    v->as.object.count = 0;
    v->as.object.entries = malloc(sizeof(json_entry_t) * 256);

    if (!v->as.object.entries)
    {
        if (show_error)
            fprintf(stderr, "Failed to parse: %s\n", str_object);

        free(v);

        if (stop_on_error)
            exit(1);

        return NULL;
    }

    size_t buf_capacity = 2048;
    char *entry_buf = malloc(buf_capacity);
    if (!entry_buf)
    {
        if (show_error)
            fprintf(stderr, "Cannot allocate memory to buffer\n");

        if (stop_on_error)
            exit(1);

        return v;
    }

    int in_str = 0, depth = 0;
    size_t idx = 0;

    // Skip '{'
    char *p = str_object + 1;

    while (*p)
    {
        if (idx >= buf_capacity)
        {
            while (idx >= buf_capacity)
                buf_capacity *= 2;

            char *tmp = realloc(entry_buf, buf_capacity);
            if (!tmp)
            {
                if (show_error)
                    fprintf(stderr, "Reallocation failed\n");

                free(entry_buf);

                if (stop_on_error)
                    exit(1);

                return v;
            }
            entry_buf = tmp;
        }

        json_entry_t *entry = malloc(sizeof(json_entry_t));
        if (!entry)
        {
            if (show_error)
                fprintf(stderr, "Cannot allocate memory to entry\n");

            if (stop_on_error)
                exit(1);

            return v;
        }
        entry->key = NULL;
        entry->value = NULL;

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

                trim(entry_buf, show_error, stop_on_error);

                if (*entry_buf)
                {
                    char *colon = strchr(entry_buf, ':');

                    if (colon)
                    {
                        *colon = '\0';

                        char *key = entry_buf;
                        char *value = colon + 1;

                        trim(key, show_error, stop_on_error);
                        trim(value, show_error, stop_on_error);

                        entry->key = strdup(key);
                        entry->value = str_to_json_value(value, show_error, stop_on_error);

                        if (v->as.object.count == v->as.object.capacity)
                        {
                            v->as.object.capacity *= 2;
                            json_entry_t **entries = realloc(v->as.object.entries, sizeof(json_entry_t) * v->as.object.capacity);
                            if (!v->as.object.entries)
                            {
                                if (show_error)
                                    fprintf(stderr, "Reallocation failed\n");

                                free(v->as.object.entries);

                                if (stop_on_error)
                                    exit(1);

                                return v;
                            }
                            v->as.object.entries = entries;
                        }

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
        free_json_entry(entry);
        p++;
    }

    return v;
}

/* Manipulate struct */

size_t count_elements(json_value_t json)
{
    if (json.type == ARRAY)
    {
        return json.as.array.length;
    }

    if (json.type == OBJECT)
    {
        return json.as.object.count;
    }

    return 0;
}

size_t idxentry(json_value_t json, char *key, int show_error, int stop_on_error)
{
    if (!key)
    {
        if (show_error)
            fprintf(stderr, "Incorrect key");

        if (stop_on_error)
            exit(1);

        return 0;
    }

    if (json.type != OBJECT)
    {
        if (show_error)
            fprintf(stderr, "Target must be a json_value_t with OBJECT as type\n");

        if (stop_on_error)
            exit(1);

        return 0;
    }

    if (count_elements(json) == 0)
    {
        if (show_error)
            fprintf(stderr, "Object is empty\n");

        if (stop_on_error)
            exit(1);

        return 0;
    }

    for (size_t i = 0; i < json.as.object.count; ++i)
    {
        if (strcmp(json.as.object.entries[i]->key, key) == 0)
        {
            return i;
        }
    }

    return 0;
}

/*
 * TODO: trigger exit only when object is fully traversed if key is absent
 */
json_entry_t *getentry(json_value_t json, char *key, int show_error)
{
    if (!key)
    {
        if (show_error)
            fprintf(stderr, "Incorrect key");

        return NULL;
    }

    if (count_elements(json) == 0)
    {
        if (show_error)
            fprintf(stderr, "Object is empty\n");

        return NULL;
    }

    if (json.type == OBJECT)
    {
        for (size_t i = 0; i < json.as.object.count; ++i)
        {
            json_entry_t *entry = json.as.object.entries[i];
            char *k = sub(entry->key, 1, strlen(entry->key) - 1, show_error, 0);
            if (!k)
            {
                if (show_error)
                    fprintf(stderr, "Object is empty\n");

                return NULL;
            }

            if (strcmp(k, key) == 0)
            {
                free(k);
                return entry;
            }
            free(k);

            if (entry->value->type == ARRAY || entry->value->type == OBJECT)
            {
                json_entry_t *res = getentry(*entry->value, key, show_error);
                if (res)
                    return res;
            }
        }
    }
    else if (json.type == ARRAY)
    {
        for (size_t i = 0; i < json.as.array.length; ++i)
        {
            json_value_t *value = json.as.array.items[i];
            if (value->type == ARRAY || value->type == OBJECT)
            {
                json_entry_t *res = getentry(*value, key, show_error);
                if (res)
                    return res;
            }
        }
    }

    if (show_error)
        fprintf(stderr, "key must be within an object\n");

    printf("Unknown key\n");
    return NULL;
}

json_entry_t *entry_at(json_value_t json, size_t index, int show_error, int stop_on_error)
{
    if (index >= json.as.object.count)
    {
        if (show_error)
            fprintf(stderr, "Index %zu out of bounds\n", index);

        if (stop_on_error)
            exit(1);

        return NULL;
    }

    return json.as.object.entries[index];
}

int addentry(json_value_t *dest, json_entry_t *entry, size_t position, int show_error, int stop_on_error)
{
    if (!dest || dest->type != OBJECT)
    {
        if (show_error)
            fprintf(stderr, "Unknown object\n");

        if (stop_on_error)
            exit(1);

        return 0;
    }

    if (!entry)
    {
        if (show_error)
            fprintf(stderr, "Unknown entry\n");

        if (stop_on_error)
            exit(1);

        return 0;
    }

    if (position >= count_elements(*dest))
    {
        if (show_error)
            fprintf(stderr, "Object cannot support new entry\n");

        if (stop_on_error)
            exit(1);

        return 0;
    }

    if (getentry(*dest, entry->key, show_error))
    {
        if (show_error)
            fprintf(stderr, "Cannot add entry with same key\n");

        if (stop_on_error)
            exit(1);

        return 0;
    }

    if (dest->as.object.count == dest->as.object.capacity)
    {
        dest->as.object.capacity *= 2;
        dest->as.object.entries = realloc(dest->as.object.entries, sizeof(json_entry_t) * dest->as.object.capacity);
        if (!dest->as.object.entries)
        {
            if (show_error)
                fprintf(stderr, "An error has occurred\n");

            if (stop_on_error)
                exit(1);

            return 0;
        }
    }

    for (size_t i = dest->as.object.count; i > position; --i)
    {
        dest->as.object.entries[i] = dest->as.object.entries[i - 1];
    }
    dest->as.object.count++;
    dest->as.object.entries[position] = entry;
    return 1;
}

int setentry(json_value_t *dest, char *key, json_value_t *value, int show_error, int stop_on_error)
{
    if (!dest || dest->type != OBJECT)
    {
        if (show_error)
            fprintf(stderr, "Unknown object\n");

        if (stop_on_error)
            exit(1);

        return 0;
    }

    if (!key)
    {
        if (show_error)
            fprintf(stderr, "Incorrect key\n");

        if (stop_on_error)
            exit(1);

        return 0;
    }

    if (!value)
    {
        return 0;
    }

    if (count_elements(*dest) == 0)
    {
        if (show_error)
            fprintf(stderr, "Object is empty\n");

        if (stop_on_error)
            exit(1);

        return 0;
    }

    json_entry_t *entry = getentry(*dest, key, show_error);
    if (!entry)
    {
        return 0;
    }
    entry->value = value;
    return 1;
}

int rementry(json_value_t *dest, char *key, int show_error, int stop_on_error)
{
    if (!dest || dest->type != OBJECT)
    {
        if (show_error)
            fprintf(stderr, "Unknown object\n");

        if (stop_on_error)
            exit(1);

        return 0;
    }

    if (!key)
    {
        if (show_error)
            fprintf(stderr, "Incorrect key");

        if (stop_on_error)
            exit(1);

        return 0;
    }

    if (!getentry(*dest, key, show_error))
    {
        if (show_error)
            fprintf(stderr, "Cannot find entry with key:%s\n", key);

        if (stop_on_error)
            exit(1);

        return 0;
    }

    size_t index = idxentry(*dest, key, show_error, stop_on_error);
    for (size_t i = index; i < dest->as.object.count - 1; ++i)
    {
        dest->as.object.entries[i] = dest->as.object.entries[i + 1];
    }
    dest->as.object.count--;
    return 1;
}

/*
 * TODO: handle depth for nested objects or arrays of objects
 */
char **getkeys(json_value_t json, int show_error, int stop_on_error)
{
    if (count_elements(json) == 0)
    {
        if (show_error)
            fprintf(stderr, "Object is empty\n");

        if (stop_on_error)
            exit(1);

        return NULL;
    }

    size_t length = 256;
    char **keys = malloc(sizeof(char *) * length);
    if (!keys)
    {
        if (show_error)
            fprintf(stderr, "Failed to allocate memory before fetching keys\n");

        if (stop_on_error)
            exit(1);

        return NULL;
    }
    size_t index = 0;

    if (json.type == OBJECT)
    {
        while (index < json.as.object.count)
        {
            json_entry_t *entry = json.as.object.entries[index];
            keys[index++] = entry->key;

            if (index > length)
            {
                length *= 2;
                char **copy = realloc(keys, sizeof(char *) * length);
                if (!copy)
                {
                    if (show_error)
                        fprintf(stderr, "Reallocation failed\n");

                    free(keys);

                    if (stop_on_error)
                        exit(1);

                    return NULL;
                }
                keys = copy;
            }
        }

        return keys;
    }

    if (show_error)
        fprintf(stderr, "No keys found\n");

    if (stop_on_error)
        exit(1);

    return NULL;
}

/*
 * TODO: handle depth for nested objects or arrays of objects
 */
json_value_t **getvalues(json_value_t json, int show_error, int stop_on_error)
{
    if (count_elements(json) == 0)
    {
        if (show_error)
            fprintf(stderr, "Object is empty\n");

        if (stop_on_error)
            exit(1);

        return NULL;
    }

    size_t length = 256;
    json_value_t **values = malloc(sizeof(json_value_t *) * length);
    if (!values)
    {
        if (show_error)
            fprintf(stderr, "Failed to allocate memory before fetching values\n");

        if (stop_on_error)
            exit(1);

        return NULL;
    }
    size_t index = 0;

    if (json.type == OBJECT)
    {
        while (index < json.as.object.count)
        {
            json_entry_t *entry = json.as.object.entries[index];
            values[index++] = entry->value;

            if (index > length)
            {
                length *= 2;
                json_value_t **copy = realloc(values, sizeof(json_value_t *) * length);
                if (!copy)
                {
                    if (show_error)
                        fprintf(stderr, "Reallocation failed\n");

                    free(values);

                    if (stop_on_error)
                        exit(1);

                    return NULL;
                }
                values = copy;
            }
        }

        return values;
    }

    if (show_error)
        fprintf(stderr, "No keys found\n");

    if (stop_on_error)
        exit(1);

    return NULL;
}

/* Traverse struct with iterator */
// json_iterator_t *alloc_iterator(size_t default_position, json_value_t *default_value);
// int has_next(json_iterator_t *iterator);
// void next(json_iterator_t *iterator);
// void reset(json_iterator_t *iterator);
// void dump_iterator(json_iterator_t *iterator);

// json_iterator_t *alloc_iterator(size_t default_position, json_value_t *default_value)
// {
//     json_iterator_t *iterator = malloc(sizeof(json_iterator_t));
//     if (!iterator)
//     {
//         printf("Cannot allocate iterator\n");
//         return NULL;
//     }
//
//     if (default_value && default_value->type != ARRAY && default_value->type != OBJECT)
//     {
//         printf("Default json_value_t type must be ARRAY or OBJECT\n");
//         return NULL;
//     }
//
//     iterator->pos = default_position;
//     iterator->value = default_value;
//     iterator->current.item = NULL;
//     iterator->current.entry = NULL;
//
//     return iterator;
// }

// int has_next(json_iterator_t *iterator)
// {
//     if (!iterator || !iterator->value)
//         return 0;
//
//     if (iterator->value->type == ARRAY)
//     {
//         if (iterator->value->as.array.length == 0 || iterator->pos >= iterator->value->as.array.length)
//             return 0;
//
//         if (iterator->value->as.array.items[iterator->pos])
//             return 1;
//     }
//
//     if (iterator->value->type == OBJECT)
//     {
//         if (iterator->value->as.object.count == 0 || iterator->pos >= iterator->value->as.object.count)
//             return 0;
//
//         if (iterator->value->as.object.entries[iterator->pos])
//             return 1;
//     }
//
//     return 0;
// }

// void next(json_iterator_t *iterator)
// {
//     if (!has_next(iterator))
//         return;
//
//     if (iterator->value->type == ARRAY)
//     {
//         if (iterator->value->as.array.length == 0 || iterator->pos >= iterator->value->as.array.length)
//             return;
//
//         json_value_t *item = iterator->value->as.array.items[iterator->pos];
//         if (item)
//         {
//             iterator->pos++;
//             iterator->current.item = item;
//             return;
//         }
//
//         return;
//     }
//
//     if (iterator->value->type == OBJECT)
//     {
//         if (iterator->value->as.object.count == 0 || iterator->pos >= iterator->value->as.object.count)
//             return;
//
//         json_entry_t *entry = iterator->value->as.object.entries[iterator->pos];
//         if (entry && entry->value)
//         {
//             iterator->pos++;
//             iterator->current.entry = entry;
//             return;
//         }
//     }
//
//     return;
// }

// void reset(json_iterator_t *iterator)
// {
//     if (!iterator || !iterator->value || iterator->pos == 0)
//         return;
//
//     iterator->pos = 0;
//     iterator->value = NULL;
//     iterator->current.item = NULL;
//     iterator->current.entry = NULL;
// }
//
// void dump_iterator(json_iterator_t *iterator)
// {
//     if (!iterator)
//     {
//         printf("No iterator\n");
//         return;
//     }
//
//     printf("[TARGET=");
//     dump_json(iterator->value);
//     printf("]\n");
//
//     printf("[CURRENT_POS=%zu]\n", iterator->pos);
//     printf("[CURRENT_ITEM=");
//     if (iterator->current.item)
//     {
//         dump_json(iterator->current.item);
//     }
//     else
//     {
//         printf("NULL");
//     }
//     printf("]\n");
//
//     printf("[CURRENT_ENTRY=");
//     if (iterator->current.entry)
//     {
//         printf("[key=%s, ", iterator->current.entry->key);
//         printf("value= ");
//         dump_json(iterator->current.entry->value);
//         printf("]");
//     }
//     else
//     {
//         printf("NULL");
//     }
//     printf("]\n");
// }

// size_t count_elements(json_value_t json)
// {
//     json_iterator_t *iterator = alloc_iterator(0, &json);
//     if (!iterator)
//     {
//         printf("Cannot count items\n");
//         return 0;
//     }
//
//     size_t count = 0;
//
//     while (has_next(iterator))
//     {
//         next(iterator);
//         count++;
//     }
//
//     free(iterator);
//
//     return count;
// }
//
// json_entry_t *getentry(json_value_t json, char *key)
// {
//     if (!key)
//     {
//         printf("Incorrect key");
//         return NULL;
//     }
//
//     if (json.type != OBJECT)
//     {
//         printf("Target must be a json_value_t with OBJECT as type\n");
//         return NULL;
//     }
//
//     if (count_elements(json) == 0)
//     {
//         printf("Object is empty\n");
//         return NULL;
//     }
//
//     json_iterator_t *iterator = alloc_iterator(0, &json);
//     if (!iterator)
//     {
//         printf("Cannot iterate over object\n");
//         return 0;
//     }
//
//     while (has_next(iterator))
//     {
//         next(iterator);
//         if (strcmp(iterator->current.entry->key, key) == 0)
//         {
//             return iterator->current.entry;
//         }
//     }
//
//     free(iterator);
//
//     printf("Unknown key\n");
//     return NULL;
// }

// int addentry(json_value_t *dest, json_entry_t *entry, size_t position)
// {
//     if (!dest || dest->type != OBJECT)
//     {
//         printf("Unknown object\n");
//         return 0;
//     }
//
//     if (!entry)
//     {
//         printf("Unknown entry\n");
//         return 0;
//     }
//
//     if (position >= count_elements(*dest))
//     {
//         printf("Object cannot support new entry\n");
//         return 0;
//     }
//
//     if (getentry(*dest, entry->key))
//     {
//         printf("Cannot add entry with same key\n");
//         return 0;
//     }
//
//     json_iterator_t *iterator = alloc_iterator(0, dest);
//     if (!iterator)
//     {
//         printf("Cannot iterate over object\n");
//         return 0;
//     }
//
//     while (has_next(iterator))
//     {
//         next(iterator);
//         if (iterator->pos == position)
//         {
//             for (size_t i = dest->as.object.count; i > iterator->pos; --i)
//             {
//                 dest->as.object.entries[i] = dest->as.object.entries[i - 1];
//             }
//             dest->as.object.count++;
//             dest->as.object.entries[iterator->pos] = entry;
//             return 1;
//         }
//     }
//
//     free(iterator);
//
//     return 0;
// }

// int setentry(json_value_t *dest, char *key, json_value_t *value)
// {
//     if (!dest || dest->type != OBJECT)
//     {
//         printf("Unknown object\n");
//         return 0;
//     }
//
//     if (!key)
//     {
//         printf("Incorrect key");
//         return 0;
//     }
//
//     if (!value)
//     {
//         return 0;
//     }
//
//     if (count_elements(*dest) == 0)
//     {
//         printf("Object is empty\n");
//         return 0;
//     }
//
//     json_entry_t *entry = getentry(*dest, key);
//     if (!entry)
//     {
//         return 0;
//     }
//
//     entry->value = value;
//
//     return 1;
// }

// int rementry(json_value_t *dest, char *key)
// {
//     if (!dest || dest->type != OBJECT)
//     {
//         printf("Unknown object\n");
//         return 0;
//     }
//
//     if (!key)
//     {
//         printf("Incorrect key");
//         return 0;
//     }
//
//     if (!getentry(*dest, key))
//     {
//         printf("Cannot find entry with key:%s\n", key);
//         return 0;
//     }
//
//     json_iterator_t *iterator = alloc_iterator(0, dest);
//     if (!iterator)
//     {
//         printf("Cannot iterate over object\n");
//         return 0;
//     }
//
//     while (has_next(iterator))
//     {
//         next(iterator);
//         if (strcmp(iterator->current.entry->key, key) == 0)
//         {
//             for (size_t i = iterator->pos - 1; i < dest->as.object.count - 1; ++i)
//             {
//                 dest->as.object.entries[i] = dest->as.object.entries[i + 1];
//             }
//             dest->as.object.count--;
//             return 1;
//         }
//     }
//
//     free(iterator);
//
//     return 0;
// }