#include "contact6.h"

void initContactList(ContactList *list) {
    list->size = 0;
    list->capacity = 10;
    list->contacts = (Contact *)malloc(list->capacity * sizeof(Contact));
    if (list->contacts == NULL) {
        printf("Error: Memory allocation failed!\n");
        exit(1);
    }
}

void addContact(ContactList *list) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->contacts = (Contact *)realloc(list->contacts, list->capacity * sizeof(Contact));
        if (list->contacts == NULL) {
            printf("Error: Memory reallocation failed!\n");
            exit(1);
        }
    }
    
    Contact newContact;
    printf("Enter name: ");
    fgets(newContact.name, sizeof(newContact.name), stdin);
    newContact.name[strcspn(newContact.name, "\n")] = '\0';
    
    printf("Enter phone: ");
    fgets(newContact.phone, sizeof(newContact.phone), stdin);
    newContact.phone[strcspn(newContact.phone, "\n")] = '\0';
    
    printf("Enter email: ");
    fgets(newContact.email, sizeof(newContact.email), stdin);
    newContact.email[strcspn(newContact.email, "\n")] = '\0';
    if (!isValidEmail(newContact.email)) {
        printf("Error: Invalid email format!\n");
        return;
    }
    
    printf("Enter group (0=Friends, 1=Family, 2=Work): ");
    int groupInput;
    if (scanf("%d", &groupInput) != 1 || groupInput < 0 || groupInput > 2) {
        printf("Error: Invalid group!\n");
        clearInputBuffer();
        return;
    }
    newContact.group = (Group)groupInput;
    clearInputBuffer();
    
    list->contacts[list->size++] = newContact;
    printf("Contact added successfully.\n");
}

void deleteContact(ContactList *list) {
    if (list->size == 0) {
        printf("No contacts to delete.\n");
        return;
    }
    
    printf("Enter index to delete (0 to %d): ", list->size - 1);
    int index;
    if (scanf("%d", &index) != 1 || index < 0 || index >= list->size) {
        printf("Error: Invalid index.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    printf("Confirm delete %s? (y/n): ", list->contacts[index].name);
    char confirm;
    scanf("%c", &confirm);
    clearInputBuffer();
    if (confirm != 'y' && confirm != 'Y') {
        printf("Delete cancelled.\n");
        return;
    }
    
    for (int i = index; i < list->size - 1; i++) {
        list->contacts[i] = list->contacts[i + 1];
    }
    list->size--;
    printf("Contact deleted.\n");
}

void updateContact(ContactList *list) {
    if (list->size == 0) {
        printf("No contacts to update.\n");
        return;
    }
    
    printf("Enter index to update (0 to %d): ", list->size - 1);
    int index;
    if (scanf("%d", &index) != 1 || index < 0 || index >= list->size) {
        printf("Error: Invalid index.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    printf("Confirm update %s? (y/n): ", list->contacts[index].name);
    char confirm;
    scanf("%c", &confirm);
    clearInputBuffer();
    if (confirm != 'y' && confirm != 'Y') {
        printf("Update cancelled.\n");
        return;
    }
    
    printf("Enter new name: ");
    fgets(list->contacts[index].name, sizeof(list->contacts[index].name), stdin);
    list->contacts[index].name[strcspn(list->contacts[index].name, "\n")] = '\0';
    
    printf("Enter new phone: ");
    fgets(list->contacts[index].phone, sizeof(list->contacts[index].phone), stdin);
    list->contacts[index].phone[strcspn(list->contacts[index].phone, "\n")] = '\0';
    
    printf("Enter new email: ");
    fgets(list->contacts[index].email, sizeof(list->contacts[index].email), stdin);
    list->contacts[index].email[strcspn(list->contacts[index].email, "\n")] = '\0';
    if (!isValidEmail(list->contacts[index].email)) {
        printf("Error: Invalid email format!\n");
        return;
    }
    
    printf("Enter new group (0=Friends, 1=Family, 2=Work): ");
    int groupInput;
    if (scanf("%d", &groupInput) != 1 || groupInput < 0 || groupInput > 2) {
        printf("Error: Invalid group.\n");
        clearInputBuffer();
        return;
    }
    list->contacts[index].group = (Group)groupInput;
    clearInputBuffer();
    
    printf("Contact updated.\n");
}

void displayContact(ContactList *list) {
    if (list->size == 0) {
        printf("No contacts to display.\n");
        return;
    }
    
    printf("Enter index to display (0 to %d): ", list->size - 1);
    int index;
    if (scanf("%d", &index) != 1 || index < 0 || index >= list->size) {
        printf("Error: Invalid index.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();
    
    Contact c = list->contacts[index];
    const char *groupStr[] = {"Friends", "Family", "Work"};
    printf("Name: %s\nPhone: %s\nEmail: %s\nGroup: %s\n", c.name, c.phone, c.email, groupStr[c.group]);
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

void freeContactList(ContactList *list) {
    free(list->contacts);
    list->contacts = NULL;
    list->size = 0;
    list->capacity = 0;
}

int isValidEmail(const char *email) {
    return (strchr(email, '@') != NULL && strchr(email, '.') != NULL);
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}