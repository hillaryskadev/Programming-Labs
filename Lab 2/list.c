#include "contact.h"

void initContactList(ContactList *list) {
    list->size = 0;
    list->capacity = 10;
    list->contacts = malloc(list->capacity * sizeof(Contact));
    if (!list->contacts) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
}

void freeContactList(ContactList *list) {
    free(list->contacts);
    list->contacts = NULL;
    list->size = 0;
    list->capacity = 0;
}

void addContact(ContactList *list) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->contacts = realloc(list->contacts, list->capacity * sizeof(Contact));
        if (!list->contacts) {
            printf("Memory reallocation failed!\n");
            exit(1);
        }
    }

    Contact newContact;
    getStringInput(newContact.name, sizeof(newContact.name), "Enter name: ");
    getStringInput(newContact.phone, sizeof(newContact.phone), "Enter phone: ");
    getStringInput(newContact.email, sizeof(newContact.email), "Enter email: ");

    if (!isValidEmail(newContact.email)) {
        printf("Invalid email!\n");
        return;
    }

    int groupInput;
    printf("Enter group (0=Friends, 1=Family, 2=Work): ");
    scanf("%d", &groupInput);
    clearInputBuffer();
    if (groupInput < 0 || groupInput > 2) {
        printf("Invalid group!\n");
        return;
    }
    newContact.group = groupInput;

    list->contacts[list->size++] = newContact;
    printf("Contact added.\n");
}

void deleteContact(ContactList *list) {
    if (list->size == 0) {
        printf("No contacts.\n");
        return;
    }

    int index;
    printf("Enter index to delete (0 to %d): ", list->size - 1);
    scanf("%d", &index);
    clearInputBuffer();

    if (index < 0 || index >= list->size) {
        printf("Invalid index.\n");
        return;
    }

    char confirm;
    printf("Delete %s? (y/n): ", list->contacts[index].name);
    scanf(" %c", &confirm);
    clearInputBuffer();

    if (confirm == 'y' || confirm == 'Y') {
        for (int i = index; i < list->size - 1; i++) {
            list->contacts[i] = list->contacts[i + 1];
        }
        list->size--;
        printf("Contact deleted.\n");
    } else {
        printf("Delete cancelled.\n");
    }
}

void updateContact(ContactList *list) {
    if (list->size == 0) {
        printf("No contacts.\n");
        return;
    }

    int index;
    printf("Enter index to update (0 to %d): ", list->size - 1);
    scanf("%d", &index);
    clearInputBuffer();

    if (index < 0 || index >= list->size) {
        printf("Invalid index.\n");
        return;
    }

    char confirm;
    printf("Update %s? (y/n): ", list->contacts[index].name);
    scanf(" %c", &confirm);
    clearInputBuffer();

    if (confirm != 'y' && confirm != 'Y') {
        printf("Update cancelled.\n");
        return;
    }

    getStringInput(list->contacts[index].name, sizeof(list->contacts[index].name), "Enter new name: ");
    getStringInput(list->contacts[index].phone, sizeof(list->contacts[index].phone), "Enter new phone: ");
    getStringInput(list->contacts[index].email, sizeof(list->contacts[index].email), "Enter new email: ");

    if (!isValidEmail(list->contacts[index].email)) {
        printf("Invalid email!\n");
        return;
    }

    int groupInput;
    printf("Enter new group (0=Friends, 1=Family, 2=Work): ");
    scanf("%d", &groupInput);
    clearInputBuffer();
    if (groupInput < 0 || groupInput > 2) {
        printf("Invalid group!\n");
        return;
    }
    list->contacts[index].group = groupInput;

    printf("Contact updated.\n");
}