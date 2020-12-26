#include "color.h"
#include <stdio.h>

#define MAX_RGB        100 /** Maximum value the R,G and B components can take */
#define MAX_HUE        360 /** Maxmimum value the hue can take */
#define HUE_REGION     (MAX_HUE/6) /** Number of values in a HUE region */

RgbColor HsvToRgb(HsvColor hsv)
{
    RgbColor rgb;
    uint32_t region = 0, remainder = 0;
    uint32_t hdp = 0, inc = 0, dec = 0;

    if (hsv.s == 0)
    {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        return rgb;
    }

    region    = hsv.h / HUE_REGION;
    remainder = hsv.h % HUE_REGION;

    // Calculate intermediate values
    hdp = (hsv.v * (MAX_RGB -   hsv.s                                                  )) / MAX_RGB;
    dec = (hsv.v * (MAX_RGB - ((hsv.s *                    remainder)  /(HUE_REGION-1)))) / MAX_RGB;
    inc = (hsv.v * (MAX_RGB - ((hsv.s * ((HUE_REGION-1)  - remainder)) /(HUE_REGION-1)))) / MAX_RGB;

    switch (region)
    {
        case 0: // 0 ~ 60 deg
            rgb.r = hsv.v;   rgb.g = inc;     rgb.b = hdp;
            break;
        case 1: // 60 ~ 120 deg
            rgb.r = dec;     rgb.g = hsv.v;   rgb.b = hdp;
            break;
        case 2: // 120 ~ 180 deg
            rgb.r = hdp;     rgb.g = hsv.v;   rgb.b = inc;
            break;
        case 3: // 180 ~ 240 deg
            rgb.r = hdp;     rgb.g = dec;     rgb.b = hsv.v;
            break;
        case 4: // 240 ~ 300 deg
            rgb.r = inc;     rgb.g = hdp;     rgb.b = hsv.v;
            break;
        default: // 300 ~ 360 deg
            rgb.r = hsv.v;   rgb.g = hdp;     rgb.b = dec;
            break;
    }

    return rgb;
}