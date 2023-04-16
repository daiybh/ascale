#pragma once
#include <stdio.h>
#include <stdint.h>
#include <type_traits>
#include "putbits.h"

#include "pixelHeader.h"

void UYVY8bit_to_v210(const uint8_t* sourceUYVY8bits, uint8_t* targetV210, int32_t width, int32_t height)
{
	const unsigned char* src = (unsigned char*)sourceUYVY8bits;
#define GET_linePitch(w) (((w + 47) / 48) * 48)
    int linePitch = GET_linePitch(width);
    int lineSize = (width / 48 + ((width % 48) ? 1 : 0)) * 128;
    unsigned int* buffer = nullptr;
    int32_t step = linePitch / 6;
    for (int h = 0; h < height; h++)
    {
        buffer = (unsigned int*)(targetV210 + h * lineSize);
        for (int i = 0; i < step; i++)
        {
            Pixel2_1xV210* p0 = (Pixel2_1xV210*)buffer;
            buffer += 4;
            auto B8_to_B10A = [&]() {
				short p = *src++; p = p << 2;
				return p;
			};
#define B8_to_B10(x){ x=B8_to_B10A();}
			B8_to_B10(p0->U0); B8_to_B10(p0->Y0);  B8_to_B10(p0->V0); B8_to_B10(p0->Y1);
			B8_to_B10(p0->U1); B8_to_B10(p0->Y2);  B8_to_B10(p0->V1); B8_to_B10(p0->Y3);
			B8_to_B10(p0->U2); B8_to_B10(p0->Y4);  B8_to_B10(p0->V2); B8_to_B10(p0->Y5);
		}
    }
}


static void UYVY8bit_to_UYVY10bitBytePacking(const uint8_t* sourceUYVY8bits, uint8_t* targetUYVY10bitBytePacking, int32_t w, int32_t h)
{
	const unsigned char* src = (unsigned char*)sourceUYVY8bits;
	unsigned char* dstUYVY = (unsigned char*)targetUYVY10bitBytePacking;
	const auto           nr4Samples = w * h / 2;//w*h*2/4

	for (unsigned i = 0; i < nr4Samples; i++)
	{
		/*
		|-byte0-|| -byte1-||-byte2 -|| -byte3 - | 
		 98765432 10987654  32109876   54321098 76543210
		|---U------| ----Y---- - | ----V---- - | ----Y---- - |
		 98765432 xx 987654 32xx   9876 5432xx   98765432xx

		 98765432 
		           98765432>>2=987654
				         32xx   |src2[2] 5432
			*/
		*dstUYVY++ = src[0];       //src[0] U
		*dstUYVY++ = (src[1] >> 2);  //src[1] Y
		*dstUYVY++ = (src[1] << 6) | (src[2] >> 4);
		*dstUYVY++ = (src[2] << 4) | (src[3] >> 6);
		*dstUYVY++ = (src[3] << 2);

		src += 4;
	}
}