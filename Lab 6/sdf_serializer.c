#include "sdf_serializer.h"

static void serialize_indent(FILE *f, int level) {
    for (int i = 0; i < level * 2; i++) fputc(' ', f);
}

static void serialize_node(FILE *f, const SdfNode *node, int level) {
    if (!node) return;

    serialize_indent(f, level);

    if (node->key) fprintf(f, "%s = ", node->key);

    switch (node->type) {
        case SDF_STRING:
            fprintf(f, "\"%s\"", node->value.string_val);
            break;
        case SDF_NUMBER:
            fprintf(f, "%.0f", node->value.number_val);
            break;
        case SDF_BOOL:
            fprintf(f, "%s", node->value.bool_val ? "true" : "false");
            break;
        case SDF_NULL:
            fprintf(f, "null");
            break;
        case SDF_ARRAY:
            fprintf(f, "[\n");
            for (size_t i = 0; i < node->value.array.count; i++) {
                serialize_node(f, node->value.array.items[i], level + 1);
                if (i < node->value.array.count - 1) fprintf(f, ",\n");
            }
            serialize_indent(f, level);
            fprintf(f, "]");
            break;
        case SDF_OBJECT:
            fprintf(f, "{\n");
            for (size_t i = 0; i < node->value.object.count; i++) {
                serialize_node(f, node->value.object.pairs[i], level + 1);
                if (i < node->value.object.count - 1) fprintf(f, ",\n");
            }
            serialize_indent(f, level);
            fprintf(f, "}");
            break;
    }
}

int sdf_serialize_file(const SdfNode *node, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return 0;

    serialize_node(f, node, 0);
    fprintf(f, "\n");
    fclose(f);
    return 1;
}