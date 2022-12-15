#include <stdio.h>
#include <thread>
#include "scaler1080.h"
void covert()
{
    // makeBlackWhite();
    int w = 1920;
    int h = 1080;
    uint8_t *targetBuffer = new uint8_t[1920 * 1080 * 10];
    uint8_t *sourceBuffer = new uint8_t[1920 * 1080 * 10];
    FILE *fps, *fpD;
    fopen_s(&fps, R"(D:\clips\Cam(A).live.yuv)", "rb");
    fopen_s(&fpD, "d:\\clips\\aa_1920x1080_uyvy.yuv", "wb");

    fseek(fps, 0, SEEK_END);
    int nLength = ftell(fps);
    rewind(fps);
    nLength = std::min<int>(nLength,1920 * 1080 * 10);
    fread(sourceBuffer, nLength, 1, fps);
    fclose(fps);

    // The data is arranged in blocks of 4 samples. How many of these are there?
   // Scaler_1920_1080::V210_to_960_540_YUV(targetBuffer,sourceBuffer);
Scaler_1920_1080YUV422::V210_to_960_540_YUV422(targetBuffer,sourceBuffer);
    fwrite(targetBuffer, 1920 * 1080 * 2, 1, fpD);
}

int main()
{
    printf("hhlll");
    covert();
    return 0;
}