package cn.retech.custom_control;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.TextView;
import cn.retech.activity.R;
import cn.retech.domainbean_model.book.Book;
import cn.retech.domainbean_model.book.Book.BookStateEnum;
import cn.retech.domainbean_model.book.BookFile;
import cn.retech.global_data_cache.GlobalDataCacheForMemorySingleton;
import cn.retech.theme_manage.ThemeBean;
import cn.retech.theme_manage.ThemeManagerSingleton;
import cn.retech.theme_manage.ThemeModel.ThemeBuilder;

import com.google.common.collect.Lists;

public class BookFolderLayout extends FrameLayout implements DragView {
	private final ArrayList<View> childViews = Lists.newArrayList();
	private MyGridLayout gridLayout;
	private TextView folderName;
	private BookFile bookFile;
	private View downloadingView;// 下载中指示器,一个gif动画
	private TextView textView;// 书籍数目

	public BookFolderLayout(Context context) {
		super(context);

		init();
	}

	public BookFolderLayout(Context context, AttributeSet attrs) {
		super(context, attrs);

		init();
	}

	public BookFolderLayout(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);

		init();
	}

	/**
	 * 
	 * @param index
	 *            小于0则顺序插入(插入至最末尾);大于等于0则按指定索引值(index)插入
	 * @param childView
	 * @return
	 */
	public boolean addChildView(int index, View childView) {
		if (childViews.contains(childView)) {
			return false;
		}

		if (null == childView.getTag()) {
			// 结构层操作
			// tag为null说明此view在添加至文件夹内之前是处于桌面上的,所以需要将其从根文件夹对象中删除
			GlobalDataCacheForMemorySingleton.getInstance.getRootDirectory().getListFiles().remove(((DragView) childView).getBookFile());
		} else {
			((BookFolderLayout) childView.getTag()).removeChildView(childView, true);
		}

		if (null != childView.getParent()) {
			((ViewGroup) childView.getParent()).removeView(childView);
		}

		if (index < 0) {
			childViews.add(childView);
		} else {
			childViews.add(index, childView);
		}

		// 生成缩略图控件
		BookThumbnailLayout bookThumbnailLayout = new BookThumbnailLayout(getContext());
		bookThumbnailLayout.bind((BookShelfBookCell) childView);
		childView.setTag(R.id.bookThumbnailView, bookThumbnailLayout);

		MyGridLayout.LayoutParams layoutParams = new MyGridLayout.LayoutParams();
		layoutParams.width = (int) getResources().getDimension(R.dimen.book_cell_width) / 3;
		layoutParams.height = (int) getResources().getDimension(R.dimen.book_image_height) / 3;
		layoutParams.topMargin = 4;
		layoutParams.leftMargin = 4;
		layoutParams.rightMargin = 4;
		layoutParams.bottomMargin = 4;

		// 顺序插入时,封面只显示前4本书
		if (index < 0) {
			if (gridLayout.getChildCount() < 4) {
				gridLayout.addView(bookThumbnailLayout, layoutParams);
			}
		}

		// 指定插入时,若索引值小于4则需将封面上多余的一张缩略图(即原第4张)删除
		else if (index < 4) {
			if (gridLayout.getChildCount() == 4) {
				gridLayout.removeViewAt(3);
			}
			gridLayout.addView(bookThumbnailLayout, index, layoutParams);
		}

		childView.setTag(this);

		updateFrontCover();// 刷新封面

		return true;
	}

	public boolean addChildView(View childView) {
		return addChildView(-1, childView);
	}

	public Iterator<View> getChildViewsIterator() {
		return childViews.iterator();
	}

	public View getChildViewByIndex(int index) {
		return childViews.size() == 0 ? null : childViews.get(index);
	}

	public int getChildViewsCount() {
		return childViews.size();
	}

	/**
	 * @param childView
	 *            要删除的视图
	 * @param isDeleteFolder
	 *            是否删除只剩一本书的文件夹
	 */
	public void removeChildView(View childView, boolean isDeleteFolder) {
		childView.setTag(null);

		if (childViews.contains(childView)) {
			childViews.remove(childView);
		}

		View bookThumbnailView = (View) childView.getTag(R.id.bookThumbnailView);
		int bookFaceIndex = gridLayout.indexOfChild(bookThumbnailView);
		gridLayout.removeView(bookThumbnailView);// 删除文件夹封面上显示的书籍缩略图

		/**
		 * 如果删除的是前4本书中的其中一本，且删除后文件夹内至少还有4本书，则需要更新文件夹封面，将第4本书的缩略图进行补位
		 */
		if (bookFaceIndex >= 0 && childViews.size() >= 4) {
			View bookFaceImageView = (View) childViews.get(3).getTag(R.id.bookThumbnailView);
			if (bookFaceImageView.getParent() != null) {
				((ViewGroup) bookFaceImageView.getParent()).removeView(bookFaceImageView);
			}

			MyGridLayout.LayoutParams layoutParams = new MyGridLayout.LayoutParams();
			layoutParams.width = (int) getResources().getDimension(R.dimen.book_cell_width) / 3;
			layoutParams.height = (int) getResources().getDimension(R.dimen.book_image_height) / 3;
			layoutParams.topMargin = 4;
			layoutParams.leftMargin = 4;
			layoutParams.rightMargin = 4;
			layoutParams.bottomMargin = 4;

			gridLayout.addView(bookFaceImageView, 3, layoutParams);
		}

		if (null != childView.getParent()) {
			((ViewGroup) childView.getParent()).removeView(childView);
		}

		// 结构层操作
		bookFile.getListFiles().remove(((DragView) childView).getBookFile());

		// 如果不需要自动删除只剩一本书籍的文件夹则在此处直接返回,终止方法体
		if (!isDeleteFolder) {
			return;
		}

		updateFrontCover();// 刷新封面

		/**
		 * 该文件夹没有书籍时,则删除该文件夹
		 */
		if (0 == childViews.size()) {
			ViewGroup viewGroup = (ViewGroup) this.getParent();
			BookFile rootBookFile = GlobalDataCacheForMemorySingleton.getInstance.getRootDirectory();
			viewGroup.removeView(this);
			// 结构层操作
			rootBookFile.getListFiles().remove(bookFile);
		}
	}

	public void setName(String folderName) {
		this.folderName.setText(folderName);
	}

	private void init() {
		LayoutInflater layoutInflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		layoutInflater.inflate(R.layout.book_folder_layout, this);

		folderName = (TextView) findViewById(R.id.view_cell_bookname_textView);
		downloadingView = findViewById(R.id.downloading_view);

		ThemeBean themeBean = ThemeManagerSingleton.getInstance.getNewThemeModel().getThemeBean();
		Drawable numberTextBackDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(getContext(), themeBean.getBookCountBackgroundInFolderCell());
		textView = (TextView) findViewById(R.id.book_number_textView);
		textView.setBackgroundDrawable(numberTextBackDrawable);
		// TODO
		try {
			byte imgdata[] = ThemeBuilder.InputStreamToByte(ThemeBuilder.getInputStreamAccordingScreenDensity(getContext(), "downloading"));
			((GifMovieView) downloadingView).setMovieByteArray(imgdata);
		} catch (IOException e) {
			e.printStackTrace();
		}

		gridLayout = (MyGridLayout) findViewById(R.id.book_folder_gridLayout);
		gridLayout.setColumnCount(2);
		gridLayout.setRowCount(2);
	}

	@Override
	protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
		super.onLayout(changed, left, top, right, bottom);

		View gridLayout = findViewById(R.id.book_folder_gridLayout);

		downloadingView.setX(gridLayout.getX() + gridLayout.getWidth() / 3);
		downloadingView.setY(gridLayout.getY() + gridLayout.getHeight() - 20);

		textView.setText(childViews.size() + "");
		textView.setX(gridLayout.getWidth() - textView.getWidth());
		textView.setY(gridLayout.getHeight() - 33);
	}

	public void updateFrontCover() {
		downloadingView.setVisibility(INVISIBLE);
		for (View viewItem : childViews) {
			Book book = GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList().bookByContentID(((DragView) viewItem).getBookFile().getContentID());
			if (book != null && book.getState() == BookStateEnum.kBookStateEnum_Downloading) {
				downloadingView.setVisibility(VISIBLE);
			}
		}
	}

	@Override
	public void hiddenSelf() {
		findViewById(R.id.cellWhite).setVisibility(VISIBLE);
	}

	@Override
	public void showSelf() {
		findViewById(R.id.cellWhite).setVisibility(INVISIBLE);
	}

	public void setBookFile(BookFile bookFile) {
		this.bookFile = bookFile;

		folderName.setText(bookFile.getDirectoryName());
	}

	@Override
	public BookFile getBookFile() {
		return bookFile;
	}
}
