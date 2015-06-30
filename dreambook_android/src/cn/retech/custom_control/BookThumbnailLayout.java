package cn.retech.custom_control;

import java.util.Observable;
import java.util.Observer;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;
import cn.retech.activity.R;
import cn.retech.domainbean_model.book.Book;
import cn.retech.global_data_cache.GlobalDataCacheForMemorySingleton;

import com.nostra13.universalimageloader.core.ImageLoader;

/**
 * 缩略图控件
 */
public class BookThumbnailLayout extends FrameLayout implements Observer {
	private ImageView bookFaceImageView;
	private BookThumbnailProgressView bookThumbnailProgressView;

	public BookThumbnailLayout(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		init();
	}

	public BookThumbnailLayout(Context context, AttributeSet attrs) {
		super(context, attrs);
		init();
	}

	public BookThumbnailLayout(Context context) {
		super(context);
		init();
	}

	// 与Book对象绑定,除了设置缩略图之外,还需要建立观察者模式
	public void bind(BookShelfBookCell bookShelfBookCell) {
		// 绑定观察者模式
		Book book = GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList().bookByContentID(
				bookShelfBookCell.getBookFile().getContentID());
		book.addObserver(this);

		// 设置缩略图
		ImageLoader.getInstance().displayImage(bookShelfBookCell.getThumbnail(), bookFaceImageView);

		update(book, null);
	}

	private void init() {
		LayoutInflater layoutInflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		layoutInflater.inflate(R.layout.book_thumbnail_layout, this);

		bookFaceImageView = (ImageView) findViewById(R.id.thumbnail_imageView);
		bookThumbnailProgressView = (BookThumbnailProgressView) findViewById(R.id.thumbnail_progress);
		bookThumbnailProgressView.setLayerType(View.LAYER_TYPE_HARDWARE, null);
	}

	@Override
	public void update(Observable observable, Object data) {
		Book book = (Book) observable;

		float downloadProgress = book.getDownloadProgress();
		bookThumbnailProgressView.setBookDownLoadPer(downloadProgress);
		bookThumbnailProgressView.invalidate();

		switch (book.getState()) {
		case kBookStateEnum_Installed:
			bookThumbnailProgressView.setLayerType(View.LAYER_TYPE_NONE, null);
			bookThumbnailProgressView.setVisibility(View.GONE);

			break;
		default:
			break;
		}
	}
}
