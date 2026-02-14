#include "auth6.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned int simpleHash(const char *password) {
    unsigned int hash = HASH_SEED;
    while (*password) {
        hash = hash * 31 + (*password++);
    }
    return hash;
}

void initUserList(UserList *list) {
    list->size = 0;
    list->capacity = 10;
    list->users = (User *)malloc(list->capacity * sizeof(User));
    if (!list->users) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
}

void freeUserList(UserList *list) {
    free(list->users);
    list->users = NULL;
    list->size = 0;
    list->capacity = 0;
}

int registerUser(UserList *list) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->users = (User *)realloc(list->users, list->capacity * sizeof(User));
        if (!list->users) {
            printf("Memory reallocation failed!\n");
            return 0;
        }
    }

    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    getStringInput(username, MAX_USERNAME, "Enter username: ");
    getStringInput(password, MAX_PASSWORD, "Enter password: ");

    for (int i = 0; i < list->size; i++) {
        if (strcmp(list->users[i].username, username) == 0) {
            printf("Error: Username already exists!\n");
            return 0;
        }
    }

    User newUser;
    strncpy(newUser.username, username, MAX_USERNAME - 1);
    newUser.username[MAX_USERNAME - 1] = '\0';
    newUser.password_hash = simpleHash(password);

    list->users[list->size++] = newUser;
    printf("Registration successful!\n");
    return 1;
}

int login(UserList *list, char *current_user) {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    getStringInput(username, MAX_USERNAME, "Enter username: ");
    getStringInput(password, MAX_PASSWORD, "Enter password: ");

    unsigned int hash = simpleHash(password);

    for (int i = 0; i < list->size; i++) {
        if (strcmp(list->users[i].username, username) == 0 &&
            list->users[i].password_hash == hash) {
            strcpy(current_user, username);
            printf("Login successful! Welcome, %s!\n", username);
            return 1;
        }
    }

    printf("Invalid username or password.\n");
    return 0;
}

int loadUsers(UserList *list) {
    FILE *file = fopen(USERS_FILE, "rb");
    if (!file) {
        printf("No users file found. Starting with empty users.\n");
        return 1;
    }

    int count;
    fread(&count, sizeof(int), 1, file);
    if (count < 0 || count > 1000) {
        printf("Error: Invalid user count.\n");
        fclose(file);
        return 0;
    }

    User *temp = (User *)malloc(count * sizeof(User));
    if (!temp) {
        fclose(file);
        return 0;
    }

    size_t read = fread(temp, sizeof(User), count, file);
    fclose(file);

    if (read != (size_t)count) {
        free(temp);
        return 0;
    }

    free(list->users);
    list->users = temp;
    list->size = count;
    list->capacity = count > 10 ? count : 10;

    printf("Loaded %d users.\n", count);
    return 1;
}

int saveUsers(const UserList *list) {
    FILE *file = fopen(USERS_FILE, "wb");
    if (!file) {
        printf("Error: Cannot save users.\n");
        return 0;
    }

    int count = list->size;
    fwrite(&count, sizeof(int), 1, file);
    fwrite(list->users, sizeof(User), list->size, file);
    fclose(file);
    return 1;
}

void getStringInput(char *buffer, int max_len, const char *prompt) {
    printf("%s", prompt);
    fgets(buffer, max_len, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}