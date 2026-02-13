#ifndef FILTERS_H
#define FILTERS_H

#include "ppm_io.h"

typedef enum {
    FILTER_GRAYSCALE,
    FILTER_GAUSSIAN,
    FILTER_MEDIAN
} FilterType;

void apply_filter_single(Image *img, FilterType type);
void apply_filter_multi(Image *img, FilterType type, int num_threads);

const char* filter_name(FilterType type);

#endif