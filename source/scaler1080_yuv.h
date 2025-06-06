#include <stdio.h>
namespace Scaler_1920_1080
{
    void handleYUV(unsigned char *&cur, int c, int d, int e)
    {
        *cur++ = (c >> 2) & 0xFF;
        *cur++ = (d >> 2) & 0xFF;
        *cur++ = (e >> 2) & 0xFF;
    };

    void V210_to_960_540_YUV(unsigned char *small960Frame, unsigned char *fullFrame)
    {
        unsigned char *line = small960Frame; // dest
        unsigned char *cur;
        int t_width = 0;
        int destWidth = 960;
        t_width = destWidth * 3;

#define clip(var) ((var >= 255) ? 255 : (var <= 0) ? 0 \
                                                   : var)
#define GET_linePitch(w) (((w + 47) / 48) * 48)

        int linePitch = GET_linePitch(1920);
        int lineSize = (1920 / 48 + ((1920 % 48) ? 1 : 0)) * 128;
        unsigned short U1_10b;
        unsigned short Y1_10b;
        unsigned short V1_10b;
        unsigned short Y2_10b;
        unsigned short U2_10b;
        unsigned short Y3_10b;
        unsigned short V2_10b;
        unsigned short Y4_10b;
        unsigned short U3_10b;
        unsigned short Y5_10b;
        unsigned short V3_10b;
        unsigned short Y6_10b;
        unsigned int *buffer = nullptr;
        int32_t step = linePitch / 6;
        for (int h = 0; h < 1080; h++)
        {
            if (h % 2 != 0)
                continue;
            cur = line;
            buffer = (unsigned int *)(fullFrame + h * lineSize);
            for (int i = 0; i < step; i++)
            {
                U1_10b = (0x3FF & buffer[0]);
                Y1_10b = (0xFF300 & buffer[0]) >> 10;
                V1_10b = (0x3ff00000 & buffer[0]) >> 20;
                // Y2_10b = (0x3FF & buffer[1]);
                U2_10b = (0xFF300 & buffer[1]) >> 10;
                Y3_10b = (0x3ff00000 & buffer[1]) >> 20;
                V2_10b = (0x3FF & buffer[2]);
                // Y4_10b = (0xFF300 & buffer[2]) >> 10;
                U3_10b = (0x3ff00000 & buffer[2]) >> 20;
                Y5_10b = (0x3FF & buffer[3]);
                V3_10b = (0xFF300 & buffer[3]) >> 10;
                // Y6_10b = (0x3ff00000 & buffer[3]) >> 20;

                handleYUV(cur, Y1_10b, U1_10b, V1_10b);
                handleYUV(cur, Y3_10b, U2_10b, V2_10b);
                handleYUV(cur, Y5_10b, U3_10b, V3_10b);

                buffer += 4;
            }
            line += t_width;
        }
    }
	void V210_to_UYVY(unsigned char* small960Frame, unsigned char* fullFrame)
	{
		unsigned char* line = small960Frame; // dest
		unsigned char* cur;
		int t_width = 0;
		int destWidth = 960;
		t_width = destWidth * 3;

#define clip(var) ((var >= 255) ? 255 : (var <= 0) ? 0 \
                                                   : var)
#define GET_linePitch(w) (((w + 47) / 48) * 48)

		int linePitch = GET_linePitch(1920);
		int lineSize = (1920 / 48 + ((1920 % 48) ? 1 : 0)) * 128;
		unsigned short U1_10b;
		unsigned short Y1_10b;
		unsigned short V1_10b;
		unsigned short Y2_10b;
		unsigned short U2_10b;
		unsigned short Y3_10b;
		unsigned short V2_10b;
		unsigned short Y4_10b;
		unsigned short U3_10b;
		unsigned short Y5_10b;
		unsigned short V3_10b;
		unsigned short Y6_10b;
		unsigned int* buffer = nullptr;
		int32_t step = linePitch / 6;
		for (int h = 0; h < 1080; h++)
		{
			if (h % 2 != 0)
				continue;
			cur = line;
			buffer = (unsigned int*)(fullFrame + h * lineSize);
			for (int i = 0; i < step; i++)
			{
				U1_10b = (0x3FF & buffer[0]);
				Y1_10b = (0xFF300 & buffer[0]) >> 10;
				V1_10b = (0x3ff00000 & buffer[0]) >> 20;
				// Y2_10b = (0x3FF & buffer[1]);
				U2_10b = (0xFF300 & buffer[1]) >> 10;
				Y3_10b = (0x3ff00000 & buffer[1]) >> 20;
				V2_10b = (0x3FF & buffer[2]);
				// Y4_10b = (0xFF300 & buffer[2]) >> 10;
				U3_10b = (0x3ff00000 & buffer[2]) >> 20;
				Y5_10b = (0x3FF & buffer[3]);
				V3_10b = (0xFF300 & buffer[3]) >> 10;
				// Y6_10b = (0x3ff00000 & buffer[3]) >> 20;

				handleYUV(cur, Y1_10b, U1_10b, V1_10b);
				handleYUV(cur, Y3_10b, U2_10b, V2_10b);
				handleYUV(cur, Y5_10b, U3_10b, V3_10b);

				buffer += 4;
			}
			line += t_width;
		}
	}
};
