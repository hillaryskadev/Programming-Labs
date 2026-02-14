#ifndef CONTACT_H
#define CONTACT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { FRIENDS, FAMILY, WORK } Group;

typedef struct {
    char name[100];
    char phone[20];
    char email[100];
    Group group;
} Contact;

typedef struct {
    Contact *contacts;
    int size;
    int capacity;
} ContactList;

void initContactList(ContactList *list);
void freeContactList(ContactList *list);

void addContact(ContactList *list);
void deleteContact(ContactList *list);
void updateContact(ContactList *list);

void displayContact(ContactList *list);
void displayAllContacts(ContactList *list);

int isValidEmail(const char *email);
void clearInputBuffer();
void getStringInput(char *buffer, int max_len, const char *prompt);

#endif