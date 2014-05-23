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

#ifndef AGG_ROUNDED_RECT_INCLUDED
#define AGG_ROUNDED_RECT_INCLUDED

#include "agg_basics.h"
#include "agg_arc.h"

namespace agg
{
    //------------------------------------------------------------rounded_rect
    //
    // See Implemantation agg_rounded_rect.cpp
    //
    class rounded_rect
    {
    public:
        rounded_rect() {}
        rounded_rect(float x1, float y1, float x2, float y2, float r);

        void rect(float x1, float y1, float x2, float y2);
        void radius(float r);
        void radius(float rx, float ry);
        void radius(float rx_bottom, float ry_bottom, float rx_top, float ry_top);
        void radius(float rx1, float ry1, float rx2, float ry2, 
                    float rx3, float ry3, float rx4, float ry4);
        void normalize_radius();

        void approximation_scale(float s) { m_arc.approximation_scale(s); }
        float approximation_scale() const { return m_arc.approximation_scale(); }

        void rewind(unsigned);
        unsigned vertex(float* x, float* y);
		

    protected:
        float m_x1;
        float m_y1;
        float m_x2;
        float m_y2;
        float m_rx1;
        float m_ry1;
        float m_rx2;
        float m_ry2;
        float m_rx3;
        float m_ry3;
        float m_rx4;
        float m_ry4;	
        unsigned m_status;
        arc      m_arc;
    };

	class part_round_rect : public rounded_rect
	{
	public:
		part_round_rect(){};
		part_round_rect(float x1, float y1, float x2, float y2, float r):rounded_rect(x1,y1,x2,y2,r){};
		unsigned vertex(float* x, float* y);
		void setDirection(int direction_){m_direction = direction_;};
	protected:
		int m_direction;
	};

	class distorted_rect
	{
	public:
		distorted_rect() {}
		distorted_rect(float x1, float y1, float x2, float y2, float r1, float fDist);

		void rect(float x1, float y1, float x2, float y2);
		void radius_rounded(float r) { m_r1 = r; }
		void radius_distorted(float r) { m_r2 = r; }
		void normalize_radius(bool bDefaultR2 = false, float fDist = 0.0f);
		
		void approximation_rounded_scale(float s) { m_arc1.approximation_scale(s); }
		void approximation_distorted_scale(float s) { m_arc2.approximation_scale(s); }
		float approximation_rounded_scale() const { return m_arc1.approximation_scale(); }
		float approximation_distorted_scale() const { return m_arc2.approximation_scale(); }

		void rewind(unsigned);
		unsigned vertex(float* x, float* y);
	
	private:
		unsigned m_status;

		float m_x1;
		float m_y1;
		float m_x2;
		float m_y2;
		float m_r1; 
		float m_r2;

		arc m_arc1;
		arc m_arc2;
	};
}

#endif

