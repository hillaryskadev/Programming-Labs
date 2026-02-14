#include "contact3.h"
#include "binary_file.h"

int main() {
    ContactList list;
    initContactList(&list);

    loadFromBinary(&list);

    int choice;
    do {
        printf("\n=== Contact Manager - Lab 3 ===\n");
        printf("1. Add Contact\n");
        printf("2. Delete Contact\n");
        printf("3. Update Contact\n");
        printf("4. Display Contact\n");
        printf("5. Display All Contacts\n");
        printf("6. Save to Binary File\n");
        printf("7. Load from Binary File\n");
        printf("8. Exit\n");
        printf("Enter choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Error: Invalid input.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();
        
        switch (choice) {
            case 1: addContact(&list); break;
            case 2: deleteContact(&list); break;
            case 3: updateContact(&list); break;
            case 4: displayContact(&list); break;
            case 5: displayAllContacts(&list); break;
            case 6: saveToBinary(&list); break;
            case 7: loadFromBinary(&list); break;
            case 8:
                saveToBinary(&list);
                printf("Exiting... Data auto-saved.\n");
                break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 8);
    
    freeContactList(&list);
    return 0;
}