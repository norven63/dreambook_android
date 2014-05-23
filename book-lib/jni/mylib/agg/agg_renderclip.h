#pragma once

class CAlphaMask;
template<class PixelFormat> class CClipRender;

enum EClipType
{
	eClipBase,			// rect clip
	eClipMClip,			// multi-rect clip
	eClipAMBase,		// rect clip with alpha mask
	eClipAMMClip,		// multi-rect clip with alpha mask
};

// 等同于gdi中的rgn
// 是一个带有半透明通道的图，其大小必须与目标AGGDC一致
class CAlphaMask
{
	typedef agg::renderer_base<agg::pixfmt_gray8> ren_base;
	typedef agg::renderer_scanline_aa_solid<ren_base> renderer;

private:
	BYTE* mBuffer;
	agg::rendering_buffer mRenderBuf;
	agg::pixfmt_gray8 mGrayPixelBuf;
	ren_base mRenBase;
	
	renderer mRender;
	agg::rasterizer_scanline_aa<> m_ras;
	agg::scanline_p8              m_sl;

	void Init(unsigned width,unsigned height);
public:
	CAlphaMask(unsigned nWidth,unsigned nHeight):mGrayPixelBuf(mRenderBuf)
	{
		Init(nWidth,nHeight);
	}

	CAlphaMask(CClipRender<pixel_format>& render);

public:
	unsigned width() const { return mRenBase.width(); }
	unsigned height() const{ return mRenBase.height(); }
	agg::rendering_buffer& GetAlphaMaskBuf() { return mRenderBuf; }


	void CreateRectRgn(const RECT& rc) ;
	void CreateEllipticRgn(const RECT& rc) ;
	void CreatePolygonRgn(const POINT *lppt,int cPoints);
	void CreateRoundRectRgn(const RECT& rc,float radius);

	// 将rgn中的裁剪区合并到自身
	void CombineRgn(CAlphaMask& rgn);

	BOOL IsEqual(const CAlphaMask& rgn);

	template<class VertexSource> void BuildGlyphRgn(VertexSource& vs)
	{
		m_ras.add_path(vs);
		agg::render_scanlines(m_ras, m_sl, mRender);
	}
};


// 典型用法：
//	CClipRender<pixel_format> render;
//	render可以等效的替代:
//		render_base<pixel_format>,
//		render_mclip<pixel_format>
//		并提供带有alphamask的功能
template<class PixelFormat> 
class CClipRender
{
public:
	typedef CClipRender<PixelFormat> self_type;
	typedef PixelFormat pixfmt_type;
	typedef typename pixfmt_type::color_type color_type;
	typedef typename pixfmt_type::row_data row_data;

	typedef agg::rendering_buffer  rbuf_type;
	typedef agg::rect_i rect_i;
	typedef agg::cover_type cover_type;
	typedef agg::rendering_buffer rendering_buffer;
	typedef agg::amask_no_clip_gray8 alpha_mask_type;
	typedef agg::pixfmt_amask_adaptor<pixfmt_type, alpha_mask_type> pixfmt_amask_type;
	typedef agg::renderer_base<pixfmt_type> base_ren_type;
	typedef agg::renderer_mclip<pixfmt_type> mclip_ren_type;
	typedef agg::renderer_base<pixfmt_amask_type> base_amren_type;
	typedef agg::renderer_mclip<pixfmt_amask_type> mclip_amren_type;

public:
	CClipRender()
	{
		mpAlphaMask = NULL;
	}
// 	CRenderClip(rbuf_type& rb)
// 	{
// 		mpAlphaMask = NULL;
// 		attach(rb);
// 	}
	void attach(rbuf_type& rb) 
	{
		mpAlphaMask = NULL;
		maRcClip.RemoveAll();

		m_pixBuffer.attach(rb);
		m_pixAMaskBuffer.attach_pixfmt(m_pixBuffer);

		mBaseRen.attach(m_pixBuffer);
		mMClipRen.attach(m_pixBuffer);
		mAmBaseRen.attach(m_pixAMaskBuffer);
		mAmMClipRen.attach(m_pixAMaskBuffer);

		SetClipRect(CRect(0,0,width(),height()));
	}

	const pixfmt_type& ren() const { return m_pixBuffer;  }
	pixfmt_type& ren() { return m_pixBuffer;  }

	//--------------------------------------------------------------------
	unsigned width()  const { return m_pixBuffer.width();  }
	unsigned height() const { return m_pixBuffer.height(); }


	//--------------------------------------------------------------------
	// 裁剪区函数

	void SetClipRect(const CRect& rcClip)
	{
		maRcClip.RemoveAll();
		maRcClip.Add(rcClip);
		//TRACE("rcclip: %d %d %d %d",rcClip.left,rcClip.top,rcClip.right,rcClip.bottom);
		UpdateClip();
	}
	void AddClipRect(const CRect& rcClip)
	{
		maRcClip.Add(rcClip);
		UpdateClip();
	}
	// 如果当前alphamask不为空，返回true
	BOOL SetAlphaMask(CAlphaMask* pAlphaMask)
	{
		if( pAlphaMask!=mpAlphaMask)
		{
			if( pAlphaMask==NULL || pAlphaMask->width()!=width() || pAlphaMask->height()!=height() )
			{
				mpAlphaMask=NULL;
			}else
			{
				mpAlphaMask = pAlphaMask;
			}
			UpdateClip();
		}
		return mpAlphaMask!=NULL;
	}

	int GetClipCount() const 
	{
		if( maRcClip.IsEmpty() )
			return 1;

		return maRcClip.GetSize();
	}
	void GetClipAt(int index,CRect& rcClip) const
	{
		if(index>=0 && index<maRcClip.GetSize())
		{
			rcClip = maRcClip[index];
		}else if ( maRcClip.IsEmpty() )
		{
			rcClip.left=mBaseRen.xmin();
			rcClip.top =mBaseRen.ymin();
			rcClip.right=mBaseRen.xmax()+1;
			rcClip.bottom=mBaseRen.ymax()+1;
		}
	}

	const CArray<CRect>& GetAllClipRc() const
	{
		return maRcClip;
	}
	CAlphaMask* GetAlphaMask() const
	{
		return mpAlphaMask;
	}
	template<class SrcClipRender>
	void CopyClipFrom(const SrcClipRender& from)
	{
		mpAlphaMask = from.GetAlphaMask() ;
		mrcBounding = from.clip_box();
		maRcClip.RemoveAll();
		maRcClip.Append(from.GetAllClipRc());
		UpdateClip();
	}

	const rect_i& clip_box() const { return mrcBounding; }
	int           xmin()     const { return mrcBounding.x1; }
	int           ymin()     const { return mrcBounding.y1; }
	int           xmax()     const { return mrcBounding.x2; }
	int           ymax()     const { return mrcBounding.y2; }


	//--------------------------------------------------------------------
	// 绘制函数
	void clear(const color_type& c)
	{
		mBaseRen.clear(c);
	}
	// pixel
	void copy_pixel(int x, int y, const color_type& c)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.copy_pixel(x,y,c); break;
		case eClipMClip:	mMClipRen.copy_pixel(x,y,c); break;
		case eClipAMBase:	mAmBaseRen.copy_pixel(x,y,c); break;
		case eClipAMMClip: mAmMClipRen.copy_pixel(x,y,c); break;
		} 
	}
	void blend_pixel(int x, int y, const color_type& c, cover_type cover)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.blend_pixel(x,y,c,cover); break;
		case eClipMClip:	mMClipRen.blend_pixel(x,y,c,cover); break;
		case eClipAMBase:	mAmBaseRen.blend_pixel(x,y,c,cover); break;
		case eClipAMMClip: mAmMClipRen.blend_pixel(x,y,c,cover); break;
		} 
	}
	void copy_pixel_directly(int x, int y, const color_type& c)
	{
		if(mpAlphaMask==NULL)
			m_pixBuffer.copy_pixel(x, y, c);
		else
			m_pixAMaskBuffer.copy_pixel(x,y,c);
	}

	void blend_pixel_directly(int x, int y, const color_type& c, cover_type cover)
	{
		if(mpAlphaMask==NULL)
			m_pixBuffer.blend_pixel(x, y, c,cover);
		else
			m_pixAMaskBuffer.blend_pixel(x,y,c,cover);
	}

	color_type pixel(int x, int y) const
	{
		return mBaseRen.pixel(x,y);
	}
	// hline
	void copy_hline(int x1, int y, int x2, const color_type& c)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.copy_hline(x1,y,x2,c); break;
		case eClipMClip:	mMClipRen.copy_hline(x1,y,x2,c); break;
		case eClipAMBase:	mAmBaseRen.copy_hline(x1,y,x2,c); break;
		case eClipAMMClip: mAmMClipRen.copy_hline(x1,y,x2,c); break;
		} 
	}
	void blend_hline(int x1, int y, int x2, 
		const color_type& c, cover_type cover)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.blend_hline(x1,y,x2,c,cover); break;
		case eClipMClip:	mMClipRen.blend_hline(x1,y,x2,c,cover); break;
		case eClipAMBase:	mAmBaseRen.blend_hline(x1,y,x2,c,cover); break;
		case eClipAMMClip: mAmMClipRen.blend_hline(x1,y,x2,c,cover); break;
		} 
	}
	// vline
	void copy_vline(int x, int y1, int y2, const color_type& c)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.copy_vline(x,y1,y2,c); break;
		case eClipMClip:	mMClipRen.copy_vline(x,y1,y2,c); break;
		case eClipAMBase:	mAmBaseRen.copy_vline(x,y1,y2,c); break;
		case eClipAMMClip: mAmMClipRen.copy_vline(x,y1,y2,c); break;
		} 
	}
	void blend_vline(int x, int y1, int y2, 
		const color_type& c, cover_type cover)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.blend_vline(x,y1,y2,c,cover); break;
		case eClipMClip:	mMClipRen.blend_vline(x,y1,y2,c,cover); break;
		case eClipAMBase:	mAmBaseRen.blend_vline(x,y1,y2,c,cover); break;
		case eClipAMMClip: mAmMClipRen.blend_vline(x,y1,y2,c,cover); break;
		} 
	}
	// bar
	void copy_bar(int x1, int y1, int x2, int y2, const color_type& c)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.copy_bar(x1,y1,x2,y2,c); break;
		case eClipMClip:	mMClipRen.copy_bar(x1,y1,x2,y2,c); break;
		case eClipAMBase:	mAmBaseRen.copy_bar(x1,y1,x2,y2,c); break;
		case eClipAMMClip: mAmMClipRen.copy_bar(x1,y1,x2,y2,c); break;
		} 
	}
	void blend_bar(int x1, int y1, int x2, int y2, 
		const color_type& c, cover_type cover)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.blend_bar(x1,y1,x2,y2,c,cover); break;
		case eClipMClip:	mMClipRen.blend_bar(x1,y1,x2,y2,c,cover); break;
		case eClipAMBase:	mAmBaseRen.blend_bar(x1,y1,x2,y2,c,cover); break;
		case eClipAMMClip: mAmMClipRen.blend_bar(x1,y1,x2,y2,c,cover); break;
		} 
	}
	// solid span
	void blend_solid_hspan(int x, int y, int len, 
		const color_type& c, const cover_type* covers)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.blend_solid_hspan(x,y,len,c,covers); break;
		case eClipMClip:	mMClipRen.blend_solid_hspan(x,y,len,c,covers); break;
		case eClipAMBase:	mAmBaseRen.blend_solid_hspan(x,y,len,c,covers); break;
		case eClipAMMClip: mAmMClipRen.blend_solid_hspan(x,y,len,c,covers); break;
		} 
	}
	void blend_solid_vspan(int x, int y, int len, 
		const color_type& c, const cover_type* covers)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.blend_solid_vspan(x,y,len,c,covers); break;
		case eClipMClip:	mMClipRen.blend_solid_vspan(x,y,len,c,covers); break;
		case eClipAMBase:	mAmBaseRen.blend_solid_vspan(x,y,len,c,covers); break;
		case eClipAMMClip: mAmMClipRen.blend_solid_vspan(x,y,len,c,covers); break;
		} 
	}
	void copy_solid_hspan(int x, int y, int len, const color_type& c)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.copy_solid_hspan(x,y,len,c); break;
		case eClipMClip:	mMClipRen.copy_solid_hspan(x,y,len,c); break;
		case eClipAMBase:	mAmBaseRen.copy_solid_hspan(x,y,len,c); break;
		case eClipAMMClip: mAmMClipRen.copy_solid_hspan(x,y,len,c); break;
		} 
	}
	void copy_color_hspan(int x, int y, int len, const color_type* colors)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.copy_color_hspan(x,y,len,colors); break;
		case eClipMClip:	mMClipRen.copy_color_hspan(x,y,len,colors); break;
		case eClipAMBase:	mAmBaseRen.copy_color_hspan(x,y,len,colors); break;
		case eClipAMMClip: mAmMClipRen.copy_color_hspan(x,y,len,colors); break;
		} 
	}
	void copy_color_vspan(int x, int y, int len, const color_type* colors)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.copy_color_vspan(x,y,len,colors); break;
		case eClipMClip:	mMClipRen.copy_color_vspan(x,y,len,colors); break;
		case eClipAMBase:	mAmBaseRen.copy_color_vspan(x,y,len,colors); break;
		case eClipAMMClip: mAmMClipRen.copy_color_vspan(x,y,len,colors); break;
		} 
	}
	void blend_color_hspan(int x, int y, int len, 
		const color_type* colors, 
		const cover_type* covers,
        cover_type cover = agg::cover_full)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.blend_color_hspan(x,y,len,colors,covers,cover); break;
		case eClipMClip:	mMClipRen.blend_color_hspan(x,y,len,colors,covers,cover); break;
		case eClipAMBase:	mAmBaseRen.blend_color_hspan(x,y,len,colors,covers,cover); break;
		case eClipAMMClip: mAmMClipRen.blend_color_hspan(x,y,len,colors,covers,cover); break;
		} 
	}
	void blend_color_vspan(int x, int y, int len, 
		const color_type* colors, 
		const cover_type* covers,
		cover_type cover = agg::cover_full)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.blend_color_vspan(x,y,len,colors,covers,cover); break;
		case eClipMClip:	mMClipRen.blend_color_vspan(x,y,len,colors,covers,cover); break;
		case eClipAMBase:	mAmBaseRen.blend_color_vspan(x,y,len,colors,covers,cover); break;
		case eClipAMMClip: mAmMClipRen.blend_color_vspan(x,y,len,colors,covers,cover); break;
		} 
	}
	void copy_from(const self_type& from, 
		const rect_i* rc=0, 
		int x_to=0, 
		int y_to=0)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.copy_from(from.mBaseRen,rc,x_to,y_to); break;
		case eClipMClip:	mMClipRen.copy_from(from.mMClipRen,rc,x_to,y_to); break;
		case eClipAMBase:	mAmBaseRen.copy_from(from.mAmBaseRen,rc,x_to,y_to); break;
		case eClipAMMClip: mAmMClipRen.copy_from(from.mAmMClipRen,rc,x_to,y_to); break;
		} 
	}
	template<class SrcPixelFormatRenderer>
	void blend_from(const SrcPixelFormatRenderer& src, 
		const rect_i* rect_src_ptr = 0, 
		int dx = 0, 
		int dy = 0,
		cover_type cover = agg::cover_full)
	{
		switch (meClipType)
		{
		case eClipBase:	mBaseRen.blend_from(src,rect_src_ptr,dx,dy,cover); break;
		case eClipMClip:	mMClipRen.blend_from(src,rect_src_ptr,dx,dy,cover); break;
		case eClipAMBase:	mAmBaseRen.blend_from(src,rect_src_ptr,dx,dy,cover); break;
		case eClipAMMClip: mAmMClipRen.blend_from(src,rect_src_ptr,dx,dy,cover); break;
		} 
	}

	void clear_rect(int l,int t,int r,int b,const color_type& c)
	{
		copy_bar(l,t,r,b,c);
	}
	void blend_rect(int l,int t,int r,int b,const color_type& c, cover_type cover)
	{
		blend_bar(l,t,r,b,c,cover);
	}
	void invert_hspan(int x,int y,int len)
	{
	}


private:
	base_ren_type mBaseRen;
	mclip_ren_type mMClipRen;
	base_amren_type mAmBaseRen;
	mclip_amren_type mAmMClipRen;

	pixfmt_type	m_pixBuffer;
	pixfmt_amask_type m_pixAMaskBuffer;
	alpha_mask_type m_amask;

	EClipType meClipType;
	CArray<CRect> maRcClip;
	CAlphaMask* mpAlphaMask;
	rect_i	mrcBounding;


private:
	void UpdateClip()
	{
		if ( mpAlphaMask==NULL )
		{
			meClipType = maRcClip.GetCount()>1 ? eClipMClip : eClipBase ;
		}else
		{
			m_amask.attach(mpAlphaMask->GetAlphaMaskBuf());
			m_pixAMaskBuffer.attach_alpha_mask(m_amask);
			meClipType = maRcClip.GetCount()>1 ? eClipAMMClip: eClipAMBase;
		}

		switch(meClipType)
		{
		case eClipBase:
			if( maRcClip.IsEmpty())	mBaseRen.reset_clipping(true);
			else					
			{
				CRect& rc=maRcClip[0];
				mBaseRen.clip_box(rc.left,rc.top,rc.right-1,rc.bottom-1);
			}
			mrcBounding = mBaseRen.clip_box();
			break;
		case eClipMClip:
			mMClipRen.reset_clipping(true);
			for (int i=0;i<maRcClip.GetSize();i++)
			{
				CRect& rc=maRcClip[i];
				mMClipRen.add_clip_box(rc.left,rc.top,rc.right-1,rc.bottom-1);
			}
			mrcBounding = mMClipRen.bounding_clip_box();
			break;
		case eClipAMBase:
			if( maRcClip.IsEmpty())	mAmBaseRen.reset_clipping(true);
			else					
			{
				CRect& rc=maRcClip[0];
				mAmBaseRen.clip_box(rc.left,rc.top,rc.right-1,rc.bottom-1);
			}
			mrcBounding = mAmBaseRen.clip_box();
			break;
		case eClipAMMClip:
			mAmMClipRen.reset_clipping(true);
			for (int i=0;i<maRcClip.GetSize();i++)
			{
				CRect& rc=maRcClip[i];
				mAmMClipRen.add_clip_box(rc.left,rc.top,rc.right-1,rc.bottom-1);
			}
			mrcBounding = mAmMClipRen.bounding_clip_box();
			break;
		}
	}
};
