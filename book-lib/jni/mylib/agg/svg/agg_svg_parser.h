//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.3
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
//
// SVG parser.
//
//----------------------------------------------------------------------------

#ifndef AGG_SVG_PARSER_INCLUDED
#define AGG_SVG_PARSER_INCLUDED

#include "agg_svg_path_tokenizer.h"
#include "agg_svg_path_renderer.h"
#include "expat.h"
#include <vector>
#include <map>
#include <string>

namespace agg
{
namespace svg
{
	struct text_wrapper
	{
		struct matrix
		{
			matrix() :x(0), y(0), zx(1), zy(1), sx(0), sy(0) { }
			float zx;
			float sx;
			float sy;
			float zy;
			float x;
			float y;
		};

		struct tspan
		{
			rgba8			m_color;
			CStringA			m_content;
			float			m_size;
			CStringA			m_font;
			float			x;
			float			y;

			tspan() : m_size(0), x(0), y(0) {}
		};

		void add_tspan(const tspan& ts)
		{
			m_vecTspan.push_back(ts);
		}

		tspan* get_available_tspan()
		{
			if (m_vecTspan.empty())
				return NULL;

			tspan* pts = &(m_vecTspan.back());
			if (!pts->m_content.IsEmpty())
				return NULL;

			return pts;
		}

		tspan* get_tspan_for_sure()
		{
			tspan* pts = get_available_tspan();
			if (!pts)
			{
				tspan ts;
				m_vecTspan.push_back(ts);
				pts = get_available_tspan();
			}

			return pts;
		}

		void add_tspan_fill(rgba8 color)
		{
			tspan* pts = get_tspan_for_sure();
			pts->m_color = color;
		}

		void add_tspan_content(const char* s, int len);

		void add_tspan_size(float size, tspan** ppsan)
		{
			tspan* pts = get_tspan_for_sure();
			pts->m_size = size;
			if (ppsan)
				*ppsan = pts;
		}

		void add_tspan_font(const char* s, int len)
		{
			tspan* pts = get_tspan_for_sure();
			pts->m_font = "";
		}

		void add_tspan_x(float x)
		{
			tspan* pts = get_tspan_for_sure();
			pts->x = x;
		}

		void add_tspan_y(float y)
		{
			tspan* pts = get_tspan_for_sure();
			pts->y = y;
		}

		void integrate_tspan()
		{
			size_t nSize = m_vecTspan.size();
			if (nSize <= 1)
				return;

			typedef std::multimap<float, tspan*> MAP_SORT;
		
			MAP_SORT mapY;
			for (size_t i=0; i<nSize; ++i)
				mapY.insert(std::make_pair(m_vecTspan[i].y, &m_vecTspan[i]));

			std::vector<MAP_SORT> vecRow;
			MAP_SORT firstRow;
			vecRow.push_back(firstRow);

			MAP_SORT::iterator iterY = mapY.begin();
			float dBase = iterY->first;
			for (iterY = mapY.begin(); iterY != mapY.end(); ++ iterY)
			{
				tspan* pts = iterY->second;
				if (dBase < -0.01f || dBase > 0.01f)
				{
					MAP_SORT nextRow;
					vecRow.push_back(nextRow);
					dBase = iterY->first;
				}

				vecRow.back().insert(std::make_pair(pts->x, pts));
			}

			std::vector<tspan> vecTsTemp;
			for (std::size_t i=0; i<vecRow.size(); ++i)
			{
				MAP_SORT& row = vecRow[i];
				if (row.empty())
					continue;

				tspan tsa, *pts;
				vecTsTemp.push_back(tsa);

				tspan& ts = vecTsTemp.back(); // to save a little time
				pts = (*row.begin()).second;
				ts.m_size = pts->m_size;
				ts.m_color = pts->m_color;
				ts.x = pts->x;
				ts.y = pts->y;
				
				for (MAP_SORT::iterator iter = row.begin(); iter != row.end(); ++ iter)
					ts.m_content += iter->second->m_content;
			}
			
			m_vecTspan.clear();
			m_vecTspan = vecTsTemp;
		}

		matrix	m_matrix;
		std::vector<tspan> m_vecTspan;
	};

    class parser
    {
	public:
		// This two value must be assigned when script changes
		static COLORREF m_clrJVArrowBorder;
		static COLORREF m_clrJVArrowTailBorder;
		static COLORREF m_clrJVArrow;

	public:
		enum gradient_dir { g_horizontal = 0, g_vertical, g_emit };
		enum time_day_night { time_not_known, time_night, time_day };
		enum arrow_pos { arrow_not_known, arrow_left, arrow_middle, arrow_right };
		enum arrow_type { arrow_type_unknown, arrow_single, arrow_multi };
		enum ter_color { ter_not_known, ter_eu, ter_mea, ter_urban };
		enum tunnel_type { tunnel_none, tunnel_display };
		struct SGradient
		{ 
			CStringA name_flag;
			float x1;
			float x2;
			float y1;
			float y2;

			std::vector<std::pair<float, rgba8> > colors;

			gradient_dir get_dir()
			{
				float distance = x1 - x2;
				if (distance < 0.001 && distance > - 0.001)
					return g_vertical;

				distance = y1 - y2;
				if (distance < 0.001 && distance > -0.001)
					return g_horizontal;

				return g_emit;
			}

			bool is(const char* name)
			{
				return tcscmp((LPCSTR)name_flag, name) == 0;
			}

			bool is_night()
			{
				return name_flag.Find("NIGHT") != -1;
			}

			bool is_mea()
			{
				return name_flag.Find("MEA") != -1;
			}
		};

		struct SGradientRect
		{
			SGradientRect(float x1, float y1, float w1, float h1, SGradient gra) :
				x(x1), y(y1), w(w1), h(h1), gradient(gra) { }
			
			float x;
			float y;
			float w;
			float h;

			SGradient gradient;
		};

        enum buf_size_e { buf_size = BUFSIZ };
    public:

        ~parser();
        parser(path_renderer& path, bool bIsDay, int nArrowPos, bool bArrowSingle, bool bMea, bool bTunnel);

        bool parse(const char* fname, bool bRetriveBoundInfo = false);
		bool parse(const char* pBuffer, unsigned nLen, bool bRetriveBoundInfo = false);
		RECT get_svg_window() { return m_rcViewBox; }
		trans_affine& get_signs_trans() { return m_trans; }

    private:
        // XML event handlers
        static void start_element(void* data, const char* el, const char** attr);
		static void parse_element(void* data, const char* el, const char** attr);
        static void end_element(void* data, const char* el);
        static void content(void* data, const char* s, int len);

		void control_parse_direction(void* data, const char* el, const char** attr);
		void retrieve_bound_info(void* data, const char* el, const char** attr);
		void retrieve_bound();

        void parse_attr(const char** attr);
        void parse_path(const char** attr);
        void parse_poly(const char** attr, bool close_flag);
        void parse_rect(const char** attr);
        void parse_line(const char** attr);
		void parse_circle(const char** attr);
		void parse_ellipse(const char** attr);
		void parse_text(const char** attr);
		void parse_tspan(const char** attr);
		void parse_window(const char** attr);
		void parse_signs(const char** attr);
        void parse_style(const char* str);
        void parse_transform(const char* str);

        unsigned parse_matrix(const char* str);
        unsigned parse_translate(const char* str);
        unsigned parse_rotate(const char* str);
        unsigned parse_scale(const char* str);
        unsigned parse_skew_x(const char* str);
        unsigned parse_skew_y(const char* str);

		void parse_linear_gradient(const char** attr);
		void parse_gradient_component(const char** attr);
        
        bool parse_attr(const char* name, const char* value);
        bool parse_name_value(const char* nv_start, const char* nv_end);
        void copy_name(const char* start, const char* end);
        void copy_value(const char* start, const char* end);

		unsigned parse_transform_args(const char* str, float* args, unsigned max_na, unsigned* na);

		text_wrapper* get_available_tw();
        
    public:
        path_renderer& m_path;
        path_tokenizer m_tokenizer;
        char*          m_buf;
        bool           m_path_flag;
        char*          m_attr_name;
        char*          m_attr_value;
        unsigned       m_attr_name_len;
        unsigned       m_attr_value_len;
		RECT		   m_rcViewBox;
		CRect		   m_rcBound;
		trans_affine	m_trans;
		bool			m_bStopParse;

		
		std::vector<SGradient>		m_gradient;
		std::vector<SGradientRect>	m_gra_shape;

		static bool				m_bMea;
		static bool				m_bArrowSingle;
		static int				m_nArrowPos;
		static bool				m_bIsDay;
		static bool				m_bSky;
		static bool				m_bTunnel;

		static time_day_night	m_time;
		static arrow_pos		m_arrow;
		static arrow_type		m_arrowType;
		static ter_color		m_terrain;
		static tunnel_type		m_tunnel;

		static bool				m_bRetrieveInfo;

		//XML_Parser	   m_expatParser;

	public:
		std::vector<text_wrapper>	m_vecTexts;
    };

	class color_css
	{
	public:
		static color_css& inst() { static color_css css; return css; }
		bool init(char* pBuf, int nLen);
		bool self_init();
		bool get_color(std::string& color, const std::string& name);

	private:
		color_css() : b_init(false) {};

	private:
		bool b_init;
		std::map<std::string, std::string> m_mapColors;
	};

}
}

#endif
