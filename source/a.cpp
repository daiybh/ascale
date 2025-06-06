#include <stdio.h>
#include <thread>
#include "scaler1080.h"
#include "v210_toAny.h"
#include "uyvy8bit_toAny.h"
#include "yuv10packing_toAny.h"
//#include <format>
#include <filesystem>
#include <cstring>
#include <memory>
#include "P216_toAny.h"
#include "PA16_toAny.h"
#include "define.h"

void test_1080V210(const char* _destFolder)
{
	char destFolder[255];
	strcpy(destFolder, _destFolder);
	strcat(destFolder, "\\test_1080V210\\");
	std::filesystem::create_directories(destFolder);

	int w = 3840;
	int h = 2160;
	uint8_t* targetUYVY8_Buffer = new uint8_t[w * h * 10];
	uint8_t* targetYUV10Packing_Buffer = new uint8_t[w * h * 10];
	uint8_t* sourceBuffer = new uint8_t[w * h * 10];
	FILE* fps, * fpD;
	fps = fopen(R"(\\192.168.0.250\Public\Cloud Sync\clips\YUV files\1080\v210_single.yuv)", "rb");

	fseek(fps, 0, SEEK_END);
	uint64_t nLength = ftell(fps);
	rewind(fps);
	nLength = std::min<uint64_t>(nLength, w * h * 10);
	fread(sourceBuffer, nLength, 1, fps);
	fclose(fps);

	memset(targetUYVY8_Buffer, 0, w * h * 10);
	Scaler_1920_1080::V210_to_960_540_YUV(targetUYVY8_Buffer, sourceBuffer);
	{
		fpD = fopen(getDestFilePath("v210_to_yuv_960x540"), "wb");
		fwrite(targetUYVY8_Buffer, 960 * 540 * 3, 1, fpD);
		fclose(fpD);
	}

	memset(targetUYVY8_Buffer, 0, w * h * 10);
	Scaler_1920_1080::V210_to_YUV480x270(targetUYVY8_Buffer, sourceBuffer);

	{
		fpD = fopen(getDestFilePath("v210_to_yuv_480x270"), "wb");
		fwrite(targetUYVY8_Buffer, 480 * 270 * 3, 1, fpD);
		fclose(fpD);
	}

	memset(targetUYVY8_Buffer, 0, w * h * 10);
	Scaler_1920_1080::V210_to_YUV480x270_2(targetUYVY8_Buffer, sourceBuffer);

	{
		fpD = fopen(getDestFilePath("V210_to_YUV480x270_2"), "wb");
		fwrite(targetUYVY8_Buffer, 480 * 270 * 3, 1, fpD);
		fclose(fpD);
	}

    w = 1920; h = 1080;
	memset(targetUYVY8_Buffer, 0, w * h * 10);
	v210::V210_to_YUV_Any(targetUYVY8_Buffer, sourceBuffer,1920,1080,960,540);
	{
		fpD = fopen(getDestFilePath("V210_to_YUV_Any_960x540_2"), "wb");
		fwrite(targetUYVY8_Buffer, 960*540 * 3, 1, fpD);
		fclose(fpD);
	}
    memset(targetUYVY8_Buffer, 0, w * h * 10);
	v210::V210_to_YUV_Any(targetUYVY8_Buffer, sourceBuffer, 1920, 1080, 480,270);
	{
		fpD = fopen(getDestFilePath("V210_to_YUV_Any_480x270"), "wb");
		fwrite(targetUYVY8_Buffer, 480 * 270 * 3, 1, fpD);
		fclose(fpD);
	}
    w = 3840; h = 2160;
	v210::V210_to_YUV_Any(targetUYVY8_Buffer, sourceBuffer, 3840, 2160, 480, 270);
	{
		fpD = fopen(getDestFilePath("V210_to_YUV_Any_480x270"), "wb");
		fwrite(targetUYVY8_Buffer, 480 * 270 * 3, 1, fpD);
		fclose(fpD);
	}
}
void test_V210_toAny(const char *_destFolder,int w,int h)
{ 
    char destFolder[255];
    strcpy(destFolder,_destFolder);
    strcat(destFolder,"\\v210\\");
    std::filesystem::create_directories(destFolder);

    uint8_t * targetUYVY8_Buffer = new uint8_t[w*h * 10];
    uint8_t * targetYUV10Packing_Buffer = new uint8_t[w*h* 10];
    uint8_t* sourceBuffer = new uint8_t[w*h* 10];
    FILE *fps, *fpD;
    if(h==1080)
        fps= fopen( R"(d:\clips\v210_1080.yuv)", "rb");
    else if(h==720)
        fps = fopen(R"(d:\clips\v210_720.yuv)", "rb");

    fseek(fps, 0, SEEK_END);
    int nLength = ftell(fps);
    rewind(fps);
    nLength = std::min<int>(nLength,w*h* 10);
    fread(sourceBuffer, nLength, 1, fps);
    fclose(fps);

    memset(targetUYVY8_Buffer, 0, w*h* 10);
    memset(targetYUV10Packing_Buffer, 0, w*h* 10);

    v210::to_UYVY10packing(sourceBuffer, targetYUV10Packing_Buffer, w,h);
    {
        fpD = fopen( getDestFilePath("v210_to_UYVY10"), "wb");
        fwrite(targetYUV10Packing_Buffer, w*h* 6, 1, fpD);
        fclose(fpD);
    }

    v210::to_UYVY8(sourceBuffer, targetUYVY8_Buffer,  w,h);
    {
       fpD= fopen(getDestFilePath("v210_to_UYVY8"), "wb");
        fwrite(targetUYVY8_Buffer, w*h * 6, 1, fpD);
        fclose(fpD);
    }

    UYVY8bit::to_UYVY10bitBytePacking(targetUYVY8_Buffer, targetYUV10Packing_Buffer,  w,h);
    {
       fpD= fopen( getDestFilePath("UYVY8bit_to_UYVY10bit"), "wb");
        fwrite(targetYUV10Packing_Buffer, w*h * 6, 1, fpD);
        fclose(fpD);
    }

    memset(targetYUV10Packing_Buffer, 0, w*h * 10);
    UYVY8bit::to_v210(targetUYVY8_Buffer, targetYUV10Packing_Buffer,  w,h);
    {
        fpD= fopen(  getDestFilePath("UYVY8bit_to_v210"), "wb");
        fwrite(targetYUV10Packing_Buffer, w*h * 6, 1, fpD);
        fclose(fpD);
    }

}
void test_10packing(const char *_destFolder)
{ 
    char destFolder[255];
    strcpy(destFolder,_destFolder);
    strcat(destFolder,"\\10packing\\");
    std::filesystem::create_directories(destFolder);
    
    // makeBlackWhite();
    int w = 1920;
    int h = 1080;
    uint8_t* targetBuffer = new uint8_t[1920 * 1080 * 10];
    uint8_t* sourceBuffer = new uint8_t[1920 * 1080 * 10];
    FILE* fps, * fpD;

     fps= fopen(  R"(f:\Codes\backend\Test\clips\UYVY_422p10_LE_BytePacking.yuv)", "rb");

    fseek(fps, 0, SEEK_END);
    int nLength = ftell(fps);
    rewind(fps);
    nLength = std::min<int>(nLength, 1920 * 1080 * 10);
    fread(sourceBuffer, nLength, 1, fps);
    fclose(fps);

    memset(targetBuffer, 0, 1920 * 1080 * 6);
    UYVY10bitBytePacking::to_V210(sourceBuffer, targetBuffer, 1920, 1080);
     fpD= fopen(  getDestFilePath("10packing_to_V210_1920x1080.yuv"), "wb");
    fwrite(targetBuffer, 1920 * 1080 * 6, 1, fpD);
    fclose(fpD);


    memset(targetBuffer, 0, 1920 * 1080 * 6);
    UYVY10bitBytePacking::to_yuv4448bit(sourceBuffer, targetBuffer, 1920, 1080);
     fpD= fopen(  getDestFilePath("UYVY10bitBytePacking_to_yuv4448bit_1920x1080.yuv"), "wb");
    fwrite(targetBuffer, 1920 * 1080 * 10, 1, fpD);
    fclose(fpD);
}

int main(int argc,char*argv[])
{
   // return action_PA16_to_V210(argc, argv);
   // return action_scaler1080(argc, argv);

	//checkRange(argv[1]);
//	return 0;

    const char *destFolder = R"(d:\clips\)";
   // test_p216(destFolder);
    //test_pA16(destFolder);
   // test_10packing(destFolder);

    //test_V210_toAny(destFolder,1920,1080);
   // test_V210_toAny(destFolder, 1280,720);
    test_1080V210(destFolder);
    return 0;
}