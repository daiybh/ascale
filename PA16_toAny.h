#include <stdio.h>
#include <stdint.h>
#include <type_traits>
#include "putbits.h"
#include "pixelHeader.h"
namespace PA16
{
	void to_UYVY8(uint8_t* pA16,int srcWidth,int srcHeight, uint8_t* pUyvy8, int width, int height,uint8_t* alphaBuffer)
	{
		//PA16 4224 16pp with NV12
		//YYYYYYYYYYYYYYYY
		//UV  UV  UV  UV
		//AAAAAAAAAAAAAAAA

		uint16_t* pA16Y = (uint16_t*)pA16;
		uint16_t* pA16U = (uint16_t*)(pA16 + srcWidth * srcHeight * 2);
		
		uint8_t* pAlpha = pA16 + srcHeight * srcWidth * 2 * 2;
		memcpy(alphaBuffer, pAlpha,srcWidth*srcHeight);
		for (int h = 0; h < srcHeight; h++)
		{
			uint16_t*pLineY= pA16Y +h*srcWidth;
			uint16_t*pLineU= pA16U +h*srcWidth;		
			for (int w = 0; w < srcWidth/2; w++)
			{
				uint16_t y1 = *pLineY++;
				uint16_t u = *pLineU++;
				uint16_t v = *pLineU++;
				uint16_t y2 = *pLineY++;

				
				*pUyvy8++ = (u>>8)&0xFF;
				*pUyvy8++ = (y1>>8)&0xFF;
				*pUyvy8++ = (v>>8)&0xFF;
				*pUyvy8++ = (y2>>8)&0xFF;
			}
		}
	}
	template<typename T>
	void to_yuv422(uint8_t* pA16, int srcWidth, int srcHeight, T* pUyvy8, int width, int height, uint8_t* alphaBuffer)
	{
		//PA16 4224 16pp with NV12
		//YYYYYYYYYYYYYYYY
		//UV  UV  UV  UV
		//AAAAAAAAAAAAAAAA

		uint16_t* pA16Y = (uint16_t*)pA16;
		uint16_t* pA16U = (uint16_t*)(pA16 + srcWidth * srcHeight * 2);
		uint16_t* pAlpha = (uint16_t*)(pA16 + srcHeight * srcWidth * 2 * 2);
		//memcpy(alphaBuffer, pAlpha, srcWidth * srcHeight);
		auto destY = (T*)pUyvy8;
		auto destU = (T*)(pUyvy8 + srcHeight * srcWidth );
		auto destV = (T*)(pUyvy8 + srcHeight * srcWidth*3/2);
		auto destAlpha = (T*)alphaBuffer;
		//*
		//memcpy(destY, pA16Y,1920 * 1080 * 2);		return;
		auto flag = std::is_same_v<T, uint16_t>;
		auto pLimit = [&flag](uint16_t v)
			{
				if (flag)return v;
				return uint16_t((v >> 8) & 0xff);
			};
		for (int x = 0; x < srcWidth * srcHeight / 2; x++)
		{
			*destY++ = pLimit(*pA16Y++);
			*destY++ = pLimit(*pA16Y++);
			*destU++ = pLimit(*pA16U++);
			*destV++ = pLimit(*pA16U++);			

			*destAlpha++ = pLimit(*pAlpha++);
			*destAlpha++ = pLimit(*pAlpha++);
		}
		return;/**/
	}
	void to_yuv444_480270(uint8_t* pA16, int srcWidth, int srcHeight, uint8_t* pYUV444)
	{
		uint16_t* pA16Y = (uint16_t*)pA16;
		uint16_t* pA16U = (uint16_t*)(pA16 + srcWidth * srcHeight * 2);
		uint16_t* pAlpha = (uint16_t*)(pA16 + srcHeight * srcWidth * 2 * 2);

		uint8_t* destY = (uint8_t*)pYUV444;

		//1920-->480   4
		//1080-->270   4

		for (int h = 0; h < srcHeight; h += 4)
		{
			uint16_t* pLIneY = pA16Y + h * srcWidth;
			uint16_t* pLIneU = pA16U + h * srcWidth;
			for (int w = 0; w < srcWidth; w += 2 * 4)
			{
				uint16_t y1 = pLIneY[w];
				uint16_t u = pLIneU[w];
				uint16_t v = pLIneU[w + 1];
				uint16_t y2 = pLIneY[w + 1];

				*destY++ = (y1 >> 8) & 0xFF;
				*destY++ = (u >> 8) & 0xFF;
				*destY++ = (v >> 8) & 0xFF;

				*destY++ = (y2 >> 8) & 0xFF;
				*destY++ = (u >> 8) & 0xFF;
				*destY++ = (v >> 8) & 0xFF;
			}
		}
	}
	
}

#define getDestFilePath(pre) std::format("{}\\{}_{}x{}.yuv",destFolder,pre,w,h).data()
void test_pA16(const char* _destFolder)
{
	char destFolder[255];
	strcpy(destFolder, _destFolder);
	strcat(destFolder, "\\pA16\\");
	std::filesystem::create_directories(destFolder);


	int w = 1920;
	int h = 1080;
	uint8_t* targetBuffer = new uint8_t[1920 * 1080 * 10];
	uint8_t* sourceBuffer = new uint8_t[1920 * 1080 * 10];
	uint8_t* alphaBuffer = new uint8_t[1920 * 1080 * 10];
	int nLength = w * h * 2 * 3;
	//yyyy w*h*2
	//uv   w*h*2
	//alpha w*h*2
	FILE* fps, * fpD, * fpD16, * fpDAlpha;

	//fps = fopen(R"(D:\clips\p216\pa16.yuv)", "rb");
	fps = fopen(R"(Z:\Cloud Sync\clips\YUV files\ClipsFromBryan\recPA16.yuv)", "rb");
	fpD = fopen(getDestFilePath("pA16_to_yuv4228bit_1920x1080.yuv"), "wb");
	fpD16 = fopen(getDestFilePath("pA16_to_yuv4228bit_1920x1080_16.yuv"), "wb");
	fpDAlpha = fopen(getDestFilePath("pA16_to_yuv4228bit_1920x1080_alpha.yuv"), "wb");


	for (int i = 0; i < 100; i++)
	{
		if (1 != fread(sourceBuffer, nLength, 1, fps))
			break;

		memset(targetBuffer, 0, 1920 * 1080 * 6);
		memset(alphaBuffer, 0, 1920 * 1080 * 6);
		PA16::to_yuv422(sourceBuffer, 1920, 1080, targetBuffer, 1920, 1080, alphaBuffer);
		 fwrite(targetBuffer, 1920*1080*2, 1, fpD);
		 fwrite(alphaBuffer, 1920*1080*2, 1, fpD);

		 uint16_t* pDest = (uint16_t*)targetBuffer;
		 PA16::to_yuv422(sourceBuffer, 1920, 1080, pDest, 1920, 1080, alphaBuffer);
		 fwrite(targetBuffer, 1920 * 1080 * 2*2, 1, fpD16);
		 fwrite(alphaBuffer, 1920 * 1080 * 2*2, 1, fpD16);
		//PA16::to_yuv444_480270(sourceBuffer, 1920, 1080, targetBuffer);
		//fwrite(targetBuffer, 480*270 * 3, 1, fpD);
	}
	fclose(fpD);
	fclose(fps);
}