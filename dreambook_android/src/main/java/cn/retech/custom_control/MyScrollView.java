package cn.retech.custom_control;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import cn.retech.activity.R;

public class MyScrollView extends CanPullDownAndUpScrollView {
	private boolean canAutoScroll = true;

	public MyScrollView(Context context) {
		super(context);
	}

	public MyScrollView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	/**
	 * 触碰之后,更改标志位,使View在onLayout时不再会自动上滑
	 */
	@Override
	public boolean onTouchEvent(MotionEvent ev) {
		canAutoScroll = false;
		return super.onTouchEvent(ev);
	}

	@Override
	protected void onLayout(boolean changed, int l, int t, int r, int b) {
		super.onLayout(changed, l, t, r, b);

		if (canAutoScroll) {
			this.scrollTo(0, (int) findViewById(R.id.bookshelf_gridLayout).getY() - 10);
		}
	}
}
