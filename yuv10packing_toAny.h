#pragma once
#include <stdio.h>
#include <stdint.h>
#include <type_traits>
#include "putbits.h"

#include "pixelHeader.h"



static void Convert_UYVY10bitBytePacking_to_UYVY8bit(const uint8_t* srcUYVY10bitBytePacking, uint8_t* targetUYVY8bit, int32_t w, int32_t h)
{
	const unsigned char* src = (unsigned char*)srcUYVY10bitBytePacking;
	unsigned char* dstUYVY = (unsigned char*)targetUYVY8bit;
	const auto           nr4Samples = w * h / 2;

	unsigned short values[4];
	for (unsigned i = 0; i < nr4Samples; i++)
	{
		//98765432
		values[0] = (src[0] << 2) + (src[1] >> 6);  //98765432   10
		values[1] = ((src[1] & 0x3f) << 4) + (src[2] >> 4);
		values[2] = ((src[2] & 0x0f) << 6) + (src[3] >> 2);
		values[3] = ((src[3] & 0x03) << 8) + src[4];

		*dstUYVY++ = (values[0] >> 2) & 0xFF;
		*dstUYVY++ = (values[1] >> 2) & 0xFF;
		*dstUYVY++ = (values[2] >> 2) & 0xFF;
		*dstUYVY++ = (values[3] >> 2) & 0xFF;
		src += 5;
	}
}
