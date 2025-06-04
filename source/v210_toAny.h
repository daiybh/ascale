#include <stdio.h>
#include <stdint.h>
#include <type_traits>
#include "putbits.h"
#include "pixelHeader.h"
namespace v210
{
	void to_UYVY10packing(uint8_t *pV210, uint8_t *pUyvy8, int width, int height)
	{
#define GET_linePitch(w) (((w + 47) / 48) * 48)

		int linePitch = GET_linePitch(width);
		int lineSize = (width / 48 + ((width % 48) ? 1 : 0)) * 128;
		int32_t step = linePitch / 6;
		for (int h = 0; h < height; h++)
		{
			Pixel2_1xV210 *ppV210 = (Pixel2_1xV210 *)(pV210 + h * lineSize);
			for (int i = 0; i < step / 5; i++)
			{
				Pixel2_1xV210 *p0 = ppV210;
				Pixel2_1xV210 *p1 = p0 + 1;
				Pixel2_1xV210 *p2 = p1 + 1;
				Pixel2_1xV210 *p3 = p2 + 1;
				Pixel2_1xV210 *p4 = p3 + 1;
				ppV210 += 5;
				auto B10_to_B8 = [&](short svalue0, short svalue1, short svalue2, short svalue3)
				{
					/*
					|------value---| |-----value1---| |-----value2---| |-----value3---|
					xxxxxx9876543210 xxxxxx9876543210 xxxxxx9876543210 xxxxxx9876543210
						  9876543210       9876543210       9876543210       9876543210
					>>2   98765432
								  10       9876543210       9876543210       9876543210
								  10       987654
												 3210       9876543210       9876543210
												 3210       9876
																543210       9876543210
																543210       98
																			   76543210
					*/
					*pUyvy8++ = svalue0 >> 2;					 // value0.98765432
					*pUyvy8++ = (svalue0 << 6) | (svalue1 >> 4); // 10 987654
					*pUyvy8++ = (svalue1 << 4) | (svalue2 >> 6); // 3210 9876
					*pUyvy8++ = (svalue2 << 2) | (svalue3 >> 8); // 543210 98
					*pUyvy8++ = svalue3;						 // 76543210
				};
				// 4*Pixel2_1xV210 ==3*sizeof(10bitpacking)
				B10_to_B8(p0->U0, p0->Y0, p0->V0, p0->Y1);
				B10_to_B8(p0->U1, p0->Y2, p0->V1, p0->Y3);
				B10_to_B8(p0->U2, p0->Y4, p0->V2, p0->Y5);
				B10_to_B8(p1->U0, p1->Y0, p1->V0, p1->Y1);
				B10_to_B8(p1->U1, p1->Y2, p1->V1, p1->Y3);

				B10_to_B8(p1->U2, p1->Y4, p1->V2, p1->Y5);
				B10_to_B8(p2->U0, p2->Y0, p2->V0, p2->Y1);
				B10_to_B8(p2->U1, p2->Y2, p2->V1, p2->Y3);
				B10_to_B8(p2->U2, p2->Y4, p2->V2, p2->Y5);
				B10_to_B8(p3->U0, p3->Y0, p3->V0, p3->Y1);

				B10_to_B8(p3->U1, p3->Y2, p3->V1, p3->Y3);
				B10_to_B8(p3->U2, p3->Y4, p3->V2, p3->Y5);
				B10_to_B8(p4->U0, p4->Y0, p4->V0, p4->Y1);
				B10_to_B8(p4->U1, p4->Y2, p4->V1, p4->Y3);
				B10_to_B8(p4->U2, p4->Y4, p4->V2, p4->Y5);
			}
		}
	}

	void to_UYVY8(uint8_t *pV210, uint8_t *pUyvy8, int width, int height)
	{
#define GET_linePitch(w) (((w + 47) / 48) * 48)

		int linePitch = GET_linePitch(width);
		int lineSize = (width / 48 + ((width % 48) ? 1 : 0)) * 128;
		unsigned int *buffer = nullptr;
		int32_t step = width / 6;
		for (int h = 0; h < height; h++)
		{
			auto B10_to_B8 = [&](short svalue0, short svalue1, short svalue2, short svalue3)
			{
				uint8_t value0 = (svalue0 >> 2) & 0xFF;
				uint8_t value1 = (svalue1 >> 2) & 0xFF;
				uint8_t value2 = (svalue2 >> 2) & 0xFF;
				uint8_t value3 = (svalue3 >> 2) & 0xFF;
				uint8_t src[4] = {value0, value1, value2, value3};

				*pUyvy8++ = (src[0]);
				*pUyvy8++ = (src[1]);
				*pUyvy8++ = (src[2]);
				*pUyvy8++ = (src[3]);
			};

			Pixel2_1xV210 *ppV210 = (Pixel2_1xV210 *)(pV210 + h * lineSize);

			for (int i = 0; i < step; i++)
			{
				Pixel2_1xV210 *p0 = ppV210++;

				B10_to_B8(p0->U0, p0->Y0, p0->V0, p0->Y1);
				B10_to_B8(p0->U1, p0->Y2, p0->V1, p0->Y3);
				B10_to_B8(p0->U2, p0->Y4, p0->V2, p0->Y5);
			}
			int left = width - step * 6;
			Pixel2_1xV210 *p0 = ppV210;
			for (int i = 0; i < left / 2; i++)
			{
				B10_to_B8(p0->U0, p0->Y0, p0->V0, p0->Y1);
			}
		}
	}

	void handleYUV_3840(uint8_t *&cur, int32_t c, int32_t d, int32_t e)
	{
		*cur++ = (c >> 2) & 0xFF;
		*cur++ = (d >> 2) & 0xFF;
		*cur++ = (e >> 2) & 0xFF;
	};
	void V210_to_YUV_Any(uint8_t *small960Frame, uint8_t *fullFrame,
							  int width, int height,
							  int destWidth, int destHeight)
	{
		// 计算缩放比例
		int scale_x = width / destWidth;
		int scale_y = height / destHeight;

		int t_width = destWidth * 3; // 每行字节数
#define GET_linePitch(w) (((w + 47) / 48) * 48)
		int32_t linePitch = GET_linePitch(width);
		int32_t lineSize = (width / 48 + ((width % 48) ? 1 : 0)) * 128;
		printf("\n");
		for (int dy = 0; dy < destHeight; ++dy)
		{
			uint8_t *cur = small960Frame + dy * t_width;
			int sy = dy * scale_y; // 源图像采样行
			int32_t *buffer = (int32_t *)(fullFrame + sy * lineSize);

			// 每组 Pixel2_1xV210 结构体包含 6 个像素
			int src_pixels_per_group = 6;
			int groups = width / src_pixels_per_group;
			// 3840y--->960y   4/1
				// 3840y--->480y   8/1
				//
				// 1920y--->960y   2/1
				// 1920y--->480y   4/1
			for (int dx = 0; dx < destWidth; ++dx)
			{
				int sx = dx * scale_x; // 源图像采样列
				int group_idx = sx / src_pixels_per_group;
				int pixel_in_group = sx % src_pixels_per_group;

				Pixel2_1xV210 *p = ((Pixel2_1xV210 *)buffer) + group_idx;
				if(dy==0 && dx<10)
				printf("w:%2d->%2d scale:%2d,%2d dy:%2d dx:%2d sx:%2d group_idx:%2d pixel_in_group:%2d ,p :%p\n",width,destWidth,scale_x,scale_y,dy,dx,sx,group_idx,pixel_in_group,p);
				// 取出对应像素的 YUV 分量
				int y = 0, u = 0, v = 0;
				switch (pixel_in_group)
				{
				case 0:					y = p->Y0;					u = p->U0;					v = p->V0;					break;
				case 1:					y = p->Y1;					u = p->U0;					v = p->V0;					break;
				case 2:					y = p->Y2;					u = p->U1;					v = p->V1;					break;
				case 3:					y = p->Y3;					u = p->U1;					v = p->V1;					break;
				case 4:					y = p->Y4;					u = p->U2;					v = p->V2;					break;
				case 5:					y = p->Y5;					u = p->U2;					v = p->V2;					break;
				}
				handleYUV_3840(cur, y, u, v);
			}
		}
	}
};