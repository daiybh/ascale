#pragma once
#include <stdint.h>
#include <stdio.h>
class PutBit
{
#define AV_WB32(p, darg)                 \
	do                                   \
	{                                    \
		uint32_t d = (darg);             \
		((uint8_t *)(p))[3] = (d);       \
		((uint8_t *)(p))[2] = (d) >> 8;  \
		((uint8_t *)(p))[1] = (d) >> 16; \
		((uint8_t *)(p))[0] = (d) >> 24; \
	} while (0)
#define AV_WB16(p, darg)                \
	do                                  \
	{                                   \
		uint16_t d = (darg);            \
		((uint8_t *)(p))[1] = (d);      \
		((uint8_t *)(p))[0] = (d) >> 8; \
	} while (0)

public:
	explicit PutBit(uint8_t *_buffer)
	{
		buf = _buffer;
		buf_ptr = buf;
		bit_left = 30;
		bit_buf = 0;
	}

	void putBits(int32_t _n, uint32_t _value)
	{
		if (_n < bit_left)
		{
			bit_buf = (bit_buf << _n) | _value;
			bit_left -= _n;
		}
		else
		{
			bit_buf <<= bit_left;
			bit_buf |= _value >> (_n - bit_left);

			// AV_WB32(buf_ptr, bit_buf);
			*(uint32_t *)buf_ptr = bit_buf;

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
	int32_t bit_left = 0;
	uint8_t *buf = nullptr;
	uint8_t *buf_ptr = nullptr;

	static uint32_t uintp2_c(unsigned a, unsigned p)
	{
		return a & ((1 << p) - 1);
	}
};

typedef struct
{
  uint32_t bytesize;     //Buffer size - typically maximum compressed frame size
  uint32_t bytepos;      //Byte position in bitstream
  uint8_t *bitstream;   //Compressed bit stream
  uint32_t bitbuf;       //Recent bits not written the bitstream yet
  uint32_t bitrest;      //Empty bits in bitbuf
} stream_t;

typedef struct
{
  uint32_t bytepos;      //Byte position in bitstream
  uint32_t bitbuf;       //Recent bits not written the bitstream yet
  uint32_t bitrest;      //Empty bits in bitbuf
} stream_pos_t;

static inline unsigned int mask(unsigned int n)
{
	return (1 << n) - 1;
}

static inline void flush_bitbuf(stream_t *str, int bytes)
{
	int i;
	if ((str->bytepos + bytes) > str->bytesize)
	{
		printf("Run out of bits in stream buffer.");
	}
	for (i = 3; i >= 4 - bytes; --i)
	{
		str->bitstream[str->bytepos++] = (str->bitbuf >> (8 * i)) & 0xff;
	}
	str->bitbuf = 0;
	str->bitrest = 32;
}
//
//U-->val: 9876543210          bitrest:8  n:10 
//
//FILL       76543210
//LEFT 2           98 
//Y-->9876543210
//FILL      54321098
//LEFT 4         9876
//V-->9876543210
//FILL      32109876
//LEFT 2      987654
//xx--->xx
//FILL      xx987654
//left 0

//Y-->9876543210
//     10987654
//     98765432
#include <bitset>
#include <string>



template<typename T>
void to_binary(const char*marker,T val)
{
	std::bitset<sizeof(T)*CHAR_BIT> bs(val);

	printf("\n %s==>",marker);
	printf(bs.to_string().c_str());

	auto ull = bs.to_ullong();

	printf("    %08x\n",ull);
	for (int i = bs.size()-1; i >-1; i--)
	{
		int a = bs[i];
		printf("%s%d", ((i +1) % 10 == 0) ? " " : "", a);
	}


}
static inline unsigned int putbits(unsigned int n, unsigned int val, stream_t *str,bool bDebug=false)
{
	if(bDebug)
		to_binary("start ",val);
	unsigned int rest;

	if (n <= str->bitrest)
	{
		str->bitbuf |= ((val & mask(n)) << (str->bitrest - n));
		str->bitrest -= n;
	}
	else
	{
		rest = n - str->bitrest;
		str->bitbuf |= (val >> rest) & mask(n - rest);
		flush_bitbuf(str, 4);
		str->bitbuf |= (val & mask(rest)) << (32 - rest);
		str->bitrest -= rest;
	}

	if (bDebug)
	to_binary("stop  ",str->bitbuf);

	return n;
}


// static inline unsigned int putbits(unsigned int n, unsigned int val, NewStream *str)
// {
// 	unsigned int rest;
// 	if (n <= str->bitrest)
// 	{
// 		str->bitbuf |= ((val & mask(n)) << (str->bitrest - n));
// 		str->bitrest -= n;
// 	}
// 	else{
// 		rest = n - str->bitrest;
// 		str->bitbuf |= (val >> rest) & mask(n - rest);
// 		flush_bitbuf(str, 4);
// 		str->bitbuf |= (val & mask(rest)) << (32 - rest);
// 		str->bitrest -= rest;
// 	}
// }