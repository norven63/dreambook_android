package cn.retech.activity;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.Toast;
import cn.retech.custom_control.AnimatorDirector;
import cn.retech.custom_control.AnimatorDirector.AnimatorBuilder;
import cn.retech.custom_control.RichTextPageContentView;
import cn.retech.custom_control.RichTextPageContentView.OperateBookListener;
import cn.retech.rich_text_reader.BookReader;
import cn.retech.rich_text_reader.BookReader.BookOperation;
import cn.retech.rich_text_reader.BookReader.FontSizeOpterationEnum;
import cn.retech.rich_text_reader.CRBClickListener;
import cn.retech.rich_text_reader.CalcLayoutListener;

public class RichTextReaderActivity extends Activity {
	public static final String BOOKPATHKEY = "BOOT_PATH";
	private String bookPath = "/storage/sdcard0/DreamBook/index.book";

	private RichTextPageContentView richTextPageContentView;// 渲染书籍的容器
	private View lightLevelView;// 亮度隔层
	private ProgressBar loadProgressBar;// 分页进度条
	private SeekBar lightLevelSeekBar;// 亮度调节滑块
	private RelativeLayout pageNumberLayout;// 跳转功能布局
	private Button finishOperationButton;// 关闭按钮
	private EditText pagetNumberEditText;// 跳转功能输入框
	private Button pageNmberConfirmButton;// 跳转按钮
	private RelativeLayout opterationLayout;// 操作项布局
	private Button reduceFontSizeButton;// 减小字体按钮
	private Button addFontSizeButton;// 增大字体按钮

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.rich_text_reader_layout);

		if (null != getIntent().getExtras()) {
			bookPath = getIntent().getExtras().getString(BOOKPATHKEY);
		}

		loadProgressBar = (ProgressBar) findViewById(R.id.loadProgressBar);
		opterationLayout = (RelativeLayout) findViewById(R.id.opterationLayout);

		richTextPageContentView = (RichTextPageContentView) findViewById(R.id.showBookImageView);
		richTextPageContentView.setOperateBookListener(new OperateBookListener() {
			@Override
			public void onUp(float x, float y) {
				BookReader.getInstance.onClick(x, y);

				if (!richTextPageContentView.hasUpOrMove()) {
					BookReader.getInstance.selSentence(x, y);
				}
			}

			@Override
			public void onLongPress(float x, float y) {
				BookReader.getInstance.dragComment(x, y);
			}

			@Override
			public void onMove(float x, float y) {
				BookReader.getInstance.onMove(x, y);
			}

			@Override
			public boolean onDown(float x, float y) {
				return BookReader.getInstance.dragSentence(x, y);
			}

			@Override
			public void removeBookmark() {
				BookReader.getInstance.removeBookmark();
			}

			@Override
			public void onLastPage() {
				if (BookReader.getInstance.operateBook(BookOperation.LAST)) {
					BookReader.getInstance.drawBookOnView(BookReader.getInstance.getFullScreenRect(), true);
				}
			}

			@Override
			public void onNextPage() {
				if (BookReader.getInstance.operateBook(BookOperation.NEXT)) {
					BookReader.getInstance.drawBookOnView(BookReader.getInstance.getFullScreenRect(), true);
				}
			}
		});

		// 调节亮度
		lightLevelView = findViewById(R.id.lightLevelView);
		lightLevelSeekBar = (SeekBar) findViewById(R.id.lightLevelSeekBar);
		lightLevelSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
			}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
			}

			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				if (fromUser) {
					lightLevelView.setAlpha((float) progress / 10);
				}
			}
		});

		pageNumberLayout = (RelativeLayout) findViewById(R.id.pageNumberLayout);
		pagetNumberEditText = (EditText) findViewById(R.id.pageNumberEditText);
		finishOperationButton = (Button) findViewById(R.id.finishOperationButton);
		finishOperationButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				hiddenOpterationLayout();
			}
		});
		pageNmberConfirmButton = (Button) findViewById(R.id.pageNmberConfirmButton);
		pageNmberConfirmButton.setOnClickListener(new OnClickListener() {
			// 跳转指定页
			@Override
			public void onClick(View v) {
				String gotoPageNumber = pagetNumberEditText.getText().toString();
				if (TextUtils.isEmpty(gotoPageNumber)) {
					return;
				}

				InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
				imm.hideSoftInputFromWindow(pagetNumberEditText.getWindowToken(), 0);
				pagetNumberEditText.setText("");

				BookOperation.GOTO.setGotoPageNumber(Integer.parseInt(gotoPageNumber) - 1);
				if (BookReader.getInstance.operateBook(BookOperation.GOTO)) {
					BookReader.getInstance.drawBookOnView(BookReader.getInstance.getFullScreenRect(), true);
				}
			}
		});

		// 设置字体
		reduceFontSizeButton = (Button) findViewById(R.id.reduceFontSizeButton);
		reduceFontSizeButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (BookReader.getInstance.setFontSize(FontSizeOpterationEnum.REDUCE)) {
					BookReader.getInstance.drawBookOnView(BookReader.getInstance.getFullScreenRect(), true);
				}
			}
		});
		addFontSizeButton = (Button) findViewById(R.id.addFontSizeButton);
		addFontSizeButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (BookReader.getInstance.setFontSize(FontSizeOpterationEnum.ADD)) {
					BookReader.getInstance.drawBookOnView(BookReader.getInstance.getFullScreenRect(), true);
				}
			}
		});
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);

		// 缓存书籍操作项布局的状态，显示动画时用
		if (null == opterationLayout.getTag()) {
			pageNumberLayout.setY(-pageNumberLayout.getHeight());
			opterationLayout.setTag(opterationLayout.getY());
			opterationLayout.setY(opterationLayout.getY() + opterationLayout.getHeight());
			pageNumberLayout.setVisibility(View.VISIBLE);
			opterationLayout.setVisibility(View.VISIBLE);
		}

		CRBClickListener crbClickListener = new CRBClickListener();
		CalcLayoutListener calcLayoutListener = new CalcLayoutListener(loadProgressBar);
		if (!BookReader.getInstance.prepareToWork(richTextPageContentView, crbClickListener, calcLayoutListener, bookPath)) {
			Toast.makeText(this, "书籍加载失败!", Toast.LENGTH_LONG).show();

			return;
		}

		if (BookReader.getInstance.operateBook(BookOperation.LOAD)) {
			BookReader.getInstance.drawBookOnView(BookReader.getInstance.getFullScreenRect(), true);
		}
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		boolean returnValue = false;

		switch (keyCode) {
		case KeyEvent.KEYCODE_MENU:
			// 点击MENU键进入编或者退出编辑状态（即MENU键是一个双向操作）
			if (richTextPageContentView.isOperatingBook()) {
				hiddenOpterationLayout();
			} else {
				showOpterationLayout();
			}

			returnValue = super.onKeyDown(keyCode, event);

			break;
		case KeyEvent.KEYCODE_BACK:
			// 点击BACK键退出编辑状态
			if (richTextPageContentView.isOperatingBook()) {
				hiddenOpterationLayout();

				returnValue = true;
			} else {
				returnValue = super.onKeyDown(keyCode, event);
			}

			break;
		default:
			returnValue = super.onKeyDown(keyCode, event);

			break;
		}

		return returnValue;
	}

	/**
	 * 显示各个操作项
	 */
	private void showOpterationLayout() {
		richTextPageContentView.setIsOperatingBook(true);

		AnimatorDirector.animate(new AnimatorBuilder().view(pageNumberLayout).y(0f).setDuration(350));
		AnimatorDirector.animate(new AnimatorBuilder().view(opterationLayout).y((Float) opterationLayout.getTag()).setDuration(350));
	}

	/**
	 * 隐藏各个操作项
	 */
	private void hiddenOpterationLayout() {
		richTextPageContentView.setIsOperatingBook(false);

		AnimatorDirector.animate(new AnimatorBuilder().view(pageNumberLayout).y(-pageNumberLayout.getHeight()).setDuration(350));
		AnimatorDirector.animate(new AnimatorBuilder().view(opterationLayout).y(opterationLayout.getY() + opterationLayout.getHeight())
				.setDuration(350));

		// 隐藏输入法
		InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
		imm.hideSoftInputFromWindow(pagetNumberEditText.getWindowToken(), 0);
		pagetNumberEditText.setText("");
	}
}
