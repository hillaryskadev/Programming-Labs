#ifndef AUTH_H
#define AUTH_H

#include "contact4.h"

#define USERS_FILE "users.dat"
#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define HASH_SEED 12345  // Untuk hash sederhana

typedef struct {
    char username[MAX_USERNAME];
    unsigned int password_hash;
} User;

typedef struct {
    User *users;
    int size;
    int capacity;
} UserList;

void initUserList(UserList *list);
void freeUserList(UserList *list);
int registerUser(UserList *list);
int login(UserList *list, char *current_user);
unsigned int simpleHash(const char *password);
int loadUsers(UserList *list);
int saveUsers(const UserList *list);
void getStringInput(char *buffer, int max_len, const char *prompt);

#endif