package cn.retech.custom_control;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.view.View;

public class BookThumbnailProgressView extends View {
	private float bookDownLoadProgress = 0;// 下载百分比
	private final int redress = 8;// 矫正算子
	private final Paint whitePaint = new Paint();
	private final Paint blackPaint = new Paint();
	private final RectF oval = new RectF();

	public void setBookDownLoadPer(float bookDownLoadPer) {
		this.bookDownLoadProgress = bookDownLoadPer;
	}

	public BookThumbnailProgressView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		init();
	}

	public BookThumbnailProgressView(Context context, AttributeSet attrs) {
		super(context, attrs);
		init();
	}

	public BookThumbnailProgressView(Context context) {
		super(context);
		init();
	}

	@Override
	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);
		int width = canvas.getWidth();
		int height = canvas.getHeight();
		canvas.drawColor(Color.BLACK);

		canvas.drawCircle(width / 2, height / 2, width * 4 / 9, whitePaint);

		oval.left = width / 4 - redress;
		oval.top = height / 2 - width / 4 - redress;
		oval.right = width * 3 / 4 + redress;
		oval.bottom = height / 2 + width / 4 + redress;
		canvas.drawArc(oval, -90, -(360 - (bookDownLoadProgress / 100) * 360), true, blackPaint);
	}

	private void init() {
		whitePaint.setAntiAlias(true);
		whitePaint.setColor(Color.WHITE);
		blackPaint.setAntiAlias(true);
		blackPaint.setColor(Color.BLACK);
	}
}
