    }

    length = strlen(value);
    if (length >= sizeof(record->data.text)) {
        return 0;
    }

    strcpy(record->data.text, value);
    record->type = VALUE_TEXT;
    return 1;
}

void display_record(const TypedRecord *record)
{
    if (record == NULL) {
        printf("Invalid record.\n");
        return;
    }

    switch (record->type) {
        case VALUE_INT:
            printf("Integer: %d\n", record->data.number);
            break;

        case VALUE_FLOAT:
            printf("Float: %.2f\n", record->data.decimal);
            break;

        case VALUE_TEXT:
            printf("Text: %s\n", record->data.text);
            break;

        default:
            printf("Invalid value type.\n");
            break;
    }
}

void show_memory_layout(void)
{
    StructData s = {10, 220.5f, "Structure"};
    UnionData u;
    int i;

    printf("\n--- Structure memory information ---\n");
    printf("sizeof(StructData): %zu bytes\n", sizeof(StructData));
    printf("alignment of StructData: %zu bytes\n", alignof(StructData));
    printf("Address of structure: %p\n", (void *)&s);
    printf("Address of number: %p\n", (void *)&s.number);
    printf("Address of decimal: %p\n", (void *)&s.decimal);
    printf("Address of text: %p\n", (void *)s.text);

    printf("Offset of number: %zu\n", offsetof(StructData, number));
    printf("Offset of decimal: %zu\n", offsetof(StructData, decimal));
    printf("Offset of text: %zu\n", offsetof(StructData, text));

    printf("\n--- Union memory information ---\n");
    u.number = 10;
    printf("sizeof(UnionData): %zu bytes\n", sizeof(UnionData));
    printf("alignment of UnionData: %zu bytes\n", alignof(UnionData));
    printf("Address of union: %p\n", (void *)&u);
    printf("Address of number: %p\n", (void *)&u.number);
    printf("Address of decimal: %p\n", (void *)&u.decimal);
    printf("Address of text: %p\n", (void *)u.text);

    printf("\nMemory for 100 structure records: %zu bytes\n",
           100 * sizeof(StructData));
    printf("Memory for 100 union records: %zu bytes\n",
           100 * sizeof(UnionData));

    printf("\n--- Observation ---\n");
    printf("Structure members have separate storage locations.\n");
    printf("Union members share the same starting memory address.\n");
    printf("The compiler may add padding for alignment.\n");
    printf("Union saves memory when only one member is needed at a time.\n");

    printf("\nStructure member addresses may differ by padding bytes.\n");
    printf("Union member addresses above should have the same value.\n");

    (void)i;
}

int main(void)
{
    TypedRecord records[5] = {0};
    size_t i;

    show_memory_layout();

    printf("\n--- Tagged union records ---\n");

    set_integer(&records[0], 10);
    set_float(&records[1], 220.5f);

    if (!set_text(&records[2], "Union Programming")) {
        printf("Text is too long.\n");
    }

    set_integer(&records[3], 500);
    set_text(&records[4], "C Language");

    for (i = 0; i < 5; i++) {
        printf("Record %zu: ", i + 1);
        display_record(&records[i]);
    }

    printf("\n--- Changing one record's active type ---\n");
    set_integer(&records[0], 25);
    display_record(&records[0]);

    set_text(&records[0], "Changed to text");
    display_record(&records[0]);

    set_float(&records[0], 9.75f);
    display_record(&records[0]);

    printf("\nSize of tagged record: %zu bytes\n", sizeof(TypedRecord));
    printf("Size of plain union: %zu bytes\n", sizeof(UnionData));
    printf("Size of plain structure: %zu bytes\n", sizeof(StructData));

    printf("\nThe tag tells the program which union member is valid.\n");
    printf("Never read another union member before setting its value.\n");

    return 0;
}
