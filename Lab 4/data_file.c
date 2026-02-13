#include "data_file.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static unsigned int calculateChecksum(const void *data, size_t size) {
    unsigned int checksum = 0;
    const unsigned char *bytes = (const unsigned char *)data;
    for (size_t i = 0; i < size; i++) checksum ^= bytes[i];
    return checksum;
}

int saveUserData(const ContactList *list, const char *username) {
    char filename[100];
    snprintf(filename, sizeof(filename), "data_%s.dat", username);

    FILE *file = fopen(filename, "wb");
    if (!file) return 0;

    fwrite("ITMO2025", 1, 8, file);
    int version = 1;
    fwrite(&version, sizeof(int), 1, file);
    int count = list->size;
    fwrite(&count, sizeof(int), 1, file);
    unsigned int checksum = calculateChecksum(list->contacts, list->size * sizeof(Contact));
    fwrite(&checksum, sizeof(unsigned int), 1, file);

    if (list->size > 0) {
        fwrite(list->contacts, sizeof(Contact), list->size, file);
    }

    fclose(file);
    return 1;
}

int loadUserData(ContactList *list, const char *username) {
    char filename[100];
    snprintf(filename, sizeof(filename), "data_%s.dat", username);

    FILE *file = fopen(filename, "rb");
    if (!file) return 0;  // File belum ada, list tetap kosong

    char magic[9] = {0};
    fread(magic, 1, 8, file);
    if (strcmp(magic, "ITMO2025") != 0) {
        fclose(file);
        return 0;
    }

    int version;
    fread(&version, sizeof(int), 1, file);
    if (version != 1) {
        fclose(file);
        return 0;
    }

    int count;
    fread(&count, sizeof(int), 1, file);
    if (count < 0 || count > 100000) {
        fclose(file);
        return 0;
    }

    unsigned int storedChecksum;
    fread(&storedChecksum, sizeof(unsigned int), 1, file);

    Contact *temp = (Contact *)malloc(count * sizeof(Contact));
    if (!temp) {
        fclose(file);
        return 0;
    }

    size_t read = fread(temp, sizeof(Contact), count, file);
    fclose(file);

    if (read != (size_t)count || calculateChecksum(temp, count * sizeof(Contact)) != storedChecksum) {
        free(temp);
        return 0;
    }

    free(list->contacts);
    list->contacts = temp;
    list->size = count;
    list->capacity = count > 10 ? count : 10;

    return 1;
}