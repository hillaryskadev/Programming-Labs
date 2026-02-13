#ifndef SDF_PARSER_H
#define SDF_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    SDF_STRING,
    SDF_NUMBER,
    SDF_BOOL,
    SDF_ARRAY,
    SDF_OBJECT,
    SDF_NULL
} SdfType;

typedef struct SdfNode {
    SdfType type;
    char *key;                // hanya untuk member object
    union {
        char *string_val;
        double number_val;
        int bool_val;
        struct {
            struct SdfNode **items;
            size_t count;
        } array;
        struct {
            struct SdfNode **pairs;
            size_t count;
        } object;
    } value;
} SdfNode;

SdfNode *sdf_parse_file(const char *filename);
void sdf_free_node(SdfNode *node);

char *sdf_get_string(SdfNode *node, const char *key);
double sdf_get_number(SdfNode *node, const char *key);
int sdf_get_bool(SdfNode *node, const char *key);
SdfNode *sdf_get_array(SdfNode *node, const char *key);
SdfNode *sdf_get_object(SdfNode *node, const char *key);

#endif