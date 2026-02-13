#include "contact.h"

void displayContact(ContactList *list) {
    if (list->size == 0) {
        printf("No contacts.\n");
        return;
    }

    int index;
    printf("Enter index to display (0 to %d): ", list->size - 1);
    scanf("%d", &index);
    clearInputBuffer();

    if (index < 0 || index >= list->size) {
        printf("Invalid index.\n");
        return;
    }

    Contact c = list->contacts[index];
    const char *groupStr[] = {"Friends", "Family", "Work"};
    printf("Name: %s\nPhone: %s\nEmail: %s\nGroup: %s\n",
           c.name, c.phone, c.email, groupStr[c.group]);
}

void displayAllContacts(ContactList *list) {
    if (list->size == 0) {
        printf("No contacts.\n");
        return;
    }

    const char *groupStr[] = {"Friends", "Family", "Work"};
    for (int i = 0; i < list->size; i++) {
        printf("%d. Name: %s, Phone: %s, Email: %s, Group: %s\n", i,
               list->contacts[i].name, list->contacts[i].phone,
               list->contacts[i].email, groupStr[list->contacts[i].group]);
    }
}