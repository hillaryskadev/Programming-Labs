#include "contact.h"

int isValidEmail(const char *email) {
    return (strchr(email, '@') != NULL && strchr(email, '.') != NULL);
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void getStringInput(char *buffer, int max_len, const char *prompt) {
    printf("%s", prompt);
    fgets(buffer, max_len, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}