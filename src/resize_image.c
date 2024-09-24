#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    int nearest_x = (int)(x + 0.5);
    int nearest_y = (int)(y + 0.5);

    if (nearest_x < 0) {nearest_x = 0;}
    if (nearest_y < 0) { nearest_y = 0;}

    if (nearest_x >= im.w) {nearest_x = im.w - 1;}
    if (nearest_y >= im.h) {nearest_y = im.h - 1;}

    return get_pixel(im, nearest_x, nearest_y, c);
}

image nn_resize(image im, int w, int h)
{
    image resized_img = make_image(w, h, im.c);

    float scale_x = (float)im.w / w;
    float scale_y = (float)im.h / h;

    for (int y = 0; y < resized_img.h; y++){
        for (int x = 0; x < resized_img.w; x++){
            for (int c = 0; c < im.c; c++){
                float pixel_val = nn_interpolate(im, x*scale_x, y*scale_y, c);
                set_pixel(resized_img, x, y, c, pixel_val);
            }
        }
    }
    return resized_img;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // Top Left Corner
    int x1 = (int)floor(x);
    int y1 = (int)floor(y);

    // Bottom Right Corner
    int x2 = x1 + 1;
    int y2 = y1 + 1;

    // Clamp Image
    if (x2 >= im.w) {x2 = im.w - 1;}
    if (y2 >= im.h) { y2 = im.h - 1;}

    // Distances
    float d1 = x - x1;
    float d2 = 1.0 - d1;
    float d3 = y - y1;
    float d4 = 1.0 - d3;

    // Pixel Values at Corners
    float v1 = get_pixel(im, x1, y1, c);
    float v2 = get_pixel(im, x1, y2, c);
    float v3 = get_pixel(im, x2, y1, c);
    float v4 = get_pixel(im, x2, y2, c);

    // Perform Bi-Linear Interpolation
    float q1 = v1 * d2 + v3 * d1;
    float q2 = v2 * d2 + v4 * d1;
    float q = q1 * d4 + q2 * d3;
    
    return q;
}

image bilinear_resize(image im, int w, int h)
{
    image resized = make_image(w, h, im.c);

    float scale_x = (float)im.w / w;
    float scale_y = (float)im.h / h;

    for (int y = 0; y < h; y++){
        for (int x = 0; x < w; x++){
            for (int c = 0; c < im.c; c++){
                // Map new image coordinates to original image coordinates
                float original_x = x * scale_x;
                float original_y = y * scale_y;

                float pixel_val = bilinear_interpolate(im, original_x, original_y, c);
                set_pixel(resized, x, y, c, pixel_val);

            }
        }
    }
    return resized;
}

