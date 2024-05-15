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
	void to_yuv444_480270(uint8_t* pA16, int srcWidth, int srcHeight, uint8_t* pYUV444,uint8_t*pBackGround)
	{
		uint16_t* pA16Y = (uint16_t*)pA16;
		uint16_t* pA16U = (uint16_t*)(pA16 + srcWidth * srcHeight * 2);
		uint16_t* pAlpha = (uint16_t*)(pA16 + srcHeight * srcWidth * 2 * 2);

		uint8_t* destY = (uint8_t*)pYUV444;

		uint8_t* pBGY = pBackGround;
		uint8_t* pBGU = pBGY + 480 * 270;
		uint8_t* pBGV = pBGU + 480 * 270/2;

		//1920-->480   4/1
		//1080-->270   4/1

		//1280-->480   8/3
		//720--->270   8/3

		//3840-->480   8
		//2160-->270   8
		struct ScaleCoe
		{
			int source;
			int dest;
		};
		ScaleCoe widthCoe = { 4,1 };
		ScaleCoe heightCoe = { 4,1 };

		if (srcWidth == 1280)
		{
			widthCoe = { 8,3 };
			heightCoe = { 8,3 };
		}
		else if (srcWidth == 3840)
		{
			widthCoe = { 8,1 };
			heightCoe = { 8,1 };
		}

		auto pKey = [](uint16_t alpha, uint16_t value, uint16_t bg)
			{
				if (alpha == 0xeb00)
				{
					return value;
				}
				return bg;
			};
		auto pickOnePixel = [&](int h, int w) {

			uint16_t* pLineY = pA16Y + h * srcWidth;
			uint16_t* pLineU = pA16U + h * srcWidth;
			uint16_t* pLineAlpha = pAlpha + h * srcWidth;

			uint16_t y1 = pLineY[w];
			uint16_t u = pLineU[w];
			uint16_t v = pLineU[w + 1];
			uint16_t y2 = pLineY[w + 1];
			uint16_t alpha_1 = pLineAlpha[w];
			uint16_t alpha_2 = pLineAlpha[w + 1];

			*destY++ = pKey(alpha_1, (y1 >> 8) & 0xFF, *pBGY++);
			*destY++ = pKey(alpha_1, (u >> 8) & 0xFF, *pBGU);
			*destY++ = pKey(alpha_1, (v >> 8) & 0xFF, *pBGV);

			*destY++ = pKey(alpha_2, (y2 >> 8) & 0xFF, *pBGY++);
			*destY++ = pKey(alpha_2, (u >> 8) & 0xFF, *pBGU++);
			*destY++ = pKey(alpha_2, (v >> 8) & 0xFF, *pBGV++);
			};
		auto pickOneLine = [&](int h) {
			for (int w = 0; w < srcWidth; w += 2 * widthCoe.source)
			{
				for (int i = 0; i < widthCoe.dest; i++)
					pickOnePixel(h, w + i * 2);
			}
			};
		for (int h = 0; h < srcHeight; h += heightCoe.source)
		{
			for (int i = 0; i < heightCoe.dest; i++)
				pickOneLine(h + i);
		}
	}
	void* prepareBackGround(uint8_t* pBackGroud,int width=480,int height=270)
	{
		int gridWidth = 16;
		int gridHeight = 16;

		int gridLine = 0;

		uint8_t* pBackGround8bits = pBackGroud;
		if(pBackGroud==nullptr)
			pBackGround8bits=new uint8_t[width * height * 4];
		gridLine = 0;
		// FFFFFF ,D9D9D9
		memset(pBackGround8bits, 0xFF, width * height * 4);
		memset(pBackGround8bits + width * height, 0x80, width * height);
		for (int h = 0; h < height - gridHeight; h += gridHeight)
		{
			for (int w = 0; w < width; w += gridWidth * 2)
			{
				for (int gridH = 0; gridH < gridHeight; gridH++)
				{
					unsigned char *y = pBackGround8bits + (width * (h + gridH) + w + ((gridLine % 2 == 0) ? 0 : gridWidth)) * 1;
					memset(y, 0xCB, gridWidth);
				}
			}
			++gridLine;
		}
		for (int w = 0; w < width; w += gridWidth * 2)
		{
			for (int gridH = 0; gridH < (height - (gridHeight * (height / gridHeight))); gridH++)
			{
				unsigned char *y = pBackGround8bits + (width * (gridLine * gridHeight + gridH) + w + ((gridLine % 2 == 0) ? 0 : gridWidth));
				memset(y, 0xCB, gridWidth);
			}
		}
		return pBackGround8bits;
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
	uint8_t* pBackGroud = new uint8_t[1920 * 1080 * 10];
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

	 PA16::prepareBackGround(pBackGroud);
	fwrite(pBackGroud,480*270*3,1, fpD);
	

	for (int i = 0; i < 100; i++)
	{
		if (1 != fread(sourceBuffer, nLength, 1, fps))
			break;

		memset(targetBuffer, 0, 1920 * 1080 * 6);
		memset(alphaBuffer, 0, 1920 * 1080 * 6);
		
		//PA16::to_yuv422(sourceBuffer, 1920, 1080, targetBuffer, 1920, 1080, alphaBuffer);
		 //fwrite(targetBuffer, 1920*1080*2, 1, fpD);
		 //fwrite(alphaBuffer, 1920*1080*2, 1, fpD);

		 uint16_t* pDest = (uint16_t*)targetBuffer;
		 PA16::to_yuv422(sourceBuffer, 1920, 1080, pDest, 1920, 1080, alphaBuffer);
		 fwrite(targetBuffer, 1920 * 1080 * 2*2, 1, fpD16);
		 fwrite(alphaBuffer, 1920 * 1080 * 2*2, 1, fpD16);
		 /**/
		PA16::to_yuv444_480270(sourceBuffer, 1920, 1080, targetBuffer,pBackGroud);
		fwrite(targetBuffer, 480*270 * 3, 1, fpD);
	}
	fclose(fpD);
	fclose(fps);
}