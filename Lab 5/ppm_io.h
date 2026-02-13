#ifndef PPM_IO_H
#define PPM_IO_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned char r, g, b;
} Pixel;

typedef struct {
    int width;
    int height;
    Pixel **pixels;
} Image;

Image* load_ppm(const char *filename);
int save_ppm(const char *filename, const Image *img);
void free_image(Image *img);
Image* copy_image(const Image *src);

#endif