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
		for (int h = 2; h < height; h++)
		{
			Pixel2_1xV210* ppV210 = (Pixel2_1xV210*)(pV210 + h * lineSize);
			for (int i = 0; i < step/5; i++)
			{
				Pixel2_1xV210* p0 = ppV210;
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
					*pUyvy8++ = svalue0 >> 2;               //value0.98765432
					*pUyvy8++ = (svalue0 << 6) | (svalue1 >> 4);    //10 987654
					*pUyvy8++ = (svalue1 << 4) | (svalue2 >> 6);    //3210 9876
					*pUyvy8++ = (svalue2 << 2) | (svalue3 >> 8);    //543210 98
					*pUyvy8++ = svalue3;                           //76543210	
					
				};
				//4*Pixel2_1xV210 ==3*sizeof(10bitpacking)
				B10_to_B8(p0->U0, p0->Y0, p0->V0, p0->Y1);
				B10_to_B8(p0->U1, p0->Y2, p0->V1, p0->Y3);
				B10_to_B8(p0->U2, p0->Y4,p0->V2, p0->Y5);
				B10_to_B8(p1->U0, p1->Y0, p1->V0,p1->Y1);
				B10_to_B8(p1->U1, p1->Y2, p1->V1, p1->Y3);

				B10_to_B8(p1->U2, p1->Y4, p1->V2, p1->Y5);
				B10_to_B8(p2->U0,p2->Y0, p2->V0, p2->Y1);
				B10_to_B8(p2->U1, p2->Y2,p2->V1, p2->Y3);
				B10_to_B8(p2->U2, p2->Y4, p2->V2,p2->Y5);
				B10_to_B8(p3->U0, p3->Y0, p3->V0, p3->Y1);

				B10_to_B8(p3->U1, p3->Y2, p3->V1, p3->Y3);
				B10_to_B8(p3->U2,p3->Y4, p3->V2, p3->Y5);
				B10_to_B8(p4->U0, p4->Y0,p4->V0, p4->Y1);
				B10_to_B8(p4->U1, p4->Y2, p4->V1,p4->Y3);
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
			for (int i = 0; i < left/2; i++)
			{
				B10_to_B8(p0->U0, p0->Y0, p0->V0, p0->Y1);
			}
		}
	}
};