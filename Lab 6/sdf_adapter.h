#ifndef SDF_ADAPTER_H
#define SDF_ADAPTER_H

#include "contact6.h"

int save_contact_list_sdf(const ContactList *list, const char *username);
int load_contact_list_sdf(ContactList *list, const char *username);

#endif