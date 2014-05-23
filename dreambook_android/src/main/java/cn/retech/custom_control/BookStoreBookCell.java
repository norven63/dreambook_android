package cn.retech.custom_control;

import java.util.Observable;
import java.util.Observer;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import cn.retech.activity.R;
import cn.retech.domainbean_model.book.Book;
import cn.retech.theme_manage.ThemeManagerSingleton;
import cn.retech.theme_manage.ThemeModel;
import cn.retech.toolutils.ToolsFunctionForThisProgect;

import com.nostra13.universalimageloader.core.ImageLoader;

public class BookStoreBookCell extends RelativeLayout implements Observer {
	private final Context mContext;
	// 书籍名称
	private final TextView bookname_textView;
	// 书籍图片
	private final ImageView book_imageView;
	// 书籍价格
	private final ImageView book_price_imageView;
	// 书籍zip资源包大小, 以byte为单位.
	private final TextView booksize_textView;
	// 书籍作者
	private final TextView author_textView;
	// 滚动条以及半透明背景
	private final View translucent_image_layout;
	// 滚动进度条控件
	private final CircleProgressBar circleProgressBar;
	// 右下角的三角图片
	private final ImageView sjImageView;

	public BookStoreBookCell(Context context) {
		super(context);
		mContext = context;
		final LayoutInflater inflaterInstance = LayoutInflater.from(mContext);
		inflaterInstance.inflate(R.layout.book_store_bookcell_layout, this);
		bookname_textView = (TextView) findViewById(R.id.bookname_textView);
		booksize_textView = (TextView) findViewById(R.id.booksize_textView);
		author_textView = (TextView) findViewById(R.id.author_textView);
		book_imageView = (ImageView) findViewById(R.id.book_imageView);
		book_price_imageView = (ImageView) findViewById(R.id.book_price_imageView);
		// 滚动条以及半透明背景
		translucent_image_layout = findViewById(R.id.translucent_image_layout);
		// 滚动进度条控件
		circleProgressBar = (CircleProgressBar) findViewById(R.id.circle_progressBar);
		sjImageView = (ImageView) findViewById(R.id.sj_book_imageView);

	}

	public void bind(Book book) {
		book.addObserver(this);
		resetUIState(book);

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
	}

	private void resetUIState(Book book) {
		book_imageView.setImageDrawable(getResources().getDrawable(R.drawable.book_bg));
		ImageLoader.getInstance().displayImage(book.getInfo().getThumbnail(), book_imageView);
		booksize_textView.setText(ToolsFunctionForThisProgect.bytesToKbOrMb(Long.parseLong(book.getInfo().getSize())));
		bookname_textView.setText(book.getInfo().getName());
		author_textView.setText(book.getInfo().getAuthor());

		Drawable priceBitmap;
		if (book.getInfo().getPrice().equals("0.0")) {
			ThemeModel themeModel = ThemeManagerSingleton.getInstance.getNewThemeModel();
			priceBitmap = ThemeModel.ThemeBuilder.getBitmapAccordingScreenDensity(mContext, themeModel.getThemeBean().getPriceTag_free());
		} else {
			priceBitmap = ThemeModel.ThemeBuilder.priceTagImageByPriceString(mContext, book.getInfo().getPrice());
		}
		book_price_imageView.setBackgroundDrawable(priceBitmap);

		translucent_image_layout.setVisibility(View.GONE);
		circleProgressBar.setVisibility(View.GONE);
		sjImageView.setVisibility(View.GONE);
		updateFunctionButtonUIWithBookObject(book);
	}

	private void updateFunctionButtonUIWithBookObject(final Book book) {

		switch (book.getState()) {
		// 未付费(只针对收费的书籍, 如果是免费的书籍, 会直接到下一个状态.
		case kBookStateEnum_Unpaid:
			translucent_image_layout.setVisibility(View.GONE);
			circleProgressBar.setVisibility(View.GONE);
			sjImageView.setVisibility(View.GONE);
			book_price_imageView.setVisibility(View.VISIBLE);
			break;
		// 支付中....
		case kBookStateEnum_Paiding:
			break;
		// 已付费(已付费的书籍可以直接下载了)
		case kBookStateEnum_Paid:
			translucent_image_layout.setVisibility(View.GONE);
			circleProgressBar.setVisibility(View.GONE);
			sjImageView.setVisibility(View.GONE);
			book_price_imageView.setVisibility(View.VISIBLE);
			break;
		// 免费书籍
		case kBookStateEnum_Free:
			translucent_image_layout.setVisibility(View.GONE);
			circleProgressBar.setVisibility(View.GONE);
			sjImageView.setVisibility(View.GONE);
			book_price_imageView.setVisibility(View.VISIBLE);

			break;
		// 下载中
		case kBookStateEnum_Downloading:
			// DebugLog.i(TAG, "下载。。。");
			book_price_imageView.setVisibility(View.GONE);
			translucent_image_layout.setVisibility(View.VISIBLE);
			circleProgressBar.setVisibility(View.VISIBLE);
			sjImageView.setVisibility(View.VISIBLE);
			circleProgressBar.setShowViewTypeEnum(CircleProgressBar.ShowViewTypeEnum.Download_Status);
			break;
		// 暂停(也就是未下载完成, 可以进行断点续传)
		case kBookStateEnum_Pause:
			// DebugLog.i(TAG, "暂停。。。");
			book_price_imageView.setVisibility(View.GONE);
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
			bookname_textView.setText("未安装");
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
			translucent_image_layout.setVisibility(View.VISIBLE);
			book_price_imageView.setVisibility(View.GONE);
			sjImageView.setVisibility(View.VISIBLE);
			circleProgressBar.setVisibility(View.VISIBLE);
			circleProgressBar.setShowViewTypeEnum(CircleProgressBar.ShowViewTypeEnum.Networking_status);
			break;
		default:
			break;
		}
	}

}
