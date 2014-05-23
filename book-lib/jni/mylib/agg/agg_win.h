#ifndef _agg_win_h
#define _agg_win_h

#include "agg/incAggBase.h"

CString AggGetFontFullPath(const CString& strFontName,BOOL bUseDefaultOnFail=TRUE);
void AggSetFontPath(CString szFontPath);
CString AggGetFontPath();
BOOL AggRegistFont(CString sFontName,CString sFontPath);
BOOL AggUnRegistFont(CString sFontName);
BOOL AggUnRegistFontByPath(CString sFontPath);
void AggListAllFont(CStringArray& saFont);
BOOL AggIsBuildedInFont(CString sFontName);

#ifdef WINDOW
typedef agg::pixfmt_bgra32 pixel_format;
typedef agg::pixfmt_bgra32_pre pixel_format_pre;
#define g_ePixFormat pix_format_bgra32
#else
typedef agg::pixfmt_rgba32 pixel_format;
typedef agg::pixfmt_rgba32_pre pixel_format_pre;
#define g_ePixFormat pix_format_rgba32
#endif

typedef pixel_format::color_type color_type;


namespace agg
{	
	enum pix_format_e
	{
		pix_format_undefined = 0,  // By default. No conversions are applied 
		pix_format_bw,             // 1 bit per color B/W
		pix_format_gray8,          // Simple 256 level grayscale
		pix_format_gray16,         // Simple 65535 level grayscale
		pix_format_rgb555,         // 15 bit rgb. Depends on the byte ordering!
		pix_format_rgb565,         // 16 bit rgb. Depends on the byte ordering!
		pix_format_rgbAAA,         // 30 bit rgb. Depends on the byte ordering!
		pix_format_rgbBBA,         // 32 bit rgb. Depends on the byte ordering!
		pix_format_bgrAAA,         // 30 bit bgr. Depends on the byte ordering!
		pix_format_bgrABB,         // 32 bit bgr. Depends on the byte ordering!
		pix_format_rgb24,          // R-G-B, one byte per color component
		pix_format_bgr24,          // B-G-R, native win32 BMP format.
		pix_format_rgba32,         // R-G-B-A, one byte per color component
		pix_format_argb32,         // A-R-G-B, native MAC format
		pix_format_abgr32,         // A-B-G-R, one byte per color component
		pix_format_bgra32,         // B-G-R-A, native win32 BMP format
		pix_format_rgb48,          // R-G-B, 16 bits per color component
		pix_format_bgr48,          // B-G-R, native win32 BMP format.
		pix_format_rgba64,         // R-G-B-A, 16 bits byte per color component
		pix_format_argb64,         // A-R-G-B, native MAC format
		pix_format_abgr64,         // A-B-G-R, one byte per color component
		pix_format_bgra64,         // B-G-R-A, native win32 BMP format

		end_of_pix_formats
	};


// #ifdef ANDROID 
// 	typedef pixfmt_bgra32 pixel_format;
// #define g_ePixFormat pix_format_bgra32
// #elif defined TARGEG_OS_IPHONE
// 	typedef pixfmt_rgba32 pixel_format;
// #define g_ePixFormat pix_format_rgba32
// #else
// 	typedef pixfmt_bgr24 pixel_format;
// #define g_ePixFormat pix_format_bgr24
// #endif

	typedef renderer_base<pixel_format> render_pixel_format;

	inline rgba8 Clr2Rgba8(COLORREF clr)
	{
		return rgba8(GetRValue(clr), GetGValue(clr), GetBValue(clr),GetAValue(clr));
	}
	inline COLORREF Rgba82Clr(const rgba8& r)
	{
		return RGBA(r.r,r.g,r.b,r.a);
	}

	enum org_e
	{
		org_mono8   = 8,
		org_color16 = 16,
		org_color24 = 24,
		org_color32 = 32,
		org_color48 = 48,
		org_color64 = 64
	};

	class pixel_map
	{
	public:
		pixel_map();
		~pixel_map();

	public:
		void        destroy();
		bool        create(pix_format_e format,
			unsigned width, 
			unsigned height, 
			unsigned clear_val=256);


		pix_format_e   getformat() const { return m_format; }
		unsigned char* buf() { return m_buf; }
		unsigned       width() const { return m_nWidth; }
		unsigned       height() const{ return m_nHeight; }
		unsigned       stride() const{ return calc_row_len(m_nWidth,m_bpp); }
		unsigned       bpp() const { return m_bpp; }

		void clear(unsigned clear_val);
		unsigned buffersize() const { return m_img_size; }

		static unsigned calc_row_len(unsigned width, unsigned bits_per_pixel);
		static unsigned calc_bpp(pix_format_e format) ;

	private:
		pixel_map(const pixel_map&){}
		const pixel_map& operator = (const pixel_map&){return *this;}

	private:
		unsigned char* m_buf;
		pix_format_e   m_format ;
		unsigned       m_bpp;
		int			   m_nWidth ;
		int			   m_nHeight;
		unsigned       m_img_size;
	};

	class agg_win32
	{
	public:
		agg_win32(pix_format_e format);
		virtual ~agg_win32();

		rendering_buffer&	GetBuffer()			{ return m_renderBuffer; }
	
		void Resize(int nx=0, int ny=0);

		unsigned	GetWidth()  const { return m_pmp.width(); }
		unsigned	GetHeight() const { return m_pmp.height(); }
		unsigned	GetStride() const { return m_pmp.stride(); }

		void clear(BYTE b) { m_pmp.clear(b); }
		unsigned buffersize() { return m_pmp.buffersize(); }

	private:
		agg_win32(const agg_win32&);
		const agg_win32& operator = (const agg_win32&);

		pixel_map			m_pmp ;
		pix_format_e		m_format;
		rendering_buffer	m_renderBuffer;
	};
}

// get the color's red, green, blue, alpha heft
inline void GetColorHeft(const COLORREF color, byte& r, byte& g, byte& b, byte& a)
{
	r = (byte)GetRValue(color);
	g = (byte)GetGValue(color);
	b = (byte)GetBValue(color);
	a = (byte)GetAValue(color);
};

#include "agg_renderclip.h"
typedef CClipRender<pixel_format> CClipPixelFmtRender;
typedef CClipRender<pixel_format_pre> CClipPixelFmtPreRender;

#endif
