#include "sdf_parser.h"

static const char *pos;

static const char *skip_ws(const char *p) {
    while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) p++;
    return p;
}

static const char *skip_comment(const char *p) {
    p = skip_ws(p);
    while (*p == '#') {
        while (*p && *p != '\n') p++;
        p = skip_ws(p);
    }
    return p;
}

static char *read_quoted_string(const char **p) {
    if (**p != '"') return NULL;
    (*p)++;
    char *buf = malloc(1024);
    int i = 0;
    while (**p && **p != '"') {
        if (**p == '\\') {
            (*p)++;
            if (**p == 'n') buf[i++] = '\n';
            else if (**p == '"') buf[i++] = '"';
            else buf[i++] = **p;
        } else {
            buf[i++] = **p;
        }
        (*p)++;
    }
    if (**p != '"') {
        free(buf);
        return NULL;
    }
    (*p)++;
    buf[i] = '\0';
    return buf;
}

static SdfNode *parse_primitive() {
    pos = skip_comment(pos);
    if (*pos == '"') {
        char *str = read_quoted_string(&pos);
        if (!str) return NULL;
        SdfNode *n = calloc(1, sizeof(SdfNode));
        n->type = SDF_STRING;
        n->value.string_val = str;
        return n;
    }

    char *end;
    double num = strtod(pos, &end);
    if (end != pos) {
        pos = end;
        SdfNode *n = calloc(1, sizeof(SdfNode));
        n->type = SDF_NUMBER;
        n->value.number_val = num;
        return n;
    }

    if (strncmp(pos, "true", 4) == 0) {
        pos += 4;
        SdfNode *n = calloc(1, sizeof(SdfNode));
        n->type = SDF_BOOL;
        n->value.bool_val = 1;
        return n;
    }
    if (strncmp(pos, "false", 5) == 0) {
        pos += 5;
        SdfNode *n = calloc(1, sizeof(SdfNode));
        n->type = SDF_BOOL;
        n->value.bool_val = 0;
        return n;
    }
    if (strncmp(pos, "null", 4) == 0) {
        pos += 4;
        SdfNode *n = calloc(1, sizeof(SdfNode));
        n->type = SDF_NULL;
        return n;
    }

    return NULL;
}

static SdfNode *parse_array();

static SdfNode *parse_object();

static SdfNode *parse_value() {
    pos = skip_comment(pos);

    if (*pos == '{') return parse_object();
    if (*pos == '[') return parse_array();
    return parse_primitive();
}

static SdfNode *parse_array() {
    pos = skip_comment(pos);
    if (*pos != '[') return NULL;
    pos++;

    SdfNode *arr = calloc(1, sizeof(SdfNode));
    arr->type = SDF_ARRAY;

    while (1) {
        pos = skip_comment(pos);
        if (*pos == ']') break;

        SdfNode *val = parse_value();
        if (!val) {
            sdf_free_node(arr);
            return NULL;
        }

        arr->value.array.items = realloc(arr->value.array.items, (arr->value.array.count + 1) * sizeof(SdfNode*));
        arr->value.array.items[arr->value.array.count++] = val;

        pos = skip_comment(pos);
        if (*pos == ',') pos++;
        else if (*pos != ']') {
            sdf_free_node(arr);
            return NULL;
        }
    }

    pos++;
    return arr;
}

static SdfNode *parse_object() {
    pos = skip_comment(pos);
    if (*pos != '{') return NULL;
    pos++;

    SdfNode *obj = calloc(1, sizeof(SdfNode));
    obj->type = SDF_OBJECT;

    while (1) {
        pos = skip_comment(pos);
        if (*pos == '}') break;

        char *key = NULL;
        pos = read_quoted_string(&pos);
        if (!key) {
            sdf_free_node(obj);
            return NULL;
        }

        pos = skip_comment(pos);
        if (*pos != '=') {
            free(key);
            sdf_free_node(obj);
            return NULL;
        }
        pos++;

        SdfNode *val = parse_value();
        if (!val) {
            free(key);
            sdf_free_node(obj);
            return NULL;
        }
        val->key = key;

        obj->value.object.pairs = realloc(obj->value.object.pairs, (obj->value.object.count + 1) * sizeof(SdfNode*));
        obj->value.object.pairs[obj->value.object.count++] = val;

        pos = skip_comment(pos);
        if (*pos == ',') pos++;
        else if (*pos != '}') {
            sdf_free_node(obj);
            return NULL;
        }
    }

    pos++;
    return obj;
}

SdfNode *sdf_parse_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buf = malloc(len + 1);
    fread(buf, 1, len, fp);
    buf[len] = '\0';
    fclose(fp);

    pos = buf;
    SdfNode *root = parse_object();
    free(buf);
    return root;
}

void sdf_free_node(SdfNode *node) {
    if (!node) return;

    if (node->key) free(node->key);

    switch (node->type) {
        case SDF_STRING:
            free(node->value.string_val);
            break;
        case SDF_ARRAY:
            for (size_t i = 0; i < node->value.array.count; i++)
                sdf_free_node(node->value.array.items[i]);
            free(node->value.array.items);
            break;
        case SDF_OBJECT:
            for (size_t i = 0; i < node->value.object.count; i++)
                sdf_free_node(node->value.object.pairs[i]);
            free(node->value.object.pairs);
            break;
        default:
            break;
    }
    free(node);
}

char *sdf_get_string(SdfNode *node, const char *key) {
    if (node->type != SDF_OBJECT) return NULL;
    for (size_t i = 0; i < node->value.object.count; i++) {
        SdfNode *pair = node->value.object.pairs[i];
        if (pair->key && strcmp(pair->key, key) == 0 && pair->type == SDF_STRING) {
            return strdup(pair->value.string_val);
        }
    }
    return NULL;
}

double sdf_get_number(SdfNode *node, const char *key) {
    if (node->type != SDF_OBJECT) return 0;
    for (size_t i = 0; i < node->value.object.count; i++) {
        SdfNode *pair = node->value.object.pairs[i];
        if (pair->key && strcmp(pair->key, key) == 0 && pair->type == SDF_NUMBER) {
            return pair->value.number_val;
        }
    }
    return 0;
}

int sdf_get_bool(SdfNode *node, const char *key) {
    if (node->type != SDF_OBJECT) return 0;
    for (size_t i = 0; i < node->value.object.count; i++) {
        SdfNode *pair = node->value.object.pairs[i];
        if (pair->key && strcmp(pair->key, key) == 0 && pair->type == SDF_BOOL) {
            return pair->value.bool_val;
        }
    }
    return 0;
}

SdfNode *sdf_get_array(SdfNode *node, const char *key) {
    if (node->type != SDF_OBJECT) return NULL;
    for (size_t i = 0; i < node->value.object.count; i++) {
        SdfNode *pair = node->value.object.pairs[i];
        if (pair->key && strcmp(pair->key, key) == 0 && pair->type == SDF_ARRAY) {
            return pair;
        }
    }
    return NULL;
}