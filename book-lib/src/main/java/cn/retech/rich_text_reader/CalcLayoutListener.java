package cn.retech.rich_text_reader;

import android.view.View;
import android.widget.ProgressBar;

/**
 * 分页监听器
 */
public class CalcLayoutListener {
	private boolean isDraw = false;// 标志是否已渲染
	private int progressRange = 1;// 最大页数
	private final ProgressBar progressBar;

	public CalcLayoutListener(ProgressBar progressBar) {
		super();
		this.progressBar = progressBar;
	}

	/**
	 * 分页开始时的回调方法
	 * 
	 * @param progressRange
	 *          最大页数
	 */
	public void onProgressRangeBack(int progressRange) {
		this.progressRange = progressRange;
		isDraw = false;

		progressBar.post(new Runnable() {
			@Override
			public void run() {
				progressBar.setVisibility(View.VISIBLE);
			}
		});
	}

	/**
	 * 分页执行过程中的回调方法
	 * 
	 * @param progressPos
	 *          已分页的页数
	 */
	public void onUpdateProgressPos(final int progressPos) {
		progressBar.post(new Runnable() {
			@Override
			public void run() {
				progressBar.setProgress(progressPos * 100 / progressRange);

				if (progressBar.getProgress() >= 100) {
					progressBar.setVisibility(View.GONE);
				}

				// 每次回调都检查一次当前页是否可渲染，以此来获取最佳的渲染时机
				if (BookReader.getInstance.isCurPosVisible() && !isDraw) {
					BookReader.getInstance.drawBookOnView(BookReader.getInstance.getFullScreenRect(), true);
					isDraw = true;
				}
			}
		});
	}
}
