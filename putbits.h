#include <stdint.h>
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