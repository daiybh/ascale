#include <stdio.h>
#include <stdint.h>
#include <type_traits>
#include "putbits.h"
#include "pixelHeader.h"
#include "define.h"
namespace P216
{
	//P216 4224 16pp with NV12
	//YYYYYYYYYYYYYYYY
	//UV  UV  UV  UV
	// 
	// https://docs.rs/ndi/latest/ndi/enum.FourCCVideoType.html#variant.P216
	// p216 in ffmpeg is p21le 
	//ffmpeg -f rawvideo -pix_fmt uyvy422 -s:v 1920x1080 -i E:\1080i50.yuv -c:v rawvideo -pix_fmt p216le E:\clips\output_file(p216le).yuv

	void to_UYVY8(uint8_t* p216,int srcWidth,int srcHeight, uint8_t* pUyvy8, int width, int height)
	{
		//P216 422 16pp with NV12
		//YYYYYYYYYYYYYYYY
		//UV  UV  UV  UV

		uint16_t* p216Y = (uint16_t*)p216;
		uint16_t* p216U = (uint16_t*)(p216 + srcWidth*srcHeight*2) ;
		
		/*
		for (int x = 0; x < srcWidth * srcHeight/2; x++)
		{
			*pUyvy8++ = (*p216U++ >> 8) & 0xFF;
			*pUyvy8++ = (*p216Y++ >> 8) & 0xFF;
			*pUyvy8++ = (*p216U++ >> 8) & 0xFF;
			*pUyvy8++ = (*p216Y++ >> 8) & 0xFF;
		}
		return;/**/
		for (int h = 0; h < srcHeight; h++)
		{
			uint16_t*pLIneY= p216Y+h*srcWidth;
			uint16_t*pLIneU= p216U+h*srcWidth;		
			for (int w = 0; w < srcWidth/2; w++)
			{
				uint16_t y1 = *pLIneY++;
				uint16_t u = *pLIneU++;
				uint16_t v = *pLIneU++;
				uint16_t y2 = *pLIneY++;

				*pUyvy8++ = (u>>8)&0xFF;
				*pUyvy8++ = (y1>>8)&0xFF;
				*pUyvy8++ = (v>>8)&0xFF;
				*pUyvy8++ = (y2>>8)&0xFF;
			}
		}
	}
	void to_yuv444_480270(uint8_t* p216, int srcWidth, int srcHeight, uint8_t* pYUV444)
	{
		uint16_t* p216Y = (uint16_t*)p216;
		uint16_t* p216U = (uint16_t*)(p216 + srcWidth * srcHeight * 2);		
		uint8_t* destY = (uint8_t*)pYUV444;

		//1920-->480   4
		//1080-->270   4

		for (int h = 0; h < srcHeight; h+=4)
		{
			uint16_t* pLIneY = p216Y + h * srcWidth;
			uint16_t* pLIneU = p216U + h * srcWidth;
			for (int w = 0; w < srcWidth ; w+=2*4)
			{				
				uint16_t y1 = pLIneY[w];
				uint16_t u = pLIneU[w];
				uint16_t v = pLIneU[w+1];
				uint16_t y2 = pLIneY[w+1];

				*destY++ = (y1 >> 8) & 0xFF;
				*destY++ = (u >> 8) & 0xFF;				
				*destY++ = (v >> 8) & 0xFF;

				*destY++ = (y2 >> 8) & 0xFF;
				*destY++ = (u >> 8) & 0xFF;
				*destY++ = (v >> 8) & 0xFF;
			}
		}
	}
	void to_PA16(uint8_t* p216, int srcWidth, int srcHeight, uint8_t* pA16)
	{
		uint16_t* p216Y = (uint16_t*)p216;
		uint16_t* p216U = (uint16_t*)(p216 + srcWidth * srcHeight * 2);
		uint16_t* pDest = (uint16_t*)pA16;

		memcpy(pDest, p216, srcHeight * srcWidth * 2 * 2);


	}
}


void test_p216(const char* _destFolder)
{
	char destFolder[255];
	strcpy(destFolder, _destFolder);
	strcat(destFolder, "\\p216\\");
	std::filesystem::create_directories(destFolder);


	int w = 1920;
	int h = 1080;
	uint8_t* targetBuffer = new uint8_t[1920 * 1080 * 10];
	uint8_t* sourceBuffer = new uint8_t[1920 * 1080 * 10];
	int nLength = w * h * 2 * 2;

	FILE* fps, * fpD;

	fps = fopen(R"(E:\clips\output_file(p216le).yuv)", "rb");

	fpD = fopen(getDestFilePath("p216_to_yuv4448bit_1920x1080.yuv"), "wb");
	for (int i = 0; i < 10; i++)
	{
		fread(sourceBuffer, nLength, 1, fps);

		memset(targetBuffer, 0, 1920 * 1080 * 6);
		//P216::to_UYVY8(sourceBuffer, 1920, 1080, targetBuffer, 1920, 1080);
		P216::to_PA16(sourceBuffer, 1920, 1080, targetBuffer);

		fwrite(targetBuffer, 1920 * 1080*2* 3, 1, fpD);
	}
	fclose(fpD);
	fclose(fps);
}
