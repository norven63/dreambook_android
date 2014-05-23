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

#ifndef AGG_STROKE_MATH_INCLUDED
#define AGG_STROKE_MATH_INCLUDED

#include "agg_math.h"
#include "agg_vertex_sequence.h"

namespace agg
{
    //-------------------------------------------------------------line_cap_e
    enum line_cap_e
    {
        butt_cap,
        square_cap,
        round_cap
    };

    //------------------------------------------------------------line_join_e
    enum line_join_e
    {
        miter_join         = 0,
        miter_join_revert  = 1,
        round_join         = 2,
        bevel_join         = 3,
        miter_join_round   = 4
    };


    //-----------------------------------------------------------inner_join_e
    enum inner_join_e
    {
        inner_bevel,
        inner_miter,
        inner_jag,
        inner_round
    };

    //------------------------------------------------------------math_stroke
    template<class VertexConsumer> class math_stroke
    {
    public:
        typedef typename VertexConsumer::value_type coord_type;

        math_stroke();

        void line_cap(line_cap_e lc)     { m_line_cap = lc; }
        void line_join(line_join_e lj)   { m_line_join = lj; }
        void inner_join(inner_join_e ij) { m_inner_join = ij; }

        line_cap_e   line_cap()   const { return m_line_cap; }
        line_join_e  line_join()  const { return m_line_join; }
        inner_join_e inner_join() const { return m_inner_join; }

        void width(float w);
		 float width_abs() { return m_width_abs; }
		 float GetWidth(){return m_width;};
        void miter_limit(float ml) { m_miter_limit = ml; }
        void miter_limit_theta(float t);
        void inner_miter_limit(float ml) { m_inner_miter_limit = ml; }
        void approximation_scale(float as) { m_approx_scale = as; }

        float width() const { return m_width * 2.0f; }
        float miter_limit() const { return m_miter_limit; }
        float inner_miter_limit() const { return m_inner_miter_limit; }
        float approximation_scale() const { return m_approx_scale; }

        void calc_cap(VertexConsumer& vc,
                      const vertex_dist& v0, 
                      const vertex_dist& v1, 
                      float len,
					  int nHalf, bool bFront);
		void calc_half_cap(VertexConsumer& vc, 
						const vertex_dist& v0, 
						const vertex_dist& v1,
						float len,
						int nHalf, bool bFront);

        void calc_join(VertexConsumer& vc,
                       const vertex_dist& v0, 
                       const vertex_dist& v1, 
                       const vertex_dist& v2,
                       float len1, 
                       float len2, bool bInner = false);

		 void normalize_2(float& x, float& y);

        AGG_INLINE void add_vertex(VertexConsumer& vc, float x, float y)
        {
            vc.add(coord_type(x, y));
        }

	private:
        void calc_arc(VertexConsumer& vc,
                      float x,   float y, 
                      float dx1, float dy1, 
                      float dx2, float dy2);

        void calc_miter(VertexConsumer& vc,
                        const vertex_dist& v0, 
                        const vertex_dist& v1, 
                        const vertex_dist& v2,
                        float dx1, float dy1, 
                        float dx2, float dy2,
                        line_join_e lj,
                        float mlimit,
                        float dbevel);

        float       m_width;
        float       m_width_abs;
        float       m_width_eps;
        int          m_width_sign;
        float       m_miter_limit;
        float       m_inner_miter_limit;
        float       m_approx_scale;
        line_cap_e   m_line_cap;
        line_join_e  m_line_join;
        inner_join_e m_inner_join;
    };

    //-----------------------------------------------------------------------
    template<class VC> math_stroke<VC>::math_stroke() :
        m_width(0.5f),
        m_width_abs(0.5f),
        m_width_eps(0.5f/1024.0f),
        m_width_sign(1),
        m_miter_limit(4.0f),
        m_inner_miter_limit(1.01f),
        m_approx_scale(1.0f),
        m_line_cap(butt_cap),
        m_line_join(miter_join),
        m_inner_join(inner_miter)
    {
    }

    //-----------------------------------------------------------------------
    template<class VC> void math_stroke<VC>::width(float w)
    { 
        m_width = w * 0.5f; 
        if(m_width < 0)
        {
            m_width_abs  = -m_width;
            m_width_sign = -1;
        }
        else
        {
            m_width_abs  = m_width;
            m_width_sign = 1;
        }
        m_width_eps = m_width / 1024.0f;
    }

    //-----------------------------------------------------------------------
    template<class VC> void math_stroke<VC>::miter_limit_theta(float t)
    { 
        m_miter_limit = 1.0f / (float)sin(t * 0.5f) ;
    }

    //-----------------------------------------------------------------------
    template<class VC> 
    void math_stroke<VC>::calc_arc(VC& vc,
                                   float x,   float y, 
                                   float dx1, float dy1, 
                                   float dx2, float dy2)
    {
        float a1 = (float)atan2(dy1 * m_width_sign, dx1 * m_width_sign);
        float a2 = (float)atan2(dy2 * m_width_sign, dx2 * m_width_sign);
        float da = a1 - a2;
        int i, n;

        da = (float)acos(m_width_abs / (m_width_abs + 0.125f / m_approx_scale)) * 2;

        add_vertex(vc, x + dx1, y + dy1);
        if(m_width_sign > 0)
        {
            if(a1 > a2) a2 += 2 * pi;
            n = int((a2 - a1) / da);
            da = (a2 - a1) / (n + 1);
            a1 += da;
            for(i = 0; i < n; i++)
            {
                add_vertex(vc, x + (float)cos(a1) * m_width, y + (float)sin(a1) * m_width);
                a1 += da;
            }
        }
        else
        {
            if(a1 < a2) a2 -= 2 * pi;
            n = int((a1 - a2) / da);
            da = (a1 - a2) / (n + 1);
            a1 -= da;
            for(i = 0; i < n; i++)
            {
                add_vertex(vc, x + (float)cos(a1) * m_width, y + (float)sin(a1) * m_width);
                a1 -= da;
            }
        }
        add_vertex(vc, x + dx2, y + dy2);
    }

    //-----------------------------------------------------------------------
    template<class VC> 
    void math_stroke<VC>::calc_miter(VC& vc,
                                     const vertex_dist& v0, 
                                     const vertex_dist& v1, 
                                     const vertex_dist& v2,
                                     float dx1, float dy1, 
                                     float dx2, float dy2,
                                     line_join_e lj,
                                     float mlimit,
                                     float dbevel)
    {
        float xi  = v1.x;
        float yi  = v1.y;
        float di  = 1;
        float lim = m_width_abs * mlimit;
        bool miter_limit_exceeded = true; // Assume the worst
        bool intersection_failed  = true; // Assume the worst

        if(calc_intersection(v0.x + dx1, v0.y - dy1,
                             v1.x + dx1, v1.y - dy1,
                             v1.x + dx2, v1.y - dy2,
                             v2.x + dx2, v2.y - dy2,
                             &xi, &yi))
        {
            // Calculation of the intersection succeeded
            //---------------------
            di = calc_distance(v1.x, v1.y, xi, yi);
            if(di <= lim)
            {
                // Inside the miter limit
                //---------------------
                add_vertex(vc, xi, yi);
                miter_limit_exceeded = false;
            }
            intersection_failed = false;
        }
        else
        {
            // Calculation of the intersection failed, most probably
            // the three points lie one straight line. 
            // First check if v0 and v2 lie on the opposite sides of vector: 
            // (v1.x, v1.y) -> (v1.x+dx1, v1.y-dy1), that is, the perpendicular
            // to the line determined by vertices v0 and v1.
            // This condition determines whether the next line segments continues
            // the previous one or goes back.
            //----------------
            float x2 = v1.x + dx1;
            float y2 = v1.y - dy1;
            if((cross_product(v0.x, v0.y, v1.x, v1.y, x2, y2) < 0.0f) == 
               (cross_product(v1.x, v1.y, v2.x, v2.y, x2, y2) < 0.0f))
            {
                // This case means that the next segment continues 
                // the previous one (straight line)
                //-----------------
                add_vertex(vc, v1.x + dx1, v1.y - dy1);
                miter_limit_exceeded = false;
            }
        }

        if(miter_limit_exceeded)
        {
            // Miter limit exceeded
            //------------------------
            switch(lj)
            {
            case miter_join_revert:
                // For the compatibility with SVG, PDF, etc, 
                // we use a simple bevel join instead of
                // "smart" bevel
                //-------------------
                add_vertex(vc, v1.x + dx1, v1.y - dy1);
                add_vertex(vc, v1.x + dx2, v1.y - dy2);
                break;

            case miter_join_round:
                calc_arc(vc, v1.x, v1.y, dx1, -dy1, dx2, -dy2);
                break;

            default:
                // If no miter-revert, calculate new dx1, dy1, dx2, dy2
                //----------------
                if(intersection_failed)
                {
                    mlimit *= m_width_sign;
                    add_vertex(vc, v1.x + dx1 + dy1 * mlimit, 
                                   v1.y - dy1 + dx1 * mlimit);
                    add_vertex(vc, v1.x + dx2 - dy2 * mlimit, 
                                   v1.y - dy2 - dx2 * mlimit);
                }
                else
                {
                    float x1 = v1.x + dx1;
                    float y1 = v1.y - dy1;
                    float x2 = v1.x + dx2;
                    float y2 = v1.y - dy2;
                    di = (lim - dbevel) / (di - dbevel);
                    add_vertex(vc, x1 + (xi - x1) * di, 
                                   y1 + (yi - y1) * di);
                    add_vertex(vc, x2 + (xi - x2) * di, 
                                   y2 + (yi - y2) * di);
                }
                break;
            }
        }
    }

    //--------------------------------------------------------stroke_calc_cap

	template<class VC>
	void math_stroke<VC>::calc_half_cap(VC& vc, 
										const vertex_dist& v0, 
										const vertex_dist& v1,
										float len,
										int nHalf, bool bFront)
	{
		float dx1 = (v1.y - v0.y) / len;
		float dy1 = (v1.x - v0.x) / len;
		float dx2 = 0;
		float dy2 = 0;

		dx1 *= m_width;
		dy1 *= m_width;

		float da, a1;
		int i, n = 5;
		da = pi / (n + 1);

		n = 3;
		if (bFront)
		{
			if (nHalf == -1)
				add_vertex(vc, v0.x - dx1, v0.y + dy1);

			a1 = (float)atan2(dy1, -dx1);
			a1 += da;

			if (nHalf == 1)
				a1 += 2*da;

			for(i = 0; i < n; i++)
			{
				add_vertex(vc, v0.x + (float)cos(a1) * m_width, 
					v0.y + (float)sin(a1) * m_width);
				a1 += da;
			}
		
			if (nHalf == 1)
				add_vertex(vc, v0.x + dx1, v0.y - dy1);
		}
		else
		{
			if (nHalf == 1)
				add_vertex(vc, v0.x - dx1, v0.y + dy1);

			a1 = (float)atan2(dy1, -dx1);
			a1 += da;

			if (nHalf == -1)
				a1 += 2*da;

			for(i = 0; i < n; i++)
			{
				add_vertex(vc, v0.x + (float)cos(a1) * m_width, 
					v0.y + (float)sin(a1) * m_width);
				a1 += da;
			}

			if (nHalf == -1)
				add_vertex(vc, v0.x + dx1, v0.y - dy1);
		}

	}
    template<class VC> 
    void math_stroke<VC>::calc_cap(VC& vc,
                                   const vertex_dist& v0, 
                                   const vertex_dist& v1, 
                                   float len,
								   int nHalf, bool bFront)
    {
		const static float f_zero = 0.0f;
		const static float f_limit = 0.5f; // 30
		vc.remove_all();

		float dx = (v1.y - v0.y);// / len;
		float dy = (v1.x - v0.x);// / len;
		normalize_2(dx,dy);
	
		float dx1 = dx * m_width;
		float dy1 = dy * m_width;
		float dx2 = dy1 * 0.3f;
		float dy2 = dx1 * 0.3f;

		if (nHalf == 1 || nHalf == -1)
		{
			calc_half_cap(vc, v0, v1, len, nHalf, bFront);
		}
		else
		{
			add_vertex(vc, v0.x - dx1 - dx2, v0.y + dy1 - dy2);
			if (m_width_abs > 4.0f)
			{
				if ((dx < -f_limit || dx > f_limit) && (dy < -f_limit || dy > f_limit))
				{
					if ((dx > f_zero && dy > f_zero) || (dx < f_zero && dy < f_zero))
						add_vertex(vc, v0.x - dx1, v0.y - dy1);
					else
						add_vertex(vc, v0.x + dx1, v0.y + dy1);
				}
				else
				{
					if (dy > -f_limit && dy < f_limit)
						add_vertex(vc, v0.x, v0.y - dx1);
					else
						add_vertex(vc, v0.x - dy1, v0.y);
				}
			}

			add_vertex(vc, v0.x + dx1 - dx2, v0.y - dy1 - dy2);
		}

        //vc.remove_all();

        //float dx1 = (v1.y - v0.y) / len;
        //float dy1 = (v1.x - v0.x) / len;
        //float dx2 = 0;
        //float dy2 = 0;

        //dx1 *= m_width;
        //dy1 *= m_width;

        //if(m_line_cap != round_cap)
        //{
        //    if(m_line_cap == square_cap)
        //    {
        //        dx2 = dy1 * m_width_sign;
        //        dy2 = dx1 * m_width_sign;
        //    }
        //    add_vertex(vc, v0.x - dx1 - dx2, v0.y + dy1 - dy2);
        //    add_vertex(vc, v0.x + dx1 - dx2, v0.y - dy1 - dy2);
        //}
        //else
        //{
        //    float da = pi/3.0f;//(float)acos(m_width_abs / (m_width_abs + 0.125f / m_approx_scale)) * 2;
        //    float a1;
        //    int i;
        //    int n = int(pi / da);

        //    da = pi / (n + 1);
        //    add_vertex(vc, v0.x - dx1, v0.y + dy1);
        //    if(m_width_sign > 0)
        //    {
        //        a1 = (float)atan2(dy1, -dx1);
        //        a1 += da;
        //        for(i = 0; i < n; i++)
        //        {
        //            add_vertex(vc, v0.x + (float)cos(a1) * m_width, 
        //                           v0.y + (float)sin(a1) * m_width);
        //            a1 += da;
        //        }
        //    }
        //    else
        //    {
        //        a1 = (float)atan2(-dy1, dx1);
        //        a1 -= da;
        //        for(i = 0; i < n; i++)
        //        {
        //            add_vertex(vc, v0.x + (float)cos(a1) * m_width, 
        //                           v0.y + (float)sin(a1) * m_width);
        //            a1 -= da;
        //        }
        //    }
        //    add_vertex(vc, v0.x + dx1, v0.y - dy1);
        //}
    }
	 
	//-----------------------------------------------------------------------
	template<class VC> 
	void math_stroke<VC>::normalize_2(float& x, float& y)
	{
		float len = (float)sqrt(x*x + y*y);
		x /= len;
		y /= len;
	}

    //-----------------------------------------------------------------------
    template<class VC> 
    void math_stroke<VC>::calc_join(VC& vc,
                                    const vertex_dist& v0, 
                                    const vertex_dist& v1, 
                                    const vertex_dist& v2,
                                    float len1, 
                                    float len2, bool bInner)
    {
		vc.remove_all();
		if (int(m_inner_join) ==int(miter_join))
		{
			if(fabs(v1.fcp)  >  0.00001f && m_width > 0)
			{
				if (bInner)
					add_vertex(vc, v1.x - v1.dx, v1.y - v1.dy);
				else
					add_vertex(vc, v1.x + v1.dx, v1.y + v1.dy);
			}
		}
		else
		{
			float dx1 = m_width * (v1.y - v0.y) / len1;
			float dy1 = m_width * (v1.x - v0.x) / len1;
			float dx2 = m_width * (v2.y - v1.y) / len2;
			float dy2 = m_width * (v2.x - v1.x) / len2;

			vc.remove_all();
			add_vertex(vc, v1.x + dx1, v1.y - dy1);
			add_vertex(vc, v1.x + dx2, v1.y - dy2);
		}

   //     float dx1 = m_width * (v1.y - v0.y) / len1;
   //     float dy1 = m_width * (v1.x - v0.x) / len1;
   //     float dx2 = m_width * (v2.y - v1.y) / len2;
   //     float dy2 = m_width * (v2.x - v1.x) / len2;

   //     vc.remove_all();

   //     float cp = cross_product(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
   //     if(cp != 0 && (cp > 0) == (m_width > 0))
   //     {
   //         // Inner join
   //         //---------------
   //         float limit = ((len1 < len2) ? len1 : len2) / m_width_abs;
   //         if(limit < m_inner_miter_limit)
   //         {
   //             limit = m_inner_miter_limit;
   //         }

   //         switch(m_inner_join)
   //         {
   //         default: // inner_bevel
   //             add_vertex(vc, v1.x + dx1, v1.y - dy1);
   //             add_vertex(vc, v1.x + dx2, v1.y - dy2);
   //             break;

   //         case inner_miter:
   //             calc_miter(vc,
   //                        v0, v1, v2, dx1, dy1, dx2, dy2, 
   //                        miter_join_revert, 
   //                        limit, 0);
   //             break;

   //         case inner_jag:
   //         case inner_round:
   //             cp = (dx1-dx2) * (dx1-dx2) + (dy1-dy2) * (dy1-dy2);
   //             if(cp < len1 * len1 && cp < len2 * len2)
   //             {
   //                 calc_miter(vc,
   //                            v0, v1, v2, dx1, dy1, dx2, dy2, 
   //                            miter_join_revert, 
   //                            limit, 0);
   //             }
   //             else
   //             {
   //                 if(m_inner_join == inner_jag)
   //                 {
   //                     add_vertex(vc, v1.x + dx1, v1.y - dy1);
   //                     add_vertex(vc, v1.x,       v1.y      );
   //                     add_vertex(vc, v1.x + dx2, v1.y - dy2);
   //                 }
   //                 else
   //                 {
   //                     add_vertex(vc, v1.x + dx1, v1.y - dy1);
   //                     add_vertex(vc, v1.x,       v1.y      );
   //                     calc_arc(vc, v1.x, v1.y, dx2, -dy2, dx1, -dy1);
   //                     add_vertex(vc, v1.x,       v1.y      );
   //                     add_vertex(vc, v1.x + dx2, v1.y - dy2);
   //                 }
   //             }
   //             break;
   //         }
   //     }
   //     else
   //     {
   //         // Outer join
   //         //---------------

   //         // Calculate the distance between v1 and 
   //         // the central point of the bevel line segment
   //         //---------------
   //         float dx = (dx1 + dx2) / 2;
   //         float dy = (dy1 + dy2) / 2;
			//float dbevel = (float)sqrt(dx * dx + dy * dy);

   //         if(m_line_join == round_join || m_line_join == bevel_join)
   //         {
   //             // This is an optimization that reduces the number of points 
   //             // in cases of almost collinear segments. If there's no
   //             // visible difference between bevel and miter joins we'd rather
   //             // use miter join because it adds only one point instead of two. 
   //             //
   //             // Here we calculate the middle point between the bevel points 
   //             // and then, the distance between v1 and this middle point. 
   //             // At outer joins this distance always less than stroke width, 
   //             // because it's actually the height of an isosceles triangle of
   //             // v1 and its two bevel points. If the difference between this
   //             // width and this value is small (no visible bevel) we can 
   //             // add just one point. 
   //             //
   //             // The constant in the expression makes the result approximately 
   //             // the same as in round joins and caps. You can safely comment 
   //             // out this entire "if".
   //             //-------------------
   //             if(m_approx_scale * (m_width_abs - dbevel) < m_width_eps)
   //             {
   //                 if(calc_intersection(v0.x + dx1, v0.y - dy1,
   //                                      v1.x + dx1, v1.y - dy1,
   //                                      v1.x + dx2, v1.y - dy2,
   //                                      v2.x + dx2, v2.y - dy2,
   //                                      &dx, &dy))
   //                 {
   //                     add_vertex(vc, dx, dy);
   //                 }
   //                 else
   //                 {
   //                     add_vertex(vc, v1.x + dx1, v1.y - dy1);
   //                 }
   //                 return;
   //             }
   //         }

   //         switch(m_line_join)
   //         {
   //         case miter_join:
   //         case miter_join_revert:
   //         case miter_join_round:
   //             calc_miter(vc, 
   //                        v0, v1, v2, dx1, dy1, dx2, dy2, 
   //                        m_line_join, 
   //                        m_miter_limit,
   //                        dbevel);
   //             break;

   //         case round_join:
   //             calc_arc(vc, v1.x, v1.y, dx1, -dy1, dx2, -dy2);
   //             break;

   //         default: // Bevel join
   //             add_vertex(vc, v1.x + dx1, v1.y - dy1);
   //             add_vertex(vc, v1.x + dx2, v1.y - dy2);
   //             break;
   //         }
   //     }
    }




}

#endif
