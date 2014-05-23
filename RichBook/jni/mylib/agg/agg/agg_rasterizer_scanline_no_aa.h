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
// The author gratefully acknowleges the support of David Turner, 
// Robert Wilhelm, and Werner Lemberg - the authors of the FreeType 
// libray - in producing this work. See http://www.freetype.org for details.
//
//----------------------------------------------------------------------------
//
// Adaptation for 32-bit screen coordinates has been sponsored by 
// Liberty Technology Systems, Inc., visit http://lib-sys.com
//
// Liberty Technology Systems, Inc. is the provider of
// PostScript and PDF technology for software developers.
// 
//----------------------------------------------------------------------------

#ifndef AGG_RASTERIZER_SCANLINE_NO_AA_INCLUDED
#define AGG_RASTERIZER_SCANLINE_NO_AA_INCLUDED

#include "agg_rasterizer_cells_no_aa.h"
#include "agg_rasterizer_sl_clip.h"
#include "agg_gamma_functions.h"

namespace agg
{
	//-----------------------------------------------------------------cell_aa
	// A pixel cell. There're no constructors defined and it was done 
	// intentionally in order to avoid extra overhead when allocating an 
	// array of cells.
	struct cell_no_aa
	{
		int x;
		int y;
		int cover;

		void initial()
		{
			x = 0x7FFFFFFF;
			y = 0x7FFFFFFF;
			cover = 0;
		}

		void style(const cell_no_aa&) {}

		int not_equal(int ex, int ey, const cell_no_aa&) const
		{
			return (ex - x) | (ey - y);
		}
	};


	//==================================================rasterizer_scanline_no_aa
	// Polygon rasterizer that is used to render filled polygons with 
	// high-quality Anti-Aliasing. Internally, by default, the class uses 
	// integer coordinates in format 24.8, i.e. 24 bits for integer part 
	// and 8 bits for fractional - see poly_subpixel_shift. This class can be 
	// used in the following  way:
	//
	// 1. filling_rule(filling_rule_e ft) - optional.
	//
	// 2. gamma() - optional.
	//
	// 3. reset()
	//
	// 4. move_to(x, y) / line_to(x, y) - make the polygon. One can create 
	//    more than one contour, but each contour must consist of at least 3
	//    vertices, i.e. move_to(x1, y1); line_to(x2, y2); line_to(x3, y3);
	//    is the absolute minimum of vertices that define a triangle.
	//    The algorithm does not check either the number of vertices nor
	//    coincidence of their coordinates, but in the worst case it just 
	//    won't draw anything.
	//    The order of the vertices (clockwise or counterclockwise) 
	//    is important when using the non-zero filling rule (fill_non_zero).
	//    In this case the vertex order of all the contours must be the same
	//    if you want your intersecting polygons to be without "holes".
	//    You actually can use different vertices order. If the contours do not 
	//    intersect each other the order is not important anyway. If they do, 
	//    contours with the same vertex order will be rendered without "holes" 
	//    while the intersecting contours with different orders will have "holes".
	//
	// filling_rule() and gamma() can be called anytime before "sweeping".
	//------------------------------------------------------------------------
	template<class Clip=rasterizer_sl_clip_none> class rasterizer_scanline_no_aa
	{
		enum status
		{
			status_initial,
			status_move_to,
			status_line_to,
			status_closed
		};

	public:
		typedef Clip                      clip_type;
		typedef typename Clip::conv_type  conv_type;
		typedef typename Clip::coord_type coord_type;

		enum aa_scale_e
		{
			aa_shift  = 1,
			aa_scale  = 1 << aa_shift,
			aa_mask   = aa_scale - 1,
			aa_scale2 = aa_scale * 2,
			aa_mask2  = aa_scale2 - 1
		};

		//--------------------------------------------------------------------
		rasterizer_scanline_no_aa() : 
			m_outline(),
				m_clipper(),
				m_filling_rule(fill_non_zero),
				m_auto_close(true),
				m_start_x(0),
				m_start_y(0),
				m_status(status_initial)
			{
			}

			//--------------------------------------------------------------------
			void reset(); 
			void SetScreenRect(int nXmin,int nYmin,int nXmax,int nYmax);
			void FreeCover();
			void reset_clipping();
			void clip_box(float x1, float y1, float x2, float y2);
			void filling_rule(filling_rule_e filling_rule);
			void auto_close(bool flag) { m_auto_close = flag; }

			//--------------------------------------------------------------------
			cell_no_aa* get_cell(int n)
			{
				return m_outline.get_cell(n);
			}

			int total_cells()
			{
				return m_outline.total_cells();
			}

			//--------------------------------------------------------------------
			void move_to(int x, int y);
			void line_to(int x, int y);
			void move_to_d(float x, float y);
			void line_to_d(float x, float y);
			void close_polygon();
			void add_vertex(float x, float y, unsigned cmd);

			void edge(int x1, int y1, int x2, int y2);
			void edge_d(float x1, float y1, float x2, float y2);

			//-------------------------------------------------------------------
			template<class VertexSource>
			void add_path(VertexSource& vs, unsigned path_id=0)
			{
				float x;
				float y;

				unsigned cmd;
				vs.rewind(path_id);
				if(m_outline.sorted()) reset();

				while(!is_stop(cmd = vs.vertex(&x, &y)))
				{
					add_vertex(x, y, cmd);
				}
			}

			//--------------------------------------------------------------------
			int min_x() const { return m_outline.min_x(); }
			int min_y() const { return m_outline.min_y(); }
			int max_x() const { return m_outline.max_x(); }
			int max_y() const { return m_outline.max_y(); }

			int min_s_x() const { return m_outline.min_s_x(); }
			int min_s_y() const { return m_outline.min_s_y(); }
			int max_s_x() const { return m_outline.max_s_x(); }
			int max_s_y() const { return m_outline.max_s_y(); }

			//--------------------------------------------------------------------
			void sort();
			bool rewind_scanlines();
			bool navigate_scanline(int y);

			//--------------------------------------------------------------------
			AGG_INLINE unsigned calculate_alpha(int area) const
			{
				int cover = area;
				if(cover < 0) cover = -cover;
				if(m_filling_rule == fill_even_odd)
				{
					cover &= aa_mask2;
					if(cover > aa_scale)
					{
						cover = aa_scale2 - cover;
					}
				}
				if(cover > aa_mask) cover = aa_mask;
				return cover;
			}

			template<class Scanline> bool sweep_scanline(Scanline& sl)
		 {
			 int nS_Xmin = m_outline.min_s_x();
			 int nS_Ymin = m_outline.min_s_y();
			 int nS_Xmax = m_outline.max_s_x();
			 int nS_Ymax = m_outline.max_s_y();
			 int nYmax = m_outline.max_y();
			 for(;;)
			 {
				 if(m_scan_y > nYmax) return false;
				 if (m_scan_y > nS_Ymax)return false;
				 if (m_scan_y < nS_Ymin)
				 {
					++m_scan_y;
					continue;
				 };
				 sl.reset_spans();
				 unsigned num_cells = m_outline.scanline_num_cells(m_scan_y);
				 const cell_no_aa* const* cells = m_outline.scanline_cells(m_scan_y);
				 int cover = 0;
				 while(num_cells)
				 {
					 const cell_no_aa* cur_cell = *cells;
					 int x    = cur_cell->x;

					 cover += cur_cell->cover;

					 //accumulate all cells with the same X
					 while(--num_cells)
					 {
						 cur_cell = *++cells;
						 if(cur_cell->x != x) break;
						 cover += cur_cell->cover;
					 }

					 if(num_cells && cur_cell->x > x)
					 {
						 if(cover << 1)
						 {
							/* if ((cur_cell->x > nS_Xmin && cur_cell->x < nS_Xmax) || 
								 (x < nS_Xmax && x > nS_Xmin))
							 {*/
								 sl.add_span(x, cur_cell->x - x, 255);
							 /*}
							 else
							 {
								 int kkkk = 0;
							 }*/
						 }
					 }
				 }

				 if(sl.num_spans()) break;
				 ++m_scan_y;
			 }

			 sl.finalize(m_scan_y);
			 ++m_scan_y;
			 return true;


			 /*for(;;)
			 {
				 int nMinx = m_outline.min_s_x();
				 int nMaxx = m_outline.max_s_x();
				 int nYmin = m_outline.min_s_y();
				 int nYmax = m_outline.max_s_y();
				 if(m_scan_y < nYmin || m_scan_y >= nYmax) return false;
				 sl.reset_spans();
				 char cover = 0;
				 char tcover = 0;
				 int nXpre = nMinx;
				 for (int i = nMinx ; i < nMaxx; i++)
				 {
					tcover =m_outline.GetCover(i,m_scan_y) ;
					if (tcover)
					{
						if (cover == 0)
						{
							nXpre = i;
							cover += tcover;
							continue;
						}
						sl.add_span(nXpre, i - nXpre, 255);
						cover += tcover;
						nXpre = i;
					}						
				 }
				 if(sl.num_spans()) break;
				 ++m_scan_y;
			 }

			 sl.finalize(m_scan_y);
			 ++m_scan_y;
			 return true;*/
		 }

			//--------------------------------------------------------------------
			bool hit_test(int tx, int ty);


	private:
		//--------------------------------------------------------------------
		// Disable copying
		rasterizer_scanline_no_aa(const rasterizer_scanline_no_aa<Clip>&);
		const rasterizer_scanline_no_aa<Clip>& 
			operator = (const rasterizer_scanline_no_aa<Clip>&);

	private:
		rasterizer_cells_no_aa<cell_no_aa> m_outline;
		clip_type      m_clipper;
		filling_rule_e m_filling_rule;
		bool           m_auto_close;
		coord_type     m_start_x;
		coord_type     m_start_y;
		unsigned       m_status;
		int            m_scan_y;
		int*		   m_cover;
	};












	//------------------------------------------------------------------------
	template<class Clip> 
	void rasterizer_scanline_no_aa<Clip>::reset() 
	{ 
		m_outline.reset(); 
		m_status = status_initial;
	}
	template<class Clip> 
	void rasterizer_scanline_no_aa<Clip>::FreeCover()
	{
		m_outline.FreeCover();
	}
	template<class Clip> 
	void rasterizer_scanline_no_aa<Clip>::SetScreenRect(int nXmin,int nYmin,int nXmax,int nYmax)
	{ 
		m_outline.SetScreenRect(nXmin, nYmin, nXmax, nYmax); 
		m_status = status_initial;
	}


	//------------------------------------------------------------------------
	template<class Clip> 
	void rasterizer_scanline_no_aa<Clip>::filling_rule(filling_rule_e filling_rule) 
	{ 
		m_filling_rule = filling_rule; 
	}

	//------------------------------------------------------------------------
	template<class Clip> 
	void rasterizer_scanline_no_aa<Clip>::clip_box(float x1, float y1, 
		float x2, float y2)
	{
		reset();
		m_clipper.clip_box(conv_type::upscale(x1), conv_type::upscale(y1), 
			conv_type::upscale(x2), conv_type::upscale(y2));
	}

	//------------------------------------------------------------------------
	template<class Clip> 
	void rasterizer_scanline_no_aa<Clip>::reset_clipping()
	{
		reset();
		m_clipper.reset_clipping();
	}

	//------------------------------------------------------------------------
	template<class Clip> 
	void rasterizer_scanline_no_aa<Clip>::close_polygon()
	{
		if(m_status == status_line_to)
		{
			m_clipper.line_to(m_outline, m_start_x, m_start_y);
			m_status = status_closed;
		}
	}

	//------------------------------------------------------------------------
	template<class Clip> 
	void rasterizer_scanline_no_aa<Clip>::move_to(int x, int y)
	{
		if(m_outline.sorted()) reset();
		if(m_auto_close) close_polygon();
		m_clipper.move_to(m_start_x = conv_type::downscale(x), 
			m_start_y = conv_type::downscale(y));
		m_status = status_move_to;
	}

	//------------------------------------------------------------------------
	template<class Clip> 
	void rasterizer_scanline_no_aa<Clip>::line_to(int x, int y)
	{
		m_clipper.line_to(m_outline, 
			conv_type::downscale(x), 
			conv_type::downscale(y));
		m_status = status_line_to;
	}

	//------------------------------------------------------------------------
	template<class Clip> 
	void rasterizer_scanline_no_aa<Clip>::move_to_d(float x, float y) 
	{ 
		if(m_outline.sorted()) reset();
		if(m_auto_close) close_polygon();
		m_clipper.move_to(m_start_x = conv_type::upscale(x), 
			m_start_y = conv_type::upscale(y)); 
		m_status = status_move_to;
	}

	//------------------------------------------------------------------------
	template<class Clip> 
	void rasterizer_scanline_no_aa<Clip>::line_to_d(float x, float y) 
	{ 
		m_clipper.line_to(m_outline, 
			conv_type::upscale(x), 
			conv_type::upscale(y)); 
		m_status = status_line_to;
	}

	//------------------------------------------------------------------------
	template<class Clip> 
	void rasterizer_scanline_no_aa<Clip>::add_vertex(float x, float y, unsigned cmd)
	{
		if(is_move_to(cmd)) 
		{
			move_to_d(x, y);
		}
		else 
			if(is_vertex(cmd))
			{
				line_to_d(x, y);
			}
			else
				if(is_close(cmd))
				{
					close_polygon();
				}
	}

	//------------------------------------------------------------------------
	template<class Clip> 
	void rasterizer_scanline_no_aa<Clip>::edge(int x1, int y1, int x2, int y2)
	{
		if(m_outline.sorted()) reset();
		m_clipper.move_to(conv_type::downscale(x1), conv_type::downscale(y1));
		m_clipper.line_to(m_outline, 
			conv_type::downscale(x2), 
			conv_type::downscale(y2));
		m_status = status_move_to;
	}

	//------------------------------------------------------------------------
	template<class Clip> 
	void rasterizer_scanline_no_aa<Clip>::edge_d(float x1, float y1, 
		float x2, float y2)
	{
		if(m_outline.sorted()) reset();
		m_clipper.move_to(conv_type::upscale(x1), conv_type::upscale(y1)); 
		m_clipper.line_to(m_outline, 
			conv_type::upscale(x2), 
			conv_type::upscale(y2)); 
		m_status = status_move_to;
	}

	//------------------------------------------------------------------------
	template<class Clip> 
	void rasterizer_scanline_no_aa<Clip>::sort()
	{
		if(m_auto_close) close_polygon();
		m_outline.sort_cells();
	}

	//------------------------------------------------------------------------
	template<class Clip> 
	AGG_INLINE bool rasterizer_scanline_no_aa<Clip>::rewind_scanlines()
	{
		if(m_auto_close) close_polygon();
		m_outline.sort_cells();
		if(m_outline.total_cells() == 0) 
		{
			return false;
		}
		m_scan_y = m_outline.min_y();
		//m_scan_y = m_outline.min_s_y();
		return true;
	}


	//------------------------------------------------------------------------
	template<class Clip> 
	AGG_INLINE bool rasterizer_scanline_no_aa<Clip>::navigate_scanline(int y)
	{
		if(m_auto_close) close_polygon();
		m_outline.sort_cells();
		if(m_outline.total_cells() == 0 || 
			y < m_outline.min_y() || 
			y > m_outline.max_y()) 
		{
			return false;
		}
		m_scan_y = y;
		return true;
	}

	//------------------------------------------------------------------------
	template<class Clip> 
	bool rasterizer_scanline_no_aa<Clip>::hit_test(int tx, int ty)
	{
		if(!navigate_scanline(ty)) return false;
		scanline_hit_test sl(tx);
		sweep_scanline(sl);
		return sl.hit();
	}



}



#endif

