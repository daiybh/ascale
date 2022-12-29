#include <stdio.h>
#include <stdint.h>
namespace v210_to_10packing
{
#pragma pack(push,1)

	struct Pixel2_1xV210
	{
		uint32_t U0 : 10;		
		uint32_t Y0 : 10;		
		uint32_t V0 : 10;
		uint32_t : 2;

		uint32_t Y1 : 10;
		uint32_t U1 : 10;
		uint32_t Y2 : 10;
		uint32_t : 2;

		uint32_t V1 : 10;
		uint32_t Y3 : 10;
		uint32_t U2 : 10;
		uint32_t : 2;

		uint32_t Y4 : 10;
		uint32_t V2 : 10;
		uint32_t Y5 : 10;
		uint32_t : 2;
	};

    struct Pixel_10packingA
	{
        uint16_t U0 : 10;//FF 2
        uint16_t a : 6;
        uint16_t Y0 : 10;//   2 FF
        uint16_t V0 : 10;//        FF 2
	};
	struct Pixel_10packingAA
	{
        //40  5
        //FFFFFFFF 
        uint16_t U0 : 10;//FF 2
		uint16_t Y0 : 10;//   2 FF
		uint16_t V0 : 10;//        FF 2
        uint16_t Y1 : 10;//           2 FF      //2(FF FF FF FF) +8 
                         //FF F FF FF F FF
                         //FF FF FF FF FF
                         // 
        //uint32_t:32 - 8;//FFFF
	};
    struct Pixel_10packing
    {
        uint32_t U0 : 10;
        uint32_t Y0 : 10;
        uint32_t V0 : 10;
        uint32_t Y1 : 10;

        uint32_t U1 : 10;
        uint32_t Y2 : 10;
        uint32_t V1 : 10;
        uint32_t Y3 : 10;

        uint32_t U2 : 10;
        uint32_t Y4 : 10;
        uint32_t V2 : 10;
        uint32_t Y5 : 10;

        uint32_t U3 : 10;
        uint32_t Y6 : 10;
        uint32_t V3 : 10;
        uint32_t Y7 : 10;
    };

#pragma pack(pop)

/*
    12Pixels
    16 bytes

    xxU0Y0V0
    xxY1U1Y2
    xxV1Y3U2
    xxY4V2Y5


    16 pixels
    20 bytes

    U0Y0V0Y1  40 
    U1Y2V1Y3  40
    U2Y4V2Y5  40
    U3Y6V3Y7  40
    
    u8u8u8u8u8  32+8=40

    

*/
    void v210_to_10packing(uint8_t *pV210, uint8_t *p10Packing, int width, int height)
    {
#define GET_linePitch(w) (((w + 47) / 48) * 48)

        int linePitch = GET_linePitch(width);
        int lineSize = (width / 48 + ((width % 48) ? 1 : 0)) * 128;

        unsigned int *buffer = nullptr;
        int32_t step = linePitch / 6;
        Pixel_10packingAA* pBox = (Pixel_10packingAA*)p10Packing;
        Pixel_10packingAA* pBox2 = pBox + 1;

		int aa = sizeof(Pixel_10packingA);
		int aaA = sizeof(Pixel_10packingAA);
        int a = sizeof(Pixel_10packing);
        int b = sizeof(Pixel2_1xV210);
        for (int h = 0; h < height; h++)
        {
            buffer = (unsigned int *)(pV210 + h * lineSize);
            for (int i = 0; i < step/4; i++)
            {

                // 4*Pixel2_1xV210  3*Pixel_10packing
                Pixel2_1xV210 *p0 = (Pixel2_1xV210 *)buffer;                buffer += 4;
                Pixel2_1xV210 *p1 = (Pixel2_1xV210 *)buffer;                buffer += 4;
                Pixel2_1xV210 *p2 = (Pixel2_1xV210 *)buffer;                buffer += 4;
                Pixel2_1xV210 *p3 = (Pixel2_1xV210 *)buffer;                buffer += 4;

                Pixel_10packing*pDest0 = (Pixel_10packing*)p10Packing;      p10Packing+=20;
                Pixel_10packing*pDest1 = (Pixel_10packing*)p10Packing;      p10Packing+=20;
                Pixel_10packing*pDest2 = (Pixel_10packing*)p10Packing;      p10Packing+=20;

				/**dstUYVY++ = src[0];
				*dstUYVY++ = (src[1] >> 2);
				*dstUYVY++ = (src[1] << 6) | (src[2] >> 4);
				*dstUYVY++ = (src[2] << 4) | (src[3] >> 6);
				*dstUYVY++ = (src[3] << 2);*/


                pDest0->U0 = p0->U0;
                pDest0->Y0 = p0->Y0;
                pDest0->V0 = p0->V0;
                pDest0->Y1 = p0->Y1;

                pDest0->U1 = p0->U1;
                pDest0->Y2 = p0->Y2;
                pDest0->V1 = p0->V1;
                pDest0->Y3 = p0->Y3;

                pDest0->U2 = p0->U2;
                pDest0->Y4 = p0->Y4;
                pDest0->V2 = p0->V2;
                pDest0->Y5 = p0->Y5;

                pDest0->U3 = p1->U0;
                pDest0->Y6 = p1->Y0;
                pDest0->V3 = p1->V0;
                pDest0->Y7 = p1->Y1;    
                
                pDest1->U0 = p1->U1;
                pDest1->Y0 = p1->Y2;
                pDest1->V0 = p1->V1;
                pDest1->Y1 = p1->Y3;

                pDest1->U1 = p1->U2;
                pDest1->Y2 = p1->Y4;
                pDest1->V1 = p1->V2;
                pDest1->Y3 = p1->Y5;

                pDest1->U2 = p2->U0;
                pDest1->Y4 = p2->Y0;
                pDest1->V2 = p2->V0;
                pDest1->Y5 = p2->Y1;

                pDest1->U3 = p2->U1;
                pDest1->Y6 = p2->Y2;
                pDest1->V3 = p2->V1;
                pDest1->Y7 = p2->Y3;

                pDest2->U0 = p2->U2;
                pDest2->Y0 = p2->Y4;
                pDest2->V0 = p2->V2;
                pDest2->Y1 = p2->Y5;

                pDest2->U1 = p3->U0;
                pDest2->Y2 = p3->Y0;
                pDest2->V1 = p3->V0;
                pDest2->Y3 = p3->Y1;

                pDest2->U2 = p3->U1;
                pDest2->Y4 = p3->Y2;
                pDest2->V2 = p3->V1;
                pDest2->Y5 = p3->Y3;

                pDest2->U3 = p3->U2;
                pDest2->Y6 = p3->Y4;
                pDest2->V3 = p3->V2;
                pDest2->Y7 = p3->Y5;



            }
        }
    }
};