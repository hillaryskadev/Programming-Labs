#ifndef DATA_FILE_H
#define DATA_FILE_H

#include "contact4.h"

int saveUserData(const ContactList *list, const char *username);
int loadUserData(ContactList *list, const char *username);

#endif