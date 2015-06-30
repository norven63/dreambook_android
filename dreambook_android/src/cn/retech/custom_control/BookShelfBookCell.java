package cn.retech.custom_control;

import java.util.Observable;
import java.util.Observer;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import cn.retech.activity.R;
import cn.retech.domainbean_model.book.Book;
import cn.retech.domainbean_model.book.BookFile;
import cn.retech.global_data_cache.GlobalDataCacheForMemorySingleton;

import com.nostra13.universalimageloader.core.ImageLoader;

public class BookShelfBookCell extends RelativeLayout implements Observer, DragView {

	private final String TAG = this.getClass().getSimpleName();
	private final Context mContext;

	// 书籍文件对象
	private BookFile bookFile;
	// 书籍名称
	private final TextView bookname_textView;
	// 书籍图片
	private final ImageView book_imageView;
	private final View view;
	// 滚动条以及半透明背景
	private final View translucent_image_layout;
	// 滚动进度条控件
	private final CircleProgressBar circleProgressBar;
	// 右下角的三角图片
	private final ImageView sjImageView;
	// 封面的URL
	private String thumbnail;

	public BookShelfBookCell(Context context) {
		super(context);
		mContext = context;
		final LayoutInflater inflaterInstance = LayoutInflater.from(mContext);
		view = inflaterInstance.inflate(R.layout.book_shelf_bookcell_layout, this);
		bookname_textView = (TextView) view.findViewById(R.id.bookname_textView);
		book_imageView = (ImageView) view.findViewById(R.id.book_imageView);
		// 滚动条以及半透明背景
		translucent_image_layout = view.findViewById(R.id.translucent_image_layout);
		// 滚动进度条控件
		circleProgressBar = (CircleProgressBar) view.findViewById(R.id.circle_progressBar);
		sjImageView = (ImageView) view.findViewById(R.id.sj_book_imageView);
	}

	public void bind(BookFile bookFile) {
		this.bookFile = bookFile;

		Book book = GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList().bookByContentID(bookFile.getContentID());
		thumbnail = book.getInfo().getThumbnail();
		book.addObserver(this);
		ImageLoader.getInstance().displayImage(book.getInfo().getThumbnail(), book_imageView);
		bookname_textView.setText(book.getInfo().getName());

		updateFunctionButtonUIWithBookObject(book);
	}

	/**
	 * @return the thumbnail
	 */
	public String getThumbnail() {
		return thumbnail;
	}

	@Override
	public void update(Observable observable, Object data) {
		final Book book = (Book) observable;
		Book.ObserverEnum observerEnum = (Book.ObserverEnum) data;
		switch (observerEnum) {
		case kBookDownloadProgress:
			translucent_image_layout.setVisibility(View.VISIBLE);
			circleProgressBar.setProgressNotInUiThread(book.getDownloadProgress());
			break;
		case kBookState:
			updateFunctionButtonUIWithBookObject(book);
			break;
		default:
			break;
		}

		// 刷新文件夹封面
		BookFolderLayout folderLayout = (BookFolderLayout) getTag();
		if (folderLayout != null) {
			folderLayout.updateFrontCover();
		}
	}

	private void updateFunctionButtonUIWithBookObject(final Book book) {
		switch (book.getState()) {

		// 未付费(只针对收费的书籍, 如果是免费的书籍, 会直接到下一个状态.
		case kBookStateEnum_Unpaid:
			break;
		// 支付中....
		case kBookStateEnum_Paiding:
			break;
		// 已付费(已付费的书籍可以直接下载了)
		case kBookStateEnum_Paid:
			break;
		// 免费书籍
		case kBookStateEnum_Free:

			break;
		// 下载中
		case kBookStateEnum_Downloading:
			// DebugLog.i(TAG, "下载。。。");
			translucent_image_layout.setVisibility(View.VISIBLE);
			circleProgressBar.setVisibility(View.VISIBLE);
			sjImageView.setVisibility(View.VISIBLE);
			circleProgressBar.setShowViewTypeEnum(CircleProgressBar.ShowViewTypeEnum.Download_Status);
			break;
		// 暂停(也就是未下载完成, 可以进行断点续传)
		case kBookStateEnum_Pause:
			// DebugLog.i(TAG, "暂停。。。");
			circleProgressBar.setVisibility(View.VISIBLE);
			sjImageView.setVisibility(View.VISIBLE);
			translucent_image_layout.setVisibility(View.VISIBLE);
			circleProgressBar.setShowViewTypeEnum(CircleProgressBar.ShowViewTypeEnum.Suspended_state);
			break;
		// 未安装(已经下载完成, 还未完成安装)
		case kBookStateEnum_NotInstalled:
			circleProgressBar.setVisibility(View.GONE);
			sjImageView.setVisibility(View.GONE);
			translucent_image_layout.setVisibility(View.VISIBLE);
			if (book.isFromSharedDirectory()) {
				sjImageView.setBackgroundDrawable(getResources().getDrawable(R.drawable.sj_z));
				sjImageView.setVisibility(VISIBLE);
				bookname_textView.setText(book.getInfo().getName());
			}
			break;

		// 解压书籍zip资源包中....
		case kBookStateEnum_Unziping:
			// DebugLog.i(TAG, "解压中。。。");
			translucent_image_layout.setVisibility(View.VISIBLE);
			sjImageView.setVisibility(GONE);
			circleProgressBar.setVisibility(GONE);
			break;

		// 已安装(已经解压开的书籍, 可以正常阅读了)
		case kBookStateEnum_Installed:
			translucent_image_layout.setVisibility(View.GONE);
			circleProgressBar.setVisibility(View.GONE);
			sjImageView.setVisibility(View.GONE);
			bookname_textView.setText(book.getInfo().getName());
			break;
		// 有可以更新的内容
		case kBookStateEnum_Update:
			break;
		// 等待下载
		case kBookStateEnum_WaitForDownload:
			// 正在获取书籍下载地址
		case kBookStateEnum_GetBookDownloadUrl:
			// DebugLog.e(TAG, "kBookStateEnum_GetBookDownloadUrl");
			circleProgressBar.setShowViewTypeEnum(CircleProgressBar.ShowViewTypeEnum.Networking_status);
			translucent_image_layout.setVisibility(View.VISIBLE);
			sjImageView.setVisibility(View.VISIBLE);
			circleProgressBar.setVisibility(View.VISIBLE);
			break;
		default:
			break;
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

	@Override
	public BookFile getBookFile() {
		return bookFile;
	}
}
