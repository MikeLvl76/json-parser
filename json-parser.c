#include <stdio.h>
#include <stdlib.h>

#define READ_FILE_PATH "test_parse.json"
#define MAX_BUFFER_SIZE 256

int main()
{
    FILE *file = fopen(READ_FILE_PATH, "r");

    if (!file)
    {
        printf("Error: cannot open file at path %s\n", READ_FILE_PATH);
        exit(1);
    }

    char *buffer = (char *)malloc(MAX_BUFFER_SIZE);

    if (!buffer)
    {
        printf("Error: cannot allocate memory\n");
        exit(1);
    }

    while (!feof(file))
    {
        if (ferror(file))
        {
            fprintf(stderr, "Error while reading file: code %d\n", errno);
            exit(1);
        }

        fgets(buffer, MAX_BUFFER_SIZE, file);

        printf("%s", buffer);
    }

    return 0;
}