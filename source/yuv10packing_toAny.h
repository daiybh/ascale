#pragma once
#include <stdio.h>
#include <stdint.h>
#include <type_traits>
#include "putbits.h"

#include "pixelHeader.h"

namespace UYVY10bitBytePacking
{
	static void to_yuv4448bit(const uint8_t *srcUYVY10bitBytePacking, uint8_t *targetYUV8bit, int32_t w, int32_t h)
	{
		const unsigned char *src = (unsigned char *)srcUYVY10bitBytePacking;
		unsigned char *dstUYVY = (unsigned char *)targetYUV8bit;
		const auto nr4Samples = w * h / 2;

		unsigned short values[4];
		for (unsigned i = 0; i < nr4Samples; i++)
		{
			// 98765432
			values[0] = (src[0] << 2) + (src[1] >> 6); // 98765432   10
			values[1] = ((src[1] & 0x3f) << 4) + (src[2] >> 4);
			values[2] = ((src[2] & 0x0f) << 6) + (src[3] >> 2);
			values[3] = ((src[3] & 0x03) << 8) + src[4];

			*dstUYVY++ = (values[1] >> 2) & 0xFF; // Y
			*dstUYVY++ = (values[0] >> 2) & 0xFF; // U
			*dstUYVY++ = (values[2] >> 2) & 0xFF; // V

			*dstUYVY++ = (values[3] >> 2) & 0xFF; // Y
			*dstUYVY++ = (values[0] >> 2) & 0xFF; // U
			*dstUYVY++ = (values[2] >> 2) & 0xFF; // V

			src += 5;
		}
	}

	static void to_UYVY8bit(const uint8_t *srcUYVY10bitBytePacking, uint8_t *targetUYVY8bit, int32_t w, int32_t h)
	{
		const unsigned char *src = (unsigned char *)srcUYVY10bitBytePacking;
		unsigned char *dstUYVY = (unsigned char *)targetUYVY8bit;
		const auto nr4Samples = w * h / 2;

		unsigned short values[4];
		for (unsigned i = 0; i < nr4Samples; i++)
		{
			// 98765432
			values[0] = (src[0] << 2) + (src[1] >> 6); // 98765432   10
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

	static void to_V210(const uint8_t *srcUYVY10bitBytePacking, uint8_t *targetUYVYV210, int32_t width, int32_t height)
	{
#define GET_linePitch(w) (((w + 47) / 48) * 48)
		uint8_t *src = (uint8_t *)srcUYVY10bitBytePacking;
		int linePitch = GET_linePitch(width);
		int lineSize = (width / 48 + ((width % 48) ? 1 : 0)) * 128;
		int32_t step = linePitch / 6;
		for (int h = 0; h < height; h++)
		{
			Pixel2_1xV210 *ppV210 = (Pixel2_1xV210 *)(targetUYVYV210 + h * lineSize);
			for (int i = 0; i < step / 5; i++)
			{
				Pixel2_1xV210 *p0 = ppV210;
				Pixel2_1xV210 *p1 = p0 + 1;
				Pixel2_1xV210 *p2 = p1 + 1;
				Pixel2_1xV210 *p3 = p2 + 1;
				Pixel2_1xV210 *p4 = p3 + 1;
				ppV210 += 5;

#define B10_to_B210(a, b, c, d)                     \
	{                                               \
		a = (src[0] << 2) + (src[1] >> 6);          \
		b = ((src[1] & 0x3f) << 4) + (src[2] >> 4); \
		c = ((src[2] & 0x0f) << 6) + (src[3] >> 2); \
		d = ((src[3] & 0x03) << 8) + src[4];        \
		src += 5;                                   \
	}
				// 4*Pixel2_1xV210 ==3*sizeof(10bitpacking)
				B10_to_B210(p0->U0, p0->Y0, p0->V0, p0->Y1);
				B10_to_B210(p0->U1, p0->Y2, p0->V1, p0->Y3);
				B10_to_B210(p0->U2, p0->Y4, p0->V2, p0->Y5);
				B10_to_B210(p1->U0, p1->Y0, p1->V0, p1->Y1);
				B10_to_B210(p1->U1, p1->Y2, p1->V1, p1->Y3);

				B10_to_B210(p1->U2, p1->Y4, p1->V2, p1->Y5);
				B10_to_B210(p2->U0, p2->Y0, p2->V0, p2->Y1);
				B10_to_B210(p2->U1, p2->Y2, p2->V1, p2->Y3);
				B10_to_B210(p2->U2, p2->Y4, p2->V2, p2->Y5);
				B10_to_B210(p3->U0, p3->Y0, p3->V0, p3->Y1);

				B10_to_B210(p3->U1, p3->Y2, p3->V1, p3->Y3);
				B10_to_B210(p3->U2, p3->Y4, p3->V2, p3->Y5);
				B10_to_B210(p4->U0, p4->Y0, p4->V0, p4->Y1);
				B10_to_B210(p4->U1, p4->Y2, p4->V1, p4->Y3);
				B10_to_B210(p4->U2, p4->Y4, p4->V2, p4->Y5);
			}
		}
	}

	static void to_YUV444_960x540(uint8_t* srcUYVY10bitBytePacking, uint8_t* small960Frame, int32_t width, int32_t height)
	{
		const uint8_t* src = srcUYVY10bitBytePacking;
		uint8_t*       dstUYVY = small960Frame;

		int32_t  lineSize = width * 5 / 2;
		uint16_t values[4];
		for (unsigned h = 0; h < 540; h++)
		{
			for (unsigned w = 0; w < 960; w++)
			{
				values[0] = (src[0] << 2) + (src[1] >> 6); //U
				values[1] = ((src[1] & 0x3f) << 4) + (src[2] >> 4); //Y
				values[2] = ((src[2] & 0x0f) << 6) + (src[3] >> 2); //V
				values[3] = ((src[3] & 0x03) << 8) + src[4]; //Y

				*dstUYVY++ = (values[1] >> 2) & 0xFF; // Y
				*dstUYVY++ = (values[0] >> 2) & 0xFF; // U
				*dstUYVY++ = (values[2] >> 2) & 0xFF; // V

				src += 5;
			}
			src += lineSize;
		}
	}

	static void to_YUV444_480x270(uint8_t* srcUYVY10bitBytePacking, uint8_t* small960Frame, int32_t width, int32_t height)
	{
		const uint8_t* src = srcUYVY10bitBytePacking;
		uint8_t*       dstUYVY = small960Frame;

		int32_t  lineSize = width * 5 / 2;
		uint16_t values[4];
		for (unsigned h = 0; h < 270; h++)
		{
			for (unsigned w = 0; w < 480; w++)
			{
				values[0] = (src[0] << 2) + (src[1] >> 6); //U
				values[1] = ((src[1] & 0x3f) << 4) + (src[2] >> 4); //Y
				values[2] = ((src[2] & 0x0f) << 6) + (src[3] >> 2); //V
				values[3] = ((src[3] & 0x03) << 8) + src[4]; //Y

				*dstUYVY++ = (values[1] >> 2) & 0xFF; // Y
				*dstUYVY++ = (values[0] >> 2) & 0xFF; // U
				*dstUYVY++ = (values[2] >> 2) & 0xFF; // V

				src += 10;
			}
			src += lineSize;
		}
	}
};