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

#ifndef AGG_SPAN_GRADIENT_INCLUDED
#define AGG_SPAN_GRADIENT_INCLUDED

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "agg_basics.h"
#include "agg_math.h"
#include "agg_array.h"


namespace agg
{

    enum gradient_subpixel_scale_e
    {
        gradient_subpixel_shift = 4,                              //-----gradient_subpixel_shift
        gradient_subpixel_scale = 1 << gradient_subpixel_shift,   //-----gradient_subpixel_scale
        gradient_subpixel_mask  = gradient_subpixel_scale - 1     //-----gradient_subpixel_mask
    };



    //==========================================================span_gradient
    template<class ColorT,
             class Interpolator,
             class GradientF, 
             class ColorF>
    class span_gradient
    {
    public:
        typedef Interpolator interpolator_type;
        typedef ColorT color_type;

        enum downscale_shift_e
        {
            downscale_shift = interpolator_type::subpixel_shift - 
                              gradient_subpixel_shift
        };

        //--------------------------------------------------------------------
        span_gradient() {}

        //--------------------------------------------------------------------
        span_gradient(interpolator_type& inter,
                      const GradientF& gradient_function,
                      const ColorF& color_function,
                      float d1, float d2) : 
            m_interpolator(&inter),
            m_gradient_function(&gradient_function),
            m_color_function(&color_function),
            m_d1(iround(d1 * gradient_subpixel_scale)),
            m_d2(iround(d2 * gradient_subpixel_scale))
        {}

        //--------------------------------------------------------------------
        interpolator_type& interpolator() { return *m_interpolator; }
        const GradientF& gradient_function() const { return *m_gradient_function; }
        const ColorF& color_function() const { return *m_color_function; }
        float d1() const { return float(m_d1) / gradient_subpixel_scale; }
        float d2() const { return float(m_d2) / gradient_subpixel_scale; }

        //--------------------------------------------------------------------
        void interpolator(interpolator_type& i) { m_interpolator = &i; }
        void gradient_function(const GradientF& gf) { m_gradient_function = &gf; }
        void color_function(const ColorF& cf) { m_color_function = &cf; }
        void d1(float v) { m_d1 = iround(v * gradient_subpixel_scale); }
        void d2(float v) { m_d2 = iround(v * gradient_subpixel_scale); }

        //--------------------------------------------------------------------
        void prepare() {}

        //--------------------------------------------------------------------
        void generate(color_type* span, int x, int y, unsigned len)
        {   
            int dd = m_d2 - m_d1;
            if(dd < 1) dd = 1;
            m_interpolator->begin(x+0.5f, y+0.5f, len);
            do
            {
                m_interpolator->coordinates(&x, &y);
                int d = m_gradient_function->calculate(x >> downscale_shift, 
                                                       y >> downscale_shift, m_d2);
                d = ((d - m_d1) * (int)m_color_function->size()) / dd;
                if(d < 0) d = 0;
                if(d >= (int)m_color_function->size()) d = m_color_function->size() - 1;
                *span++ = (*m_color_function)[d];
                ++(*m_interpolator);
            }
            while(--len);
        }

    private:
        interpolator_type* m_interpolator;
        const GradientF*   m_gradient_function;
        const ColorF*      m_color_function;
        int                m_d1;
        int                m_d2;
    };

	template<class ColorT, class ColorF>
	class span_gradient_x_y
	{
	public:
		typedef ColorT color_type;

		//--------------------------------------------------------------------
		span_gradient_x_y(
			const ColorF& color_function,
			float d1, float d2, bool bOnX) : 
			m_color_function(&color_function),
			m_d1(d1),
			m_d2(d2),
			m_bX(bOnX),
			m_span(NULL)
		{
			m_color_unit = ((m_color_function->size())<<12) / (d2 - d1);
		}

		//--------------------------------------------------------------------
		const ColorF& color_function() const { return *m_color_function; }
		int d1() const { return m_d1; }
		int d2() const { return m_d2; }

		//--------------------------------------------------------------------
		void color_function(const ColorF& cf) { m_color_function = &cf; }
		void d1(float v) { m_d1 = v; }
		void d2(float v) { m_d2 = v; }

		//--------------------------------------------------------------------
		void prepare()
		{
			int d;
			int nLen = m_d2-m_d1+1;
			m_span = m_span_alloc.allocate(m_d2-m_d1+1);
			for (int i=0; i<nLen; ++i)
			{
				d = (i*m_color_unit)>>12;
				m_span[i] = (*m_color_function)[d];
			}
		}

		//--------------------------------------------------------------------
		 void generate(color_type* span, int x, int y, unsigned len)
		 {
			 int pos = m_bX ? x : y;
			 memcpy(span, m_span+pos-m_d1, len*sizeof(color_type));
		 }

	private:
		span_gradient_x_y() {}

		color_type*					m_span;
		span_allocator<color_type>	m_span_alloc;
		const ColorF*      m_color_function;
		int                m_d1;
		int                m_d2;
		int				   m_color_unit;
		bool			   m_bX;
	};



    //=====================================================gradient_linear_color
    template<class ColorT> 
    struct gradient_linear_color
    {
        typedef ColorT color_type;

        gradient_linear_color() {}
        gradient_linear_color(const color_type& c1, const color_type& c2, 
                              unsigned size = 256) :
            m_c1(c1), m_c2(c2), m_size(size) {}

        unsigned size() const { return m_size; }
        color_type operator [] (unsigned v) const 
        {
            return m_c1.gradient(m_c2, float(v) / float(m_size - 1));
        }

        void colors(const color_type& c1, const color_type& c2, unsigned size = 256)
        {
            m_c1 = c1;
            m_c2 = c2;
            m_size = size;
        }

        color_type m_c1;
        color_type m_c2;
        unsigned m_size;
    };






    //==========================================================gradient_circle
    class gradient_circle
    {
        // Actually the same as radial. Just for compatibility
    public:
        static AGG_INLINE int calculate(int x, int y, int)
        {
            return int(fast_sqrt(x*x + y*y));
        }
    };


    //==========================================================gradient_radial
    class gradient_radial
    {
    public:
        static AGG_INLINE int calculate(int x, int y, int)
        {
            return int(fast_sqrt(x*x + y*y));
        }
    };

    //========================================================gradient_radial_d
    class gradient_radial_d
    {
    public:
        static AGG_INLINE int calculate(int x, int y, int)
        {
            return uround((float)sqrt(float(x)*float(x) + float(y)*float(y)));
        }
    };

    //====================================================gradient_radial_focus
    class gradient_radial_focus
    {
    public:
        //---------------------------------------------------------------------
        gradient_radial_focus() : 
            m_r(100 * gradient_subpixel_scale), 
            m_fx(0), 
            m_fy(0)
        {
            update_values();
        }

        //---------------------------------------------------------------------
        gradient_radial_focus(float r, float fx, float fy) : 
            m_r (iround(r  * gradient_subpixel_scale)), 
            m_fx(iround(fx * gradient_subpixel_scale)), 
            m_fy(iround(fy * gradient_subpixel_scale))
        {
            update_values();
        }

        //---------------------------------------------------------------------
        void init(float r, float fx, float fy)
        {
            m_r  = iround(r  * gradient_subpixel_scale);
            m_fx = iround(fx * gradient_subpixel_scale);
            m_fy = iround(fy * gradient_subpixel_scale);
            update_values();
        }

        //---------------------------------------------------------------------
        float radius()  const { return float(m_r)  / gradient_subpixel_scale; }
        float focus_x() const { return float(m_fx) / gradient_subpixel_scale; }
        float focus_y() const { return float(m_fy) / gradient_subpixel_scale; }

        //---------------------------------------------------------------------
        int calculate(int x, int y, int) const
        {
            float dx = float(x - m_fx);
            float dy = float(y - m_fy);
            float d2 = dx * m_fy - dy * m_fx;
            float d3 = m_r2 * (dx * dx + dy * dy) - d2 * d2;
            return iround((float)(dx * m_fx + dy * m_fy + sqrt(fabs(d3))) * m_mul);
        }

    private:
        //---------------------------------------------------------------------
        void update_values()
        {
            // Calculate the invariant values. In case the focal center
            // lies exactly on the gradient circle the divisor degenerates
            // into zero. In this case we just move the focal center by
            // one subpixel unit possibly in the direction to the origin (0,0)
            // and calculate the values again.
            //-------------------------
            m_r2  = float(m_r)  * float(m_r);
            m_fx2 = float(m_fx) * float(m_fx);
            m_fy2 = float(m_fy) * float(m_fy);
            float d = (m_r2 - (m_fx2 + m_fy2));
            if(d == 0)
            {
                if(m_fx) { if(m_fx < 0) ++m_fx; else --m_fx; }
                if(m_fy) { if(m_fy < 0) ++m_fy; else --m_fy; }
                m_fx2 = float(m_fx) * float(m_fx);
                m_fy2 = float(m_fy) * float(m_fy);
                d = (m_r2 - (m_fx2 + m_fy2));
            }
            m_mul = m_r / d;
        }

        int    m_r;
        int    m_fx;
        int    m_fy;
        float m_r2;
        float m_fx2;
        float m_fy2;
        float m_mul;
    };


    //==============================================================gradient_x
    class gradient_x
    {
    public:
        static int calculate(int x, int, int) { return x; }
    };


    //==============================================================gradient_y
    class gradient_y
    {
    public:
        static int calculate(int, int y, int) { return y; }
    };

    //========================================================gradient_diamond
    class gradient_diamond
    {
    public:
        static AGG_INLINE int calculate(int x, int y, int) 
        { 
            int ax = abs(x);
            int ay = abs(y);
            return ax > ay ? ax : ay; 
        }
    };

    //=============================================================gradient_xy
    class gradient_xy
    {
    public:
        static AGG_INLINE int calculate(int x, int y, int d) 
        { 
            return abs(x) * abs(y) / d; 
        }
    };

    //========================================================gradient_sqrt_xy
    class gradient_sqrt_xy
    {
    public:
        static AGG_INLINE int calculate(int x, int y, int) 
        { 
            return fast_sqrt(abs(x) * abs(y)); 
        }
    };

    //==========================================================gradient_conic
    class gradient_conic
    {
    public:
        static AGG_INLINE int calculate(int x, int y, int d) 
        { 
            return uround((float)fabs(atan2(float(y), float(x))) * float(d) / pi);
        }
    };

    //=================================================gradient_repeat_adaptor
    template<class GradientF> class gradient_repeat_adaptor
    {
    public:
        gradient_repeat_adaptor(const GradientF& gradient) : 
            m_gradient(&gradient) {}

        AGG_INLINE int calculate(int x, int y, int d) const
        {
            int ret = m_gradient->calculate(x, y, d) % d;
            if(ret < 0) ret += d;
            return ret;
        }

    private:
        const GradientF* m_gradient;
    };

    //================================================gradient_reflect_adaptor
    template<class GradientF> class gradient_reflect_adaptor
    {
    public:
        gradient_reflect_adaptor(const GradientF& gradient) : 
            m_gradient(&gradient) {}

        AGG_INLINE int calculate(int x, int y, int d) const
        {
            int d2 = d << 1;
            int ret = m_gradient->calculate(x, y, d) % d2;
            if(ret <  0) ret += d2;
            if(ret >= d) ret  = d2 - ret;
            return ret;
        }

    private:
        const GradientF* m_gradient;
    };


}

#endif
