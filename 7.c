

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#define DESCRIPTION_SIZE 100
#define MAX_LINE 300
#define MAX_TASKS 10000

typedef enum {
    COMMAND_ADD,
    COMMAND_DELETE,
    COMMAND_VIEW,
    COMMAND_UPDATE,
    COMMAND_COMPLETE,
    COMMAND_FIND,
    COMMAND_HELP,
    COMMAND_EXIT,
    COMMAND_UNKNOWN
} CommandType;

typedef enum {
    PENDING,
    DONE
} TaskStatus;

typedef struct {
    int id;
    char description[DESCRIPTION_SIZE];
    TaskStatus status;
} Task;

typedef struct {
    Task *items;
    size_t count;
    size_t capacity;
} TaskList;

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

void make_upper(char *text)
{
    size_t i;

    for (i = 0; text[i] != '\0'; i++) {
        text[i] = (char)toupper((unsigned char)text[i]);
    }
}

CommandType get_command(const char *text)
{
    char command[MAX_LINE];

    if (strlen(text) >= sizeof(command)) {
        return COMMAND_UNKNOWN;
    }

    strcpy(command, text);
    make_upper(command);

    if (strcmp(command, "ADD") == 0) return COMMAND_ADD;
    if (strcmp(command, "DELETE") == 0) return COMMAND_DELETE;
    if (strcmp(command, "VIEW") == 0) return COMMAND_VIEW;
    if (strcmp(command, "UPDATE") == 0) return COMMAND_UPDATE;
    if (strcmp(command, "COMPLETE") == 0) return COMMAND_COMPLETE;
    if (strcmp(command, "FIND") == 0) return COMMAND_FIND;
    if (strcmp(command, "HELP") == 0) return COMMAND_HELP;
    if (strcmp(command, "EXIT") == 0) return COMMAND_EXIT;

    return COMMAND_UNKNOWN;
}

int read_id(int *id)
{
    char text[100];
    char *end;
    long number;
    int status;

    while (1) {
        status = read_line("Enter task ID: ", text, sizeof(text));
        if (status == 0) return 0;
        if (status < 0) {
            printf("ID input is too long.\n");
            continue;
        }

        errno = 0;
        number = strtol(text, &end, 10);
        while (*end == ' ' || *end == '\t') end++;

        if (end == text || *end != '\0' || errno != 0 ||
            number <= 0 || number > INT_MAX) {
            printf("Enter a valid positive task ID.\n");
            continue;
        }

        *id = (int)number;
        return 1;
    }
}

int read_description(char description[DESCRIPTION_SIZE])
{
    char text[MAX_LINE];
    size_t i;
    int status;

    while (1) {
        status = read_line("Enter task description: ", text, sizeof(text));
        if (status == 0) return 0;

        if (status < 0 || strlen(text) >= DESCRIPTION_SIZE) {
            printf("Description must be 1 to 99 characters.\n");
            continue;
        }

        for (i = 0; text[i] != '\0'; i++) {
            if (!isspace((unsigned char)text[i])) break;
        }

        if (text[i] == '\0') {
            printf("Description cannot be empty.\n");
            continue;
        }

        strcpy(description, text);
        return 1;
    }
}

int find_task(const TaskList *list, int id)
{
    size_t i;

    for (i = 0; i < list->count; i++) {
        if (list->items[i].id == id) return (int)i;
    }

    return -1;
}

int grow_tasks(TaskList *list)
{
    size_t new_capacity;
    Task *temp;

    if (list->capacity >= MAX_TASKS) {
        printf("Task limit reached.\n");
        return 0;
    }

    new_capacity = list->capacity * 2;
    if (new_capacity > MAX_TASKS) new_capacity = MAX_TASKS;

    temp = realloc(list->items, new_capacity * sizeof(Task));
    if (temp == NULL) {
        printf("Could not allocate memory for more tasks.\n");
        return 0;
    }

    list->items = temp;
    list->capacity = new_capacity;
    return 1;
}

int add_task(TaskList *list)
{
    Task task;

    if (!read_id(&task.id)) return 0;

    if (find_task(list, task.id) != -1) {
        printf("Task ID already exists.\n");
        return 1;
    }

    if (!read_description(task.description)) return 0;

    task.status = PENDING;

    if (list->count == list->capacity && !grow_tasks(list)) return 1;

    list->items[list->count] = task;
    list->count++;

    printf("Task %d added successfully.\n", task.id);
    return 1;
}

void view_tasks(const TaskList *list)
{
    size_t i;

    if (list->count == 0) {
        printf("Task list is empty.\n");
        return;
    }

    for (i = 0; i < list->count; i++) {
        printf("ID: %d | %s | Status: %s\n",
               list->items[i].id,
               list->items[i].description,
               list->items[i].status == DONE ? "DONE" : "PENDING");
    }
}

int find_and_show(const TaskList *list)
{
    int id;
    int index;

    if (!read_id(&id)) return 0;

    index = find_task(list, id);
    if (index == -1) {
        printf("Task %d was not found.\n", id);
    } else {
        printf("ID: %d | %s | Status: %s\n",
               list->items[index].id,
               list->items[index].description,
               list->items[index].status == DONE ? "DONE" : "PENDING");
    }

    return 1;
}

int delete_task(TaskList *list)
{
    int id;
    int index;
    size_t i;

    if (!read_id(&id)) return 0;

    index = find_task(list, id);
    if (index == -1) {
        printf("Task %d was not found.\n", id);
        return 1;
    }

    for (i = (size_t)index; i + 1 < list->count; i++) {
        list->items[i] = list->items[i + 1];
    }

    list->count--;
    printf("Task %d deleted successfully.\n", id);
    return 1;
}

int update_task(TaskList *list)
{
    int id;
    int index;
    char new_description[DESCRIPTION_SIZE];

    if (!read_id(&id)) return 0;

    index = find_task(list, id);
    if (index == -1) {
        printf("Task %d was not found.\n", id);
        return 1;
    }

    if (!read_description(new_description)) return 0;

    strcpy(list->items[index].description, new_description);
    printf("Task %d updated successfully.\n", id);
    return 1;
}

int complete_task(TaskList *list)
{
    int id;
    int index;

    if (!read_id(&id)) return 0;

    index = find_task(list, id);
    if (index == -1) {
        printf("Task %d was not found.\n", id);
        return 1;
    }

    list->items[index].status = DONE;
    printf("Task %d marked as DONE.\n", id);
    return 1;
}

void show_help(void)
{
    printf("\nADD      - Add a new task\n");
    printf("DELETE   - Delete a task using its ID\n");
    printf("VIEW     - Display all tasks\n");
    printf("UPDATE   - Change a task description\n");
    printf("COMPLETE - Mark a task as done\n");
    printf("FIND     - Search for a task by ID\n");
    printf("HELP     - Show this help message\n");
    printf("EXIT     - Close the program\n");
}

int main(void)
{
    TaskList list = {NULL, 0, 2};
    char input[MAX_LINE];
    CommandType command;
    int running = 1;
    int status;

    list.items = malloc(list.capacity * sizeof(Task));
    if (list.items == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    printf("Command-Driven Task Manager\n");
    printf("Type HELP to see available commands.\n");

    while (running) {
        status = read_line("\nCommand: ", input, sizeof(input));

        if (status == 0) {
            printf("\nInput ended.\n");
            break;
        }
        if (status < 0) {
            printf("Command is too long. Try again.\n");
            continue;
        }

        command = get_command(input);

        switch (command) {
            case COMMAND_ADD:
                running = add_task(&list);
                break;
            case COMMAND_DELETE:
                running = delete_task(&list);
                break;
            case COMMAND_VIEW:
                view_tasks(&list);
                break;
            case COMMAND_UPDATE:
                running = update_task(&list);
                break;
            case COMMAND_COMPLETE:
                running = complete_task(&list);
                break;
            case COMMAND_FIND:
                running = find_and_show(&list);
                break;
            case COMMAND_HELP:
                show_help();
                break;
            case COMMAND_EXIT:
                running = 0;
                break;
            default:
                printf("Unknown command. Type HELP for command list.\n");
                break;
        }
    }

    free(list.items);
    printf("Task manager closed. Memory released.\n");
    return 0;
}
