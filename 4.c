
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>

#define NAME_SIZE 50
#define MAX_STUDENTS 10000

typedef struct {
    int roll_number;
    char name[NAME_SIZE];
    int age;
    float gpa;
} Student;


typedef struct {
    Student *students;
    size_t count;
    size_t capacity;
} StudentList;


int read_line(const char *message, char *text, size_t size)
{
    size_t length;
    int ch;

    printf("%s", message);

    if (fgets(text, (int)size, stdin) == NULL) {
        return 0;
    }

    length = strlen(text);

    if (length > 0 && text[length - 1] == '\n') {
        text[length - 1] = '\0';
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


int get_integer(const char *message, int *value)
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
            printf("Input is too long. Try again.\n");
            continue;
        }

        errno = 0;
        number = strtol(text, &end, 10);

        while (*end == ' ' || *end == '\t') {
            end++;
        }

        if (end == text || *end != '\0' || errno == ERANGE ||
            number < INT_MIN || number > INT_MAX) {
            printf("Please enter a valid whole number.\n");
            continue;
        }

        *value = (int)number;
        return 1;
    }
}


int get_gpa(const char *message, float *gpa)
{
    char text[100];
    char *end;
    float number;
    int status;

    while (1) {
        status = read_line(message, text, sizeof(text));

        if (status == 0) {
            return 0;
        }

        if (status < 0) {
            printf("Input is too long. Try again.\n");
            continue;
        }

        errno = 0;
        number = strtof(text, &end);

        while (*end == ' ' || *end == '\t') {
            end++;
        }

        if (end == text || *end != '\0' || errno == ERANGE ||
            !isfinite(number) || number < 0.0f || number > 4.0f) {
            printf("Enter a GPA between 0.00 and 4.00.\n");
            continue;
        }

        *gpa = number;
        return 1;
    }
}

int get_name(char name[NAME_SIZE])
{
    char text[200];
    size_t i;
    int status;

    while (1) {
        status = read_line("Enter name: ", text, sizeof(text));

        if (status == 0) {
            return 0;
        }

        if (status < 0 || strlen(text) >= NAME_SIZE) {
            printf("Name must be between 1 and 49 characters.\n");
            continue;
        }

        for (i = 0; text[i] != '\0'; i++) {
            if (text[i] != ' ' && text[i] != '\t') {
                break;
            }
        }

        if (text[i] == '\0') {
            printf("Name cannot be empty.\n");
            continue;
        }

        strcpy(name, text);
        return 1;
    }
}

// Search student using roll number
int find_student(const StudentList *list, int roll_number)
{
    size_t i;

    for (i = 0; i < list->count; i++) {
        if (list->students[i].roll_number == roll_number) {
            return (int)i;
        }
    }

    return -1;
}


int grow_list(StudentList *list)
{
    size_t new_capacity;
    Student *temp;

    if (list->capacity >= MAX_STUDENTS) {
        printf("Maximum number of students reached.\n");
        return 0;
    }

    new_capacity = list->capacity * 2;

    if (new_capacity > MAX_STUDENTS) {
        new_capacity = MAX_STUDENTS;
    }

    if (new_capacity > SIZE_MAX / sizeof(Student)) {
        printf("Requested size is too large.\n");
        return 0;
    }

    temp = realloc(list->students,
                   new_capacity * sizeof(Student));

    if (temp == NULL) {
        printf("Could not allocate more memory.\n");
        return 0;
    }

    list->students = temp;
    list->capacity = new_capacity;

    printf("List capacity increased to %zu.\n", list->capacity);

    return 1;
}


void display_student(const Student *student)
{
    printf("Roll: %d | Name: %s | Age: %d | GPA: %.2f\n",
           student->roll_number,
           student->name,
           student->age,
           student->gpa);
}


int add_student(StudentList *list)
{
    Student student;

    if (!get_integer("Enter positive roll number: ",
                     &student.roll_number)) {
        return 0;
    }

    if (student.roll_number <= 0) {
        printf("Roll number must be positive.\n");
        return 1;
    }

    if (find_student(list, student.roll_number) != -1) {
        printf("This roll number already exists.\n");
        return 1;
    }

    if (!get_name(student.name)) {
        return 0;
    }

    while (1) {
        if (!get_integer("Enter age (16-100): ", &student.age)) {
            return 0;
        }

        if (student.age >= 16 && student.age <= 100) {
            break;
        }

        printf("Age must be between 16 and 100.\n");
    }

    if (!get_gpa("Enter GPA (0.00-4.00): ", &student.gpa)) {
        return 0;
    }

    if (list->count == list->capacity && !grow_list(list)) {
        return 1;
    }

    list->students[list->count] = student;
    list->count++;

    printf("Student added successfully.\n");

    return 1;
}


void display_all(const StudentList *list)
{
    size_t i;

    if (list->count == 0) {
        printf("No student records found.\n");
        return;
    }

    printf("\n--- Student Records ---\n");

    for (i = 0; i < list->count; i++) {
        display_student(&list->students[i]);
    }
}


int search_student(const StudentList *list)
{
    int roll_number;
    int index;

    if (!get_integer("Enter roll number to search: ", &roll_number)) {
        return 0;
    }

    index = find_student(list, roll_number);

    if (index == -1) {
        printf("Student not found.\n");
    } else {
        display_student(&list->students[index]);
    }

    return 1;
}


int update_student(StudentList *list)
{
    int roll_number;
    int index;
    Student updated;

    if (!get_integer("Enter roll number to update: ", &roll_number)) {
        return 0;
    }

    index = find_student(list, roll_number);

    if (index == -1) {
        printf("Student not found.\n");
        return 1;
    }

    updated = list->students[index];

    if (!get_name(updated.name)) {
        return 0;
    }

    while (1) {
        if (!get_integer("Enter new age (16-100): ", &updated.age)) {
            return 0;
        }

        if (updated.age >= 16 && updated.age <= 100) {
            break;
        }

        printf("Age must be between 16 and 100.\n");
    }

    if (!get_gpa("Enter new GPA (0.00-4.00): ", &updated.gpa)) {
        return 0;
    }

    list->students[index] = updated;

    printf("Student record updated.\n");

    return 1;
}


int delete_student(StudentList *list)
{
    int roll_number;
    int index;
    size_t i;

    if (!get_integer("Enter roll number to delete: ", &roll_number)) {
        return 0;
    }

    index = find_student(list, roll_number);

    if (index == -1) {
        printf("Student not found.\n");
        return 1;
    }

    for (i = (size_t)index; i + 1 < list->count; i++) {
        list->students[i] = list->students[i + 1];
    }

    list->count--;

    printf("Student deleted.\n");

    return 1;
}


void show_gpa_summary(const StudentList *list)
{
    size_t i;
    size_t highest = 0;
    size_t lowest = 0;
    double total = 0.0;

    if (list->count == 0) {
        printf("No records available for GPA summary.\n");
        return;
    }

    for (i = 0; i < list->count; i++) {
        total += list->students[i].gpa;

        if (list->students[i].gpa >
            list->students[highest].gpa) {
            highest = i;
        }

        if (list->students[i].gpa <
            list->students[lowest].gpa) {
            lowest = i;
        }
    }

    printf("Highest GPA: %.2f (%s)\n",
           list->students[highest].gpa,
           list->students[highest].name);

    printf("Lowest GPA: %.2f (%s)\n",
           list->students[lowest].gpa,
           list->students[lowest].name);

    printf("Average GPA: %.2f\n",
           total / (double)list->count);
}


int compare_gpa(const void *a, const void *b)
{
    const Student *first = (const Student *)a;
    const Student *second = (const Student *)b;

    if (first->gpa < second->gpa) {
        return 1;
    }

    if (first->gpa > second->gpa) {
        return -1;
    }

    if (first->roll_number < second->roll_number) {
        return -1;
    }

    if (first->roll_number > second->roll_number) {
        return 1;
    }

    return 0;
}


void show_sorted_students(const StudentList *list)
{
    Student *copy;
    size_t i;

    if (list->count == 0) {
        printf("No records available for sorting.\n");
        return;
    }

    copy = malloc(list->count * sizeof(Student));

    if (copy == NULL) {
        printf("Could not create a sorted list.\n");
        return;
    }

    memcpy(copy, list->students,
           list->count * sizeof(Student));

    qsort(copy, list->count, sizeof(Student), compare_gpa);

    printf("\n--- Students Sorted by GPA ---\n");

    for (i = 0; i < list->count; i++) {
        display_student(&copy[i]);
    }

    free(copy);
}


int main(void)
{
    StudentList list;
    int initial_capacity;
    int choice;
    int running = 1;

    list.students = NULL;
    list.count = 0;
    list.capacity = 0;

    printf("Student Records and Academic Analytics\n");

    while (1) {
        if (!get_integer("Enter initial capacity (1-10000): ",
                         &initial_capacity)) {
            return 0;
        }

        if (initial_capacity >= 1 &&
            initial_capacity <= MAX_STUDENTS) {
            break;
        }

        printf("Capacity must be between 1 and %d.\n",
               MAX_STUDENTS);
    }

    list.students = malloc((size_t)initial_capacity * sizeof(Student));

    if (list.students == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    list.capacity = (size_t)initial_capacity;

    while (running) {
        printf("\n1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. GPA Summary\n");
        printf("7. Sort by GPA\n");
        printf("8. Exit\n");

        if (!get_integer("Enter your choice: ", &choice)) {
            break;
        }

        switch (choice) {
            case 1:
                running = add_student(&list);
                break;

            case 2:
                display_all(&list);
                break;

            case 3:
                running = search_student(&list);
                break;

            case 4:
                running = update_student(&list);
                break;

            case 5:
                running = delete_student(&list);
                break;

            case 6:
                show_gpa_summary(&list);
                break;

            case 7:
                show_sorted_students(&list);
                break;

            case 8:
                running = 0;
                break;

            default:
                printf("Invalid choice. Please select 1-8.\n");
        }
    }

    free(list.students);

    printf("Program closed. Memory released.\n");

    return 0;
}
