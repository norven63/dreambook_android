package cn.retech.custom_control;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ScrollView;
import cn.retech.custom_control.AnimatorDirector.AnimatorBuilder;

public class CanPullDownAndUpScrollView extends ScrollView {
	private boolean canPull = false;// 标记是否可以拖拽

	public CanPullDownAndUpScrollView(Context context) {
		super(context);

		init();
	}

	public CanPullDownAndUpScrollView(Context context, AttributeSet attrs) {
		super(context, attrs);

		init();
	}

	protected final void init() {
		this.setOnTouchListener(new OnTouchListener() {
			private float startPointY;

			@Override
			public boolean onTouch(View v, MotionEvent event) {
				if (null == v.getTag()) {
					v.setTag(v.getY());
				}

				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					startPointY = event.getY();

					break;
				case MotionEvent.ACTION_UP:
					// 复位动画
					AnimatorDirector.animate(new AnimatorBuilder().view(v).translationY((Float) v.getTag()));

					// 重置标记位
					canPull = false;

					break;
				case MotionEvent.ACTION_MOVE:
					float move = event.getY() - startPointY;
					startPointY = event.getY();

					do {
						if (canPull && Math.abs(move) > 1) {
							v.setY(v.getY() + move * 1 / 3);
						}
					} while (false);

					break;
				}

				return false;
			}
		});
	}

	@Override
	protected void onOverScrolled(int scrollX, int scrollY, boolean clampedX, boolean clampedY) {
		canPull = clampedY;// 捕捉可以下拉的时机，利用此法感觉良好

		super.onOverScrolled(scrollX, scrollY, clampedX, clampedY);
	}
}
