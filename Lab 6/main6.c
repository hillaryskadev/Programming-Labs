#include "contact6.h"
#include "auth6.h"
#include "sdf_adapter.h"
#include <stdio.h>

int main() {
    UserList user_list;
    initUserList(&user_list);
    loadUsers(&user_list);

    char current_user[MAX_USERNAME] = "";
    ContactList contacts;

    int choice;
    do {
        printf("\n=== Contact Manager - Lab 6 ===\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Pilih: ");
        scanf("%d", &choice);
        clearInputBuffer();

        if (choice == 1) {
            registerUser(&user_list);
            saveUsers(&user_list);
        } else if (choice == 2) {
            if (login(&user_list, current_user)) {
                initContactList(&contacts);
                load_contact_list_sdf(&contacts, current_user);

                int sub;
                do {
                    printf("\n=== %s ===\n", current_user);
                    printf("1. Tambah Kontak\n");
                    printf("2. Hapus Kontak\n");
                    printf("3. Update Kontak\n");
                    printf("4. Tampil Satu Kontak\n");
                    printf("5. Tampil Semua Kontak\n");
                    printf("6. Logout (simpan data)\n");
                    printf("Pilih: ");
                    scanf("%d", &sub);
                    clearInputBuffer();

                    switch (sub) {
                        case 1: addContact(&contacts); break;
                        case 2: deleteContact(&contacts); break;
                        case 3: updateContact(&contacts); break;
                        case 4: displayContact(&contacts); break;
                        case 5: displayAllContacts(&contacts); break;
                        case 6: 
                            save_contact_list_sdf(&contacts, current_user);
                            printf("Data disimpan. Logout...\n");
                            current_user[0] = '\0';
                            freeContactList(&contacts);
                            break;
                        default: printf("Pilihan salah.\n");
                    }
                } while (sub != 6);
            }
        } else if (choice == 3) {
            saveUsers(&user_list);
            printf("Keluar...\n");
        } else {
            printf("Pilihan salah.\n");
        }
    } while (choice != 3);

    freeUserList(&user_list);
    return 0;
}