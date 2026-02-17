#include "binary_file.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static unsigned int calculateChecksum(const void *data, size_t size) {
    unsigned int checksum = 0;
    const unsigned char *bytes = (const unsigned char *)data;
    for (size_t i = 0; i < size; i++) {
        checksum ^= bytes[i];
    }
    return checksum;
}

int saveToBinary(const ContactList *list) {
    FILE *file = fopen(DATA_FILE, "wb");
    if (!file) {
        printf("Error: Cannot open file for writing.\n");
        return 0;
    }

    fwrite(MAGIC, 1, 8, file);

    int version = VERSION;
    fwrite(&version, sizeof(int), 1, file);

    int count = (int)list->size;
    fwrite(&count, sizeof(int), 1, file);

    size_t dataSize = list->size * sizeof(Contact);
    unsigned int checksum = (list->size > 0) ? calculateChecksum(list->contacts, dataSize) : 0;
    fwrite(&checksum, sizeof(unsigned int), 1, file);

    if (list->size > 0) {
        fwrite(list->contacts, sizeof(Contact), list->size, file);
    }

    fclose(file);
    printf("Data successfully saved to %s\n", DATA_FILE);
    return 1;
}

int loadFromBinary(ContactList *list) {
    FILE *file = fopen(DATA_FILE, "rb");
    if (!file) {
        printf("No saved data found. Starting with empty list.\n");
        return 1;
    }

    char magic[9] = {0};
    fread(magic, 1, 8, file);
    if (strncmp(magic, MAGIC, 8) != 0) {
        printf("Error: Invalid magic number.\n");
        fclose(file);
        return 0;
    }

    int version;
    fread(&version, sizeof(int), 1, file);
    if (version != VERSION) {
        printf("Error: Unsupported version.\n");
        fclose(file);
        return 0;
    }

    int count;
    fread(&count, sizeof(int), 1, file);
    if (count < 0 || count > 100000) {
        printf("Error: Invalid contact count.\n");
        fclose(file);
        return 0;
    }

    unsigned int storedChecksum;
    fread(&storedChecksum, sizeof(unsigned int), 1, file);

    Contact *temp = (Contact *)malloc(count * sizeof(Contact));
    if (!temp) {
        printf("Error: Memory allocation failed.\n");
        fclose(file);
        return 0;
    }

    size_t readCount = fread(temp, sizeof(Contact), count, file);
    fclose(file);

    if (readCount != (size_t)count) {
        printf("Error: Failed to read all contacts.\n");
        free(temp);
        return 0;
    }

    unsigned int calcChecksum = (count > 0) ? calculateChecksum(temp, count * sizeof(Contact)) : 0;
    if (calcChecksum != storedChecksum) {
        printf("Error: File corrupted (checksum mismatch).\n");
        free(temp);
        return 0;
    }

    free(list->contacts);
    list->contacts = temp;
    list->size = count;
    list->capacity = count > 10 ? count : 10;

    printf("Successfully loaded %d contacts from %s\n", count, DATA_FILE);
    return 1;
}