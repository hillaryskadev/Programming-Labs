#include "filters.h"
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    Image *input;
    Image *output;
    int start_row;
    int end_row;
    FilterType type;
} ThreadArg;

void grayscale_single(Image *img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            unsigned char gray = 0.299 * img->pixels[i][j].r +
                                 0.587 * img->pixels[i][j].g +
                                 0.114 * img->pixels[i][j].b;
            img->pixels[i][j].r = img->pixels[i][j].g = img->pixels[i][j].b = gray;
        }
    }
}

void *grayscale_thread(void *arg) {
    ThreadArg *t = (ThreadArg*)arg;
    for (int i = t->start_row; i < t->end_row; i++) {
        for (int j = 0; j < t->input->width; j++) {
            unsigned char gray = 0.299 * t->input->pixels[i][j].r +
                                 0.587 * t->input->pixels[i][j].g +
                                 0.114 * t->input->pixels[i][j].b;
            t->output->pixels[i][j].r = t->output->pixels[i][j].g = t->output->pixels[i][j].b = gray;
        }
    }
    return NULL;
}

static const int gauss_kernel[5][5] = {
    {1, 4, 7, 4, 1},
    {4,16,26,16, 4},
    {7,26,41,26, 7},
    {4,16,26,16, 4},
    {1, 4, 7, 4, 1}
};
static const int gauss_sum = 273;

void gaussian_single(Image *img) {
    Image *temp = copy_image(img);
    for (int i = 2; i < img->height - 2; i++) {
        for (int j = 2; j < img->width - 2; j++) {
            int r=0, g=0, b=0;
            for (int dy = -2; dy <= 2; dy++) {
                for (int dx = -2; dx <= 2; dx++) {
                    r += temp->pixels[i+dy][j+dx].r * gauss_kernel[dy+2][dx+2];
                    g += temp->pixels[i+dy][j+dx].g * gauss_kernel[dy+2][dx+2];
                    b += temp->pixels[i+dy][j+dx].b * gauss_kernel[dy+2][dx+2];
                }
            }
            img->pixels[i][j].r = r / gauss_sum;
            img->pixels[i][j].g = g / gauss_sum;
            img->pixels[i][j].b = b / gauss_sum;
        }
    }
    free_image(temp);
}

void *gaussian_thread(void *arg) {
    ThreadArg *t = (ThreadArg*)arg;
    for (int i = t->start_row + 2; i < t->end_row - 2; i++) {  // avoid borders
        for (int j = 2; j < t->input->width - 2; j++) {
            int r=0, g=0, b=0;
            for (int dy = -2; dy <= 2; dy++) {
                for (int dx = -2; dx <= 2; dx++) {
                    r += t->input->pixels[i+dy][j+dx].r * gauss_kernel[dy+2][dx+2];
                    g += t->input->pixels[i+dy][j+dx].g * gauss_kernel[dy+2][dx+2];
                    b += t->input->pixels[i+dy][j+dx].b * gauss_kernel[dy+2][dx+2];
                }
            }
            t->output->pixels[i][j].r = r / gauss_sum;
            t->output->pixels[i][j].g = g / gauss_sum;
            t->output->pixels[i][j].b = b / gauss_sum;
        }
    }
    return NULL;
}

int cmp(const void *a, const void *b) {
    return (*(unsigned char*)a - *(unsigned char*)b);
}

void median_single(Image *img) {
    Image *temp = copy_image(img);
    for (int i = 1; i < img->height - 1; i++) {
        for (int j = 1; j < img->width - 1; j++) {
            unsigned char rs[9], gs[9], bs[9];
            int idx = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    rs[idx] = temp->pixels[i+dy][j+dx].r;
                    gs[idx] = temp->pixels[i+dy][j+dx].g;
                    bs[idx] = temp->pixels[i+dy][j+dx].b;
                    idx++;
                }
            }
            qsort(rs, 9, sizeof(unsigned char), cmp);
            qsort(gs, 9, sizeof(unsigned char), cmp);
            qsort(bs, 9, sizeof(unsigned char), cmp);
            img->pixels[i][j].r = rs[4];
            img->pixels[i][j].g = gs[4];
            img->pixels[i][j].b = bs[4];
        }
    }
    free_image(temp);
}

void *median_thread(void *arg) {
    ThreadArg *t = (ThreadArg*)arg;
    for (int i = t->start_row + 1; i < t->end_row - 1; i++) {
        for (int j = 1; j < t->input->width - 1; j++) {
            unsigned char rs[9], gs[9], bs[9];
            int idx = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    rs[idx] = t->input->pixels[i+dy][j+dx].r;
                    gs[idx] = t->input->pixels[i+dy][j+dx].g;
                    bs[idx] = t->input->pixels[i+dy][j+dx].b;
                    idx++;
                }
            }
            qsort(rs, 9, sizeof(unsigned char), cmp);
            qsort(gs, 9, sizeof(unsigned char), cmp);
            qsort(bs, 9, sizeof(unsigned char), cmp);
            t->output->pixels[i][j].r = rs[4];
            t->output->pixels[i][j].g = gs[4];
            t->output->pixels[i][j].b = bs[4];
        }
    }
    return NULL;
}

void apply_filter_multi(Image *img, FilterType type, int num_threads) {
    if (num_threads < 2) {
        apply_filter_single(img, type);
        return;
    }

    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    ThreadArg *args = malloc(num_threads * sizeof(ThreadArg));

    Image *output = copy_image(img);  // hasil ditulis ke output

    int rows_per_thread = img->height / num_threads;

    for (int i = 0; i < num_threads; i++) {
        args[i].input = img;
        args[i].output = output;
        args[i].type = type;
        args[i].start_row = i * rows_per_thread;
        args[i].end_row = (i == num_threads - 1) ? img->height : (i+1) * rows_per_thread;

        void *(*func)(void*) = NULL;
        if (type == FILTER_GRAYSCALE) func = grayscale_thread;
        else if (type == FILTER_GAUSSIAN) func = gaussian_thread;
        else if (type == FILTER_MEDIAN) func = median_thread;

        pthread_create(&threads[i], NULL, func, &args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Copy hasil kembali ke img asli
    for (int i = 0; i < img->height; i++) {
        memcpy(img->pixels[i], output->pixels[i], img->width * sizeof(Pixel));
    }
    free_image(output);
    free(threads);
    free(args);
}

void apply_filter_single(Image *img, FilterType type) {
    if (type == FILTER_GRAYSCALE) grayscale_single(img);
    else if (type == FILTER_GAUSSIAN) gaussian_single(img);
    else if (type == FILTER_MEDIAN) median_single(img);
}

const char* filter_name(FilterType type) {
    if (type == FILTER_GRAYSCALE) return "Grayscale";
    if (type == FILTER_GAUSSIAN) return "Gaussian";
    if (type == FILTER_MEDIAN) return "Median";
    return "Unknown";
}