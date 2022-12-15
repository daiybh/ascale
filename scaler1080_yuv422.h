#include <stdio.h>
namespace Scaler_1920_1080YUV422
{

#pragma pack(push,1)

	struct Pixel2_1xV210
	{
		uint32_t : 2;
		uint32_t U0 : 8;
		uint32_t : 2;
		uint32_t Y0 : 8;
		uint32_t : 2;
		uint32_t V0 : 8;
		uint32_t : 2;


		uint32_t : 2;
		uint32_t Y1 : 8;
		uint32_t : 2;
		uint32_t U1 : 8;
		uint32_t : 2;
		uint32_t Y2 : 8;
		uint32_t : 2;


		uint32_t : 2;
		uint32_t V1 : 8;
		uint32_t : 2;
		uint32_t Y3 : 8;
		uint32_t : 2;
		uint32_t U2 : 8;
		uint32_t : 2;

		uint32_t : 2;
		uint32_t Y4 : 8;
		uint32_t : 2;
		uint32_t V2 : 8;
		uint32_t : 2;
		uint32_t Y5 : 8;
		uint32_t : 2;
	};

#pragma pack(pop)

	void handleYUV(unsigned char*& cur, int c, int d, int e)
	{
		*cur++ = (c >> 2) & 0xFF;
		*cur++ = (d >> 2) & 0xFF;
		*cur++ = (e >> 2) & 0xFF;
	};

	void V210_to_960_540_YUV422(unsigned char* small960Frame, unsigned char* fullFrame)
	{
		uint8_t* pDestY = small960Frame;            // dest
		uint8_t* pDestU = pDestY + 1920 * 1080;     // dest
		uint8_t* pDestV = pDestU + 1920 * 1080 / 2; // dest
		unsigned char* cur;
		int t_width = 0;
		int destWidth = 960;
		t_width = destWidth * 3;

#define clip(var) ((var >= 255) ? 255 : (var <= 0) ? 0 \
                                                   : var)
#define GET_linePitch(w) (((w + 47) / 48) * 48)

		int linePitch = GET_linePitch(1920);
		int lineSize = (1920 / 48 + ((1920 % 48) ? 1 : 0)) * 128;
		unsigned int* buffer = nullptr;
		int32_t step = linePitch / 6;
		for (int h = 0; h < 1080; h++)
		{
			buffer = (unsigned int*)(fullFrame + h * lineSize);
			for (int i = 0; i < step; i++)
			{
				Pixel2_1xV210* p = (Pixel2_1xV210*)buffer;

				*pDestU++ = p->U0;
				*pDestY++ = p->Y0;
				*pDestV++ = p->V0;
				*pDestY++ = p->Y1;

				*pDestU++ = p->U1;
				*pDestY++ = p->Y2;
				*pDestV++ = p->V1;
				*pDestY++ = p->Y3;

				*pDestU++ = p->U2;
				*pDestY++ = p->Y4;
				*pDestV++ = p->V2;
				*pDestY++ = p->Y5;

				buffer += 4;
			}
		}
	}
};
