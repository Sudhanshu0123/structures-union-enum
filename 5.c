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

           list->items[smallest].area, list->items[smallest].id);
    printf("Total area: %.2f\n", total);
    printf("Average area: %.2f\n", total / (double)list->count);
}

int compare_area(const void *a, const void *b)
{
    const Rectangle *first = a;
    const Rectangle *second = b;

    if (first->area < second->area) {
        return -1;
    }
    if (first->area > second->area) {
        return 1;
    }
    if (first->id < second->id) {
        return -1;
    }
    if (first->id > second->id) {
        return 1;
    }
    return 0;
}

void show_sorted(const RectangleList *list)
{
    Rectangle *copy;
    size_t i;

    if (list->count == 0) {
        printf("No rectangles to sort.\n");
        return;
    }

    copy = malloc(list->count * sizeof(Rectangle));
    if (copy == NULL) {
        printf("Could not create sorted copy.\n");
        return;
    }

    memcpy(copy, list->items, list->count * sizeof(Rectangle));
    qsort(copy, list->count, sizeof(Rectangle), compare_area);

    printf("Rectangles sorted by area (ascending):\n");
    for (i = 0; i < list->count; i++) {
        display_rectangle(&copy[i]);
    }

    free(copy);
}

int filter_by_area(const RectangleList *list)
{
    double threshold;
    size_t i;
    int found = 0;

    if (!read_threshold(&threshold)) {
        return 0;
    }

    for (i = 0; i < list->count; i++) {
        if (list->items[i].area > threshold) {
            display_rectangle(&list->items[i]);
            found = 1;
        }
    }

    if (!found) {
        printf("No rectangle has area greater than %.2f.\n", threshold);
    }

    return 1;
}

int main(void)
{
    RectangleList list = {NULL, 0, 0};
    int initial_count;
    int choice;
    int running = 1;

    printf("Rectangle Inventory and Geometry Analysis\n");

    while (1) {
        if (!read_int("Enter initial capacity (1-1000): ", &initial_count)) {
            return 0;
        }
        if (initial_count <= MAX_RECTANGLES) {
            break;
        }
        printf("Capacity cannot exceed %d.\n", MAX_RECTANGLES);
    }

    list.items = malloc((size_t)initial_count * sizeof(Rectangle));
    if (list.items == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }
    list.capacity = (size_t)initial_count;

    while (running) {
        printf("\n1. Add rectangle\n2. Display all\n3. Search by ID\n");
        printf("4. Update rectangle\n5. Delete rectangle\n");
        printf("6. Area statistics\n7. Sort by area\n8. Filter by area\n");
        printf("9. Exit\n");

        if (!read_int("Choose: ", &choice)) {
            break;
        }

        switch (choice) {
            case 1: running = add_rectangle(&list); break;
            case 2: display_all(&list); break;
            case 3: running = search_rectangle(&list); break;
            case 4: running = update_rectangle(&list); break;
            case 5: running = delete_rectangle(&list); break;
            case 6: show_statistics(&list); break;
            case 7: show_sorted(&list); break;
            case 8: running = filter_by_area(&list); break;
            case 9: running = 0; break;
            default: printf("Invalid choice.\n"); break;
        }
    }

    free(list.items);
    printf("Program ended. Memory released.\n");
}
