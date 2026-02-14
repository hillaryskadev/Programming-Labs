#include "benchmark.h"
#include "filters.h"
#include "timer.h"
#include <stdio.h>

static const int thread_counts[] = {1, 2, 4, 8};
static const int n_threads = sizeof(thread_counts) / sizeof(int);

void run_benchmark(const char *filename) {
    Image *img = load_ppm(filename);
    if (!img) return;

    printf("Benchmark for image %dx%d\n", img->width, img->height);
    printf("Filter\tThreads\tTime (s)\tSpeedup\n");

    for (int f = 0; f < 3; f++) {
        FilterType type = f;

        double base_time = 0.0;
        Image *temp = copy_image(img);
        double t1 = get_time();
        apply_filter_single(temp, type);
        base_time = get_time() - t1;
        free_image(temp);

        printf("%s\t1\t%.4f\t1.00\n", filter_name(type), base_time);

        for (int i = 1; i < n_threads; i++) {
            int nt = thread_counts[i];
            double total = 0.0;
            for (int run = 0; run < 3; run++) {
                Image *temp = copy_image(img);
                double start = get_time();
                apply_filter_multi(temp, type, nt);
                total += get_time() - start;
                free_image(temp);
            }
            double avg = total / 3.0;
            double speedup = base_time / avg;
            printf("%s\t%d\t%.4f\t%.2f\n", filter_name(type), nt, avg, speedup);
        }
    }

    free_image(img);
}