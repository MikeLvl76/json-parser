#include "json_parser.h"

int main()
{
    JsonValue *json = read_json(READ_TEST_FILE_PATH);

    dump_json(json, 0);
    printf("\n");

    free(json);
    return 0;
}