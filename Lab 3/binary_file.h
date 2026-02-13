#ifndef BINARY_FILE_H
#define BINARY_FILE_H

#include "contact3.h"

#define DATA_FILE "contacts_lab3.dat"
#define MAGIC "ITMO2025"
#define VERSION 1

int saveToBinary(const ContactList *list);
int loadFromBinary(ContactList *list);

#endif