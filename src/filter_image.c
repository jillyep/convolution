#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    float value = 1.0 / (im.w * im.w);
    for (int y = 0; y < im.w; y++) {
        for (int x = 0; x < im.w; x++) {
            set_pixel(im, x, y, 0, value); // Set the value for the single channel
        }
    }
}

image make_box_filter(int w)
{
    image box_filter = make_image(w, w, 1);
    l1_normalize(box_filter);
    return box_filter;
}

image convolve_image(image im, image filter, int preserve)
{
    assert(filter.c == 1 || filter.c == im.c); // Filter must have 1 channel or the same number of channels as the image.

    int output_channels = (preserve == 1) ? im.c : 1; // If preserve is 1, keep the same number of channels.
    image result = make_image(im.w, im.h, output_channels); // Create output image.
    float half_filter_x = filter.w / 2;
    float half_filter_y = filter.h / 2;

    float kernel_sum = 0.0;
    // Calculate the sum of the kernel
    for (int ky = 0; ky < filter.h; ky++) {
        for (int kx = 0; kx < filter.w; kx++) {
            kernel_sum += get_pixel(filter, kx, ky, 0);
        }
    }

    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            for (int c = 0; c < output_channels; c++) {
                float pixel_value = 0.0;

                // Initialize the output channel to 0 if preserve == 0 (sum over channels)
                if (c == 0 && preserve == 0) {
                    set_pixel(result, x, y, 0, 0);  // Initialize the first channel to 0 before accumulation
                }

                for (int fy = 0; fy < filter.h; fy++) {
                    for (int fx = 0; fx < filter.w; fx++) {
                        int image_x = x + fx - half_filter_x;
                        int image_y = y + fy - half_filter_y;

                        // Clamp image_x and image_y to stay within image boundaries
                        if (image_x <= 0) image_x = 0;
                        if (image_y <= 0) image_y = 0;
                        if (image_x >= im.w) image_x = im.w - 1;
                        if (image_y >= im.h) image_y = im.h - 1;

                        float filter_value = (filter.c == 1) ? get_pixel(filter, fx, fy, 0) : get_pixel(filter, fx, fy, c);
                        float image_value = get_pixel(im, image_x, image_y, c);

                        pixel_value += image_value * filter_value;
                    }
                }

                // Normalize pixel value by kernel sum if necessary
                if (kernel_sum != 0) {
                    pixel_value /= kernel_sum;
                }

                // Apply clamping to ensure pixel value is in range [0, 255]
                if (pixel_value < 0) pixel_value = 0;
                if (pixel_value > 255) pixel_value = 255;

                if (preserve == 1) {
                    // Set the pixel value for each channel if preserve is true.
                    set_pixel(result, x, y, c, pixel_value);
                } else {
                    if (c == 0) {
                        float final_value = get_pixel(result, x, y, 0) + pixel_value;
                        // Clamp the summed pixel value
                        if (final_value < 0) final_value = 0;
                        if (final_value > 255) final_value = 255;
                        set_pixel(result, x, y, 0, final_value);
                    }
                }
            }
        }
    }

    return result;
}


image make_highpass_filter()
{
    image filter = make_image(3, 3, 1);

    float values[3][3] = {
        {0, -1, 0},
        {-1, 4, -1},
        {0, -1, 0}
    };

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            set_pixel(filter, x, y, 0, values[y][x]);
        }
    }
    return filter;
}

image make_sharpen_filter()
{
    image filter = make_image(3, 3, 1);

    float values[3][3] = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
    };

    for (int y = 0; y < filter.h; y++){
        for (int x = 0; x < filter.w; x++){
            set_pixel(filter, x, y, 0, values[y][x]);
        }
    }
    return filter;
}

image make_emboss_filter()
{
    image filter = make_image(3, 3, 1);

    float values[3][3] = {
        {-2, -1, 0},
        {-1, 1, 1},
        {0, 1, 2}
    };

    for (int y = 0; y < filter.h; y++){
        for (int x = 0; x < filter.w; x++){
            set_pixel(filter, x, y, 0, values[y][x]);
        }
    }
    return filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: TODO

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: TODO

image make_gaussian_filter(float sigma)
{
    // Compute the size of the filter
    float size = 6 * sigma;
    int filter_size = (int)size;
    if (filter_size % 2 == 0) {
        filter_size += 1; // Ensure the size is odd
    }

    // Create the filter image
    image filter = make_image(filter_size, filter_size, 1);

    // Calculate the filter center
    int center = filter_size / 2;
    // double M_PI = 3.141592653;

    // Compute the Gaussian filter values
    float sum = 0.0; // For normalization
    for (int y = 0; y < filter_size; y++) {
        for (int x = 0; x < filter_size; x++) {
            // Calculate the distance from the center
            float dx = x - center;
            float dy = y - center;
            float exponent = -(dx*dx + dy*dy) / (2 * sigma * sigma);
            float value = exp(exponent) / (TWOPI * sigma * sigma);
            
            // Set the filter pixel value
            set_pixel(filter, x, y, 0, value);

            // Accumulate the sum for normalization
            sum += value;
        }
    }

    // Normalize the filter to ensure the sum of all values is 1
    l1_normalize(filter);

    return filter;
}

image add_image(image a, image b)
{
    // TODO
    return make_image(1,1,1);
}

image sub_image(image a, image b)
{
    // TODO
    return make_image(1,1,1);
}

image make_gx_filter()
{
    // TODO
    return make_image(1,1,1);
}

image make_gy_filter()
{
    // TODO
    return make_image(1,1,1);
}

void feature_normalize(image im)
{
    // TODO
}

image *sobel_image(image im)
{
    // TODO
    return calloc(2, sizeof(image));
}

image colorize_sobel(image im)
{
    // TODO
    return make_image(1,1,1);
}
