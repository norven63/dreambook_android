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

#ifndef AGG_RASTERIZER_CELLS_NO_AA_INCLUDED
#define AGG_RASTERIZER_CELLS_NO_AA_INCLUDED

#include <string.h>
#include <math.h>
#include "agg_math.h"
#include "agg_array.h"

namespace agg
{

	//-----------------------------------------------------rasterizer_cells_no_aa
	// An internal class that implements the main rasterization algorithm.
	// Used in the rasterizer. Should not be used direcly.
	template<class Cell> class rasterizer_cells_no_aa
	{
		enum cell_block_scale_e
		{
			cell_block_shift = 12,
			cell_block_size  = 1 << cell_block_shift,
			cell_block_mask  = cell_block_size - 1,
			cell_block_pool  = 256,
			cell_block_limit = 1024
		};

		struct sorted_y
		{
			unsigned start;
			unsigned num;
		};

	public:
		typedef Cell cell_type;
		typedef rasterizer_cells_no_aa<Cell> self_type;

		~rasterizer_cells_no_aa();
		rasterizer_cells_no_aa();

		void reset();
		void SetScreenRect(int nXmin,int nYmin,int nXmax,int nYmax);
		void FreeCover();
		void style(const cell_type& style_cell);
		void line(int x1, int y1, int x2, int y2);
		void line_with_aa(int x1, int y1, int x2, int y2);
		void line_no_aa(int x1, int y1, int x2, int y2);

		int min_x() const { return m_min_x; }
		int min_y() const { return m_min_y; }
		int max_x() const { return m_max_x; }
		int max_y() const { return m_max_y; }
		int min_s_x() const{return m_s_minx;}
		int min_s_y() const{return m_s_miny;}
		int max_s_x() const{return m_s_maxx;}
		int max_s_y() const{return m_s_maxy;}
		char GetCover(int x, int y){return(m_cover[y*m_s_width + x]);}

		void sort_cells();

		unsigned total_cells() const 
		{
			return m_num_cells;
		}

		cell_type* get_cell(int n)
		{
			return m_cells[0]+n;
		}

		unsigned scanline_num_cells(unsigned y) const 
		{ 
			return m_sorted_y[y - m_min_y].num; 
		}

		const cell_type* const* scanline_cells(unsigned y) const
		{ 
			return m_sorted_cells.data() + m_sorted_y[y - m_min_y].start; 
		}

		bool sorted() const { return m_sorted; }

	private:
		rasterizer_cells_no_aa(const self_type&);
		const self_type& operator = (const self_type&);

		void set_curr_cell(int x, int y);
		void add_curr_cell();
		void render_hline(int ey, int x1, int y1, int x2, int y2);
		void allocate_block();

	private:
		unsigned                m_num_blocks;
		unsigned                m_max_blocks;
		unsigned                m_curr_block;
		unsigned                m_num_cells;
		cell_type**             m_cells;
		cell_type*              m_curr_cell_ptr;
		pod_vector<cell_type*>  m_sorted_cells;
		pod_vector<sorted_y>    m_sorted_y;
		cell_type               m_curr_cell;
		cell_type               m_style_cell;
		int                     m_min_x;
		int                     m_min_y;
		int                     m_max_x;
		int                     m_max_y;
		// add by zhaojian means the rect of the map;
		int						m_s_minx;
		int						m_s_miny;
		int						m_s_maxx;
		int						m_s_maxy;
		int						m_s_width;
		int						m_s_height;
		char*					m_cover;

		bool                    m_sorted;
	};

	//------------------------------------------------------------------------
	template<class Cell> 
	rasterizer_cells_no_aa<Cell>::~rasterizer_cells_no_aa()
	{
		if(m_num_blocks)
		{
			cell_type** ptr = m_cells + m_num_blocks - 1;
			while(m_num_blocks--)
			{
				pod_allocator<cell_type>::deallocate(*ptr, cell_block_size);
				ptr--;
			}
			pod_allocator<cell_type*>::deallocate(m_cells, m_max_blocks);
		}
		if (m_cover != NULL)
		{
			delete[]m_cover;
			m_cover = NULL;
		}
	}
	template<class Cell> 
	void rasterizer_cells_no_aa<Cell>::FreeCover()
	{
		if (m_cover != NULL)
		{
			free(m_cover);
			m_cover = NULL;
		}

	}

	//------------------------------------------------------------------------
	template<class Cell> 
	rasterizer_cells_no_aa<Cell>::rasterizer_cells_no_aa() :
	m_num_blocks(0),
		m_max_blocks(0),
		m_curr_block(0),
		m_num_cells(0),
		m_cells(0),
		m_curr_cell_ptr(0),
		m_sorted_cells(),
		m_sorted_y(),
		m_min_x(0x7FFFFFFF),
		m_min_y(0x7FFFFFFF),
		m_max_x(-0x7FFFFFFF),
		m_max_y(-0x7FFFFFFF),
		m_sorted(false)
	{
		m_style_cell.initial();
		m_curr_cell.initial();
		m_cover = NULL;
		m_s_minx = -0x7FFFFFFF;
		m_s_maxx =  0x7FFFFFFF;
		m_s_miny = -0x7FFFFFFF;
		m_s_maxy =  0x7FFFFFFF;
		m_s_width = 0;
		m_s_height = 0;
	}

	//------------------------------------------------------------------------
	template<class Cell> 
	void rasterizer_cells_no_aa<Cell>::reset()
	{
		m_num_cells = 0; 
		m_curr_block = 0;
		m_curr_cell.initial();
		m_style_cell.initial();
		m_sorted = false;
		m_min_x =  0x7FFFFFFF;
		m_min_y =  0x7FFFFFFF;
		m_max_x = -0x7FFFFFFF;
		m_max_y = -0x7FFFFFFF;
		m_s_minx = -0x7FFFFFFF;
		m_s_maxx =  0x7FFFFFFF;
		m_s_miny = -0x7FFFFFFF;
		m_s_maxy =  0x7FFFFFFF;
		m_s_width = 0;
		m_s_height = 0;
	}
	template<class Cell> 
	void rasterizer_cells_no_aa<Cell>::SetScreenRect(int nXmin,int nYmin,int nXmax,int nYmax)
	{
		m_s_minx = nXmin;
		m_s_maxx = nXmax;
		m_s_miny = nYmin;
		m_s_maxy = nYmax;
		m_s_width = nXmax - nXmin;
		m_s_height = nYmax - nYmin;
		/*int nSize = m_s_width * m_s_height;
		if (m_cover != NULL)
		{
			delete[]m_cover;
			m_cover = NULL;
		}
		m_cover = new char[nSize];
		memset(m_cover, 0, sizeof(char) * nSize);*/

	}

	//------------------------------------------------------------------------
	template<class Cell> 
	AGG_INLINE void rasterizer_cells_no_aa<Cell>::add_curr_cell()
	{
		if (m_curr_cell.cover)
		{
			cell_type* cell_ptr = m_curr_cell_ptr;
			if((m_num_cells & cell_block_mask) == 0)
			{
				if(m_num_blocks >= cell_block_limit) return;
				allocate_block();
			}
			*m_curr_cell_ptr++ = m_curr_cell;
			++m_num_cells;
		}
	}

	//------------------------------------------------------------------------
	template<class Cell> 
	AGG_INLINE void rasterizer_cells_no_aa<Cell>::set_curr_cell(int x, int y)
	{
		if (x < m_s_minx)
			x = m_s_minx -1 ;
		if(x > m_s_maxx)
			x = m_s_maxx ;
		if (y < m_s_miny)
			y = m_s_miny - 1;
		if(y > m_s_maxy)
			y = m_s_maxy ; 
	/*	m_curr_cell.x     = x;
		m_curr_cell.y     = y;*/
		if(m_curr_cell.not_equal(x, y, m_style_cell))
		{
			add_curr_cell();
			m_curr_cell.style(m_style_cell);
			m_curr_cell.x     = x;
			m_curr_cell.y     = y;
			m_curr_cell.cover  = 0;
		}
	}

	//------------------------------------------------------------------------
	template<class Cell> 
	AGG_INLINE void rasterizer_cells_no_aa<Cell>::render_hline(int ey,
																int x1, int y1, 
																int x2, int y2)
	{
		int delta, p, dx;
		int incr;
		
		if (ey > m_s_maxy || ey < m_s_miny)
		{
			return;
		}

		//trivial case. Happens often
		if(y1 == y2)
		{
			set_curr_cell(x2, ey);
			return;
		}

		//everything is located in a single cell.  That is easy!
		if(x1 == x2)
		{
			m_curr_cell.cover += y2 - y1;
			//m_cover[m_curr_cell.x + m_curr_cell.y*m_s_width] += y2- y1;
			return;
		}

		//ok, we'll have to render a run of adjacent cells on the same
		//hline...
		p     =  y2 - y1;
		incr  = 1;

		dx = x2 - x1;
		if(dx < 0)
		{
			p     = 0;
			incr  = -1;
			dx    = -dx;
		}

		delta = p / dx;
		if(p < 0)
			delta--;


		m_curr_cell.cover += delta;
		//m_cover[m_curr_cell.x +  m_curr_cell.y*m_s_width] += delta;

		x1 += incr;
		set_curr_cell(x1, ey);
		y1  += delta;

		delta = y2 - y1;
		m_curr_cell.cover += delta;
		//m_cover[m_curr_cell.x +  m_curr_cell.y*m_s_width] += delta;
	}


	//------------------------------------------------------------------------
	template<class Cell> 
	AGG_INLINE void rasterizer_cells_no_aa<Cell>::style(const cell_type& style_cell)
	{ 
		m_style_cell.style(style_cell); 
	}

	//------------------------------------------------------------------------
	template<class Cell> 
	void rasterizer_cells_no_aa<Cell>::line(int x1, int y1, int x2, int y2)
	{
		enum dx_limit_e { dx_limit = 16384 };

		int dx = x2 - x1;

		if(dx >= dx_limit || dx <= -dx_limit)
		{
			return;
			int cx = (x1 + x2) >> 1;
			int cy = (y1 + y2) >> 1;
			line(x1, y1, cx, cy);
			line(cx, cy, x2, y2);
		}

		int dy = y2 - y1;
		int ex1 = x1;
		int ex2 = x2;
		int ey1 = y1;
		int ey2 = y2;

		int x_from, x_to;
		int p, rem, mod, lift, delta, first, incr;

		if(ex1 < m_min_x) m_min_x = ex1;
		if(ex1 > m_max_x) m_max_x = ex1;
		if(ey1 < m_min_y) m_min_y = ey1;
		if(ey1 > m_max_y) m_max_y = ey1;
		if(ex2 < m_min_x) m_min_x = ex2;
		if(ex2 > m_max_x) m_max_x = ex2;
		if(ey2 < m_min_y) m_min_y = ey2;
		if(ey2 > m_max_y) m_max_y = ey2;

		set_curr_cell(ex1, ey1);

		//everything is on a single hline
		if(ey1 == ey2)
		{
			render_hline(ey1, x1, 0, x2, 0);
			return;
		}

		//Vertical line - we have to calculate start and end cells,
		//and then - the common values of the area and coverage for
		//all cells of the line. We know exactly there's only one 
		//cell, so, we don't have to call render_hline().
		incr  = 1;
		if(dx == 0)
		{
			int ex = x1;

			first = 1;
			if(dy < 0)
			{
				first = 0;
				incr  = -1;
			}

			x_from = x1;

			delta = first;
			m_curr_cell.cover += delta;
			//m_cover[m_curr_cell.x + m_curr_cell.y*m_s_width] += delta;

			ey1 += incr;
			set_curr_cell(ex, ey1);

			delta = first + first - 1;
			while(ey1 != ey2)
			{
				m_curr_cell.cover = delta;
				//m_cover[m_curr_cell.x + m_curr_cell.y*m_s_width] = delta;
				ey1 += incr;
				set_curr_cell(ex, ey1);
			}

			delta = first-1;
			m_curr_cell.cover += delta;
			//m_cover[m_curr_cell.x + m_curr_cell.y*m_s_width] += delta;
			return;
		}

		//ok, we have to render several hlines
		p     = dx;
		first = 1;

		if(dy < 0)
		{
			p     = 0;
			first = 0;
			incr  = -1;
			dy    = -dy;
		}

		delta = p / dy;
		mod   = p % dy;

		if(mod < 0)
		{
			delta--;
			mod += dy;
		}

		x_from = x1 + delta;
		render_hline(ey1, x1, 0, x_from, first);

		ey1 += incr;
		set_curr_cell(x_from, ey1);

		if(ey1 != ey2)
		{
			p     = dx;
			lift  = p / dy;
			rem   = p % dy;

			if(rem < 0)
			{
				lift--;
				rem += dy;
			}
			mod -= dy;

			while(ey1 != ey2)
			{
				delta = lift;
				mod  += rem;
				if (mod >= 0)
				{
					mod -= dy;
					delta++;
				}
				x_to = x_from + delta;
				render_hline(ey1, x_from, 1 - first, x_to, first);
				x_from = x_to;

				ey1 += incr;
				set_curr_cell(x_from, ey1);
			}
		}
		render_hline(ey1, x_from, 1 - first, x2, 0);
	}

	//------------------------------------------------------------------------
	template<class Cell> 
	void rasterizer_cells_no_aa<Cell>::allocate_block()
	{
		if(m_curr_block >= m_num_blocks)
		{
			if(m_num_blocks >= m_max_blocks)
			{
				cell_type** new_cells = 
					pod_allocator<cell_type*>::allocate(m_max_blocks + 
					cell_block_pool);

				if(m_cells)
				{
					memcpy(new_cells, m_cells, m_max_blocks * sizeof(cell_type*));
					pod_allocator<cell_type*>::deallocate(m_cells, m_max_blocks);
				}
				m_cells = new_cells;
				m_max_blocks += cell_block_pool;
			}

			m_cells[m_num_blocks++] = 
				pod_allocator<cell_type>::allocate(cell_block_size);

		}
		m_curr_cell_ptr = m_cells[m_curr_block++];
	}



	//------------------------------------------------------------------------
	template <class T> static AGG_INLINE void swap_cells_no_aa(T* a, T* b)
	{
		T temp = *a;
		*a = *b;
		*b = temp;
	}


	//------------------------------------------------------------------------
	enum
	{
		qsort_threshold_no_aa = 9
	};


	//------------------------------------------------------------------------
	template<class Cell>
	void qsort_cells_no_aa(Cell** start, unsigned num)
	{
		Cell**  stack[80];
		Cell*** top; 
		Cell**  limit;
		Cell**  base;

		limit = start + num;
		base  = start;
		top   = stack;

		for (;;)
		{
			int len = int(limit - base);

			Cell** i;
			Cell** j;
			Cell** pivot;

			if(len > qsort_threshold_no_aa)
			{
				// we use base + len/2 as the pivot
				pivot = base + len / 2;
				swap_cells_no_aa(base, pivot);

				i = base + 1;
				j = limit - 1;

				// now ensure that *i <= *base <= *j 
				if((*j)->x < (*i)->x)
				{
					swap_cells_no_aa(i, j);
				}

				if((*base)->x < (*i)->x)
				{
					swap_cells_no_aa(base, i);
				}

				if((*j)->x < (*base)->x)
				{
					swap_cells_no_aa(base, j);
				}

				for(;;)
				{
					int x = (*base)->x;
					do i++; while( (*i)->x < x );
					do j--; while( x < (*j)->x );

					if(i > j)
					{
						break;
					}

					swap_cells_no_aa(i, j);
				}

				swap_cells_no_aa(base, j);

				// now, push the largest sub-array
				if(j - base > limit - i)
				{
					top[0] = base;
					top[1] = j;
					base   = i;
				}
				else
				{
					top[0] = i;
					top[1] = limit;
					limit  = j;
				}
				top += 2;
			}
			else
			{
				// the sub-array is small, perform insertion sort
				j = base;
				i = j + 1;

				for(; i < limit; j = i, i++)
				{
					for(; j[1]->x < (*j)->x; j--)
					{
						swap_cells_no_aa(j + 1, j);
						if (j == base)
						{
							break;
						}
					}
				}

				if(top > stack)
				{
					top  -= 2;
					base  = top[0];
					limit = top[1];
				}
				else
				{
					break;
				}
			}
		}
	}


	//------------------------------------------------------------------------
	template<class Cell> 
	void rasterizer_cells_no_aa<Cell>::sort_cells()
	{
		if(m_sorted) return; //Perform sort only the first time.

		add_curr_cell();
		m_curr_cell.x     = 0x7FFFFFFF;
		m_curr_cell.y     = 0x7FFFFFFF;
		m_curr_cell.cover = 0;

		if(m_num_cells == 0) return;
		if (m_min_y > m_s_maxy) return;

		// Allocate the array of cell pointers
		m_sorted_cells.allocate(m_num_cells, 16);

		// Allocate and zero the Y array
		m_sorted_y.allocate(m_max_y - m_min_y + 1, 16);
		m_sorted_y.zero();

		// Create the Y-histogram (count the numbers of cells for each Y)
		cell_type** block_ptr = m_cells;
		cell_type*  cell_ptr;
		unsigned nb = m_num_cells >> cell_block_shift;
		unsigned i;
		while(nb--)
		{
			cell_ptr = *block_ptr++;
			i = cell_block_size;
			while(i--) 
			{
				m_sorted_y[cell_ptr->y - m_min_y].start++;
				++cell_ptr;
			}
		}

		cell_ptr = *block_ptr++;
		i = m_num_cells & cell_block_mask;
		while(i--) 
		{
			m_sorted_y[cell_ptr->y - m_min_y].start++;
			++cell_ptr;
		}

		// Convert the Y-histogram into the array of starting indexes
		unsigned start = 0;
		for(i = 0; i < m_sorted_y.size(); i++)
		{
			unsigned v = m_sorted_y[i].start;
			m_sorted_y[i].start = start;
			start += v;
		}

		// Fill the cell pointer array sorted by Y
		block_ptr = m_cells;
		nb = m_num_cells >> cell_block_shift;
		while(nb--)
		{
			cell_ptr = *block_ptr++;
			i = cell_block_size;
			while(i--) 
			{
				sorted_y& curr_y = m_sorted_y[cell_ptr->y - m_min_y];
				m_sorted_cells[curr_y.start + curr_y.num] = cell_ptr;
				++curr_y.num;
				++cell_ptr;
			}
		}

		cell_ptr = *block_ptr++;
		i = m_num_cells & cell_block_mask;
		while(i--) 
		{
			sorted_y& curr_y = m_sorted_y[cell_ptr->y - m_min_y];
			m_sorted_cells[curr_y.start + curr_y.num] = cell_ptr;
			++curr_y.num;
			++cell_ptr;
		}

		// Finally arrange the X-arrays
		for(i = 0; i < m_sorted_y.size(); i++)
		{
			const sorted_y& curr_y = m_sorted_y[i];
			if(curr_y.num)
			{
				qsort_cells_no_aa(m_sorted_cells.data() + curr_y.start, curr_y.num);
			}
		}
		m_sorted = true;
	}



	//------------------------------------------------------scanline_hit_test
	class scanline_hit_test_no_aa
	{
	public:
		scanline_hit_test_no_aa(int x) : m_x(x), m_hit(false) {}

		void reset_spans() {}
		void finalize(int) {}
		void add_cell(int x, int)
		{
			if(m_x == x) m_hit = true;
		}
		void add_span(int x, int len, int)
		{
			if(m_x >= x && m_x < x+len) m_hit = true;
		}
		unsigned num_spans() const { return 1; }
		bool hit() const { return m_hit; }

	private:
		int  m_x;
		bool m_hit;
	};


}

#endif
