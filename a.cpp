#include <stdio.h>
#include <thread>
#include "scaler1080.h"
#include "v210_toAny.h"
#include "uyvy8bit_toAny.h"
#include "yuv10packing_toAny.h"


void covert_V210_t0_10packing()
{
    // makeBlackWhite();
    int w = 1920;
    int h = 1080;
    uint8_t * targetUYVY8_Buffer = new uint8_t[1920 * 1080 * 10];
    uint8_t * targetYUV10Packing_Buffer = new uint8_t[1920 * 1080 * 10];
    uint8_t* sourceBuffer = new uint8_t[1920 * 1080 * 10];
    FILE *fps, *fpD;
    fopen_s(&fps, R"(d:\clips\v210_1080.yuv)", "rb");
    fseek(fps, 0, SEEK_END);
    int nLength = ftell(fps);
    rewind(fps);
    nLength = std::min<int>(nLength,1920 * 1080 * 10);
    fread(sourceBuffer, nLength, 1, fps);
    fclose(fps);

    memset(targetUYVY8_Buffer, 0, 1920 * 1080 * 10);
    memset(targetYUV10Packing_Buffer, 0, 1920 * 1080 * 10);

    //先转成 uyvy8
    // 再通过 uyvy8 转成 yuv10packing
    // 
    v210_to_10packing::v210_to_UYVY10packing(sourceBuffer, targetYUV10Packing_Buffer, 1920,1080);
    {
        fopen_s(&fpD, "d:\\clips\\aa1080_1920x1080_v210_to_UYVY10packing.yuv", "wb");
        fwrite(targetYUV10Packing_Buffer, 1920 * 1080 * 6, 1, fpD);
        fclose(fpD);
    }

    v210_to_10packing::v210_to_UYVY8(sourceBuffer, targetUYVY8_Buffer, 1920, 1080);
    {
        fopen_s(&fpD, "d:\\clips\\aa1080_1920x1080_uyvy.yuv", "wb");
        fwrite(targetUYVY8_Buffer, 1920 * 1080 * 6, 1, fpD);
        fclose(fpD);
    }

    UYVY8bit_to_UYVY10bitBytePacking(targetUYVY8_Buffer, targetYUV10Packing_Buffer, 1920, 1080);
    {
        fopen_s(&fpD, "d:\\clips\\aa1080_1920x1080_10packing.yuv", "wb");
        fwrite(targetYUV10Packing_Buffer, 1920 * 1080 * 6, 1, fpD);
        fclose(fpD);
    }

    memset(targetYUV10Packing_Buffer, 0, 1920 * 1080 * 10);
    UYVY8bit_to_v210(targetUYVY8_Buffer, targetYUV10Packing_Buffer, 1920, 1080);
    {
        fopen_s(&fpD, "d:\\clips\\aa1080_1920x1080_v210.yuv", "wb");
        fwrite(targetYUV10Packing_Buffer, 1920 * 1080 * 6, 1, fpD);
        fclose(fpD);
    }

}
void covert_10packing_to_V210()
{
    // makeBlackWhite();
    int w = 1920;
    int h = 1080;
    uint8_t* targetBuffer = new uint8_t[1920 * 1080 * 10];
    uint8_t* sourceBuffer = new uint8_t[1920 * 1080 * 10];
    FILE* fps, * fpD;
    fopen_s(&fps, R"(D:\Codes\backend\Test\clips\UYVY_422p10_LE_BytePacking.yuv)", "rb");

    fseek(fps, 0, SEEK_END);
    int nLength = ftell(fps);
    rewind(fps);
    nLength = std::min<int>(nLength, 1920 * 1080 * 10);
    fread(sourceBuffer, nLength, 1, fps);
    fclose(fps);

    memset(targetBuffer, 0, 1920 * 1080 * 4);
   // v210_to_10packing::Convert_UYVY10bitBytePacking_to_V210(sourceBuffer, targetBuffer, 1920, 1080);

    fopen_s(&fpD, "d:\\clips\\covert_10packing_to_V210_1920x1080.yuv", "wb");
    fwrite(targetBuffer, 1920 * 1080 * 4, 1, fpD);
    fclose(fpD);

}
#include "putbits.h"

void bits_10bits_to_v210()
{
#define MAX_BUFFER_SIZE 10
    stream_t stream;

    int bufferSize = MAX_BUFFER_SIZE * sizeof(uint8_t);
    uint8_t* pBuffer = (uint8_t*)malloc(bufferSize);
    memset(pBuffer, 0, bufferSize);
    stream.bitstream = pBuffer;
    stream.bitbuf = 0;
    stream.bitrest = 32;
    stream.bytepos = 0;
    stream.bytesize = MAX_BUFFER_SIZE;

    int a = 0b1000000001;
    putbits(10, a, &stream);

    a = 0x303;
    putbits(10, a, &stream);

    a = 0x387;
    putbits(10, a, &stream);

    a = 0;
    putbits(10, a, &stream);
    uint32_t* p32 = (uint32_t*)pBuffer;
    to_binary("p32", *p32);

    uint8_t* p8 = (uint8_t*)pBuffer;
    to_binary("p8 ", *p8++);
    to_binary("p8 ", *p8++);
    to_binary("p8 ", *p8++);
    to_binary("p8 ", *p8++);
}

void bits_210_to_10bits()
{
#define MAX_BUFFER_SIZE 10
    stream_t stream;

    int bufferSize = MAX_BUFFER_SIZE * sizeof(uint8_t);
    uint8_t* pBuffer = (uint8_t*)malloc(bufferSize);
    memset(pBuffer, 0, bufferSize);
    stream.bitstream = pBuffer;
    stream.bitbuf = 0;
    stream.bitrest = 32;
    stream.bytepos = 0;
    stream.bytesize = MAX_BUFFER_SIZE;

    int a = 0b1000000011;
    putbits(10, a, &stream);

    a = 0b1000000111;
    putbits(10, a, &stream);

    a = 0b1000001111;
    putbits(10, a, &stream);

    a = 0;
    putbits(10, a, &stream);
    uint32_t* p32 = (uint32_t*)pBuffer;
    to_binary("p32", *p32);

    uint8_t* p8 = (uint8_t*)pBuffer;
    to_binary("p8 ", *p8++);
    to_binary("p8 ", *p8++);
    to_binary("p8 ", *p8++);
    to_binary("p8 ", *p8++);
}
int main()
{
   



    printf("hhhhhjh\n$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    //covert_10packing_to_V210();
    covert_V210_t0_10packing();
    return 0;
}