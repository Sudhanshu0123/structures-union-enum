#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <stdint.h>

#define MAX_RECTANGLES 1000
#define TOLERANCE 0.0001

typedef struct {
    int id;
    double width;
    double height;
    double area;
    double perimeter;
} Rectangle;

typedef struct {
    Rectangle *items;
    size_t count;
    size_t capacity;
} RectangleList;

int read_line(const char *message, char *text, size_t size)
{
    size_t len;
    int ch;

    printf("%s", message);

    if (fgets(text, (int)size, stdin) == NULL) {
        return 0;
    }

    len = strlen(text);

    if (len > 0 && text[len - 1] == '\n') {
        text[len - 1] = '\0';
        return 1;
    }

    if (feof(stdin)) {
        return 1;
    }

    ch = getchar();
    if (ch == '\n' || ch == EOF) {
        return 1;
    }

    while (ch != '\n' && ch != EOF) {
        ch = getchar();
    }

    return -1;
}

int read_int(const char *message, int *value)
{
    char text[100];
    char *end;
    long number;
    int status;

    while (1) {
        status = read_line(message, text, sizeof(text));
        if (status == 0) {
            return 0;
        }
        if (status < 0) {
            printf("Input is too long.\n");
            continue;
        }

        errno = 0;
        number = strtol(text, &end, 10);
        while (*end == ' ' || *end == '\t') {
            end++;
        }

        if (end == text || *end != '\0' || errno != 0 ||
            number < 1 || number > 2147483647L) {
            printf("Enter a positive whole number.\n");
            continue;
        }
