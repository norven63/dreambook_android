package cn.retech.custom_control;

import java.util.List;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.ArrayAdapter;
import android.widget.AutoCompleteTextView;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.SearchView.OnQueryTextListener;
import cn.retech.activity.R;
import cn.retech.domainbean_model.book.Book;
import cn.retech.domainbean_model.book.BookList;
import cn.retech.global_data_cache.GlobalDataCacheForMemorySingleton;
import cn.retech.theme_manage.ThemeBean;
import cn.retech.theme_manage.ThemeManagerSingleton;
import cn.retech.theme_manage.ThemeModel.ThemeBuilder;

public class SearchLayout extends RelativeLayout {
	public interface OnCloseListener {
		public void onCancel();

		public void onClose();
	}

	private RelativeLayout mainLayout;
	private AutoCompleteTextView searchEditText;
	private ImageView leftImageView;
	private ImageButton cancelButton;
	private OnCloseListener onCloseListener;
	private OnQueryTextListener onQueryTextListener;

	public SearchLayout(Context context) {
		super(context);
		init(context);
	}

	public SearchLayout(Context context, AttributeSet attrs) {
		super(context, attrs);

		init(context);
	}

	public SearchLayout(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);

		init(context);
	}

	public void closeSearch() {
		if (onCloseListener != null) {
			onCloseListener.onClose();
		}
	}

	/**
	 * 设置搜索控件的图片,即自定义风格
	 * 
	 * @param imageViewIconId
	 *            放大镜icon的资源id
	 * @param editTextIconId
	 *            输入框背景图的资源id
	 * @param imageButtonIconId
	 *            取消按钮背景图的资源id
	 */
	public void setButtonsIcon(int imageViewIconId, int editTextIconId, int imageButtonIconId) {
		findViewById(R.id.search_left_imageView).setBackgroundDrawable(getResources().getDrawable(imageViewIconId));

		searchEditText.setBackgroundDrawable(getResources().getDrawable(editTextIconId));

		cancelButton.setBackgroundDrawable(getResources().getDrawable(imageButtonIconId));
	}

	public void setOnCloseListener(OnCloseListener onCloseListener) {
		this.onCloseListener = onCloseListener;
	}

	public void setOnQueryTextListener(OnQueryTextListener onQueryTextListener) {
		this.onQueryTextListener = onQueryTextListener;
	}

	/**
	 * 设置搜索建议
	 * 
	 * @param suggestions
	 *            建议数组
	 */
	@SuppressWarnings("unchecked")
	private void setSuggestions() {
		// 设置搜索建议
		BookList localBookList = GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList();
		List<Book> books = localBookList.getShallowCopyBookList();
		String[] bookNames = new String[books.size()];
		for (int i = 0; i < bookNames.length; i++) {
			bookNames[i] = books.get(i).getInfo().getName();
		}
		ArrayAdapter adapter = new ArrayAdapter(getContext(), android.R.layout.simple_list_item_1, bookNames);
		searchEditText.setAdapter(adapter);
	}

	/**
	 * 显示或者隐藏输入法
	 * 
	 * @param isShow
	 *            标记是否显示或隐藏
	 */
	public void showOrHidenInput(boolean isShow) {
		InputMethodManager imm = (InputMethodManager) getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
		if (isShow) {
			searchEditText.requestFocus();
			imm.showSoftInput(searchEditText, InputMethodManager.SHOW_FORCED);
		} else {
			imm.hideSoftInputFromWindow(searchEditText.getWindowToken(), 0);
			searchEditText.setText("");
		}
	}

	private void init(Context context) {
		LayoutInflater layoutInflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		layoutInflater.inflate(R.layout.search_layout, this);

		mainLayout = (RelativeLayout) findViewById(R.id.search_mainLayout);
		leftImageView = (ImageView) findViewById(R.id.search_left_imageView);

		searchEditText = (AutoCompleteTextView) findViewById(R.id.search_editText);
		searchEditText.setOnKeyListener(new OnKeyListener() {
			@Override
			/**
			 * 捕获输入法的KEYCODE_ENTER事件,回调onQueryTextListener.onQueryTextSubmit()方法
			 */
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				if (event.getKeyCode() == KeyEvent.KEYCODE_ENTER) {
					if (null != onQueryTextListener) {
						onQueryTextListener.onQueryTextSubmit(((AutoCompleteTextView) v).getEditableText().toString());
					}

					return true;
				} else {
					return false;
				}
			}
		});

		cancelButton = (ImageButton) findViewById(R.id.cancel_button_ImageButton);
		cancelButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (onCloseListener != null) {
					onCloseListener.onCancel();

					if (searchEditText.getText().toString().equals("")) {
						showOrHidenInput(false);
						onCloseListener.onClose();
					} else {
						searchEditText.setText("");
					}
				}
			}
		});

		/*
		 * 皮肤设置
		 */
		ThemeBean themeBean = ThemeManagerSingleton.getInstance.getNewThemeModel().getThemeBean();
		ThemeBuilder.replaceControlsBackgroung(themeBean.getMainColor(), mainLayout);
		leftImageView.setBackgroundDrawable(ThemeBuilder.getBitmapAccordingScreenDensity(getContext(), themeBean.getSearchButtonDefaultInBookStoreLeft()));
		searchEditText.setBackgroundDrawable(ThemeBuilder.getBitmapAccordingScreenDensity(getContext(), themeBean.getSearchButtonDefaultInBookStoreMiddle()));

		final Drawable cancelButtonProssedDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(getContext(), themeBean.getSearchButtonHighlightedInBookStoreRight());
		final Drawable cancelButtonDefaultDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(getContext(), themeBean.getSearchButtonDefaultInBookStoreRight());
		cancelButton.setBackgroundDrawable(cancelButtonDefaultDrawable);
		cancelButton.setOnTouchListener(new OnTouchListener() {
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					cancelButton.setBackgroundDrawable(cancelButtonProssedDrawable);
					break;
				case MotionEvent.ACTION_UP:
					cancelButton.setBackgroundDrawable(cancelButtonDefaultDrawable);
					break;
				default:
					break;
				}

				return false;
			}
		});
	}
}
