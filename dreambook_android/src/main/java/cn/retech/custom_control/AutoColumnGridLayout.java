package cn.retech.custom_control;

import android.animation.LayoutTransition;
import android.content.Context;
import android.database.DataSetObserver;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import cn.retech.activity.R;

/**
 * 扩展MyGridLayout:自动计算间距,添加布局动画,提供设置adapter接口
 */
public class AutoColumnGridLayout extends MyGridLayout {
	private DataSetObserver dataSetObserver;
	protected int margin;
	protected BaseAdapter adapter;
	protected OnClickListener onItemClickListener;

	public AutoColumnGridLayout(Context context) {
		super(context);
		init();
	}

	public AutoColumnGridLayout(Context context, AttributeSet attrs) {
		super(context, attrs);
		init();
	}

	public AutoColumnGridLayout(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		init();
	}

	@Override
	public void addView(View child, int index, android.view.ViewGroup.LayoutParams params) {
		super.addView(child, index, params);

		// 添加子元素时自动设置其间距
		LayoutParams layoutParams = (LayoutParams) child.getLayoutParams();
		layoutParams.width = LayoutParams.WRAP_CONTENT;
		layoutParams.height = LayoutParams.WRAP_CONTENT;
		layoutParams.leftMargin = margin;
		layoutParams.rightMargin = margin;
	}

	/**
	 * @param adapter
	 *          the adapter to set
	 */
	public void setAdapter(BaseAdapter newAdapter) {
		adapter = newAdapter;

		adapter.registerDataSetObserver(dataSetObserver);

		updateData();
	}

	public void setOnItemClickListener(OnClickListener onItemClickListener) {
		this.onItemClickListener = onItemClickListener;
	}

	// 计算margin和columnCount值
	public void calculateParmenters() {
		int screenWidth = getResources().getDisplayMetrics().widthPixels;
		int bookCellWidth = (int) getResources().getDimension(R.dimen.book_folder_width);
		// int eachColumnBookNumber = screenWidth / (bookCellWidth + 2 * 10);
		int eachColumnBookNumber = Integer.parseInt(getResources().getString(R.string.eachColumnBookNumber));
		margin = (screenWidth - eachColumnBookNumber * bookCellWidth) / (2 * eachColumnBookNumber);

		this.setColumnCount(eachColumnBookNumber);
	}

	public void updateData() {
		if (null != adapter) {
			this.removeAllViews();

			calculateParmenters();

			for (int i = 0; i < adapter.getCount(); i++) {
				final View view = adapter.getView(i, null, this);

				if (null != onItemClickListener) {
					view.setOnClickListener(onItemClickListener);
				}

				this.addView(view);
			}
		}
	}

	private void init() {
		setLayoutTransition(new LayoutTransition());// 添加布局动画

		dataSetObserver = new DataSetObserver() {
			@Override
			public void onChanged() {
				updateData();
			}

			@Override
			public void onInvalidated() {
				updateData();
			}
		};
	}

	@Override
	protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
		super.onLayout(changed, left, top, right, bottom);

		// 自动拉长,使父布局可滑动
		int screenHeight = getResources().getDisplayMetrics().heightPixels;
		if (getHeight() < screenHeight) {
			this.getLayoutParams().height = screenHeight;
			((ViewGroup) getParent()).updateViewLayout(this, this.getLayoutParams());
		}
	}
}
