#include "sdf_adapter.h"
#include "sdf_parser.h"
#include "sdf_serializer.h"
#include <stdio.h>

int save_contact_list_sdf(const ContactList *list, const char *username) {
    SdfNode *root = calloc(1, sizeof(SdfNode));
    root->type = SDF_OBJECT;

    SdfNode *arr_node = calloc(1, sizeof(SdfNode));
    arr_node->type = SDF_ARRAY;
    arr_node->key = strdup("contacts");
    arr_node->value.array.count = list->size;
    arr_node->value.array.items = calloc(list->size, sizeof(SdfNode*));

    for (int i = 0; i < list->size; i++) {
        SdfNode *contact_obj = calloc(1, sizeof(SdfNode));
        contact_obj->type = SDF_OBJECT;
        contact_obj->value.object.count = 4;
        contact_obj->value.object.pairs = calloc(4, sizeof(SdfNode*));

        // name
        SdfNode *n = calloc(1, sizeof(SdfNode));
        n->type = SDF_STRING;
        n->key = strdup("name");
        n->value.string_val = strdup(list->contacts[i].name);
        contact_obj->value.object.pairs[0] = n;

        // phone
        n = calloc(1, sizeof(SdfNode));
        n->type = SDF_STRING;
        n->key = strdup("phone");
        n->value.string_val = strdup(list->contacts[i].phone);
        contact_obj->value.object.pairs[1] = n;

        // email
        n = calloc(1, sizeof(SdfNode));
        n->type = SDF_STRING;
        n->key = strdup("email");
        n->value.string_val = strdup(list->contacts[i].email);
        contact_obj->value.object.pairs[2] = n;

        // group
        n = calloc(1, sizeof(SdfNode));
        n->type = SDF_NUMBER;
        n->key = strdup("group");
        n->value.number_val = list->contacts[i].group;
        contact_obj->value.object.pairs[3] = n;

        arr_node->value.array.items[i] = contact_obj;
    }

    root->value.object.count = 1;
    root->value.object.pairs = calloc(1, sizeof(SdfNode*));
    root->value.object.pairs[0] = arr_node;

    char filename[256];
    snprintf(filename, sizeof(filename), "data_%s.sdf", username);

    int ret = sdf_serialize_file(root, filename);
    sdf_free_node(root);
    return ret;
}

int load_contact_list_sdf(ContactList *list, const char *username) {
    char filename[256];
    snprintf(filename, sizeof(filename), "data_%s.sdf", username);

    SdfNode *root = sdf_parse_file(filename);
    if (!root) return 0;

    SdfNode *contacts_arr = sdf_get_array(root, "contacts");
    if (!contacts_arr) {
        sdf_free_node(root);
        return 0;
    }

    initContactList(list);

    for (size_t i = 0; i < contacts_arr->value.array.count; i++) {
        SdfNode *obj = contacts_arr->value.array.items[i];
        if (obj->type != SDF_OBJECT) continue;

        Contact c = {0};
        char *val;

        val = sdf_get_string(obj, "name");
        if (val) {
            strncpy(c.name, val, sizeof(c.name) - 1);
            free(val);
        }

        val = sdf_get_string(obj, "phone");
        if (val) {
            strncpy(c.phone, val, sizeof(c.phone) - 1);
            free(val);
        }

        val = sdf_get_string(obj, "email");
        if (val) {
            strncpy(c.email, val, sizeof(c.email) - 1);
            free(val);
        }

        c.group = (Group)(int)sdf_get_number(obj, "group");

        if (list->size == list->capacity) {
            list->capacity *= 2;
            list->contacts = realloc(list->contacts, list->capacity * sizeof(Contact));
        }
        list->contacts[list->size++] = c;
    }

    sdf_free_node(root);
    return 1;
}