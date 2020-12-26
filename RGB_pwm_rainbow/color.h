#include <stdint.h>

typedef struct RgbColor
{
    uint32_t r;
    uint32_t g;
    uint32_t b;
} RgbColor;

typedef struct HsvColor
{
    uint32_t h;
    uint32_t s;
    uint32_t v;
} HsvColor;

RgbColor HsvToRgb(HsvColor hsv);