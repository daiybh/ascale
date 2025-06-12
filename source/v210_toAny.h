#include <stdio.h>
#include <stdint.h>
#include <type_traits>
#include "putbits.h"
#include "pixelHeader.h"
#include <cstring>
#include <iostream>
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
		float scale_x = (float)width / destWidth;
		float scale_y = (float)height / destHeight;

		int t_width = destWidth * 3; // 每行字节数
#define GET_linePitch(w) (((w + 47) / 48) * 48)
		int32_t linePitch = GET_linePitch(width);
		int32_t lineSize = (width / 48 + ((width % 48) ? 1 : 0)) * 128;

		for (int dy = 0; dy < destHeight; ++dy)
		{
			uint8_t *cur = small960Frame + dy * t_width;
			int sy = (int)(dy * scale_y + 0.5f); // 源图像采样行，四舍五入
			int32_t *buffer = (int32_t *)(fullFrame + sy * lineSize);

			int src_pixels_per_group = 6;
			for (int dx = 0; dx < destWidth; ++dx)
			{
				int sx = (int)(dx * scale_x + 0.5f); // 源图像采样列，四舍五入
				int group_idx = sx / src_pixels_per_group;
				int pixel_in_group = sx % src_pixels_per_group;
				
				Pixel2_1xV210 *p = ((Pixel2_1xV210 *)buffer) + group_idx;
				int y = 0, u = 0, v = 0;
				switch (pixel_in_group)
				{
				case 0: y = p->Y0; u = p->U0; v = p->V0; break;
				case 1: y = p->Y1; u = p->U0; v = p->V0; break;
				case 2: y = p->Y2; u = p->U1; v = p->V1; break;
				case 3: y = p->Y3; u = p->U1; v = p->V1; break;
				case 4: y = p->Y4; u = p->U2; v = p->V2; break;
				case 5: y = p->Y5; u = p->U2; v = p->V2; break;
				}
				handleYUV_3840(cur, y, u, v);
			}
		}
	}
			//eighth
			//2160/270    8/1  24-->3
			// |----||----||----||----| |----||----||----||----| |----||----||----|
			// 012345012345012345012345 012345012345012345012345 012345012345012345012345
			// |-------|-------|------- |-------|-------|------- |-------|---|---|---|---
			// 0       2       4        0       2       4
			// 4Group -->3 pixel
			// 
			// half
			// 1080/960  2/1   12-->6
			// 
			// quarter
			//2160/960    4/1
			//1080/270    4/1   24-->6  12-->3
			// |----||----| |----||----| |----||----||----||----||----||----||----|
			// 012345012345 012345012345 012345012345012345012345012345012345012345012345
			// |---|---|--- |---|---|--- |---|---|---|---|---|---|---|---|---|---|---|---
			// 0   4   2    0   4   2    0   4   2
			// 2Group -->3pixel

	void V210_to_YUV_8_to_1(uint8_t* small960Frame, uint8_t* fullFrame,
		int width, int height,
		int destWidth, int destHeight)
	{
		float scale_x = (float)width / destWidth;
		float scale_y = (float)height / destHeight;

		int t_width = destWidth * 3; // 每行字节数
#define GET_linePitch(w) (((w + 47) / 48) * 48)
		int32_t linePitch = GET_linePitch(width);
		int32_t lineSize = (width / 48 + ((width % 48) ? 1 : 0)) * 128;

		int32_t  step = linePitch / 24;
		for (int dy = 0; dy < destHeight; ++dy)
		{
			uint8_t* cur = small960Frame + dy * t_width;
			int sy = (int)(dy * scale_y + 0.5f); // 源图像采样行，四舍五入
			Pixel2_1xV210* p = (Pixel2_1xV210*)(fullFrame + sy * lineSize);
			for (int dx = 0; dx < step; ++dx)
			{
				handleYUV_3840(cur, p->Y0, p->U0, p->V0); p++;
				handleYUV_3840(cur, p->Y2, p->U1, p->V1); p++;
				handleYUV_3840(cur, p->Y4, p->U2, p->V2); p++;
				p++;
			}
		}
	}
	void V210_to_YUV_4_to_1(uint8_t* small960Frame, uint8_t* fullFrame,
		int width, int height,
		int destWidth, int destHeight)
	{
		float scale_x = (float)width / destWidth;
		float scale_y = (float)height / destHeight;

		int t_width = destWidth * 3; // 每行字节数
#define GET_linePitch(w) (((w + 47) / 48) * 48)
		int32_t linePitch = GET_linePitch(width);
		int32_t lineSize = (width / 48 + ((width % 48) ? 1 : 0)) * 128;

		int32_t  step = linePitch / 12;
		for (int dy = 0; dy < destHeight; ++dy)
		{
			uint8_t* cur = small960Frame + dy * t_width;
			int sy = (int)(dy * scale_y + 0.5f); // 源图像采样行，四舍五入
			Pixel2_1xV210* p = (Pixel2_1xV210*)(fullFrame + sy * lineSize);
			for (int dx = 0; dx < step; ++dx)
			{
				handleYUV_3840(cur, p->Y0, p->U0, p->V0);
				handleYUV_3840(cur, p->Y4, p->U2, p->V2);
				p++;
				handleYUV_3840(cur, p->Y2, p->U1, p->V1);
				p++;
			}
		}
	}
	void V210_to_YUV_2_to_1(uint8_t* small960Frame, uint8_t* fullFrame,
		int width, int height,
		int destWidth, int destHeight)
	{
		float scale_x = (float)width / destWidth;
		float scale_y = (float)height / destHeight;

		int t_width = destWidth * 3; // 每行字节数
#define GET_linePitch(w) (((w + 47) / 48) * 48)
		int32_t linePitch = GET_linePitch(width);
		int32_t lineSize = (width / 48 + ((width % 48) ? 1 : 0)) * 128;

		int32_t  step = linePitch / 12;
		for (int dy = 0; dy < destHeight; ++dy)
		{
			uint8_t* cur = small960Frame + dy * t_width;
			int sy = (int)(dy * scale_y + 0.5f); // 源图像采样行，四舍五入
			Pixel2_1xV210* p = (Pixel2_1xV210*)(fullFrame + sy * lineSize);
			for (int dx = 0; dx < step; ++dx)
			{
				handleYUV_3840(cur, p->Y0, p->U0, p->V0);//01
				handleYUV_3840(cur, p->Y2, p->U1, p->V1);//23
				handleYUV_3840(cur, p->Y4, p->U2, p->V2);//45
				p++;
				handleYUV_3840(cur, p->Y0, p->U0, p->V0);//01
				handleYUV_3840(cur, p->Y2, p->U1, p->V1);//23
				handleYUV_3840(cur, p->Y4, p->U2, p->V2);//45
				p++;
			}
		}
	}

	void Scaler_3840_2160_V210_to_YUV480x270(uint8_t* small480Frame, uint8_t* fullFrame)
	{
		uint8_t* line = small480Frame; //dest
		uint8_t* cur;
		int32_t  t_width = 0;
		int32_t  destWidth = 480;
		t_width = destWidth * 3;

#define GET_linePitch(w) (((w + 47) / 48) * 48)

		int32_t linePitch = GET_linePitch(3840);
		int32_t lineSize = (3840 / 48 + ((3840 % 48) ? 1 : 0)) * 128;

		int32_t* buffer = nullptr;
		int32_t  step = linePitch / 24;
		for (int32_t h = 0; h < 2160; h++)
		{
			if (h % 8 != 0)continue;
			cur = line;
			buffer = (int32_t*)(fullFrame + h * lineSize);
			for (int32_t i = 0; i < step; i++)
			{
				Pixel2_1xV210* p0 = (Pixel2_1xV210*)buffer;
				buffer += 4;
				Pixel2_1xV210* p1 = (Pixel2_1xV210*)buffer;
				buffer += 4;
				Pixel2_1xV210* p2 = (Pixel2_1xV210*)buffer;
				buffer += 4;
				//Pixel2_1xV210* p3 = (Pixel2_1xV210*)buffer; 
				//24y-->3y   1/8
				//01234501 23450123 45012345 
				//0        2        4 
				handleYUV_3840(cur, p0->Y0, p0->U0, p0->V0);
				handleYUV_3840(cur, p1->Y2, p1->U1, p1->V1);
				handleYUV_3840(cur, p2->Y4, p2->U2, p2->V2);

				buffer += 4;
			}
			line += t_width;
		}
	}


	void Scaler_3840_2160_V210_to_YUV960x540(uint8_t* small960Frame, uint8_t* fullFrame)
	{
		uint8_t* line = small960Frame; //dest
		uint8_t* cur;
		int32_t  t_width = 0;
		int32_t  destWidth = 960;
		t_width = destWidth * 3;

#define GET_linePitch(w) (((w + 47) / 48) * 48)

		int32_t linePitch = GET_linePitch(3840);
		int32_t lineSize = (3840 / 48 + ((3840 % 48) ? 1 : 0)) * 128;

		int32_t* buffer = nullptr;
		int32_t  step = linePitch / 12;
		for (int32_t h = 0; h < 2160; h++)
		{
			if (h % 4 != 0)continue;
			cur = line;
			buffer = (int32_t*)(fullFrame + h * lineSize);
			for (int32_t i = 0; i < step; i++)
			{
				Pixel2_1xV210* p = (Pixel2_1xV210*)buffer;
				buffer += 4;
				Pixel2_1xV210* p1 = (Pixel2_1xV210*)buffer;
				//y0y1y2y3 y4y5y0y1 y2y3y4y5
				//12y-->3y   1/4
				//0123 4501 2345 
				//0    4    2
				handleYUV_3840(cur, p->Y0, p->U0, p->V0);
				handleYUV_3840(cur, p->Y4, p->U2, p->V2);
				handleYUV_3840(cur, p1->Y2, p1->U1, p1->V1);

				buffer += 4;
			}
			line += t_width;
		}
	}
	void Scaler_1920_1080_V210_to_YUV480x270(uint8_t* small480Frame, uint8_t* fullFrame)
	{
		uint8_t* line = small480Frame; //dest
		uint8_t* cur = nullptr;
		int32_t  destWidth = 480;
		int32_t  t_width = destWidth * 3;

#define GET_linePitch(w) (((w + 47) / 48) * 48)

		int32_t  linePitch = GET_linePitch(1920);
		int32_t  lineSize = (1920 / 48 + ((1920 % 48) ? 1 : 0)) * 128;
		uint16_t U1_10b;
		uint16_t Y1_10b;
		uint16_t V1_10b;
		uint16_t Y2_10b;
		uint16_t U2_10b;
		uint16_t Y3_10b;
		uint16_t V2_10b;
		uint16_t Y4_10b;
		uint16_t U3_10b;
		uint16_t Y5_10b;
		uint16_t V3_10b;
		uint16_t Y6_10b;
		int32_t* buffer = nullptr;
		int32_t  step = linePitch / 12;
		for (int32_t h = 0; h < 1080; h++)
		{
			if (h % 4 != 0)
				continue;
			cur = line;
			buffer = (int32_t*)(fullFrame + h * lineSize);
			
			//each 
			for (int32_t i = 0; i < step; i++)
			{
				U1_10b = (0x3FF & buffer[0]);
				Y1_10b = (0xFF300 & buffer[0]) >> 10;
				V1_10b = (0x3ff00000 & buffer[0]) >> 20;
				//Y2_10b = (0x3FF & buffer[1]);
				U2_10b = (0xFF300 & buffer[1]) >> 10;
				Y3_10b = (0x3ff00000 & buffer[1]) >> 20;
				V2_10b = (0x3FF & buffer[2]);
				//Y4_10b = (0xFF300 & buffer[2]) >> 10;
				U3_10b = (0x3ff00000 & buffer[2]) >> 20;
				Y5_10b = (0x3FF & buffer[3]);
				V3_10b = (0xFF300 & buffer[3]) >> 10;
				//Y6_10b = (0x3ff00000 & buffer[3]) >> 20;

				handleYUV_3840(cur, Y1_10b, U1_10b, V1_10b);
				handleYUV_3840(cur, Y3_10b, U2_10b, V2_10b);
				handleYUV_3840(cur, Y5_10b, U3_10b, V3_10b);

				buffer += 8;
			}
			line += t_width;
		}
	}


};