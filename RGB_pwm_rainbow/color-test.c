#include <time.h>
#include <stdio.h>

#include "color.h"

void main(void)
{
    static HsvColor hsv= {
        .h = 0,
        .s = 0,
        .v = 0
    };
    uint16_t i = 0;
    printf("------------- Testing HSV to RGB ---------------------\r\n");
    printf("------------------------------------------------------\r\n");
    printf("\r\n############### Testing HUE #################\r\n");
    hsv.h = 0; hsv.s = 100; hsv.v = 100; // Value and saturation fixed at MAX
    for(i = 0; i < 360; i++)
    {
        hsv.h = i;
        printf("=== %03d === ", i);
        printf("HSV: %03d, %03d, %03d\t", hsv.h, hsv.s, hsv.v);
        //if (hsv.h >= 360) hsv.h = hsv.h - 360;
        RgbColor rgb = HsvToRgb(hsv);
        printf("RGB: %03d, %03d, %03d\r\n", rgb.r, rgb.g, rgb.b);
    }
    printf("\r\n############### Testing Value #################\r\n");
    hsv.h = 40; hsv.s = 50; hsv.v = 100; // Value and saturation fixed at MAX
    for(i = 0; i <= 100; i++)
    {
        hsv.v = i;
        printf("=== %03d === ", i);
        printf("HSV: %03d, %03d, %03d\t", hsv.h, hsv.s, hsv.v);
        RgbColor rgb = HsvToRgb(hsv);
        printf("RGB: %03d, %03d, %03d\r\n", rgb.r, rgb.g, rgb.b);
    }
    printf("\r\n############### Testing Saturation #################\r\n");
    hsv.h = 40; hsv.s = 100; hsv.v = 50; // Value and saturation fixed at MAX
    for(i = 0; i <= 100; i++)
    {
        hsv.s = i;
        printf("=== %03d === ", i);
        printf("HSV: %03d, %03d, %03d\t", hsv.h, hsv.s, hsv.v);
        RgbColor rgb = HsvToRgb(hsv);
        printf("RGB: %03d, %03d, %03d\r\n", rgb.r, rgb.g, rgb.b);
    }
}