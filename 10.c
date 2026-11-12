/*
 * Safe Heterogeneous Data Container Library
 * A container stores one active value: int, float, char or string.
 * Strings are copied into separately allocated memory.
 * Includes type-safe getters, cleanup, printing and deep copying.
 * Compile: gcc -std=c11 -Wall -Wextra -Wpedantic 10.c -o container
 * Tests: zero, wrong getter, empty string, repeated clear, self-copy,
 * string replacement, and independent deep copy.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TYPE_EMPTY,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING
} DataType;

typedef union {
    int integer;
    float decimal;
    char character;
    char *string;
} Data;

typedef struct {
    Data data;
    DataType type;
} Container;

void container_init(Container *c)
{
    if (c == NULL) return;

    c->type = TYPE_EMPTY;
    c->data.string = NULL;
}

void container_clear(Container *c)
{
    if (c == NULL) return;

    if (c->type == TYPE_STRING) {
        free(c->data.string);
    }

    c->data.string = NULL;
    c->type = TYPE_EMPTY;
}

int container_set_int(Container *c, int value)
{
    if (c == NULL) return 0;

    container_clear(c);
    c->data.integer = value;
    c->type = TYPE_INT;
    return 1;
}

int container_set_float(Container *c, float value)
{
    if (c == NULL) return 0;

    container_clear(c);
    c->data.decimal = value;
    c->type = TYPE_FLOAT;
    return 1;
}

int container_set_char(Container *c, char value)
{
    if (c == NULL) return 0;

    container_clear(c);
    c->data.character = value;
    c->type = TYPE_CHAR;
    return 1;
}

int container_set_string(Container *c, const char *value)
{
    char *copy;
    size_t length;

    if (c == NULL || value == NULL) return 0;

    length = strlen(value);
    copy = malloc(length + 1);

    if (copy == NULL) {
        return 0;
    }

    memcpy(copy, value, length + 1);

    /* Allocate first so the old value stays valid if allocation fails. */
    container_clear(c);
    c->data.string = copy;
    c->type = TYPE_STRING;

    return 1;
}

int container_get_int(const Container *c, int *out_value)
{
    if (c == NULL || out_value == NULL || c->type != TYPE_INT) {
        return 0;
    }

    *out_value = c->data.integer;
    return 1;
}

int container_get_float(const Container *c, float *out_value)
{
    if (c == NULL || out_value == NULL || c->type != TYPE_FLOAT) {
        return 0;
    }

    *out_value = c->data.decimal;
    return 1;
}

int container_get_char(const Container *c, char *out_value)
{
    if (c == NULL || out_value == NULL || c->type != TYPE_CHAR) {
        return 0;
    }

    *out_value = c->data.character;
    return 1;
}

int container_get_string(const Container *c, const char **out_value)
{
    if (c == NULL || out_value == NULL || c->type != TYPE_STRING) {
        return 0;
    }

    *out_value = c->data.string;
    return 1;
}

const char *container_type_name(const Container *c)
{
    if (c == NULL) return "INVALID";

    switch (c->type) {
        case TYPE_EMPTY: return "EMPTY";
        case TYPE_INT: return "INTEGER";
        case TYPE_FLOAT: return "FLOAT";
        case TYPE_CHAR: return "CHARACTER";
        case TYPE_STRING: return "STRING";
        default: return "UNKNOWN";
    }
}

void container_print(const Container *c)
{
    if (c == NULL) {
        printf("Invalid container.\n");
        return;
    }

    printf("%s: ", container_type_name(c));

    switch (c->type) {
        case TYPE_EMPTY:
            printf("(empty)\n");
            break;
        case TYPE_INT:
            printf("%d\n", c->data.integer);
            break;
        case TYPE_FLOAT:
            printf("%.2f\n", c->data.decimal);
            break;
        case TYPE_CHAR:
            printf("%c\n", c->data.character);
            break;
        case TYPE_STRING:
            printf("%s\n", c->data.string);
            break;
        default:
            printf("(invalid type)\n");
            break;
    }
}

/* Make a separate copy of the source value. */
int container_copy(Container *destination, const Container *source)
{
    Container temp;

    if (destination == NULL || source == NULL) return 0;
    if (destination == source) return 1;

    container_init(&temp);

    switch (source->type) {
        case TYPE_EMPTY:
            break;

        case TYPE_INT:
            temp.data.integer = source->data.integer;
            temp.type = TYPE_INT;
            break;

        case TYPE_FLOAT:
            temp.data.decimal = source->data.decimal;
            temp.type = TYPE_FLOAT;
            break;

        case TYPE_CHAR:
            temp.data.character = source->data.character;
            temp.type = TYPE_CHAR;
            break;

        case TYPE_STRING:
            if (source->data.string == NULL) return 0;
            temp.data.string = malloc(strlen(source->data.string) + 1);
            if (temp.data.string == NULL) return 0;
            strcpy(temp.data.string, source->data.string);
            temp.type = TYPE_STRING;
            break;

        default:
            return 0;
    }

    container_clear(destination);
    *destination = temp;

    return 1;
}

int main(void)
{
    Container values[6];
    Container copied;
    size_t i;
    int number;
    float decimal;
    const char *text;

    for (i = 0; i < 6; i++) {
        container_init(&values[i]);
    }
    container_init(&copied);

    container_set_int(&values[0], 10);
    container_set_float(&values[1], 3.14f);
    container_set_char(&values[2], 'A');
    container_set_string(&values[3], "Hello");
    container_set_int(&values[4], 0);
    container_set_string(&values[5], "Data");

    printf("--- Mixed type container list ---\n");
    for (i = 0; i < 6; i++) {
        printf("Index %zu | ", i);
        container_print(&values[i]);
    }

    printf("\n--- Getter examples ---\n");
    if (container_get_int(&values[0], &number)) {
        printf("Integer value: %d\n", number);
    }

    if (container_get_float(&values[1], &decimal)) {
        printf("Float value: %.2f\n", decimal);
    }

    if (container_get_string(&values[3], &text)) {
        printf("String value: %s\n", text);
    }

    if (!container_get_int(&values[1], &number)) {
        printf("Wrong-type getter was safely rejected.\n");
    }

    printf("\n--- Deep copy example ---\n");
    if (container_copy(&copied, &values[5])) {
        printf("Copied value: ");
        container_print(&copied);

        container_set_string(&values[5], "Changed original");

        printf("Original after change: ");
        container_print(&values[5]);

        printf("Copy remains unchanged: ");
        container_print(&copied);
    } else {
        printf("Copy failed.\n");
    }

    printf("\n--- Type changes ---\n");
    container_set_string(&values[0], "First text");
    container_print(&values[0]);

    container_set_int(&values[0], 25);
    container_print(&values[0]);

    container_set_string(&values[0], "Second text");
    container_print(&values[0]);

    container_set_float(&values[0], 8.5f);
    container_print(&values[0]);

    /* Self-copy should be safe. */
    container_copy(&values[0], &values[0]);

    /* Clearing twice should also be safe. */
    container_clear(&values[0]);
    container_clear(&values[0]);
    container_print(&values[0]);

    for (i = 0; i < 6; i++) {
        container_clear(&values[i]);
    }
    container_clear(&copied);

    printf("\nAll containers cleared. Memory released.\n");
    return 0;
}
