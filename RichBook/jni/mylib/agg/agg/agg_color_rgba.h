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
//
// Adaptation for high precision colors has been sponsored by 
// Liberty Technology Systems, Inc., visit http://lib-sys.com
//
// Liberty Technology Systems, Inc. is the provider of
// PostScript and PDF technology for software developers.
// 
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------

#ifndef AGG_COLOR_RGBA_INCLUDED
#define AGG_COLOR_RGBA_INCLUDED

#include <math.h>
#include "agg_basics.h"

namespace agg
{
    // Supported byte orders for RGB and RGBA pixel formats
    //=======================================================================
    struct order_rgb  { enum rgb_e  { R=0, G=1, B=2, rgb_tag }; };       //----order_rgb
    struct order_bgr  { enum bgr_e  { B=0, G=1, R=2, rgb_tag }; };       //----order_bgr
    struct order_rgba { enum rgba_e { R=0, G=1, B=2, A=3, rgba_tag }; }; //----order_rgba
    struct order_argb { enum argb_e { A=0, R=1, G=2, B=3, rgba_tag }; }; //----order_argb
    struct order_abgr { enum abgr_e { A=0, B=1, G=2, R=3, rgba_tag }; }; //----order_abgr
    struct order_bgra { enum bgra_e { B=0, G=1, R=2, A=3, rgba_tag }; }; //----order_bgra

    //====================================================================rgba
    struct rgba
    {
        typedef float value_type;

        float r;
        float g;
        float b;
        float a;

        //--------------------------------------------------------------------
        rgba() {}

        //--------------------------------------------------------------------
        rgba(float r_, float g_, float b_, float a_=1.0) :
            r(r_), g(g_), b(b_), a(a_) {}

        //--------------------------------------------------------------------
        rgba(const rgba& c, float a_) : r(c.r), g(c.g), b(c.b), a(a_) {}

        //--------------------------------------------------------------------
        void clear()
        {
            r = g = b = a = 0;
        }

        //--------------------------------------------------------------------
        const rgba& transparent()
        {
            a = 0.0;
            return *this;
        }

        //--------------------------------------------------------------------
        const rgba& opacity(float a_)
        {
            if(a_ < 0.0f) a_ = 0.0f;
            if(a_ > 1.0f) a_ = 1.0f;
            a = a_;
            return *this;
        }

        //--------------------------------------------------------------------
        float opacity() const
        {
            return a;
        }

        //--------------------------------------------------------------------
        const rgba& premultiply()
        {
            r *= a;
            g *= a;
            b *= a;
            return *this;
        }

        //--------------------------------------------------------------------
        const rgba& premultiply(float a_)
        {
            if(a <= 0.0 || a_ <= 0.0)
            {
                r = g = b = a = 0.0;
                return *this;
            }
            a_ /= a;
            r *= a_;
            g *= a_;
            b *= a_;
            a  = a_;
            return *this;
        }

        //--------------------------------------------------------------------
        const rgba& demultiply()
        {
            if(a == 0)
            {
                r = g = b = 0;
                return *this;
            }
            float a_ = 1.0f / a;
            r *= a_;
            g *= a_;
            b *= a_;
            return *this;
        }


        //--------------------------------------------------------------------
        rgba gradient(rgba c, float k) const
        {
            rgba ret;
            ret.r = r + (c.r - r) * k;
            ret.g = g + (c.g - g) * k;
            ret.b = b + (c.b - b) * k;
            ret.a = a + (c.a - a) * k;
            return ret;
        }

        //--------------------------------------------------------------------
        static rgba no_color() { return rgba(0,0,0,0); }

        //--------------------------------------------------------------------
        static rgba from_wavelength(float wl, float gamma = 1.0f);
	
        //--------------------------------------------------------------------
        explicit rgba(float wavelen, float gamma=1.0)
        {
            *this = from_wavelength(wavelen, gamma);
        }

    };

    //----------------------------------------------------------------rgba_pre
    inline rgba rgba_pre(float r, float g, float b, float a=1.0f)
    {
        return rgba(r, g, b, a).premultiply();
    }
    inline rgba rgba_pre(const rgba& c)
    {
        return rgba(c).premultiply();
    }
    inline rgba rgba_pre(const rgba& c, float a)
    {
        return rgba(c, a).premultiply();
    }

    //------------------------------------------------------------------------
    inline rgba rgba::from_wavelength(float wl, float gamma)
    {
        rgba t(0.0f, 0.0f, 0.0f);

        if(wl >= 380.0f && wl <= 440.0f)
        {
            t.r = -1.0f * (wl - 440.0f) / (440.0f - 380.0f);
            t.b = 1.0f;
        }
        else 
        if(wl >= 440.0f && wl <= 490.0f)
        {
            t.g = (wl - 440.0f) / (490.0f - 440.0f);
            t.b = 1.0f;
        }
        else
        if(wl >= 490.0f && wl <= 510.0f)
        {
            t.g = 1.0f;
            t.b = -1.0f * (wl - 510.0f) / (510.0f - 490.0f);
        }
        else
        if(wl >= 510.0f && wl <= 580.0f)
        {
            t.r = (wl - 510.0f) / (580.0f - 510.0f);
            t.g = 1.0f;
        }
        else
        if(wl >= 580.0f && wl <= 645.0f)
        {
            t.r = 1.0f;
            t.g = -1.0f * (wl - 645.0f) / (645.0f - 580.0f);
        }
        else
        if(wl >= 645.0f && wl <= 780.0f)
        {
            t.r = 1.0f;
        }

        float s = 1.0f;
        if(wl > 700.0f)       s = 0.3f + 0.7f * (780.0f - wl) / (780.0f - 700.0f);
        else if(wl <  420.0f) s = 0.3f + 0.7f * (wl - 380.0f) / (420.0f - 380.0f);

        t.r = (float)pow(t.r * s, gamma);
        t.g = (float)pow(t.g * s, gamma);
        t.b = (float)pow(t.b * s, gamma);
        return t;
    }



    
    //===================================================================rgba8
    struct rgba8
    {
        typedef int8u  value_type;
        typedef int32u calc_type;
        typedef int32  long_type;
        enum base_scale_e
        {
            base_shift = 8,
            base_scale = 1 << base_shift,
            base_mask  = base_scale - 1
        };
        typedef rgba8 self_type;


        value_type r;
        value_type g;
        value_type b;
        value_type a;

        //--------------------------------------------------------------------
        rgba8() {}

        //--------------------------------------------------------------------
        rgba8(unsigned r_, unsigned g_, unsigned b_, unsigned a_=base_mask) :
            r(value_type(r_)), 
            g(value_type(g_)), 
            b(value_type(b_)), 
            a(value_type(a_)) {}

        //--------------------------------------------------------------------
        rgba8(const rgba& c, float a_) :
            r((value_type)uround(c.r * float(base_mask))), 
            g((value_type)uround(c.g * float(base_mask))), 
            b((value_type)uround(c.b * float(base_mask))), 
            a((value_type)uround(a_  * float(base_mask))) {}

        //--------------------------------------------------------------------
        rgba8(const self_type& c, unsigned a_) :
            r(c.r), g(c.g), b(c.b), a(value_type(a_)) {}

        //--------------------------------------------------------------------
        rgba8(const rgba& c) :
            r((value_type)uround(c.r * float(base_mask))), 
            g((value_type)uround(c.g * float(base_mask))), 
            b((value_type)uround(c.b * float(base_mask))), 
            a((value_type)uround(c.a * float(base_mask))) {}

        //--------------------------------------------------------------------
        void clear()
        {
            r = g = b = a = 0;
        }
		void invert()
		{
			r = 255-r;
			g = 255-g;
			b = 255-b;
		}
        
        //--------------------------------------------------------------------
        const self_type& transparent()
        {
            a = 0;
            return *this;
        }

        //--------------------------------------------------------------------
        const self_type& opacity(float a_)
        {
            if(a_ < 0.0) a_ = 0.0;
            if(a_ > 1.0) a_ = 1.0;
            a = (value_type)uround(a_ * float(base_mask));
            return *this;
        }

        //--------------------------------------------------------------------
        float opacity() const
        {
            return float(a) / float(base_mask);
        }

        //--------------------------------------------------------------------
        AGG_INLINE const self_type& premultiply()
        {
            if(a == base_mask) return *this;
            if(a == 0)
            {
                r = g = b = 0;
                return *this;
            }
            r = value_type((calc_type(r) * a) >> base_shift);
            g = value_type((calc_type(g) * a) >> base_shift);
            b = value_type((calc_type(b) * a) >> base_shift);
            return *this;
        }

        //--------------------------------------------------------------------
        AGG_INLINE const self_type& premultiply(unsigned a_)
        {
            if(a == base_mask && a_ >= base_mask) return *this;
            if(a == 0 || a_ == 0)
            {
                r = g = b = a = 0;
                return *this;
            }
            calc_type r_ = (calc_type(r) * a_) / a;
            calc_type g_ = (calc_type(g) * a_) / a;
            calc_type b_ = (calc_type(b) * a_) / a;
            r = value_type((r_ > a_) ? a_ : r_);
            g = value_type((g_ > a_) ? a_ : g_);
            b = value_type((b_ > a_) ? a_ : b_);
            a = value_type(a_);
            return *this;
        }

        //--------------------------------------------------------------------
        AGG_INLINE const self_type& demultiply()
        {
            if(a == base_mask) return *this;
            if(a == 0)
            {
                r = g = b = 0;
                return *this;
            }
            calc_type r_ = (calc_type(r) * base_mask) / a;
            calc_type g_ = (calc_type(g) * base_mask) / a;
            calc_type b_ = (calc_type(b) * base_mask) / a;
            r = value_type((r_ > calc_type(base_mask)) ? calc_type(base_mask) : r_);
            g = value_type((g_ > calc_type(base_mask)) ? calc_type(base_mask) : g_);
            b = value_type((b_ > calc_type(base_mask)) ? calc_type(base_mask) : b_);
            return *this;
        }

        //--------------------------------------------------------------------
        AGG_INLINE self_type gradient(const self_type& c, float k) const
        {
            self_type ret;
            calc_type ik = uround(k * base_scale);
            ret.r = value_type(calc_type(r) + (((calc_type(c.r) - r) * ik) >> base_shift));
            ret.g = value_type(calc_type(g) + (((calc_type(c.g) - g) * ik) >> base_shift));
            ret.b = value_type(calc_type(b) + (((calc_type(c.b) - b) * ik) >> base_shift));
            ret.a = value_type(calc_type(a) + (((calc_type(c.a) - a) * ik) >> base_shift));
            return ret;
        }

        //--------------------------------------------------------------------
        AGG_INLINE void add(const self_type& c, unsigned cover)
        {
            calc_type cr, cg, cb, ca;
            if(cover == cover_mask)
            {
                if(c.a == base_mask) 
                {
                    *this = c;
                }
                else
                {
                    cr = r + c.r; r = (cr > calc_type(base_mask)) ? calc_type(base_mask) : cr;
                    cg = g + c.g; g = (cg > calc_type(base_mask)) ? calc_type(base_mask) : cg;
                    cb = b + c.b; b = (cb > calc_type(base_mask)) ? calc_type(base_mask) : cb;
                    ca = a + c.a; a = (ca > calc_type(base_mask)) ? calc_type(base_mask) : ca;
                }
            }
            else
            {
                cr = r + ((c.r * cover + cover_mask/2) >> cover_shift);
                cg = g + ((c.g * cover + cover_mask/2) >> cover_shift);
                cb = b + ((c.b * cover + cover_mask/2) >> cover_shift);
                ca = a + ((c.a * cover + cover_mask/2) >> cover_shift);
                r = (cr > calc_type(base_mask)) ? calc_type(base_mask) : cr;
                g = (cg > calc_type(base_mask)) ? calc_type(base_mask) : cg;
                b = (cb > calc_type(base_mask)) ? calc_type(base_mask) : cb;
                a = (ca > calc_type(base_mask)) ? calc_type(base_mask) : ca;
            }
        }

        //--------------------------------------------------------------------
        template<class GammaLUT>
        AGG_INLINE void apply_gamma_dir(const GammaLUT& gamma)
        {
            r = gamma.dir(r);
            g = gamma.dir(g);
            b = gamma.dir(b);
        }

        //--------------------------------------------------------------------
        template<class GammaLUT>
        AGG_INLINE void apply_gamma_inv(const GammaLUT& gamma)
        {
            r = gamma.inv(r);
            g = gamma.inv(g);
            b = gamma.inv(b);
        }

        //--------------------------------------------------------------------
        static self_type no_color() { return self_type(0,0,0,0); }

        //--------------------------------------------------------------------
        static self_type from_wavelength(float wl, float gamma = 1.0f)
        {
            return self_type(rgba::from_wavelength(wl, gamma));
        }
    };


    //-------------------------------------------------------------rgba8_pre
    inline rgba8 rgba8_pre(unsigned r, unsigned g, unsigned b, 
                           unsigned a = rgba8::base_mask)
    {
        return rgba8(r,g,b,a).premultiply();
    }
    inline rgba8 rgba8_pre(const rgba8& c)
    {
        return rgba8(c).premultiply();
    }
    inline rgba8 rgba8_pre(const rgba8& c, unsigned a)
    {
        return rgba8(c,a).premultiply();
    }
    inline rgba8 rgba8_pre(const rgba& c)
    {
        return rgba8(c).premultiply();
    }
    inline rgba8 rgba8_pre(const rgba& c, float a)
    {
        return rgba8(c,a).premultiply();
    }


    //-------------------------------------------------------------rgb8_packed
    inline rgba8 rgb8_packed(unsigned v)
    {
        return rgba8((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
    }

    //-------------------------------------------------------------bgr8_packed
    inline rgba8 bgr8_packed(unsigned v)
    {
        return rgba8(v & 0xFF, (v >> 8) & 0xFF, (v >> 16) & 0xFF);
    }

    //------------------------------------------------------------argb8_packed
    inline rgba8 argb8_packed(unsigned v)
    {
        return rgba8((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF, v >> 24);
    }

    //---------------------------------------------------------rgba8_gamma_dir
    template<class GammaLUT>
    rgba8 rgba8_gamma_dir(rgba8 c, const GammaLUT& gamma)
    {
        return rgba8(gamma.dir(c.r), gamma.dir(c.g), gamma.dir(c.b), c.a);
    }

    //---------------------------------------------------------rgba8_gamma_inv
    template<class GammaLUT>
    rgba8 rgba8_gamma_inv(rgba8 c, const GammaLUT& gamma)
    {
        return rgba8(gamma.inv(c.r), gamma.inv(c.g), gamma.inv(c.b), c.a);
    }





    //==================================================================rgba16
    struct rgba16
    {
        typedef int16u value_type;
        typedef int32u calc_type;
        typedef int64  long_type;
        enum base_scale_e
        {
            base_shift = 16,
            base_scale = 1 << base_shift,
            base_mask  = base_scale - 1
        };
        typedef rgba16 self_type;

        value_type r;
        value_type g;
        value_type b;
        value_type a;

        //--------------------------------------------------------------------
        rgba16() {}

        //--------------------------------------------------------------------
        rgba16(unsigned r_, unsigned g_, unsigned b_, unsigned a_=base_mask) :
            r(value_type(r_)), 
            g(value_type(g_)), 
            b(value_type(b_)), 
            a(value_type(a_)) {}

        //--------------------------------------------------------------------
        rgba16(const self_type& c, unsigned a_) :
            r(c.r), g(c.g), b(c.b), a(value_type(a_)) {}

        //--------------------------------------------------------------------
        rgba16(const rgba& c) :
            r((value_type)uround(c.r * float(base_mask))), 
            g((value_type)uround(c.g * float(base_mask))), 
            b((value_type)uround(c.b * float(base_mask))), 
            a((value_type)uround(c.a * float(base_mask))) {}

        //--------------------------------------------------------------------
        rgba16(const rgba& c, float a_) :
            r((value_type)uround(c.r * float(base_mask))), 
            g((value_type)uround(c.g * float(base_mask))), 
            b((value_type)uround(c.b * float(base_mask))), 
            a((value_type)uround(a_  * float(base_mask))) {}

        //--------------------------------------------------------------------
        rgba16(const rgba8& c) :
            r(value_type((value_type(c.r) << 8) | c.r)), 
            g(value_type((value_type(c.g) << 8) | c.g)), 
            b(value_type((value_type(c.b) << 8) | c.b)), 
            a(value_type((value_type(c.a) << 8) | c.a)) {}

        //--------------------------------------------------------------------
        rgba16(const rgba8& c, unsigned a_) :
            r(value_type((value_type(c.r) << 8) | c.r)), 
            g(value_type((value_type(c.g) << 8) | c.g)), 
            b(value_type((value_type(c.b) << 8) | c.b)), 
            a(value_type((             a_ << 8) | c.a)) {}

        //--------------------------------------------------------------------
        void clear()
        {
            r = g = b = a = 0;
        }
        
        //--------------------------------------------------------------------
        const self_type& transparent()
        {
            a = 0;
            return *this;
        }

        //--------------------------------------------------------------------
        AGG_INLINE const self_type& opacity(float a_)
        {
            if(a_ < 0.0f) a_ = 0.0f;
            if(a_ > 1.0f) a_ = 1.0f;
            a = (value_type)uround(a_ * float(base_mask));
            return *this;
        }

        //--------------------------------------------------------------------
        float opacity() const
        {
            return float(a) / float(base_mask);
        }

        //--------------------------------------------------------------------
        AGG_INLINE const self_type& premultiply()
        {
            if(a == base_mask) return *this;
            if(a == 0)
            {
                r = g = b = 0;
                return *this;
            }
            r = value_type((calc_type(r) * a) >> base_shift);
            g = value_type((calc_type(g) * a) >> base_shift);
            b = value_type((calc_type(b) * a) >> base_shift);
            return *this;
        }

        //--------------------------------------------------------------------
        AGG_INLINE const self_type& premultiply(unsigned a_)
        {
            if(a == base_mask && a_ >= base_mask) return *this;
            if(a == 0 || a_ == 0)
            {
                r = g = b = a = 0;
                return *this;
            }
            calc_type r_ = (calc_type(r) * a_) / a;
            calc_type g_ = (calc_type(g) * a_) / a;
            calc_type b_ = (calc_type(b) * a_) / a;
            r = value_type((r_ > a_) ? a_ : r_);
            g = value_type((g_ > a_) ? a_ : g_);
            b = value_type((b_ > a_) ? a_ : b_);
            a = value_type(a_);
            return *this;
        }

        //--------------------------------------------------------------------
        AGG_INLINE const self_type& demultiply()
        {
            if(a == base_mask) return *this;
            if(a == 0)
            {
                r = g = b = 0;
                return *this;
            }
            calc_type r_ = (calc_type(r) * base_mask) / a;
            calc_type g_ = (calc_type(g) * base_mask) / a;
            calc_type b_ = (calc_type(b) * base_mask) / a;
            r = value_type((r_ > calc_type(base_mask)) ? calc_type(base_mask) : r_);
            g = value_type((g_ > calc_type(base_mask)) ? calc_type(base_mask) : g_);
            b = value_type((b_ > calc_type(base_mask)) ? calc_type(base_mask) : b_);
            return *this;
        }

        //--------------------------------------------------------------------
        AGG_INLINE self_type gradient(const self_type& c, float k) const
        {
            self_type ret;
            calc_type ik = uround(k * base_scale);
            ret.r = value_type(calc_type(r) + (((calc_type(c.r) - r) * ik) >> base_shift));
            ret.g = value_type(calc_type(g) + (((calc_type(c.g) - g) * ik) >> base_shift));
            ret.b = value_type(calc_type(b) + (((calc_type(c.b) - b) * ik) >> base_shift));
            ret.a = value_type(calc_type(a) + (((calc_type(c.a) - a) * ik) >> base_shift));
            return ret;
        }

        //--------------------------------------------------------------------
        AGG_INLINE void add(const self_type& c, unsigned cover)
        {
            calc_type cr, cg, cb, ca;
            if(cover == cover_mask)
            {
                if(c.a == base_mask) 
                {
                    *this = c;
                }
                else
                {
                    cr = r + c.r; r = (cr > calc_type(base_mask)) ? calc_type(base_mask) : cr;
                    cg = g + c.g; g = (cg > calc_type(base_mask)) ? calc_type(base_mask) : cg;
                    cb = b + c.b; b = (cb > calc_type(base_mask)) ? calc_type(base_mask) : cb;
                    ca = a + c.a; a = (ca > calc_type(base_mask)) ? calc_type(base_mask) : ca;
                }
            }
            else
            {
                cr = r + ((c.r * cover + cover_mask) >> cover_shift);
                cg = g + ((c.g * cover + cover_mask) >> cover_shift);
                cb = b + ((c.b * cover + cover_mask) >> cover_shift);
                ca = a + ((c.a * cover + cover_mask) >> cover_shift);
                r = (cr > calc_type(base_mask)) ? calc_type(base_mask) : cr;
                g = (cg > calc_type(base_mask)) ? calc_type(base_mask) : cg;
                b = (cb > calc_type(base_mask)) ? calc_type(base_mask) : cb;
                a = (ca > calc_type(base_mask)) ? calc_type(base_mask) : ca;
            }
        }

        //--------------------------------------------------------------------
        template<class GammaLUT>
        AGG_INLINE void apply_gamma_dir(const GammaLUT& gamma)
        {
            r = gamma.dir(r);
            g = gamma.dir(g);
            b = gamma.dir(b);
        }

        //--------------------------------------------------------------------
        template<class GammaLUT>
        AGG_INLINE void apply_gamma_inv(const GammaLUT& gamma)
        {
            r = gamma.inv(r);
            g = gamma.inv(g);
            b = gamma.inv(b);
        }

        //--------------------------------------------------------------------
        static self_type no_color() { return self_type(0,0,0,0); }

        //--------------------------------------------------------------------
        static self_type from_wavelength(float wl, float gamma = 1.0f)
        {
            return self_type(rgba::from_wavelength(wl, gamma));
        }
    };



    //--------------------------------------------------------------rgba16_pre
    inline rgba16 rgba16_pre(unsigned r, unsigned g, unsigned b, 
                             unsigned a = rgba16::base_mask)
    {
        return rgba16(r,g,b,a).premultiply();
    }
    inline rgba16 rgba16_pre(const rgba16& c, unsigned a)
    {
        return rgba16(c,a).premultiply();
    }
    inline rgba16 rgba16_pre(const rgba& c)
    {
        return rgba16(c).premultiply();
    }
    inline rgba16 rgba16_pre(const rgba& c, float a)
    {
        return rgba16(c,a).premultiply();
    }
    inline rgba16 rgba16_pre(const rgba8& c)
    {
        return rgba16(c).premultiply();
    }
    inline rgba16 rgba16_pre(const rgba8& c, unsigned a)
    {
        return rgba16(c,a).premultiply();
    }


    //------------------------------------------------------rgba16_gamma_dir
    template<class GammaLUT>
    rgba16 rgba16_gamma_dir(rgba16 c, const GammaLUT& gamma)
    {
        return rgba16(gamma.dir(c.r), gamma.dir(c.g), gamma.dir(c.b), c.a);
    }

    //------------------------------------------------------rgba16_gamma_inv
    template<class GammaLUT>
    rgba16 rgba16_gamma_inv(rgba16 c, const GammaLUT& gamma)
    {
        return rgba16(gamma.inv(c.r), gamma.inv(c.g), gamma.inv(c.b), c.a);
    }


}



#endif
