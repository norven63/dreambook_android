package cn.retech.activity;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import android.animation.Animator;
import android.animation.Animator.AnimatorListener;
import android.animation.ObjectAnimator;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.app.FragmentTransaction;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.view.ViewPager.OnPageChangeListener;
import android.view.KeyEvent;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import cn.retech.activity.BookSearchFragment.SearchTypeEnum;
import cn.retech.adapter.MyFragmentPagerAdapter;
import cn.retech.custom_control.MyViewPaper;
import cn.retech.custom_control.MyViewPaper.OnScrollListener;
import cn.retech.custom_control.TabNavigatorLayout;
import cn.retech.custom_control.TabNavigatorLayout.OnScrollFullListener;
import cn.retech.custom_control.TabNavigatorLayout.OnTabChangeListener;
import cn.retech.custom_control.TitleBarForBookStore;
import cn.retech.domainbean_model.book.Book;
import cn.retech.domainbean_model.book.BookFile;
import cn.retech.domainbean_model.book_categories.BookCategoriesNetRequestBean;
import cn.retech.domainbean_model.book_categories.BookCategoriesNetRespondBean;
import cn.retech.domainbean_model.book_categories.BookCategory;
import cn.retech.domainbean_model.login.LoginNetRequestBean;
import cn.retech.domainbean_model.login.LoginNetRespondBean;
import cn.retech.global_data_cache.GlobalDataCacheForMemorySingleton;
import cn.retech.global_data_cache.GlobalDataCacheForNeedSaveToFileSystem;
import cn.retech.my_network_engine.IDomainBeanAsyncHttpResponseListener;
import cn.retech.my_network_engine.INetRequestHandle;
import cn.retech.my_network_engine.SimpleNetworkEngineSingleton;
import cn.retech.my_network_engine.net_error_handle.MyNetErrorCodeEnum;
import cn.retech.my_network_engine.net_error_handle.MyNetRequestErrorBean;
import cn.retech.theme_manage.ThemeBean;
import cn.retech.theme_manage.ThemeManagerSingleton;
import cn.retech.theme_manage.ThemeModel.ThemeBuilder;
import cn.retech.toolutils.DebugLog;
import cn.retech.toolutils.GlobalConstantForThisProject;

import com.umeng.analytics.MobclickAgent;

public class BookStoreActivity extends FragmentActivity {
	private final String TAG = this.getClass().getSimpleName();

	private INetRequestHandle netRequestHandleForLogin;
	private INetRequestHandle netRequestHandleForBookCategories;

	private final ThemeBean themeBean = ThemeManagerSingleton.getInstance.getNewThemeModel().getThemeBean();
	private TextView lineTextView;
	private TitleBarForBookStore titleBar;
	private TabNavigatorLayout tabNavigatorLayout;
	private ImageView shadowSide;// tab bar 左右阴影图片, 所以tab bar还可以滑动
	private final List<BookCategory> bookCategories = new ArrayList<BookCategory>();
	private MyFragmentPagerAdapter viewPaperAdapter;
	private MyViewPaper viewPager;
	private BookSearchFragment searchFragment;// 书籍搜索碎片
	private boolean isSearching = false;// 是否正在执行搜索

	// 绑定账号
	private final LoginNetRespondBean bindAccount = new LoginNetRespondBean();

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);

		int eachPageButtonNum = Integer.parseInt(getResources().getString(R.string.tabnavigation_button_number));
		if (bookCategories.size() != 0 && bookCategories.size() < eachPageButtonNum) {
			eachPageButtonNum = bookCategories.size();

			shadowSide.setVisibility(View.GONE);
		}
		tabNavigatorLayout.showCategory(bookCategories, eachPageButtonNum);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		// 获取标识位，用来设置是否支持屏幕翻转(手机不支持屏幕翻转)
		if (!getResources().getBoolean(R.bool.isLarge)) {
			setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
		}
		setContentView(R.layout.activity_book_store);

		lineTextView = (TextView) findViewById(R.id.line_textView);

		bindAccount.setUser(GlobalDataCacheForMemorySingleton.getInstance.getUsernameForLastSuccessfulLogon());
		bindAccount.setPassword(GlobalDataCacheForMemorySingleton.getInstance.getPasswordForLastSuccessfulLogon());

		titleBar = (TitleBarForBookStore) findViewById(R.id.titlebar);
		String titleNameString = "";
		if (GlobalDataCacheForMemorySingleton.getInstance.getUsernameForLastSuccessfulLogon().equals(GlobalConstantForThisProject.PUBLIC_ACCOUNT_USERNAME)) {
			titleNameString = "今日书院";
		} else {
			titleNameString = "企业书院";
		}

		titleBar.setTitle(titleNameString);
		titleBar.setOnButtonClickListener(new TitleBarForBookStore.OnButtonClickListener() {
			@Override
			public void OnSearchButtonClicked() {
				isSearching = true;
				findViewById(R.id.search_ImageView).setClickable(false);

				searchFragment = new BookSearchFragment();
				Bundle bundle = new Bundle();
				bundle.putSerializable("type", SearchTypeEnum.BookStore);
				bundle.putSerializable("bindAccount", bindAccount);
				searchFragment.setArguments(bundle);
				searchFragment.setOnHandlerBookListener(new BookSearchFragment.OnHandlerListener() {
					@Override
					public void onClose() {
						closeSearch();
					}

					@Override
					public void onBookClicked(Book book) {
						bookClickedHandler(book);
					}
				});

				final FragmentTransaction fragmentTransaction = getFragmentManager().beginTransaction();
				fragmentTransaction.add(R.id.search_view_layout, searchFragment);
				fragmentTransaction.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE);
				fragmentTransaction.commit();

				View view = findViewById(R.id.content_parent_store_layout);
				ObjectAnimator objectAnimator = ObjectAnimator.ofFloat(view, "translationY", -(findViewById(R.id.titlebar).getHeight() + 3));
				objectAnimator.start();
			}

			@Override
			public void OnRefreshButtonClicked() {
				if (viewPaperAdapter != null) {
					BookStoreChannelFragment fragment = (BookStoreChannelFragment) viewPaperAdapter.getItem(viewPager.getCurrentItem());
					fragment.refresh();
				}
			}

			@Override
			public void OnBackButtonClicked() {
				Intent intent = new Intent();
				intent.setClass(BookStoreActivity.this, BookShelfActivity.class);
				startActivity(intent);
			}
		});

		shadowSide = (ImageView) findViewById(R.id.shadow_side_imageView);
		tabNavigatorLayout = (TabNavigatorLayout) findViewById(R.id.tabNavigation);
		tabNavigatorLayout.setOnTabChangeListener(new OnTabChangeListener() {
			@Override
			public void onTabChange(int postion) {
				viewPager.setCurrentItem(postion);
			}
		});
		tabNavigatorLayout.setOnScrollFullLeftListener(new OnScrollFullListener() {
			@Override
			public void onScrollFullLeft() {
				int eachPageButtonNum = Integer.parseInt(getResources().getString(R.string.tabnavigation_button_number));
				if (bookCategories.size() != 0 && bookCategories.size() < eachPageButtonNum) {
					shadowSide.setVisibility(View.GONE);
				} else {
					shadowSide.setVisibility(View.VISIBLE);
					Drawable shadowSideFullLeftDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(BookStoreActivity.this, themeBean.getShadowSideFullLeft());
					shadowSide.setBackgroundDrawable(shadowSideFullLeftDrawable);
				}
			}

			@Override
			public void onScrollFullRight() {
				shadowSide.setVisibility(View.VISIBLE);
				Drawable shadowSideFullRightDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(BookStoreActivity.this, themeBean.getShadowSideFullRight());
				shadowSide.setBackgroundDrawable(shadowSideFullRightDrawable);
			}

			@Override
			public void onScrolling() {
				shadowSide.setVisibility(View.VISIBLE);
				Drawable shadowSideDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(BookStoreActivity.this, themeBean.getShadowSide());
				shadowSide.setBackgroundDrawable(shadowSideDrawable);
			}
		});
		viewPager = (MyViewPaper) findViewById(R.id.book_list_viewPager);
		viewPager.setOnPageChangeListener(new OnPageChangeListener() {
			@Override
			public void onPageScrolled(int arg0, float arg1, int arg2) {
			}

			@Override
			public void onPageScrollStateChanged(int arg0) {
			}

			@Override
			public void onPageSelected(int arg0) {
				tabNavigatorLayout.setCurrentItem(arg0);
			}
		});
		viewPager.setOnScrollListener(new OnScrollListener() {
			@Override
			public void onScroll(int l, int oldl, int width) {
				tabNavigatorLayout.scrollKit(l, oldl, width);
			}
		});

		// 首先登录
		requestLogin(GlobalDataCacheForMemorySingleton.getInstance.getUsernameForLastSuccessfulLogon(), GlobalDataCacheForMemorySingleton.getInstance.getPasswordForLastSuccessfulLogon());

		/*
		 * 皮肤设置
		 */
		lineTextView.setBackgroundColor(Color.parseColor(themeBean.getTabHostLineColorInBookStore()));
		ThemeBuilder.replaceControlsBackgroung(themeBean.getMainColor(), titleBar);
	}

	private void closeSearch() {
		isSearching = false;

		final FragmentTransaction fragmentTransaction = getFragmentManager().beginTransaction();
		fragmentTransaction.remove(searchFragment);
		fragmentTransaction.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE);
		fragmentTransaction.commit();

		View view = findViewById(R.id.content_parent_store_layout);
		ObjectAnimator objectAnimator = ObjectAnimator.ofFloat(view, "translationY", 0f);
		objectAnimator.addListener(new AnimatorListener() {
			@Override
			public void onAnimationStart(Animator animation) {
			}

			@Override
			public void onAnimationRepeat(Animator animation) {
			}

			@Override
			public void onAnimationEnd(Animator animation) {
				findViewById(R.id.search_ImageView).setClickable(true);
			}

			@Override
			public void onAnimationCancel(Animator animation) {
			}
		});
		objectAnimator.start();
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			if (isSearching) {
				closeSearch();
			} else {
				Intent intent = new Intent();
				intent.setClass(this, BookShelfActivity.class);
				startActivity(intent);
			}
		}

		return super.onKeyDown(keyCode, event);
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	@Override
	protected void onPause() {
		super.onPause();

		MobclickAgent.onPause(this);
	}

	@Override
	protected void onRestart() {
		super.onRestart();
	}

	@Override
	protected void onResume() {
		super.onResume();

		MobclickAgent.onResume(this);
	}

	@Override
	protected void onStart() {
		super.onStart();
	}

	@Override
	protected void onStop() {
		super.onStop();

		netRequestHandleForLogin.cancel();
		netRequestHandleForBookCategories.cancel();
	}

	private void requestLogin(final String userID, final String userPassWord) {

		LoginNetRequestBean netRequestBean = new LoginNetRequestBean.Builder(userID, userPassWord).builder();
		netRequestHandleForLogin = SimpleNetworkEngineSingleton.getInstance.requestDomainBean(netRequestBean, new IDomainBeanAsyncHttpResponseListener() {
			@Override
			public void onFailure(MyNetRequestErrorBean error) {
				// 表示需要登录,有可能账户名密码错误这里需要跳转到登录界面
				if (error.getErrorCodeEnum() == MyNetErrorCodeEnum.kNetErrorCodeEnum_Server_NeedLogin) {
					showDialogForLoginFail();
				} else {
					showDialogForNetError();
				}

			}

			@Override
			public void onSuccess(Object respondDomainBean) {
				// 登录成功, 获取最新的书籍分类列表
				requestBookCategories();
				// showDialogForLoginFail();
			}

		});
	}

	/**
	 * 显示 "网络问题提示框" 提示框
	 */
	private void showDialogForNetError() {
		AlertDialog.Builder builder = new Builder(BookStoreActivity.this);
		builder.setTitle("提示");
		builder.setMessage("网络异常,请检查网络!");
		builder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				BookStoreActivity.this.finish();
			}
		});
		builder.setCancelable(false);// 设置不能通过“后退”按钮关闭对话框
		AlertDialog dialog = builder.create();
		dialog.show();
	}

	/**
	 * 显示 "账户问题" 提示框
	 */
	private void showDialogForLoginFail() {
		AlertDialog.Builder builder = new Builder(BookStoreActivity.this);
		builder.setTitle("提示");
		builder.setMessage("账户名/密码错误,请重新登录!");
		builder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				Intent intent = new Intent();
				intent.setClass(BookStoreActivity.this, LoginActivity.class);
				BookStoreActivity.this.startActivity(intent);
				BookStoreActivity.this.finish();

			}
		});
		builder.setCancelable(false);// 设置不能通过“后退”按钮关闭对话框
		AlertDialog dialog = builder.create();
		dialog.show();
	}

	private void requestBookCategories() {
		BookCategoriesNetRequestBean netRequestBean = new BookCategoriesNetRequestBean();
		netRequestHandleForBookCategories = SimpleNetworkEngineSingleton.getInstance.requestDomainBean(netRequestBean, new IDomainBeanAsyncHttpResponseListener() {

			@Override
			public void onFailure(MyNetRequestErrorBean error) {
				DebugLog.e(TAG, "testBookCategories = " + error.getErrorMessage());
			}

			@Override
			public void onSuccess(Object respondDomainBean) {
				// 先清理一下 书籍分类集合
				bookCategories.clear();

				BookCategoriesNetRespondBean respondBean = (BookCategoriesNetRespondBean) respondDomainBean;
				// 书城频道碎片集合
				List<Fragment> channelFragments = new ArrayList<Fragment>();

				for (BookCategory bookCategory : respondBean.getCategories()) {
					// 只显示拥有书籍的分类
					if (bookCategory.getBookcount() > 0) {
						bookCategories.add(bookCategory);

						Bundle bundle = new Bundle();
						bundle.putString("identifier", bookCategory.getIdentifier());
						bundle.putSerializable("bindAccount", bindAccount);
						BookStoreChannelFragment fragment = (BookStoreChannelFragment) Fragment.instantiate(BookStoreActivity.this, BookStoreChannelFragment.class.getName(), bundle);
						fragment.setOnHandlerListener(new BookStoreChannelFragment.OnHandlerListener() {

							@Override
							public void onBookClicked(Book book) {
								bookClickedHandler(book);
							}
						});
						channelFragments.add(fragment);
					}
				}

				// tab bar 中显示的tab 分页数量
				int eachPageButtonNum = Integer.parseInt(getResources().getString(R.string.tabnavigation_button_number));
				if (bookCategories.size() != 0 && bookCategories.size() < eachPageButtonNum) {
					eachPageButtonNum = bookCategories.size();
					shadowSide.setVisibility(View.GONE);
				}
				tabNavigatorLayout.showCategory(bookCategories, eachPageButtonNum);

				//
				viewPaperAdapter = new MyFragmentPagerAdapter(BookStoreActivity.this.getSupportFragmentManager(), channelFragments);
				viewPager.setAdapter(viewPaperAdapter);
				viewPager.setCurrentItem(0);
			}
		});
	}

	/**
	 * 打开一本书籍
	 * 
	 * @param path
	 *            书籍存储路径
	 */
	private void openThisBookByPath(final String path) {
		File tempFile = new File(path);
		if (!tempFile.exists()) {
			Toast.makeText(this, "书籍资源文件已损坏, 请删除书籍文件后重新下载.", Toast.LENGTH_SHORT).show();
			return;
		}

		Intent intent = new Intent(this, ShowBookActivity.class);
		intent.putExtra(ShowBookActivity.EXTRA_ZIP_FILE, path);
		startActivity(intent);
	}

	public void onClickForNothing(View view) {
		// 阻隔点击事件
	}

	private void bookClickedHandler(Book book) {
		switch (book.getState()) {
		case kBookStateEnum_Unpaid: {// 未付费(只针对收费的书籍, 如果是免费的书籍, 会直接到下一个状态.
			AlertDialog.Builder builder = new AlertDialog.Builder(this);
			builder.create();
			builder.setTitle("提示");
			builder.setMessage("暂不支持付费功能，请下载免费书籍");
			builder.setPositiveButton("确认", null);
			builder.show();
		}
			break;
		case kBookStateEnum_Free:// 免费
			String bString = book.bookSaveDirPath();
			bString.charAt(10);
			File bookSaveDir = new File(book.bookSaveDirPath());
			if (!bookSaveDir.exists()) {
				if (!bookSaveDir.mkdir()) {
					Toast.makeText(BookStoreActivity.this, "创建书籍保存文件夹失败!", Toast.LENGTH_SHORT).show();
					return;
				}
			}
			// 一定要先保存书籍目录模型
			BookFile rootBookFile = GlobalDataCacheForMemorySingleton.getInstance.getRootDirectory();
			BookFile newBookFile = BookFile.createFileWithContentID(book.getInfo().getContent_id());
			rootBookFile.getListFiles().add(0, newBookFile);
			GlobalDataCacheForNeedSaveToFileSystem.saveBookFileToFileSystem(rootBookFile);

			GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList().addBook(book);
			book.startDownloadBook();
			// 及时保存发生变化时的本地书籍列表
			GlobalDataCacheForNeedSaveToFileSystem.saveLocalBookToFileSystem(book);
			break;
		case kBookStateEnum_GetBookDownloadUrl:
		case kBookStateEnum_WaitForDownload:
		case kBookStateEnum_Downloading:
			book.stopDownloadBook();
			break;
		case kBookStateEnum_Pause:// 暂停(也就是未下载完成, 可以进行断点续传)
			book.startDownloadBook();
			break;
		case kBookStateEnum_NotInstalled:// 未安装(已经下载完成, 还未完成安装)
			book.unzipBook();
			break;
		case kBookStateEnum_Installed:// 已安装(已经解压开的书籍, 可以正常阅读了)
			openThisBookByPath(book.bookSaveDirPath());
			break;
		case kBookStateEnum_Update:// 有可以更新的内容
			break;
		default:
			break;
		}
	}
}
