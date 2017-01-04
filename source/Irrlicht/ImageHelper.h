#ifndef _IMAGEHELPER_H_INCLUDED_
#define _IMAGEHELPER_H_INCLUDED_

namespace irr
{
// some 2D Defines
struct AbsRectangle
{
	s32 x0;
	s32 y0;
	s32 x1;
	s32 y1;
};

// ------------------ Video---------------------------------------
/*!
	Pixel = dest * ( 1 - alpha ) + source * alpha
	alpha [0;256]
*/
REALINLINE u32 PixelBlend32 ( const u32 c2, const u32 c1, u32 alpha )
{
	u32 srcRB = c1 & 0x00FF00FF;
	u32 srcXG = c1 & 0x0000FF00;

	u32 dstRB = c2 & 0x00FF00FF;
	u32 dstXG = c2 & 0x0000FF00;


	u32 rb = srcRB - dstRB;
	u32 xg = srcXG - dstXG;

	rb *= alpha;
	xg *= alpha;
	rb >>= 8;
	xg >>= 8;

	rb += dstRB;
	xg += dstXG;

	rb &= 0x00FF00FF;
	xg &= 0x0000FF00;

	return rb | xg;
}

/*!
	Pixel = dest * ( 1 - SourceAlpha ) + source * SourceAlpha
*/
inline u32 PixelBlend32 ( const u32 c2, const u32 c1 )
{
	// alpha test
	u32 alpha = c1 & 0xFF000000;

	if ( 0 == alpha )
		return c2;

	if ( 0xFF000000 == alpha )
	{
		return c1;
	}

	alpha >>= 24;

	// add highbit alpha, if ( alpha > 127 ) alpha += 1;
	alpha += ( alpha >> 7);

	u32 srcRB = c1 & 0x00FF00FF;
	u32 srcXG = c1 & 0x0000FF00;

	u32 dstRB = c2 & 0x00FF00FF;
	u32 dstXG = c2 & 0x0000FF00;


	u32 rb = srcRB - dstRB;
	u32 xg = srcXG - dstXG;

	rb *= alpha;
	xg *= alpha;
	rb >>= 8;
	xg >>= 8;

	rb += dstRB;
	xg += dstXG;

	rb &= 0x00FF00FF;
	xg &= 0x0000FF00;

	return (c1 & 0xFF000000) | rb | xg;
}

/*!
	Pixel = dest * ( 1 - alpha ) + source * alpha
	alpha [0;32]
*/
inline u16 PixelBlend16 ( const u16 c2, const u32 c1, const u16 alpha )
{
	const u16 srcRB = c1 & 0x7C1F;
	const u16 srcXG = c1 & 0x03E0;

	const u16 dstRB = c2 & 0x7C1F;
	const u16 dstXG = c2 & 0x03E0;

	u32 rb = srcRB - dstRB;
	u32 xg = srcXG - dstXG;

	rb *= alpha;
	xg *= alpha;
	rb >>= 5;
	xg >>= 5;

	rb += dstRB;
	xg += dstXG;

	rb &= 0x7C1F;
	xg &= 0x03E0;

	return (u16)(rb | xg);
}

// 1 - Bit Alpha Blending
inline u16 PixelBlend16 ( const u16 c2, const u16 c1 )
{
	u16 mask = ((c1 & 0x8000) >> 15 ) + 0x7fff;
	return (c2 & mask ) | ( c1 & ~mask );
}

// 1 - Bit Alpha Blending 16Bit SIMD
inline u32 PixelBlend16_simd ( const u32 c2, const u32 c1 )
{
	u32 mask = ((c1 & 0x80008000) >> 15 ) + 0x7fff7fff;
	return (c2 & mask ) | ( c1 & ~mask );
}

/*
	Pixel = c0 * (c1/31).
*/
inline u16 PixelMul16_2 ( u16 c0, u16 c1)
{
	return	(u16)(( ( (c0 & 0x7C00) * (c1 & 0x7C00) ) & 0x3E000000 ) >> 15 |
			( ( (c0 & 0x03E0) * (c1 & 0x03E0) ) & 0x000F8000 ) >> 10 |
			( ( (c0 & 0x001F) * (c1 & 0x001F) ) & 0x000003E0 ) >> 5  |
			( c0 & c1 & 0x8000));
}

/*
	Pixel = c0 * (c1/255).
*/
REALINLINE u32 PixelMul32_2 ( const u32 c0, const u32 c1)
{
	return	(( ( (c0 & 0xFF000000) >> 16 ) * ( (c1 & 0xFF000000) >> 16 ) ) & 0xFF000000 ) |
			(( ( (c0 & 0x00FF0000) >> 12 ) * ( (c1 & 0x00FF0000) >> 12 ) ) & 0x00FF0000 ) |
			(( ( (c0 & 0x0000FF00) * (c1 & 0x0000FF00) ) >> 16 ) & 0x0000FF00 ) |
			(( ( (c0 & 0x000000FF) * (c1 & 0x000000FF) ) >> 8  ) & 0x000000FF);
}

//! a more useful memset for pixel
// (standard memset only works with 8-bit values)
inline void memset16(void * dest, const u16 value, u32 bytesize)
{
	u16 * d = (u16*) dest;

	u32 i;

	// loops unrolled to reduce the number of increments by factor ~8.
	i = bytesize >> (1 + 3);
	while (i)
	{
		d[0] = value;
		d[1] = value;
		d[2] = value;
		d[3] = value;

		d[4] = value;
		d[5] = value;
		d[6] = value;
		d[7] = value;

		d += 8;
		--i;
	}

	i = (bytesize >> 1 ) & 7;
	while (i)
	{
		d[0] = value;
		++d;
		--i;
	}
}

//! a more useful memset for pixel
// (standard memset only works with 8-bit values)
inline void memset32(void * dest, const u32 value, u32 bytesize)
{
	u32 * d = (u32*) dest;

	u32 i;

	// loops unrolled to reduce the number of increments by factor ~8.
	i = bytesize >> (2 + 3);
	while (i)
	{
		d[0] = value;
		d[1] = value;
		d[2] = value;
		d[3] = value;

		d[4] = value;
		d[5] = value;
		d[6] = value;
		d[7] = value;

		d += 8;
		i -= 1;
	}

	i = (bytesize >> 2 ) & 7;
	while (i)
	{
		d[0] = value;
		d += 1;
		i -= 1;
	}
}

//! 2D Intersection test
inline bool intersect ( AbsRectangle &dest, const AbsRectangle& a, const AbsRectangle& b)
{
	dest.x0 = core::s32_max( a.x0, b.x0 );
	dest.y0 = core::s32_max( a.y0, b.y0 );
	dest.x1 = core::s32_min( a.x1, b.x1 );
	dest.y1 = core::s32_min( a.y1, b.y1 );
	return dest.x0 < dest.x1 && dest.y0 < dest.y1;
}

// integer log2 of a float ieee 754. TODO: non ieee floating point
static inline s32 s32_log2_f32( f32 f)
{
	u32 x = IR ( f );
	return ((x & 0x7F800000) >> 23) - 127;
}

static inline s32 s32_log2_s32(u32 x)
{
	return s32_log2_f32( (f32) x);
}

}
#endif