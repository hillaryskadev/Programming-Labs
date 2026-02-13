#ifndef SDF_SERIALIZER_H
#define SDF_SERIALIZER_H

#include "sdf_parser.h"

int sdf_serialize_file(const SdfNode *node, const char *filename);

#endif