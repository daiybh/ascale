#pragma once
#include <stdint.h>
#pragma pack(push, 1)

struct Pixel2_1xV210
{
    uint32_t U0 : 10;
    uint32_t Y0 : 10;
    uint32_t V0 : 10;
    uint32_t : 2;

    uint32_t Y1 : 10;
    uint32_t U1 : 10;
    uint32_t Y2 : 10;
    uint32_t : 2;

    uint32_t V1 : 10;
    uint32_t Y3 : 10;
    uint32_t U2 : 10;
    uint32_t : 2;

    uint32_t Y4 : 10;
    uint32_t V2 : 10;
    uint32_t Y5 : 10;
    uint32_t : 2;
};
struct Pixel_UYVY8bit {
    uint8_t U0;
    uint8_t Y0;
    uint8_t V0;
    uint8_t Y1;
};

struct Pixel_10packingA
{
    uint16_t U0 : 10; // FF 2
    uint16_t a : 6;
    uint16_t Y0 : 10; //   2 FF
    uint16_t V0 : 10; //        FF 2
};
struct Pixel_10packingAA
{
    // 40  5
    // FFFFFFFF
    uint16_t U0 : 10; // FF 2
    uint16_t Y0 : 10; //   2 FF
    uint16_t V0 : 10; //        FF 2
    uint16_t Y1 : 10; //           2 FF      //2(FF FF FF FF) +8
    // FF F FF FF F FF
    // FF FF FF FF FF
    //
// uint32_t:32 - 8;//FFFF
};
struct Pixel_10packing
{
    uint32_t U0 : 10;
    uint32_t Y0 : 10;
    uint32_t V0 : 10;
    uint32_t Y1 : 10;

    uint32_t U1 : 10;
    uint32_t Y2 : 10;
    uint32_t V1 : 10;
    uint32_t Y3 : 10;

    uint32_t U2 : 10;
    uint32_t Y4 : 10;
    uint32_t V2 : 10;
    uint32_t Y5 : 10;

    uint32_t U3 : 10;
    uint32_t Y6 : 10;
    uint32_t V3 : 10;
    uint32_t Y7 : 10;
};

#pragma pack(pop)