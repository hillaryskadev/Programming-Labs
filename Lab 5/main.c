#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppm_io.h"
#include "filters.h"
#include "benchmark.h"
#include "timer.h" 

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage:\n");
        printf("  %s --benchmark <image.ppm>\n", argv[0]);
        printf("  %s <input.ppm> <output.ppm> <filter> [threads]\n", argv[0]);
        printf("  filter: grayscale | gaussian | median\n");
        printf("  threads: 1-8 (default 1)\n");
        return 1;
    }

    if (strcmp(argv[1], "--benchmark") == 0) {
        if (argc != 3) {
            printf("Usage: %s --benchmark <image.ppm>\n", argv[0]);
            return 1;
        }
        run_benchmark(argv[2]);
        return 0;
    }

    if (argc < 4) {
        printf("Not enough arguments.\n");
        return 1;
    }

    const char *input = argv[1];
    const char *output = argv[2];
    const char *filter_str = argv[3];
    int threads = 1;
    if (argc >= 5) threads = atoi(argv[4]);

    FilterType type;
    if (strcmp(filter_str, "grayscale") == 0) type = FILTER_GRAYSCALE;
    else if (strcmp(filter_str, "gaussian") == 0) type = FILTER_GAUSSIAN;
    else if (strcmp(filter_str, "median") == 0) type = FILTER_MEDIAN;
    else {
        printf("Unknown filter: %s\n", filter_str);
        return 1;
    }

    Image *img = load_ppm(input);
    if (!img) return 1;

    double start = get_time();
    if (threads <= 1) {
        apply_filter_single(img, type);
    } else {
        apply_filter_multi(img, type, threads);
    }
    double elapsed = get_time() - start;

    if (!save_ppm(output, img)) {
        fprintf(stderr, "Cannot save %s\n", output);
    } else {
        printf("Processed with %s filter (%d threads) in %.4f seconds\n",
               filter_name(type), threads, elapsed);
    }

    free_image(img);
    return 0;
}