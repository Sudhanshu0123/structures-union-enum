
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#define START_SIZE 5

struct Rectangle {
    int id;
    double width;
    double height;
    double area;
    double perimeter;
};

struct Rectangle *rectangles = NULL;
int count = 0;
int capacity = 0;

/* Read a line from the user */
int readLine(char text[], int size)
{
    if (fgets(text, size, stdin) == NULL)
        return 0;

    if (strchr(text, '\n') == NULL) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
    }

    text[strcspn(text, "\n")] = '\0';
    return 1;
}

/* Read a positive integer */
int readInt(char message[], int *value)
{
    char text[100];
    char *end;
    long num;

    while (1) {
        printf("%s", message);

        if (!readLine(text, sizeof(text)))
            return 0;

        errno = 0;
        num = strtol(text, &end, 10);

        while (isspace((unsigned char)*end))
            end++;

        if (end == text || *end != '\0' ||
            errno == ERANGE || num <= 0 || num > INT_MAX) {
            printf("Please enter a valid positive integer.\n");
        } else {
            *value = (int)num;
            return 1;
        }
    }
}

/* Read a positive decimal number */
int readDouble(char message[], double *value)
{
    char text[100];
    char *end;
    double num;

    while (1) {
        printf("%s", message);

        if (!readLine(text, sizeof(text)))
            return 0;

        errno = 0;
        num = strtod(text, &end);

        while (isspace((unsigned char)*end))
            end++;

        if (end == text || *end != '\0' ||
            errno == ERANGE || num != num ||
            num <= 0 || num > 1e100) {
            printf("Please enter a valid positive number.\n");
        } else {
            *value = num;
            return 1;
        }
    }
}

/* Calculate area and perimeter */
void calculate(struct Rectangle *r)
{
    r->area = r->width * r->height;
    r->perimeter = 2 * (r->width + r->height);
}

/* Find rectangle using ID */
int findRectangle(int id)
{
    int i;

    for (i = 0; i < count; i++) {
        if (rectangles[i].id == id)
            return i;
    }

    return -1;
}

/* Increase array size when needed */
int increaseCapacity(void)
{
    int newCapacity;
    struct Rectangle *temp;

    if (capacity == 0)
        newCapacity = START_SIZE;
    else
        newCapacity = capacity * 2;

    temp = realloc(rectangles,
                   newCapacity * sizeof(struct Rectangle));

    if (temp == NULL) {
        printf("Memory allocation failed.\n");
        return 0;
    }

    rectangles = temp;
    capacity = newCapacity;

    return 1;
}

/* Check whether rectangle is a square */
int isSquare(struct Rectangle r)
{
    double difference;

    if (r.width > r.height)
        difference = r.width - r.height;
    else
        difference = r.height - r.width;

    return difference <= 0.0001 *
           (r.width > r.height ? r.width : r.height);
}

/* Display one rectangle */
void displayRectangle(struct Rectangle r)
{
    printf("\nID: %d", r.id);
    printf("\nWidth: %.2f", r.width);
    printf("\nHeight: %.2f", r.height);
    printf("\nArea: %.2f", r.area);
    printf("\nPerimeter: %.2f", r.perimeter);

    if (isSquare(r))
        printf("\nType: Square\n");
    else
        printf("\nType: Rectangle\n");
}

/* Add a rectangle */
int addRectangle(void)
{
    struct Rectangle r;

    if (!readInt("Enter rectangle ID: ", &r.id))
        return 0;

    if (findRectangle(r.id) != -1) {
        printf("This ID already exists.\n");
        return 1;
    }

    if (!readDouble("Enter width: ", &r.width))
        return 0;

    if (!readDouble("Enter height: ", &r.height))
        return 0;

    calculate(&r);

    if (count == capacity) {
        if (!increaseCapacity())
            return 1;
    }

    rectangles[count] = r;
    count++;

    printf("Rectangle added successfully!\n");
    return 1;
}

/* Display all rectangles */
void displayAll(void)
{
    int i;

    if (count == 0) {
        printf("No rectangles available.\n");
        return;
    }

    for (i = 0; i < count; i++) {
        printf("\n--------------------");
        displayRectangle(rectangles[i]);
    }
}

/* Search for a rectangle */
int searchRectangle(void)
{
    int id, index;

    if (!readInt("Enter ID to search: ", &id))
        return 0;

    index = findRectangle(id);

    if (index == -1)
        printf("Rectangle not found.\n");
    else
        displayRectangle(rectangles[index]);

    return 1;
}

/* Update rectangle details */
int updateRectangle(void)
{
    int id, index;
    double width, height;

    if (!readInt("Enter ID to update: ", &id))
        return 0;

    index = findRectangle(id);

    if (index == -1) {
        printf("Rectangle not found.\n");
        return 1;
    }

    if (!readDouble("Enter new width: ", &width))
        return 0;

    if (!readDouble("Enter new height: ", &height))
        return 0;

    rectangles[index].width = width;
    rectangles[index].height = height;

    calculate(&rectangles[index]);

    printf("Rectangle updated successfully!\n");
    return 1;
}

/* Delete a rectangle */
int deleteRectangle(void)
{
    int id, index, i;

    if (!readInt("Enter ID to delete: ", &id))
        return 0;

    index = findRectangle(id);

    if (index == -1) {
        printf("Rectangle not found.\n");
        return 1;
    }

    for (i = index; i < count - 1; i++) {
        rectangles[i] = rectangles[i + 1];
    }

    count--;

    printf("Rectangle deleted successfully!\n");
    return 1;
}

/* Show area statistics */
void showStatistics(void)
{
    int i;
    int largest = 0, smallest = 0;
    double total = 0;

    if (count == 0) {
        printf("No data available.\n");
        return;
    }

    for (i = 0; i < count; i++) {
        total += rectangles[i].area;

        if (rectangles[i].area > rectangles[largest].area)
            largest = i;

        if (rectangles[i].area < rectangles[smallest].area)
            smallest = i;
    }

    printf("\nLargest area: %.2f", rectangles[largest].area);
    printf("\nSmallest area: %.2f", rectangles[smallest].area);
    printf("\nTotal area: %.2f", total);
    printf("\nAverage area: %.2f\n", total / count);
}

/* Sort rectangles by area */
void sortRectangles(void)
{
    int i, j;
    struct Rectangle temp;

    if (count == 0) {
        printf("No rectangles available.\n");
        return;
    }

    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (rectangles[j].area > rectangles[j + 1].area) {
                temp = rectangles[j];
                rectangles[j] = rectangles[j + 1];
                rectangles[j + 1] = temp;
            }
        }
    }

    printf("Rectangles sorted by area (ascending).\n");
    displayAll();
}

/* Filter rectangles by minimum area */
int filterRectangles(void)
{
    double minimum;
    int i, found = 0;

    if (!readDouble("Enter minimum area: ", &minimum))
        return 0;

    for (i = 0; i < count; i++) {
        if (rectangles[i].area >= minimum) {
            displayRectangle(rectangles[i]);
            found = 1;
        }
    }

    if (!found)
        printf("No rectangles found with that area.\n");

    return 1;
}

/* Main function */
int main(void)
{
    int choice;
    int running = 1;

    printf("=================================\n");
    printf("   RECTANGLE INVENTORY SYSTEM\n");
    printf("=================================\n");

    while (running) {
        printf("\n1. Add Rectangle");
        printf("\n2. Display All Rectangles");
        printf("\n3. Search Rectangle");
        printf("\n4. Update Rectangle");
        printf("\n5. Delete Rectangle");
        printf("\n6. Area Statistics");
        printf("\n7. Sort by Area");
        printf("\n8. Filter by Area");
        printf("\n9. Exit\n");

        if (!readInt("\nEnter your choice: ", &choice))
            break;

        switch (choice) {
            case 1:
                running = addRectangle();
                break;

            case 2:
                displayAll();
                break;

            case 3:
                running = searchRectangle();
                break;

            case 4:
                running = updateRectangle();
                break;

            case 5:
                running = deleteRectangle();
                break;

            case 6:
                showStatistics();
                break;

            case 7:
                sortRectangles();
                break;

            case 8:
                running = filterRectangles();
                break;

            case 9:
                running = 0;
                break;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    free(rectangles);
    rectangles = NULL;

    printf("\nProgram ended. Thank you!\n");

    return 0;
}
