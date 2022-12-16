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

	void V210_to_960_540_YUV420(unsigned char* small960Frame, unsigned char* fullFrame)
	{
		int destW=960;
		int destH=540;
		int scaleX=1920/destW;
		int scaleY=1080/destH;

		uint8_t* pDestY = small960Frame;            // dest
		uint8_t* pDestU = pDestY + destW*destH;     // dest
		uint8_t* pDestV = pDestU + ( destW*destH) / 4; // dest

#define GET_linePitch(w) (((w + 47) / 48) * 48)

		int linePitch = GET_linePitch(1920);
		int lineSize = (1920 / 48 + ((1920 % 48) ? 1 : 0)) * 128;
		unsigned int* buffer = nullptr;
		int32_t step = linePitch / 6;

		for (int h = 0,iline=0; h < 1080; h+=scaleY)
		{
			buffer = (unsigned int*)(fullFrame + h * lineSize);
			uint32_t* nextLineBuffer = (uint32_t*)(fullFrame + (h +scaleY) * lineSize);

			int iU=0;
			int iY=0;
			for (int i = 0; i < step; i++)
			{
				Pixel2_1xV210* p = (Pixel2_1xV210*)buffer;
				if (iY++ % scaleX == 0)
					*pDestY++ = p->Y0;
				if (iY++ % scaleX == 0)
					*pDestY++ = p->Y1;
				if (iY++ % scaleX == 0)
					*pDestY++ = p->Y2;
				if (iY++ % scaleX == 0)
					*pDestY++ = p->Y3;
				if (iY++ % scaleX == 0)
					*pDestY++ = p->Y4;
				if (iY++ % scaleX == 0)
					*pDestY++ = p->Y5;
				
				if (iline % scaleY == 0)
				{
					Pixel2_1xV210 *np = (Pixel2_1xV210 *)nextLineBuffer;

					if (iU++ % scaleX == 0)
					{
						*pDestU++ = (p->U0 + np->U0) / 2;
						*pDestV++ = (p->V0 + np->V0) / 2;
					}
					if (iU++ % scaleX == 0)
					{
						*pDestU++ = (p->U1 + np->U1) / 2;
						*pDestV++ = (p->V1 + np->V1) / 2;
					}
					if (iU++ % scaleX == 0)
					{
						*pDestU++ = (p->U2 + np->U2) / 2;
						*pDestV++ = (p->V2 + np->V2) / 2;
					}
				}

				buffer += 4;
				nextLineBuffer += 4;
			}
			iline++;
		}
	}
	/*
	1920/480= 4
	1080/270 = 4

	1280/480= 2.6666
		3-->1
		3-->1
		2-->1 

		8  3
		1280/480 128/48 16/6  8/3 

	720/270 = 2.6666
	
	72/27  8/3
	     3-->1
		 3-->1
		 2-->1
		 
	
	*/

	void V210_to_480_270_YUV420(unsigned char* small960Frame, unsigned char* fullFrame)
	{		
		int destW=480;
		int destH=270;
		
		int scaleX=1920/destW;
		int scaleY=1080/destH;

		uint8_t* pDestY = small960Frame;            // dest
		uint8_t* pDestU = pDestY + destW*destH;     // dest
		uint8_t* pDestV = pDestU + ( destW*destH) / 4; // dest

#define GET_linePitch(w) (((w + 47) / 48) * 48)

		int linePitch = GET_linePitch(1920);
		int lineSize = (1920 / 48 + ((1920 % 48) ? 1 : 0)) * 128;
		unsigned int* buffer = nullptr;
		int32_t step = linePitch / 6;

		for (int h = 0,iline=0; h < 1080; h+=scaleY)
		{
			buffer = (unsigned int*)(fullFrame + h * lineSize);
			uint32_t* nextLineBuffer = (uint32_t*)(fullFrame + (h +scaleY) * lineSize);

			int iU=0;
			int iY=0;
			for (int i = 0; i < step; i++)
			{
				Pixel2_1xV210* p = (Pixel2_1xV210*)buffer;
				if (iY++ % scaleX == 0)
					*pDestY++ = p->Y0;
				if (iY++ % scaleX == 0)
					*pDestY++ = p->Y1;
				if (iY++ % scaleX == 0)
					*pDestY++ = p->Y2;
				if (iY++ % scaleX == 0)
					*pDestY++ = p->Y3;
				if (iY++ % scaleX == 0)
					*pDestY++ = p->Y4;
				if (iY++ % scaleX == 0)
					*pDestY++ = p->Y5;
				
				if (iline % 2 == 0)
				{
					Pixel2_1xV210 *np = (Pixel2_1xV210 *)nextLineBuffer;

					if (iU++ % scaleX == 0)
					{
						*pDestU++ = (p->U0 + np->U0) / 2;
						*pDestV++ = (p->V0 + np->V0) / 2;
					}
					if (iU++ % scaleX == 0)
					{
						*pDestU++ = (p->U1 + np->U1) / 2;
						*pDestV++ = (p->V1 + np->V1) / 2;
					}
					if (iU++ % scaleX == 0)
					{
						*pDestU++ = (p->U2 + np->U2) / 2;
						*pDestV++ = (p->V2 + np->V2) / 2;
					}
				}

				buffer += 4;
				nextLineBuffer += 4;
			}
			iline++;
		}
	}
};
