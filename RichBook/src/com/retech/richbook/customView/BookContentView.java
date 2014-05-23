package com.retech.richbook.customView;

import android.content.Context;
import android.util.AttributeSet;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.widget.ImageView;

public class BookContentView extends ImageView {
	/**
	 * 书籍操作回调接口
	 */
	public interface OperateBookListener {
		/**
		 * 长按事件
		 * 
		 * @param x
		 *          触点坐标x
		 * @param y
		 *          触点坐标y
		 */
		public void onLongPress(float x, float y);

		/**
		 * 按下事件
		 * 
		 * @param x
		 *          触点坐标x
		 * @param y
		 *          左边点y
		 * @return 手指的触点是否处于拖动滑块区域
		 */
		public boolean onDown(float x, float y);

		/**
		 * 抬起事件
		 * 
		 * @param x
		 *          触点坐标x
		 * @param y
		 *          左边点y
		 */
		public void onUp(float x, float y);

		/**
		 * 挪动事件
		 * 
		 * @param x
		 *          触点坐标x
		 * @param y
		 *          触点坐标y
		 */
		public void onMove(float x, float y);

		/**
		 * 取消选择语句状态
		 */
		public void removeBookmark();

		/**
		 * 下一页
		 */
		public void onNextPage();

		/**
		 * 上一页
		 */
		public void onLastPage();
	}

	private boolean isSelectingSentence = false;// 标识是否正处于拖拽选择书籍语句的状态
	private boolean isOperatingBook = false;// 标识是否正处于操作书籍的状态
	private boolean hasUpOrMove = false;// 标识手指是否抬起过或者挪动过，该状态会在长按时重置为false
	private float startX;// DOWN事件缓存的X坐标
	private float startY;// DOWN事件缓存的Y坐标

	private GestureDetector gestureDetector;
	private OperateBookListener operateBookListener;

	public BookContentView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);

		init();
	}

	public BookContentView(Context context, AttributeSet attrs) {
		super(context, attrs);

		init();
	}

	public BookContentView(Context context) {
		super(context);

		init();
	}

	private void init() {
		gestureDetector = new GestureDetector(getContext(), new GestureDetector.SimpleOnGestureListener() {
			private static final int FLING_MIN_DISTANCE = 120;// 移动最小距离
			private static final int FLING_MIN_VELOCITY = 80;// 移动最大速度

			@Override
			public boolean onDown(MotionEvent e) {
				// 缓存按下时的触点坐标。
				// 这里需要注意的是不要缓存MotionEvent对象，因为每次touch事件的MotionEvent对象都是同一个对象，缓存操作无法达到捕捉指定时机event状态的效果
				startX = e.getX();
				startY = e.getY();

				// 这里需要返回true才可以拦截并监听Action.DOWN之后的其他事件
				return true;
			}

			@Override
			public void onLongPress(MotionEvent e) {
				if (null != operateBookListener) {
					hasUpOrMove = false;
					isSelectingSentence = true;
					operateBookListener.onLongPress(e.getX(), e.getY());
				}
			}

			@Override
			public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
				if (null == operateBookListener || isOperatingBook || isSelectingSentence) {
					return false;
				}

				if (e1.getX() - e2.getX() > FLING_MIN_DISTANCE && Math.abs(velocityX) > FLING_MIN_VELOCITY) {
					operateBookListener.onNextPage();
				}

				if (e2.getX() - e1.getX() > FLING_MIN_DISTANCE && Math.abs(velocityX) > FLING_MIN_VELOCITY) {
					operateBookListener.onLastPage();
				}

				// if (e1.getY() - e2.getY() > FLING_MIN_DISTANCE && Math.abs(velocityY)
				// >
				// FLING_MIN_VELOCITY) {
				// operateBookListener.onFlingUP();
				// }
				//
				// if (e2.getY() - e1.getY() > FLING_MIN_DISTANCE && Math.abs(velocityY)
				// >
				// FLING_MIN_VELOCITY) {
				// operateBookListener.onFlingDown();
				// }

				return false;
			}
		});
	}

	public boolean hasUpOrMove() {
		return hasUpOrMove;
	}

	public void setIsOperatingBook(boolean isOperatingBook) {
		this.isOperatingBook = isOperatingBook;
	}

	public boolean isSelectingSentence() {
		return isSelectingSentence;
	}

	public boolean isOperatingBook() {
		return isOperatingBook;
	}

	public void setOperateBookListener(OperateBookListener operateBookListener) {
		this.operateBookListener = operateBookListener;
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		do {
			if (null == operateBookListener || !isSelectingSentence) {
				break;
			}

			switch (event.getAction()) {
			case MotionEvent.ACTION_DOWN:
				if (!operateBookListener.onDown(event.getX(), event.getY())) {
					isSelectingSentence = false;
					operateBookListener.removeBookmark();
				}

				break;
			case MotionEvent.ACTION_UP:
				operateBookListener.onUp(event.getX(), event.getY());

				hasUpOrMove = true;

				break;
			case MotionEvent.ACTION_MOVE:
				operateBookListener.onMove(event.getX(), event.getY());

				if (!hasUpOrMove && Math.abs(event.getX() - startX) > 10f || Math.abs(event.getY() - startY) > 10f) {
					hasUpOrMove = true;
				}

				break;
			}
		} while (false);

		return gestureDetector.onTouchEvent(event);
	}
}
