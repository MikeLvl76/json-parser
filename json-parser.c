#include "json_parser.h"
#include <time.h>

#define MAX_FILES_ALLOWED 16

void print_cmd()
{
    printf("Available commands:\n\n");

    printf("  help\n");
    printf("   --help                       Show general help\n");
    printf("    -h\n\n");

    printf("  default\n");
    printf("   --default                    Use default json\n");
    printf("    -d\n\n");

    printf("  files\n");
    printf("   --files <path>               Use one or multiple JSON files provided by path\n");
    printf("    -f     <path>\n\n");

    printf("  tree\n");
    printf("   --tree                       Print JSON in tree format\n");
    printf("    -t\n\n");

    printf("  with values\n");
    printf("   --with-values                Show values for tree representation\n");
    printf("    -wv\n\n");

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

    printf("  find key\n");
    printf("   --find-key <key>             Retrieve entry by its key\n");
    printf("    -fk       <key>\n\n");
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

    int show_default = 0, show_pretty = 0, show_error = 0, stop_on_error = 0,
        as_tree = 0, tree_with_values = 0, find_by_key = 0;

    char *key;

    char **paths = malloc(sizeof(char *) * MAX_FILES_ALLOWED);
    if (!paths)
    {
        fprintf(stderr, "An error has occurred\n");
        exit(1);
    }
    char **p = paths;
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

        if (strcmp(argv[i], "--files") == 0 || strcmp(argv[i], "-f") == 0)
        {
            int k = i + 1;
            while (k <= argc - 1 && !starts_with(argv[k], '-', 1, 1))
            {
                if (check_extension(argv[k], 1, 0))
                {
                    *p++ = strdup(argv[k]);
                }
                k++;
            }
            *p = NULL;
        }

        if (strcmp(argv[i], "--tree") == 0 || strcmp(argv[i], "-t") == 0)
        {
            as_tree = 1;
            if (i + 1 <= argc - 1 && (strcmp(argv[i + 1], "--with-values") == 0 || strcmp(argv[i + 1], "-wv") == 0))
            {
                tree_with_values = 1;
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
            if (i + 1 <= argc - 1 && isint(argv[i + 1], 1, 1))
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

        if (strcmp(argv[i], "--find-key") == 0 || strcmp(argv[i], "-fk") == 0)
        {
            if (i + 1 <= argc - 1)
            {
                key = strdup(argv[i + 1]);
                find_by_key = 1;
            }
            else
            {
                fprintf(stderr, "Incorrect key: %s\n", argv[i + 1]);
                exit(1);
            }
        }
    }

    if (show_default)
    {
        printf("### USING DEFAULT FILE ### \n");
        srand((unsigned int)time(NULL));
        char *default_paths[3] = {"default_dataset.json", "medium_dataset.json", "big_dataset.json"};
        for (size_t i = 0; i < 3; ++i)
        {
            if (!check_extension(default_paths[i], 1, 1))
            {
                fprintf(stderr, "Expected \".json\" extension, got: %s\n", default_paths[i]);
                exit(1);
            }
        }
        int id = rand() % 3;
        *p++ = default_paths[id];
    }

    size_t count = 0;
    while (*paths)
    {
        printf("Reading %s\n", *paths);
        printf(show_pretty ? "Pretty display: \u2714 enabled\n" : "Pretty display: \u2716 disabled\n");
        printf("Current indent: %d\n", indent);
        printf(show_error ? "Error handling: \u2714 enabled\n" : "Error handling: \u2716 disabled\n");
        printf(stop_on_error ? "Exiting on error: \u2714 enabled\n\n" : "Exiting on error: \u2716 disabled\n\n");

        JsonValue *json = read_json(*paths, show_error, stop_on_error);

        if (as_tree)
        {
            tree(json, tree_with_values);
            printf("\n");
        }
        else
        {
            dump_json(json, indent, show_pretty);
            printf("\n\n");
        }

        if (find_by_key)
        {
            if (key)
            {
                printf("Searching for \"%s\" key...\n", key);
                JsonEntry *entry = getentry(*json, key, show_error, stop_on_error);
                if (!entry)
                {
                    fprintf(stderr, "Cannot retrieve entry by key: %s\n", key);
                    exit(1);
                }

                printf("%s: ", entry->key);
                dump_json(entry->value, indent, show_pretty);
                printf("\n");
            }
            else
            {
                printf("WARNING: Could not find key\n");
            }
        }

        free(json);
        count++;
        paths++;
    }
    for (size_t i = 0; i < count; ++i)
    {
        free(paths[i]);
    }
    free(paths);
    exit(0);

    return 0;
}