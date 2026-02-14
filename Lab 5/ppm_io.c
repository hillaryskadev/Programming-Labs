#include "ppm_io.h"
#include <string.h>

Image* load_ppm(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Cannot open %s\n", filename);
        return NULL;
    }

    char buf[1024];
    int width, height, maxval;

    if (!fgets(buf, sizeof(buf), fp) || strncmp(buf, "P6", 2) != 0) {
        fprintf(stderr, "Not P6 PPM or error reading magic\n");
        fclose(fp);
        return NULL;
    }

    while (fgets(buf, sizeof(buf), fp)) {
        if (buf[0] == '#') continue;  // skip comment
        if (sscanf(buf, "%d %d", &width, &height) == 2) break;
    }
    if (width <= 0 || height <= 0) {
        fprintf(stderr, "Invalid width/height\n");
        fclose(fp);
        return NULL;
    }

    while (fgets(buf, sizeof(buf), fp)) {
        if (buf[0] == '#') continue;
        if (sscanf(buf, "%d", &maxval) == 1) break;
    }
    if (maxval != 255) {
        fprintf(stderr, "Only 255 maxval supported\n");
        fclose(fp);
        return NULL;
    }

    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            ungetc(c, fp);
            break;
        }
    }

    Image *img = malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->pixels = malloc(height * sizeof(Pixel*));
    if (!img->pixels) {
        free(img);
        fclose(fp);
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        img->pixels[i] = malloc(width * sizeof(Pixel));
        if (!img->pixels[i]) {
            // cleanup
            for (int j = 0; j < i; j++) free(img->pixels[j]);
            free(img->pixels);
            free(img);
            fclose(fp);
            return NULL;
        }
        size_t read = fread(img->pixels[i], sizeof(Pixel), width, fp);
        if (read != (size_t)width) {
            fprintf(stderr, "Incomplete read at row %d (read %zu of %d)\n", i, read, width);
            // cleanup
            for (int j = 0; j <= i; j++) free(img->pixels[j]);
            free(img->pixels);
            free(img);
            fclose(fp);
            return NULL;
        }
    }

    fclose(fp);
    printf("Loaded PPM P6: %dx%d\n", width, height);  // debug print
    return img;
}

int save_ppm(const char *filename, const Image *img) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return 0;

    fprintf(fp, "P6\n%d %d\n255\n", img->width, img->height);
    for (int i = 0; i < img->height; i++) {
        fwrite(img->pixels[i], sizeof(Pixel), img->width, fp);
    }
    fclose(fp);
    return 1;
}

void free_image(Image *img) {
    if (!img) return;
    for (int i = 0; i < img->height; i++) free(img->pixels[i]);
    free(img->pixels);
    free(img);
}

Image* copy_image(const Image *src) {
    Image *dst = malloc(sizeof(Image));
    dst->width = src->width;
    dst->height = src->height;
    dst->pixels = malloc(dst->height * sizeof(Pixel*));
    for (int i = 0; i < dst->height; i++) {
        dst->pixels[i] = malloc(dst->width * sizeof(Pixel));
        memcpy(dst->pixels[i], src->pixels[i], dst->width * sizeof(Pixel));
    }
    return dst;
}