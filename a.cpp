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
#define getDestFilePath(pre) std::format("{}\\{}_{}x{}.yuv",destFolder,pre,w,h).data()
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
int main()
{   
    const char *destFolder = R"(d:\clips\)";
    test_10packing(destFolder);

    test_V210_toAny(destFolder,1920,1080);
    test_V210_toAny(destFolder, 1280,720);
    covert_V210_t0_10packing(destFolder);
    return 0;
}