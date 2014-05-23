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

#ifndef AGG_GAMMA_FUNCTIONS_INCLUDED
#define AGG_GAMMA_FUNCTIONS_INCLUDED

#include <math.h>
#include "agg_basics.h"

namespace agg
{
    //===============================================================gamma_none
    struct gamma_none
    {
        float operator()(float x) const { return x; }
    };


    //==============================================================gamma_power
    class gamma_power
    {
    public:
        gamma_power() : m_gamma(1.0) {}
        gamma_power(float g) : m_gamma(g) {}

        void gamma(float g) { m_gamma = g; }
        float gamma() const { return m_gamma; }

        float operator() (float x) const
        {
            return (float)pow(x, m_gamma);
        }

    private:
        float m_gamma;
    };


    //==========================================================gamma_threshold
    class gamma_threshold
    {
    public:
        gamma_threshold() : m_threshold(0.5) {}
        gamma_threshold(float t) : m_threshold(t) {}

        void threshold(float t) { m_threshold = t; }
        float threshold() const { return m_threshold; }

        float operator() (float x) const
        {
            return (x < m_threshold) ? 0.0f : 1.0f;
        }

    private:
        float m_threshold;
    };


    //============================================================gamma_linear
    class gamma_linear
    {
    public:
        gamma_linear() : m_start(0.0), m_end(1.0) {}
        gamma_linear(float s, float e) : m_start(s), m_end(e) {}

        void set(float s, float e) { m_start = s; m_end = e; }
        void start(float s) { m_start = s; }
        void end(float e) { m_end = e; }
        float start() const { return m_start; }
        float end() const { return m_end; }

        float operator() (float x) const
        {
            if(x < m_start) return 0.0f;
            if(x > m_end) return 1.0f;
            return (x - m_start) / (m_end - m_start);
        }

    private:
        float m_start;
        float m_end;
    };


    //==========================================================gamma_multiply
    class gamma_multiply
    {
    public:
        gamma_multiply() : m_mul(1.0) {}
        gamma_multiply(float v) : m_mul(v) {}

        void value(float v) { m_mul = v; }
        float value() const { return m_mul; }

        float operator() (float x) const
        {
            float y = x * m_mul;
            if(y > 1.0) y = 1.0;
            return y;
        }

    private:
        float m_mul;
    };

}

#endif



