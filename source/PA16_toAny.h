#include <stdio.h>
#include <stdint.h>
#include <type_traits>
#include "putbits.h"
#include "pixelHeader.h"

#include <iostream>
#include <chrono>


namespace PA16
{
	uint32_t to_v210(uint8_t* pA16, int srcWidth, int srcHeight, uint8_t* pDestV210)
	{
		// PA16 4224 16pp with NV12
		// YYYYYYYYYYYYYYYY
		// UV  UV  UV  UV
		// AAAAAAAAAAAAAAAA
		uint16_t* pA16Y = (uint16_t*)pA16;
		uint16_t* pA16U = (uint16_t*)(pA16 + srcWidth * srcHeight * 2);
		Pixel2_1xV210* ppV210 = (Pixel2_1xV210*)(pDestV210);
		Pixel2_1xV210* ppV210Begin = (Pixel2_1xV210*)(pDestV210);

		uint16_t* pLineY = pA16Y;
		uint16_t* pLineYEnd = pA16U;
		uint16_t* pLineU = pA16U;
		for (; pLineY != pLineYEnd;)
		{
			Pixel2_1xV210* p = ppV210++;

			p->U0 = (*pLineU++) >> 6;
			p->V0 = (*pLineU++) >> 6;
			p->U1 = (*pLineU++) >> 6;
			p->V1 = (*pLineU++) >> 6;
			p->U2 = (*pLineU++) >> 6;
			p->V2 = (*pLineU++) >> 6;
			p->Y0 = (*pLineY++) >> 6;
			p->Y1 = (*pLineY++) >> 6;
			p->Y2 = (*pLineY++) >> 6;
			p->Y3 = (*pLineY++) >> 6;
			p->Y4 = (*pLineY++) >> 6;
			p->Y5 = (*pLineY++) >> 6;

		}
		return (ppV210 - ppV210Begin)*sizeof(Pixel2_1xV210);
	}
	void to_UYVY8(uint8_t* pA16, int srcWidth, int srcHeight, uint8_t* pUyvy8, int width, int height, uint8_t* alphaBuffer)
	{
		// PA16 4224 16pp with NV12
		// YYYYYYYYYYYYYYYY
		// UV  UV  UV  UV
		// AAAAAAAAAAAAAAAA

		uint16_t* pA16Y = (uint16_t*)pA16;
		uint16_t* pA16U = (uint16_t*)(pA16 + srcWidth * srcHeight * 2);

		uint8_t* pAlpha = pA16 + srcHeight * srcWidth * 2 * 2;
		memcpy(alphaBuffer, pAlpha, srcWidth * srcHeight);
		for (int h = 0; h < srcHeight; h++)
		{
			uint16_t* pLineY = pA16Y + h * srcWidth;
			uint16_t* pLineU = pA16U + h * srcWidth;
			for (int w = 0; w < srcWidth / 2; w++)
			{
				uint16_t y1 = *pLineY++;
				uint16_t u = *pLineU++;
				uint16_t v = *pLineU++;
				uint16_t y2 = *pLineY++;

				*pUyvy8++ = (u >> 8) & 0xFF;
				*pUyvy8++ = (y1 >> 8) & 0xFF;
				*pUyvy8++ = (v >> 8) & 0xFF;
				*pUyvy8++ = (y2 >> 8) & 0xFF;
			}
		}
	}
	template <typename T>
	void to_yuv422(uint8_t* pA16, int srcWidth, int srcHeight, T* pUyvy8, int width, int height, uint8_t* alphaBuffer)
	{
		// PA16 4224 16pp with NV12
		// YYYYYYYYYYYYYYYY
		// UV  UV  UV  UV
		// AAAAAAAAAAAAAAAA

		uint16_t* pA16Y = (uint16_t*)pA16;
		uint16_t* pA16U = (uint16_t*)(pA16 + srcWidth * srcHeight * 2);
		uint16_t* pAlpha = (uint16_t*)(pA16 + srcHeight * srcWidth * 2 * 2);
		// memcpy(alphaBuffer, pAlpha, srcWidth * srcHeight);
		auto destY = (T*)pUyvy8;
		auto destU = (T*)(pUyvy8 + srcHeight * srcWidth);
		auto destV = (T*)(pUyvy8 + srcHeight * srcWidth * 3 / 2);
		auto destAlpha = (T*)alphaBuffer;
		//*
		// memcpy(destY, pA16Y,1920 * 1080 * 2);		return;
		auto flag = std::is_same_v<T, uint16_t>;
		auto pLimit = [&flag](uint16_t v)
			{
				if (flag)
					return v;
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
		return; /**/
	}
	void to_yuv444_480270(uint8_t* pA16, int srcWidth, int srcHeight, uint8_t* pYUV444, uint8_t* pBackGround)
	{
		uint16_t* pA16Y = (uint16_t*)pA16;
		uint16_t* pA16U = (uint16_t*)(pA16 + srcWidth * srcHeight * 2);
		uint16_t* pAlpha = (uint16_t*)(pA16 + srcHeight * srcWidth * 2 * 2);

		uint8_t* destYUV444 = (uint8_t*)pYUV444;

		uint8_t* pBGY = pBackGround;
		uint8_t* pBGU = pBGY + 480 * 270;
		uint8_t* pBGV = pBGU + 480 * 270 / 2;

		// 1920-->480   4/1
		// 1080-->270   4/1

		// 1280-->480   8/3
		// 720--->270   8/3

		// 3840-->480   8
		// 2160-->270   8
		// 2160-->270   8








		struct ScaleCoe
		{
			int source;
			int pickedCount;
			int picked[];
		};
		ScaleCoe widthCoe = { 4, 1,{0} };
		ScaleCoe heightCoe = { 4, 1,{0} };

		if (srcWidth == 1280)
		{
			// 0123456701234567
			// 0 2  5  0 2  5
			widthCoe = { 8, 3, {0, 2,5} };
			heightCoe = { 8, 3, {0, 2,5} };
		}
		else if (srcWidth == 3840)
		{
			widthCoe = { 8, 1 };
			heightCoe = { 8, 1 };
		}
		auto pKey = [&](uint16_t alpha, uint16_t value, uint8_t bg)
			{
				uint16_t a1 = alpha >> 8;
				uint16_t a2 = 0xff - a1;
				uint16_t newValue = (a1 * (value >> 8) + a2 * bg) / 0xff;
				return newValue;
			};
		auto pickOnePixel = [&](int h, int w)
			{
				uint16_t* pLineY = pA16Y + h * srcWidth;
				uint16_t* pLineU = pA16U + h * srcWidth;
				uint16_t* pLineAlpha = pAlpha + h * srcWidth;

				uint16_t y1 = pLineY[w];
				uint16_t u = pLineU[w];
				uint16_t v = pLineU[w + 1];

				uint16_t y2 = pLineY[w + 1];

				uint16_t alpha_1 = pLineAlpha[w];
				uint16_t alpha_2 = pLineAlpha[w + 1];

				*destYUV444++ = pKey(alpha_1, y1, *pBGY++);
				*destYUV444++ = pKey(alpha_1, u, *pBGU);
				*destYUV444++ = pKey(alpha_1, v, *pBGV);

				*destYUV444++ = pKey(alpha_2, y2, *pBGY++);
				*destYUV444++ = pKey(alpha_2, u, *pBGU++);
				*destYUV444++ = pKey(alpha_2, v, *pBGV++);
			};
		auto pickOneLine = [&](int h)
			{
				for (int w = 0; w < srcWidth; w += 2 * widthCoe.source)
				{
					for (size_t i = 0; i < widthCoe.pickedCount; i++)
					{
						pickOnePixel(h, w + widthCoe.picked[i] * 2);
					}
				}
			};
		for (int h = 0; h < srcHeight; h += heightCoe.source)
		{
			for (int i = 0; i < heightCoe.pickedCount; ++i)
			{
				pickOneLine(h + heightCoe.picked[i]);
			}
		}
	}
	void* prepareBackGround(uint8_t* pBackGroud, int width = 480, int height = 270)
	{
		int gridWidth = 16;
		int gridHeight = 16;

		int gridLine = 0;

		uint8_t* pBackGround8bits = pBackGroud;
		if (pBackGroud == nullptr)
			pBackGround8bits = new uint8_t[width * height * 4];
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
					unsigned char* y = pBackGround8bits + (width * (h + gridH) + w + ((gridLine % 2 == 0) ? 0 : gridWidth)) * 1;
					memset(y, 0xCB, gridWidth);
				}
			}
			++gridLine;
		}
		for (int w = 0; w < width; w += gridWidth * 2)
		{
			for (int gridH = 0; gridH < (height - (gridHeight * (height / gridHeight))); gridH++)
			{
				unsigned char* y = pBackGround8bits + (width * (gridLine * gridHeight + gridH) + w + ((gridLine % 2 == 0) ? 0 : gridWidth));
				memset(y, 0xCB, gridWidth);
			}
		}
		return pBackGround8bits;
	}
}

#define getDestFilePath(pre) std::format("{}\\{}_{}x{}.yuv", destFolder, pre, w, h).data()
void test_pA16_toyuv444(const char* _destFolder)
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
	// yyyy w*h*2
	// uv   w*h*2
	// alpha w*h*2
	FILE* fps, * fpD, * fpD16, * fpDAlpha;

	// fps = fopen(R"(D:\clips\p216\pa16.yuv)", "rb");
	//fps = fopen(R"(e:\clips\aPA16-1_1920x1080.yuv)", "rb");
	fps = fopen(R"(e:\clips\recPA16.yuv)", "rb");
	fpD = fopen(getDestFilePath("pA16_to_yuv4228bit_480x270.yuv"), "wb");
	//fpD16 = fopen(getDestFilePath("pA16_to_yuv4228bit_1920x1080_16.yuv"), "wb");
	fpDAlpha = fopen(getDestFilePath("pA16_to_yuv4228bit_1920x1080_alpha.yuv"), "wb");

	PA16::prepareBackGround(pBackGroud);
	fwrite(pBackGroud, 480 * 270 * 3, 1, fpD);
	using namespace std::chrono;
	int i = 0;
	if (1 != fread(sourceBuffer, nLength, 1, fps))
	{
	}

	memset(targetBuffer, 0, 1920 * 1080 * 6);
	memset(alphaBuffer, 0, 1920 * 1080 * 6);
	time_point<system_clock> start = system_clock::now();

	for (; i < 1000; i++)
	{



		// PA16::to_yuv422(sourceBuffer, 1920, 1080, targetBuffer, 1920, 1080, alphaBuffer);
		// fwrite(targetBuffer, 1920*1080*2, 1, fpD);
		// fwrite(alphaBuffer, 1920*1080*2, 1, fpD);

		uint16_t* pDest = (uint16_t*)targetBuffer;
		//PA16::to_yuv422(sourceBuffer, 1920, 1080, pDest, 1920, 1080, alphaBuffer);
		//fwrite(targetBuffer, 1920 * 1080 * 2 * 2, 1, fpD16);
		//fwrite(alphaBuffer, 1920 * 1080 * 2 * 2, 1, fpDAlpha);
		/**/
		PA16::to_yuv444_480270(sourceBuffer, 1920, 1080, targetBuffer, pBackGroud);
		//fwrite(targetBuffer, 480 * 270 * 3, 1, fpD);
	}
	time_point<system_clock> end = system_clock::now();

	std::chrono::duration<double> elapsed = end - start;
	std::cout << "Elapsed time: " << elapsed.count() << "s" << "/" << i;
	fclose(fpD);
	fclose(fps);
}
void test_pA16_toV210(const char* pa16FilePath, const char* destFilePath)
{
	FILE* fpPA16;
	fpPA16 = fopen(pa16FilePath, "rb");
	if (fpPA16 == nullptr)
	{
		printf("open file failed. [%s]", pa16FilePath);
		return;
	}
	FILE* fpV210;
	fpV210 = fopen(destFilePath, "wb");
	if (fpV210 == nullptr)
	{
		printf("open file failed. [%s]", destFilePath);
		return;
	}

	int w = 1920;
	int h = 1080;
	uint8_t* targetBuffer = new uint8_t[1920 * 1080 * 10];
	uint8_t* sourceBuffer = new uint8_t[1920 * 1080 * 10];
	int nLength = w * h * 2 * 3;

	memset(targetBuffer, 0, 1920 * 1080 * 6);

#define GET_linePitch(w) (((w + 47) / 48) * 48)

	int linePitch = GET_linePitch(1920);
	int oneSize = linePitch*1080;
	for(int i=0;;i++)
	{
		if (1 != fread(sourceBuffer, nLength, 1, fpPA16))
		{
			break;
		}
		uint32_t v210Size= PA16::to_v210(sourceBuffer, 1920, 1080, targetBuffer);
		fwrite(targetBuffer, v210Size, 1, fpV210);
		memset(targetBuffer, 0, 1920 * 1080 * 6);
		printf("%d\r", i);
	}
	fclose(fpPA16);
	fclose(fpV210);

}

int action_PA16_to_V210(int argc, char* argv[]) {
	if (argc < 3)
	{
		printf("usage:\n\t %s pa16FilePath V210filePath\n", argv[0]);
		return 0;
	}


	//test_pA16_toV210(R"(E:\clips\aPA16-1_1920x1080.yuv)", R"(e:\clips\recPA16_to_v210_1920x1080.yuv)");
	test_pA16_toV210(argv[1], argv[2]);
	return 0;
}

int checkPA16Range(char* filePath)
{
	FILE* fp;
	fp = fopen(filePath, "rb");
	if (fp == nullptr)
	{
		printf("open file failed. [%s]", filePath);
		return 0;
	}
	uint16_t value;
	struct Info {
		struct Item {
			uint16_t value;
			int w;
			int h;
			std::string toString() {
				return  std::format(" w:{:04d} h:{:04d} 16:0x{:04x} 10:0x{:04x}  8:0x{:04x}",
					w, h,
					value, (value >> 6), (value >> 8));
			}
		};
		std::string toString() {
			return  std::format("min:{}  \n\tmax:{}", mininfo.toString(), maxinfo.toString());
		}
		Item mininfo{ 0xffff,0,0 };
		Item maxinfo{ 0x0,0,0 };
		void setMin(uint16_t v, int _w, int _h) {
			if (mininfo.value > v)
			{
				mininfo.value = v;
				mininfo.w = _w;
				mininfo.h = _h;
			}
		}
		void setMax(uint16_t v, int _w, int _h) {
			if (v > maxinfo.value)				maxinfo.value = v;
			maxinfo.w = _w;
			maxinfo.h = _h;
		}

	};

	Info lumaInfo;
	Info chromaInfo;
	Info alphaInfo;
	for (int h = 0; h < 1080; h++)
	{
		for (int w = 0; w < 1920; w++)
		{
			fread(&value, 2, 1, fp);
			lumaInfo.setMin(value, w, h);
			lumaInfo.setMax(value, w, h);
			//printf("h:%04d w%04d,value:%06x %s\n ", h, w, value, lumaInfo.toString().data());
		}
	}
	for (int h = 0; h < 1080; h++)
	{
		for (int w = 0; w < 1920; w++)
		{
			fread(&value, 2, 1, fp);
			chromaInfo.setMin(value, w, h);
			chromaInfo.setMax(value, w, h);
			//printf("h:%04d w%04d,value:%06x %s\n ", h, w, value,					chromaInfo.toString().data());
		}
	}
	for (int h = 0; h < 1080; h++)
	{
		for (int w = 0; w < 1920; w++)
		{
			fread(&value, 2, 1, fp);
			alphaInfo.setMin(value, w, h);
			alphaInfo.setMax(value, w, h);
			//printf("h:%04d w%04d,value:%06x %s\n ", h, w, value,					alphaInfo.toString().data());
		}
	}
	printf("Luma   info: \n\t%s\n", lumaInfo.toString().data());
	printf("Chroma info: \n\t%s\n", chromaInfo.toString().data());
	printf("Alpha  info: \n\t%s\n", alphaInfo.toString().data());
	return 0;
}