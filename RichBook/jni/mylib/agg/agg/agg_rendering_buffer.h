//----------------------------------------------------------------------------
// Anti-Grain Geometry (AGG) - Version 2.5
// A high quality rendering engine for C++
// Copyright (C) 2002-2006 Maxim Shemanarev
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://antigrain.com
// 
// AGG is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// AGG is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with AGG; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA 02110-1301, USA.
//----------------------------------------------------------------------------

#ifndef AGG_RENDERING_BUFFER_INCLUDED
#define AGG_RENDERING_BUFFER_INCLUDED

#include "agg_array.h"
#include <memory.h>

namespace agg
{

    //===========================================================row_accessor
    template<class T> class row_accessor
    {
    public:
        typedef const_row_info<T> row_data;

        //-------------------------------------------------------------------
        row_accessor() :
            m_buf(0),
            m_start(0),
            m_width(0),
            m_height(0),
            m_stride(0)
        {
        }

        //--------------------------------------------------------------------
        row_accessor(T* buf, unsigned width, unsigned height, int stride) :
            m_buf(0),
            m_start(0),
            m_width(0),
            m_height(0),
            m_stride(0)
        {
            attach(buf, width, height, stride);
        }


        //--------------------------------------------------------------------
        void attach(T* buf, unsigned width, unsigned height, int stride)
        {
			m_buf = m_start = buf;
			m_width = width;
			m_height = height;
			m_stride = stride;
			if(stride < 0) 
            { 
				m_start = m_buf - int(height - 1) * stride;
			}
        }

        //--------------------------------------------------------------------
        AGG_INLINE       T* buf()          { return m_buf;    }
        AGG_INLINE const T* buf()    const { return m_buf;    }
        AGG_INLINE unsigned width()  const { return m_width;  }
        AGG_INLINE unsigned height() const { return m_height; }
        AGG_INLINE int      stride() const { return m_stride; }
        AGG_INLINE unsigned stride_abs() const 
        {
            return (m_stride < 0) ? unsigned(-m_stride) : unsigned(m_stride); 
        }

        //--------------------------------------------------------------------
		AGG_INLINE       T* row_ptr(int, int y, unsigned) 
        { 
            return m_start + y * m_stride; 
        }
		AGG_INLINE       T* row_ptr(int y)       { return m_start + y * m_stride; }
		AGG_INLINE const T* row_ptr(int y) const { return m_start + y * m_stride; }
		AGG_INLINE row_data row    (int y) const 
        { 
            return row_data(0, m_width-1, row_ptr(y)); 
        }

        //--------------------------------------------------------------------
        template<class RenBuf>
        void copy_from(const RenBuf& src)
        {
			 unsigned h = height();
			 if(src.height() < h) h = src.height();

			 unsigned l = stride_abs();
			 if(src.stride_abs() < l) l = src.stride_abs();

            l *= sizeof(T);

            unsigned y;
            unsigned w = width();
            for (y = 0; y < h; y++)
            {
                memcpy(row_ptr(0, y, w), src.row_ptr(y), l);
            }
        }

// 		 template<class RenBuf>
// 		 void copy_from(const RenBuf& src, int left, int top, int right, int bottom)
// 		 {
// 			int w = width(), h = height(), l = stride_abs();
// 			if (w != right - left || h != bottom - top) return;
// 			if (w != src.width() || h != src.height()) return;
// 			if (stride() != src.stride()) return;
// 
// 			int pix_len = l / w;
// 			if (pix_len * w != l) return;
// 
// 			int row_step = (w - abs(left)) * pix_len;
// 			row_step *= sizeof(T);
// 
// 			unsigned start_src = 0, start_dst = 0;
// 			if (left > 0) 
// 				start_dst = (left-1) * pix_len * sizeof(T);
// 			else 
// 				start_src = (-left-1) * pix_len * sizeof(T);
// 
// 			if (top < 0) // move up
// 			{
// 				if (stride() > 0)
// 				{
// 					for (int r=0; r<bottom; r++)
// 					{
// 						memcpy(row_ptr(0, r, w) + start_dst, src.row_ptr(r+top) + start_src, row_step);
// 					}
// 				}
// 				else
// 				{
// 					for (int r=h-1; r>=-top; r--)
// 					{
// 						memcpy(row_ptr(0, r+top, w) + start_dst, src.row_ptr(r) + start_src, row_step);
// 					}
// 				}
// 			}
// 			else // move down
// 			{
// 				if (stride() > 0)
// 				{
// 					for (int r=top; r<h; r++)
// 					{
// 						memcpy(row_ptr(0, r-top, w) + start_dst, src.row_ptr(r) + start_src, row_step);
// 					}
// 				}
// 				else
// 				{
// 					for (int r=h-top-1; r >= 0; r --)
// 					{
// 						memcpy(row_ptr(0, r+top, w) + start_dst, src.row_ptr(r) + start_src, row_step);
// 					}
// 				}
// 			}
// 		 }

		 const static int gray_shift = 8;
		 const static int gray_r = int(0.3 * (1 << gray_shift));
		 const static int gray_g = int(0.59 * (1 << gray_shift));
		 const static int gray_b = int(0.11 * (1 << gray_shift));

		 template <class RenBuf>
		 void stretch_from(	RenBuf& bufSrc, 
							int nSrcX, int nSrcY, int nSrcW, int nSrcH, 
							int nDstX, int nDstY, int nDstW, int nDstH	)
		 {
			 const static int zoom_shift = 8;

			 // clip source
			 nSrcX = nSrcX > 0 ? nSrcX : 0;
			 nSrcY = nSrcY > 0 ? nSrcY : 0;
			 nSrcW = nSrcX + nSrcW <= (int)bufSrc.width() ? nSrcW : (int)bufSrc.width() - nSrcX;
			 nSrcH = nSrcY + nSrcH <= (int)bufSrc.height() ? nSrcH : (int)bufSrc.height() - nSrcY;

			 // clip destination
			 nDstX = nDstX > 0 ? nDstX : 0;
			 nDstY = nDstY > 0 ? nDstY : 0;
			 nDstW = nDstX + nDstW <= (int)m_width ? nDstW : (int)m_width - nDstX;
			 nDstH = nDstY + nDstH <= (int)m_height ? nDstH : (int)m_height - nDstY;

			 if (!nSrcH || !nSrcW || !nDstH || !nDstW)
				 return;

			 // stretch
			 int nSrcCurX = 0;
			 int nSrcCurY = 0;
			 int nStepX = int((float)nSrcW / (float)nDstW * (1<<zoom_shift));
			 int nStepY = int((float)nSrcH / (float)nDstH * (1<<zoom_shift));

			 int pix_len = stride_abs() / m_width;
			 if (pix_len * m_width != stride_abs()) return;
			 int nPixSize = pix_len;
			 int nSrcStartX = nSrcX * nPixSize;
			 int nDstStartX = nDstX * nPixSize;
			 int nSrcBottom = nSrcY + nSrcH;
			 int nDstBottom = nDstY + nDstH;

			 int nRowWidthSrc = bufSrc.stride_abs();
			 int nRowWidthDst = stride_abs();

			 byte* pDstRow = NULL;
			 byte* pSrcRow = NULL;
			 for (int i=0; i<nDstH; i++)
			 {
				 // 此处与Windows版本不同，源代码是 if (stride() > 0)
				 if (stride() < 0)
				 {
					 pDstRow = m_buf + (m_height - nDstY - i - 1)*nRowWidthDst + nDstStartX; //$$
					 pSrcRow = bufSrc.buf() + (bufSrc.height() - nSrcY - (nSrcCurY >> zoom_shift) - 1)*nRowWidthSrc + nSrcStartX;
				 }
				 else
				 {
					 pDstRow = m_buf + (nDstY+nDstH - i - 1)*nRowWidthDst + nDstStartX;
					 pSrcRow = bufSrc.buf() + (nSrcH+nSrcY - (nSrcCurY >> zoom_shift) - 1)*nRowWidthSrc + nSrcStartX;
				 }


				 for (int j=0; j<nDstW; j++)
				 {
					 memcpy(pDstRow, pSrcRow+(nSrcCurX >> zoom_shift)*nPixSize, pix_len);

					 pDstRow += pix_len;
					 nSrcCurX += nStepX;
				 }

				 nSrcCurY += nStepY;
				 nSrcCurX = 0;
			 }
		 };

        //--------------------------------------------------------------------
        void clear(T value)
        {
            unsigned y;
            unsigned w = width();
            unsigned stride = stride_abs();
            for(y = 0; y < height(); y++)
            {
                T* p = row_ptr(0, y, w);
                unsigned x;
                for(x = 0; x < stride; x++)
                {
                    *p++ = value;
                }
            }
        }

    private:
        //--------------------------------------------------------------------
        T*            m_buf;    // Pointer to renrdering buffer
        T*            m_start;  // Pointer to first pixel depending on stride 
        unsigned      m_width;  // Width in pixels
        unsigned      m_height; // Height in pixels
        int           m_stride; // Number of bytes per row. Can be < 0
    };




    //==========================================================row_ptr_cache
    template<class T> class row_ptr_cache
    {
    public:
        typedef const_row_info<T> row_data;

        //-------------------------------------------------------------------
        row_ptr_cache() :
            m_buf(0),
            m_rows(),
            m_width(0),
            m_height(0),
            m_stride(0)
        {
        }

        //--------------------------------------------------------------------
        row_ptr_cache(T* buf, unsigned width, unsigned height, int stride) :
            m_buf(0),
            m_rows(),
            m_width(0),
            m_height(0),
            m_stride(0)
        {
            attach(buf, width, height, stride);
        }

        //--------------------------------------------------------------------
        void attach(T* buf, unsigned width, unsigned height, int stride)
        {
            m_buf = buf;
            m_width = width;
            m_height = height;
            m_stride = stride;
            if(height > m_rows.size())
            {
                m_rows.resize(height);
            }

            T* row_ptr = m_buf;

            if(stride < 0)
            {
                row_ptr = m_buf - int(height - 1) * stride;
            }

            T** rows = &m_rows[0];

            while(height--)
            {
                *rows++ = row_ptr;
                row_ptr += stride;
            }
        }

        //--------------------------------------------------------------------
        AGG_INLINE       T* buf()          { return m_buf;    }
        AGG_INLINE const T* buf()    const { return m_buf;    }
        AGG_INLINE unsigned width()  const { return m_width;  }
        AGG_INLINE unsigned height() const { return m_height; }
        AGG_INLINE int      stride() const { return m_stride; }
        AGG_INLINE unsigned stride_abs() const 
        {
            return (m_stride < 0) ? unsigned(-m_stride) : unsigned(m_stride); 
        }

        //--------------------------------------------------------------------
        AGG_INLINE       T* row_ptr(int, int y, unsigned) 
        { 
            return m_rows[y]; 
        }
        AGG_INLINE       T* row_ptr(int y)       { return m_rows[y]; }
        AGG_INLINE const T* row_ptr(int y) const { return m_rows[y]; }
        AGG_INLINE row_data row    (int y) const 
        { 
            return row_data(0, m_width-1, m_rows[y]); 
        }

        //--------------------------------------------------------------------
        T const* const* rows() const { return &m_rows[0]; }

        //--------------------------------------------------------------------
        template<class RenBuf>
        void copy_from(const RenBuf& src)
        {
            unsigned h = height();
            if(src.height() < h) h = src.height();
        
            unsigned l = stride_abs();
            if(src.stride_abs() < l) l = src.stride_abs();
        
            l *= sizeof(T);

            unsigned y;
            unsigned w = width();
            for (y = 0; y < h; y++)
            {
                memcpy(row_ptr(0, y, w), src.row_ptr(y), l);
            }
        }

        //--------------------------------------------------------------------
        void clear(T value)
        {
            unsigned y;
            unsigned w = width();
            unsigned stride = stride_abs();
            for(y = 0; y < height(); y++)
            {
                T* p = row_ptr(0, y, w);
                unsigned x;
                for(x = 0; x < stride; x++)
                {
                    *p++ = value;
                }
            }
        }

    private:
        //--------------------------------------------------------------------
        T*            m_buf;        // Pointer to renrdering buffer
        pod_array<T*> m_rows;       // Pointers to each row of the buffer
        unsigned      m_width;      // Width in pixels
        unsigned      m_height;     // Height in pixels
        int           m_stride;     // Number of bytes per row. Can be < 0
    };




    //========================================================rendering_buffer
    // 
    // The definition of the main type for accessing the rows in the frame 
    // buffer. It provides functionality to navigate to the rows in a 
    // rectangular matrix, from top to bottom or from bottom to top depending 
    // on stride.
    //
    // row_accessor is cheap to create/destroy, but performs one multiplication
    // when calling row_ptr().
    // 
    // row_ptr_cache creates an array of pointers to rows, so, the access 
    // via row_ptr() may be faster. But it requires memory allocation 
    // when creating. For example, on typical Intel Pentium hardware 
    // row_ptr_cache speeds span_image_filter_rgb_nn up to 10%
    //
    // It's used only in short hand typedefs like pixfmt_rgba32 and can be 
    // redefined in agg_config.h
    // In real applications you can use both, depending on your needs
    //------------------------------------------------------------------------
#ifdef AGG_RENDERING_BUFFER
    typedef AGG_RENDERING_BUFFER rendering_buffer;
#else
//  typedef row_ptr_cache<int8u> rendering_buffer;
    typedef row_accessor<int8u> rendering_buffer;
#endif

}


#endif
