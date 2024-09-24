#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    if (x >= im.w) {x = im.w-1;}
    if (x < 0) {x = 0;}
    if (y >= im.h) {y = im.h-1;}
    if (y < 0) {y = 0;}
    int index = x + y * im.w + c * im.w * im.h;
    printf("Value: %f", im.data[index]);
    return im.data[index];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if ((x >= im.w) || (x < 0) || (y >= im.h) || (y < 0)){
        return;
    }
    else {
        int index = x + y * im.w + c * im.w * im.h;
        im.data[index] = v;
    }
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    copy.w = im.w;
    copy.h = im.h;
    copy.c = im.c;
    int num_pixels = copy.w * copy.h * copy.c;
    copy.data = memcpy(copy.data, im.data, num_pixels * sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    
    image gray = make_image(im.w, im.h, 1);
    int w = im.w;
    int h = im.h;

    for (int i = 0; i < w; i++){
        for (int j = 0; j < h; j++){
            gray.data[i + j * w] = im.data[i + j * w + w * h * 0] * 0.299 +
            im.data[i + j * w + w * h * 1] * 0.587 +
            im.data[i + j * w + w * h * 2] * 0.114;
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    for (int i = 0; i < im.w; i++){
        for (int j = 0; j < im.h; j++){
            float val = get_pixel(im, i, j, c);
            set_pixel(im, i, j, c, (val + v));
        }
    }
}

void clamp_image(image im)
{
    for (int k = 0; k < im.c; k++){
        for (int j = 0; j < im.h; j++){
            for (int i = 0; i < im.w; i++){
                if (get_pixel(im, i, j, k) < 0.0){
                   set_pixel(im, i, j, k, 0.0); 
                }
                else if (get_pixel(im, i, j, k) > 1.0){
                    set_pixel(im, i, j, k, 1.0);
                }
            }
        }
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    assert(im.c == 3);  // Ensure the image has 3 channels (RGB)

    image hsv = make_image(im.w, im.h, 3);  // Create an image to store HSV values

    for (int j = 0; j < im.h; j++) {
        for (int i = 0; i < im.w; i++) {
            // Get the RGB values for the current pixel
            float r = get_pixel(im, i, j, 0);
            float g = get_pixel(im, i, j, 1);
            float b = get_pixel(im, i, j, 2);

            // Normalize RGB values to [0, 1] if they are not already in that range
            float C_max = three_way_max(r, g, b);
            float C_min = three_way_min(r, g, b);
            float C_delta = C_max - C_min;

            // Calculate Value (V)
            float V = C_max;

            // Calculate Saturation (S)
            float S = (C_max == 0) ? 0 : (C_delta / C_max);

            // Calculate Hue (H)
            float H;
            if (C_delta == 0) {
                H = 0;  // Undefined hue, can be set to 0 or any value
            } else if (C_max == r) {
                H = fmod(((g - b) / C_delta), 6.0);
            } else if (C_max == g) {
                H = ((b - r) / C_delta) + 2.0;
            } else {
                H = ((r - g) / C_delta) + 4.0;
            }
            H *= 60.0;  // Convert to degrees
            if (H < 0) {
                H += 360.0;  // Ensure H is in [0, 360]
            }

            // Set the HSV values in the output image
            set_pixel(hsv, i, j, 0, H / 360.0);  // Normalize H to [0, 1]
            set_pixel(hsv, i, j, 1, S);
            set_pixel(hsv, i, j, 2, V);
        }
    }
}

void hsv_to_rgb(image im)
{
    assert(im.c == 3);  // Ensure the image has 3 channels (HSV)

    image rgb = make_image(im.w, im.h, 3);  // Create an image to store RGB values

    for (int j = 0; j < im.h; j++) {
        for (int i = 0; i < im.w; i++) {
            // Get the HSV values for the current pixel
            float h = get_pixel(im, i, j, 0) * 360.0;  // Convert H back to degrees
            float s = get_pixel(im, i, j, 1);
            float v = get_pixel(im, i, j, 2);

            // Calculate Chroma (C), X, and m
            float C = s * v;
            float X = C * (1 - fabs(fmod(h / 60.0, 2) - 1));
            float m = v - C;

            float r, g, b;

            // Determine the RGB values based on the hue sector
            if (h >= 0 && h < 60) {
                r = C;
                g = X;
                b = 0;
            } else if (h >= 60 && h < 120) {
                r = X;
                g = C;
                b = 0;
            } else if (h >= 120 && h < 180) {
                r = 0;
                g = C;
                b = X;
            } else if (h >= 180 && h < 240) {
                r = 0;
                g = X;
                b = C;
            } else if (h >= 240 && h < 300) {
                r = X;
                g = 0;
                b = C;
            } else {
                r = C;
                g = 0;
                b = X;
            }

            // Adjust RGB values by adding m
            r += m;
            g += m;
            b += m;

            // Clamp RGB values to ensure they are in the [0, 1] range
            r = fmin(fmax(r, 0.0), 1.0);
            g = fmin(fmax(g, 0.0), 1.0);
            b = fmin(fmax(b, 0.0), 1.0);

            // Set the RGB values in the output image
            set_pixel(rgb, i, j, 0, r);
            set_pixel(rgb, i, j, 1, g);
            set_pixel(rgb, i, j, 2, b);
        }
    }
}
