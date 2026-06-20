#include "json_parser.h"
#include <time.h>

int main(int argc, char **argv)
{
    // if (argc == 1)
    // {
    //     printf("Error: command not found\n");
    //     return 1;
    // }

    // if (strcmp(argv[1], "--default") == 0 || strcmp(argv[1], "-d") == 0)
    // {
    //     printf("Reading default JSON file\n\n\n");
    //     JsonValue *json = read_json(DEFAULT_FILEPATH);

    //     dump_json(json, 0);
    //     printf("\n");

    //     free(json);
    //     return 0;
    // }

    srand(time(NULL));
    char *paths[3] = {"default_dataset.json", "medium_dataset.json", "big_dataset.json"};
    int id = rand() % 3;

    printf("Read %s file\n", paths[id]);
    JsonValue *json = read_json(paths[id]);

    dump_json(json, 0);
    printf("\n");
    printf("Counted %zu item(s)\n", count_elements(*json));

    free(json);

    return 0;
}