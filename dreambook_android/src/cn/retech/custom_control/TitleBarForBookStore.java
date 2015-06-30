package cn.retech.custom_control;

import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import cn.retech.activity.R;
import cn.retech.theme_manage.ThemeBean;
import cn.retech.theme_manage.ThemeManagerSingleton;
import cn.retech.theme_manage.ThemeModel.ThemeBuilder;

public class TitleBarForBookStore extends LinearLayout {
	public static interface OnButtonClickListener {
		public void OnBackButtonClicked();

		public void OnSearchButtonClicked();

		public void OnRefreshButtonClicked();
	}

	private OnButtonClickListener onButtonClickListener;

	public void setOnButtonClickListener(OnButtonClickListener onButtonClickListener) {
		this.onButtonClickListener = onButtonClickListener;
	}

	private RelativeLayout mainLayout;
	private TextView titleTextView;
	private Button backButton;
	private ImageView refreshButton;
	private ImageView searchButton;

	public TitleBarForBookStore(final Context context) {
		super(context);

		init(context);
	}

	public TitleBarForBookStore(Context context, AttributeSet attrs) {
		super(context, attrs);

		init(context);
	}

	public void setTitle(String title) {
		titleTextView.setText(title);
	}

	private void init(Context context) {
		LayoutInflater layoutInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		layoutInflater.inflate(R.layout.page_title_layout, this);

		mainLayout = (RelativeLayout) findViewById(R.id.titleBar_mainLayout);
		titleTextView = (TextView) findViewById(R.id.title_TextView);

		// 返回按钮
		backButton = (Button) findViewById(R.id.back_Button);
		backButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				onButtonClickListener.OnBackButtonClicked();
			}
		});

		// 刷新按钮
		refreshButton = (ImageView) findViewById(R.id.refresh_ImageView);
		refreshButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				onButtonClickListener.OnRefreshButtonClicked();
			}
		});

		// 搜索按钮
		searchButton = (ImageView) findViewById(R.id.search_ImageView);
		searchButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				onButtonClickListener.OnSearchButtonClicked();
			}
		});

		/*
		 * 皮肤设置
		 */
		ThemeBean themeBean = ThemeManagerSingleton.getInstance.getNewThemeModel().getThemeBean();
		ThemeBuilder.replaceControlsBackgroung(themeBean.getMainColor(), mainLayout);
		titleTextView.setTextColor(Color.parseColor(themeBean.getMainTextColor()));

		// 返回按钮 文字颜色
		final int backButtonTextDefaultColor = Color.parseColor(themeBean.getMainTextColor());
		final int backButtonTextProssedColor = Color.parseColor(themeBean.getMainTextColorHighlighted());
		backButton.setTextColor(backButtonTextDefaultColor);
		backButton.setOnTouchListener(new OnTouchListener() {
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					backButton.setTextColor(backButtonTextProssedColor);
					break;
				case MotionEvent.ACTION_UP:
					backButton.setTextColor(backButtonTextDefaultColor);
					break;
				default:
					break;
				}

				return false;
			}
		});

		// 刷新按钮皮肤
		final Drawable refreshButtonProssedDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(getContext(), themeBean.getRefreshButtonHighlightedInBookStore());
		final Drawable refreshButtonDefaultDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(getContext(), themeBean.getRefreshButtonDefaultInBookStore());
		refreshButton.setImageDrawable(refreshButtonDefaultDrawable);
		refreshButton.setOnTouchListener(new OnTouchListener() {
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					refreshButton.setImageDrawable(refreshButtonProssedDrawable);
					break;
				case MotionEvent.ACTION_UP:
					refreshButton.setImageDrawable(refreshButtonDefaultDrawable);
					break;
				default:
					break;
				}

				return false;
			}
		});

		// TODO:查询按钮皮肤
		final Drawable searchButtonProssedDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(getContext(), themeBean.getSerachButtonHighlightedInBookStore());
		final Drawable searchButtonDefaultDrawable = getResources().getDrawable(R.drawable.ic_query);
		searchButton.setImageDrawable(searchButtonDefaultDrawable);
		searchButton.setOnTouchListener(new OnTouchListener() {
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					searchButton.setImageDrawable(searchButtonProssedDrawable);
					break;
				case MotionEvent.ACTION_UP:
					searchButton.setImageDrawable(searchButtonDefaultDrawable);
					break;
				default:
					break;
				}

				return false;
			}
		});
	}
}
