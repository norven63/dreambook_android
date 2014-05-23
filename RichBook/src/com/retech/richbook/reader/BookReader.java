package com.retech.richbook.reader;

import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Rect;
import android.text.TextUtils;
import android.widget.ImageView;

public enum BookReader {
	getInstance;

	private int width;// 屏幕宽度
	private int height;// 屏幕高度
	private int densityDPI;// 屏幕密度
	private String bookPath;// 书籍路径
	private Bitmap bitmap;// 渲染图像的bitmap
	private ImageView bookContentImageView;// 承载书籍展现内容的容器
	private Rect fullScreenRect;// 全屏渲染时用的矩阵对象

	/**
	 * 书籍操作枚举
	 */
	public enum BookOperation {
		// 加载书籍
		LOAD,
		// 下一页
		NEXT,
		// 上一页
		LAST,
		// 跳转至目标页
		GOTO;

		private int gotoPageNumber;// 目标跳转页数

		/**
		 * 设置跳转页数
		 * 
		 * @param gotoPageNumber
		 *          目标跳转页数
		 */
		public void setGotoPageNumber(int gotoPageNumber) {
			this.gotoPageNumber = gotoPageNumber;
		}

		/**
		 * @return 目标跳转页数
		 */
		public int getGotoPageNumber() {
			return gotoPageNumber;
		}
	}

	/**
	 * 字体增减操作类型枚举
	 */
	public enum FontSizeOpterationEnum {
		// 减小一号字体
		REDUCE(-1),
		// 增大一号字体
		ADD(+1);

		private int font;

		private FontSizeOpterationEnum(int font) {
			this.font = font;
		}

		public int getFont() {
			return font;
		}
	}

	/**
	 * 初始化渲染引擎环境
	 * 
	 * @param screenWidth
	 *          屏幕宽度
	 * @param screenHeight
	 *          屏幕高度
	 * @param screenDensityDPI
	 *          屏幕密度
	 */
	public void initProperty(int screenWidth, int screenHeight, int screenDensityDPI) {
		this.width = screenWidth;
		this.height = screenHeight;
		this.densityDPI = screenDensityDPI;
		fullScreenRect = new Rect(0, 0, width, height);

		BookReaderJni.initGlobalSettings(width, height, densityDPI);
	}

	/**
	 * @return 全屏显示用的矩阵
	 */
	public Rect getFullScreenRect() {
		return fullScreenRect;
	}

	/**
	 * 书籍解析渲染准备工作
	 * 
	 * @param bookContentImageView
	 *          承载书籍展现内容的容器
	 * @param crbClickListener
	 *          书籍点击互动监听器
	 * @param calcLayoutListener
	 *          分页监听器
	 * @param bookPath
	 *          书籍路径
	 * @return 是否成功完成准备工作
	 */
	public boolean prepareToWork(ImageView bookContentImageView, CRBClickListener crbClickListener, CalcLayoutListener calcLayoutListener,
			String bookPath) {
		if (TextUtils.isEmpty(bookPath)) {
			assert false : "书籍路径错误!";

			return false;
		}

		this.bookPath = bookPath;
		this.bookContentImageView = bookContentImageView;
		bitmap = Bitmap.createBitmap(bookContentImageView.getWidth(), bookContentImageView.getHeight(), Config.ARGB_8888);
		bookContentImageView.setImageBitmap(bitmap);

		BookReaderJni.setListeners(crbClickListener, calcLayoutListener);

		return true;
	}

	/**
	 * 字体增减操作
	 * 
	 * @param fontSizeOpterationEnum
	 *          字体增减操作类型
	 * @return 是否可以立即渲染当前页
	 */
	public boolean setFontSize(FontSizeOpterationEnum fontSizeOpterationEnum) {
		return BookReaderJni.setFontSize(fontSizeOpterationEnum.getFont());
	}

	/**
	 * 书籍翻页操作
	 * 
	 * @param bookOperation
	 *          操作类型
	 * @return 是否可以立即渲染当前页
	 */
	public boolean operateBook(BookOperation bookOperation) {
		boolean returnValue = false;

		switch (bookOperation) {
		case LOAD:
			returnValue = BookReaderJni.loadBook(bookPath);
			break;
		case GOTO:
			returnValue = BookReaderJni.goToPage(BookOperation.GOTO.getGotoPageNumber());
			break;
		case NEXT:
			returnValue = BookReaderJni.nextPage();
			break;
		case LAST:
			returnValue = BookReaderJni.lastPage();
			break;
		}

		return returnValue;
	}

	/**
	 * 准备拖拽下划线
	 * 
	 * @param x
	 *          触点坐标x
	 * @param y
	 *          触点坐标y
	 */
	public void dragComment(float x, float y) {
		BookReaderJni.dragComment(x, y);
	}

	/**
	 * 选中要拖拽的语句块并高亮显示
	 * 
	 * @param x
	 *          触点坐标x
	 * @param y
	 *          触点坐标y
	 */
	public void selSentence(float x, float y) {
		BookReaderJni.selSentence(x, y);

		drawBookOnView(fullScreenRect, true);
	}

	/**
	 * 准备拖拽语句块
	 * 
	 * @param x
	 *          触点坐标x
	 * @param y
	 *          触点坐标y
	 * @return 手指的触点是否处于拖动滑块区域
	 */
	public boolean dragSentence(float x, float y) {
		return BookReaderJni.dragSentence(x, y);
	}

	/**
	 * 点击事件
	 * 
	 * @param x
	 *          触点坐标x
	 * @param y
	 *          触点坐标y
	 */
	public void onClick(float x, float y) {
		BookReaderJni.onClick(x, y);
	}

	/**
	 * 手指移动
	 * 
	 * @param x
	 *          触点坐标x
	 * @param y
	 *          触点坐标y
	 */
	public void onMove(float x, float y) {
		int[] recValues = new int[4];
		BookReaderJni.onTouchMove(x, y, recValues);
		drawBookOnView(fullScreenRect, true);
		// drawBookOnView(new Rect(recValues[0], recValues[1], recValues[2],
		// recValues[3]), false);
	}

	/**
	 * 清除拖拽块
	 */
	public void removeBookmark() {
		BookReaderJni.removeBookmark();
		drawBookOnView(fullScreenRect, true);
	}

	/**
	 * @return 是否可以渲染当前页
	 */
	public boolean isCurPosVisible() {
		return BookReaderJni.isCurPosVisible();
	}

	/**
	 * 渲染书籍到容器上，目前全部采用全屏copy图像数据的方式。留出了局部copy的功能接口，供日后发现渲染图像有效率吃紧问题时用。
	 * 
	 * @param rect
	 *          需要刷新矩阵区域
	 * @param isFullScreen
	 *          是否需要全屏copy图像数据
	 */
	public void drawBookOnView(Rect rect, boolean isFullScreen) {
		int[] rectValues = new int[] { rect.left, rect.top, rect.right, rect.bottom };
		BookReaderJni.drawBookContent(rectValues, bitmap, isFullScreen);
		bookContentImageView.invalidate(rect);
	}
}
