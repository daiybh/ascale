#pragma once
#include <cstdint>
/*
	|-byte0-||-byte1-||-byte2-||-byte3-||-byte4-|
	 98765432 10987654 32109876 54321098 76543210
	|---U------|----Y-----|----V-----|----Y-----|
	 xx765432 10xx7654 3210xx76 543210xx 76543210

*/
static void Convert_UYVY10bitBytePacking_to_UYVY8bit(const uint8_t* srcUYVY10bitBytePacking, uint8_t* targetUYVY8bit, int32_t w, int32_t h)
{
	const unsigned char* src = (unsigned char*)srcUYVY10bitBytePacking;
	unsigned char*       dstUYVY = (unsigned char*)targetUYVY8bit;
	const auto           nr4Samples = w * h / 2;

	unsigned short values[4];
	for (unsigned i = 0; i < nr4Samples; i++)
	{
		//98765432
		values[0] = (src[0] << 2) + (src[1] >> 6);  //98765432   10
		values[1] = ((src[1] & 0x3f) << 4) + (src[2] >> 4);
		values[2] = ((src[2] & 0x0f) << 6) + (src[3] >> 2);
		values[3] = ((src[3] & 0x03) << 8) + src[4];

		*dstUYVY++ = (values[0] >> 2) & 0xFF;
		*dstUYVY++ = (values[1] >> 2) & 0xFF;
		*dstUYVY++ = (values[2] >> 2) & 0xFF;
		*dstUYVY++ = (values[3] >> 2) & 0xFF;
		src += 5;
	}
}

class PutBit
{
#define AV_WB32(p, darg) do { uint32_t d = (darg);  ((uint8_t*)(p))[3] = (d); ((uint8_t*)(p))[2] = (d)>>8;  ((uint8_t*)(p))[1] = (d)>>16;  ((uint8_t*)(p))[0] = (d)>>24; } while(0)
#define AV_WB16(p, darg) do { uint16_t d = (darg);  ((uint8_t*)(p))[1] = (d); ((uint8_t*)(p))[0] = (d)>>8;  } while(0)

public:
	explicit PutBit(uint8_t* _buffer)
	{
		buf = _buffer;
		buf_ptr = buf;
		bit_left = 30;
		bit_buf = 0;
	}

	int32_t count() const
	{
		return (buf_ptr - buf) * 8 + 32 - bit_left;
	}

	int32_t getNbBytes() const
	{
		return count() >> 3;
	}

	//void flush() // Pad the end of the output stream with zeros.
	//{
	//	if (bit_left < 32)
	//		bit_buf <<= bit_left;

	//	while (bit_left < 32)
	//	{
	//		*buf_ptr++ = bit_buf >> 24;
	//		bit_buf <<= 8;
	//		bit_left += 8;
	//	}
	//	bit_left = 32;
	//	bit_buf = 0;
	//}

	void putBits(int32_t _n, uint32_t _value)
	{
		// 0000000001
		//  
		//0000000001 0000000011
		//xx 0000000001 0000000011 0000000111 xx
		if (_n < bit_left)
		{
			bit_buf = (bit_buf << _n) | _value;
			bit_left -= _n;
		}
		else
		{
			bit_buf <<= bit_left;
			bit_buf |= _value >> (_n - bit_left);

			//AV_WB32(buf_ptr, bit_buf);
			*(uint32_t*)buf_ptr = bit_buf;

			buf_ptr += 4;
			bit_left += 30 - _n;
			bit_buf = _value;
		}
	}

	void putSigned(int32_t _n, int32_t _value)
	{
		putBits(_n, uintp2_c(_value, _n));
	}

private:
	uint32_t bit_buf = 0;
	int32_t  bit_left = 0;
	uint8_t* buf = nullptr;
	uint8_t* buf_ptr = nullptr;

	static uint32_t uintp2_c(unsigned a, unsigned p)
	{
		return a & ((1 << p) - 1);
	}
};

#pragma pack(push,1)

struct Pixel2_1xV210
{
	uint32_t U0 : 10;
	uint32_t Y0 : 10;
	uint32_t V0 : 10;
	uint32_t R0 : 2;

	uint32_t Y1 : 10;
	uint32_t U1 : 10;
	uint32_t Y2 : 10;
	uint32_t R1 : 2;

	uint32_t V1 : 10;
	uint32_t Y3 : 10;
	uint32_t U2 : 10;
	uint32_t R2 : 2;

	uint32_t Y4 : 10;
	uint32_t V2 : 10;
	uint32_t Y5 : 10;
	uint32_t R3 : 2;
};
#pragma pack(pop)

#include <memory>

//https://sdksupport.aja.com/docs/NTV2SDK_docs_16_2/d5/d32/dataformats.html#fbformats10bitcion
static void Convert_UYVY10bitBytePacking_to_v210(const uint8_t* srcUYVY10bitBytePacking, uint8_t* targetV210, int32_t w, int32_t h)
{
	uint8_t*   src = (uint8_t*)srcUYVY10bitBytePacking;
	const auto loop = w * h * 5 / 2 / 5;

	PutBit bitBuffer(targetV210);

	uint32_t temp;
	uint8_t  ttt;
	FILE*    fp = fopen(R"(C:\Logs\frame2TCP\v210_88888.yuv)", "wb");
	FILE*    fp2 = fopen(R"(C:\Logs\frame2TCP\v210_10.yuv)", "wb");

	size_t dstSize = (w / 48 + (w % 48 ? 1 : 0)) * 128 * h;
	auto   dst = std::make_unique<uint8_t[]>(dstSize);

	PutBit bitBuffer_dl(dst.get());

	for (unsigned i = 0; i < loop; i++)
	{
		temp = ((uint32_t)src[0] << 2) | ((uint32_t)src[1] >> 6);
		bitBuffer.putBits(10, temp);

		ttt = (temp >> 2) & 0xff;
		fwrite(&ttt, 1, 1, fp);
		bitBuffer_dl.putBits(10, ttt);

		temp = ((uint32_t)src[1] & 0x3F) << 4 | ((uint32_t)src[2] >> 4);
		bitBuffer.putBits(10, temp);

		ttt = (temp >> 2) & 0xff;
		fwrite(&ttt, 1, 1, fp);
		bitBuffer_dl.putBits(10, ttt);

		temp = ((uint32_t)src[2] & 0x0F) << 6 | ((uint32_t)src[3] >> 2);
		bitBuffer.putBits(10, temp);

		ttt = (temp >> 2) & 0xff;
		fwrite(&ttt, 1, 1, fp);
		bitBuffer_dl.putBits(10, ttt);

		temp = ((uint32_t)src[3] & 0x03) << 8 | (uint32_t)src[4];
		bitBuffer.putBits(10, temp);

		ttt = (temp >> 2) & 0xff;
		fwrite(&ttt, 1, 1, fp);
		bitBuffer_dl.putBits(10, ttt);

		src += 5;
	}
	fwrite(dst.get(), 1, dstSize, fp2);
}

static void Convert_UYVY8bit_to_UYVY10bitBytePacking(const uint8_t* sourceUYVY8bits, uint8_t* targetUYVY10bitBytePacking, int32_t w, int32_t h)
{
	const unsigned char* src = (unsigned char*)sourceUYVY8bits;
	unsigned char*       dstUYVY = (unsigned char*)targetUYVY10bitBytePacking;
	const auto           nr4Samples = w * h / 2;

	for (unsigned i = 0; i < nr4Samples; i++)
	{
		*dstUYVY++ = src[0];
		*dstUYVY++ = (src[1] >> 2);
		*dstUYVY++ = (src[1] << 6) | (src[2] >> 4);
		*dstUYVY++ = (src[2] << 4) | (src[3] >> 6);
		*dstUYVY++ = (src[3] << 2);

		src += 4;
	}
}

static void Convert_v210_to_UYVY10bitBytePacking(const uint8_t* sourceV210, uint8_t* targetUYVY10bitBytePacking, int32_t w, int32_t h)
{
}
