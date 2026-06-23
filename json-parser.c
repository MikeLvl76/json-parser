#include "json_parser.h"
#include <time.h>

void print_cmd()
{
    printf("Available commands:\n\n");

    printf("  help\n");
    printf("   --help                       Show general help\n");
    printf("    -h\n\n");

    printf("  default\n");
    printf("   --default                    Use default json\n");
    printf("    -d\n\n");

    printf("  file\n");
    printf("   --file <path>                Use a JSON file provided by path\n");
    printf("    -f    <path>\n\n");

    printf("  pretty\n");
    printf("   --pretty                     Print JSON with correct indents, linebreaks and spaces\n");
    printf("    -p\n\n");

    printf("  show error\n");
    printf("   --show-error                 Show errors when parsing JSON\n");
    printf("    -se\n\n");

    printf("  stop on error\n");
    printf("   --stop-on-error              Stop parsing JSON at first encountered error\n");
    printf("    -soe\n\n");

    printf("  indent\n");
    printf("   --indent <value>             Change default indent value when printing JSON content\n");
    printf("    -i      <value>\n\n");
}

void print_help()
{
    fprintf(stderr, "TODO: implement \"print_help\" function");
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Error: no command provided\n");
        print_cmd();
        return 1;
    }

    int show_default = 0, show_pretty = 0, show_error = 0, stop_on_error = 0;
    char filepath[256];
    int indent = 0;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
        {
            print_help();
            exit(1);
        }

        if (strcmp(argv[i], "--default") == 0 || strcmp(argv[i], "-d") == 0)
            show_default = 1;

        if (strcmp(argv[i], "--file") == 0 || strcmp(argv[i], "-f") == 0)
        {
            if (i + 1 < argc - 1 && check_extension(argv[i + 1], 1, 1))
            {
                strcpy(filepath, argv[i + 1]);
            }
            else
            {
                fprintf(stderr, "Cannot use file at path: %s\n", argv[i + 1]);
                exit(1);
            }
        }

        if (strcmp(argv[i], "--pretty") == 0 || strcmp(argv[i], "-p") == 0)
            show_pretty = 1;

        if (strcmp(argv[i], "--show-error") == 0 || strcmp(argv[i], "-se") == 0)
            show_error = 1;

        if (strcmp(argv[i], "--stop-on-error") == 0 || strcmp(argv[i], "-soe") == 0)
            stop_on_error = 1;

        if (strcmp(argv[i], "--indent") == 0 || strcmp(argv[i], "-i") == 0)
        {
            if (i + 1 < argc - 1 && isint(argv[i + 1], 1, 1))
            {
                int value = atoi(argv[i + 1]);
                if (value <= 0)
                {
                    printf("WARNING: setting indent with negative value will result on setting it to 0\n");
                    value = 0;
                }
                else
                {
                    indent = value;
                }
            }
            else
            {
                fprintf(stderr, "Incorrect indent value: %s\n", argv[i + 1]);
                exit(1);
            }
        }
    }

    if (show_default)
    {
        printf("### USING DEFAULT FILE ### \n");
        srand((unsigned int)time(NULL));
        char *paths[3] = {"default_dataset.json", "medium_dataset.json", "big_dataset.json"};
        for (size_t i = 0; i < 3; ++i)
        {
            if (!check_extension(paths[i], 1, 1))
            {
                fprintf(stderr, "Expected \".json\" extension, got: %s\n", paths[i]);
                exit(1);
            }
        }
        int id = rand() % 3;
        strcpy(filepath, paths[id]);
    }

    printf("Reading %s\n", filepath);
    printf("Current indent: %d\n", indent);
    printf(show_pretty ? "Pretty display enabled\n" : "Pretty display disabled\n");
    printf(show_error ? "Error handling enabled\n" : "Error handling disabled\n");
    printf(stop_on_error ? "Exiting on error enabled\n\n" : "Exiting on error disabled\n\n");

    JsonValue *json = read_json(filepath, show_error, stop_on_error);
    dump_json(json, indent, show_pretty);
    printf("\n\n");

    printf("Counted %zu main item(s)\n", count_elements(*json));

    free(json);
    exit(0);

    return 0;
}