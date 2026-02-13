#include "contact4.h"
#include "auth.h"
#include "data_file.h"

int main() {
    UserList user_list;
    initUserList(&user_list);
    loadUsers(&user_list);

    char current_user[MAX_USERNAME] = "";
    ContactList contacts;
    initContactList(&contacts);

    int choice;
    do {
        printf("\n=== Contact Manager - Lab 4 ===\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        clearInputBuffer();

        if (choice == 1) {
            registerUser(&user_list);
            saveUsers(&user_list);
        } else if (choice == 2) {
            if (login(&user_list, current_user)) {
                loadUserData(&contacts, current_user);

                int sub_choice;
                do {
                    printf("\n=== Logged in as %s ===\n", current_user);
                    printf("1. Add Contact\n");
                    printf("2. Delete Contact\n");
                    printf("3. Update Contact\n");
                    printf("4. Display Contact\n");
                    printf("5. Display All Contacts\n");
                    printf("6. Logout\n");
                    printf("Enter choice: ");
                    scanf("%d", &sub_choice);
                    clearInputBuffer();

                    switch (sub_choice) {
                        case 1: addContact(&contacts); break;
                        case 2: deleteContact(&contacts); break;
                        case 3: updateContact(&contacts); break;
                        case 4: displayContact(&contacts); break;
                        case 5: displayAllContacts(&contacts); break;
                        case 6: 
                            saveUserData(&contacts, current_user);
                            printf("Data saved. Logging out...\n");
                            current_user[0] = '\0';
                            break;
                        default: printf("Invalid choice.\n");
                    }
                } while (sub_choice != 6);
            }
        } else if (choice == 3) {
            printf("Exiting...\n");
        } else {
            printf("Invalid choice.\n");
        }
    } while (choice != 3);

    saveUsers(&user_list);
    freeUserList(&user_list);
    freeContactList(&contacts);
    return 0;
}