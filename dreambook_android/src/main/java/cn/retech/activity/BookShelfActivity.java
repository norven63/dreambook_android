package cn.retech.activity;

import java.io.File;
import java.util.Iterator;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import android.animation.Animator;
import android.animation.Animator.AnimatorListener;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.app.FragmentTransaction;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.view.DragEvent;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnDragListener;
import android.view.View.OnKeyListener;
import android.view.View.OnTouchListener;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;
import cn.retech.activity.BookSearchFragment.SearchTypeEnum;
import cn.retech.adapter.Drag_BookGridViewAdapter;
import cn.retech.custom_control.AnimatorDirector;
import cn.retech.custom_control.AnimatorDirector.AnimatorBuilder;
import cn.retech.custom_control.AutoColumnGridLayout;
import cn.retech.custom_control.BookFolderLayout;
import cn.retech.custom_control.BookShelfBookCell;
import cn.retech.custom_control.CircleProgressObservable;
import cn.retech.custom_control.DragGridLayout;
import cn.retech.custom_control.DragGridLayout.HandleActivityEvent;
import cn.retech.custom_control.DragView;
import cn.retech.custom_control.FolderNameLayout;
import cn.retech.custom_control.ShelfSearchButtonLayout;
import cn.retech.domainbean_model.book.Book;
import cn.retech.domainbean_model.book.BookFile;
import cn.retech.domainbean_model.book.BookList;
import cn.retech.domainbean_model.login.LoginNetRespondBean;
import cn.retech.domainbean_model.update.CheckSoftwareUpdateRequestBean;
import cn.retech.domainbean_model.update.CheckSoftwareUpdateRespondBean;
import cn.retech.global_data_cache.GlobalDataCacheForMemorySingleton;
import cn.retech.global_data_cache.GlobalDataCacheForNeedSaveToFileSystem;
import cn.retech.my_network_engine.IDomainBeanAsyncHttpResponseListener;
import cn.retech.my_network_engine.INetRequestHandle;
import cn.retech.my_network_engine.NetRequestHandleNilObject;
import cn.retech.my_network_engine.SimpleNetworkEngineSingleton;
import cn.retech.my_network_engine.net_error_handle.MyNetRequestErrorBean;
import cn.retech.theme_manage.ThemeBean;
import cn.retech.theme_manage.ThemeManagerSingleton;
import cn.retech.theme_manage.ThemeModel.ThemeBuilder;
import cn.retech.toolutils.DebugLog;
import cn.retech.toolutils.FastDoubleClickTest;
import cn.retech.toolutils.MD5Util;
import cn.retech.toolutils.ToolsFunctionForThisProgect;

import com.umeng.analytics.MobclickAgent;

public class BookShelfActivity extends Activity {
	private final String TAG = this.getClass().getSimpleName();

	// 获取升级信息
	private INetRequestHandle netRequestHandleForUpdate = new NetRequestHandleNilObject();;
	private boolean isSearching = false;// 是否正在执行查询
	private boolean isOpeningFolder = false;// 是否正在打开文件夹
	private RelativeLayout mainLayout;
	private BookSearchFragment searchFragment;// 书籍搜索碎片
	private View searchResultLayout;// 搜索结果层
	private ShelfSearchButtonLayout searchButton;// 搜索按钮
	private TextView publicBookstore;// 书院按钮
	private TextView optionTextView;// 设置按钮
	private DragGridLayout booksGridLayout;// 书架中显示书籍的网格控件
	private LinearLayout folderLayout;
	private FolderNameLayout folderNameLayout;// 文件名
	private EditText folderNameEditText;// 文件名编辑框
	private ImageButton folderCancelButton;// 文件名编辑退出按钮
	private AutoColumnGridLayout folderContentGridLayout;// 放置文件夹的网格控件
	private ImageView deleteImageView;// 删除图标
	// private View toolsBar;// 如果用户登录了企业账户, 就显示这个工具栏, 在此显示 "退出登录" 按钮

	private final Handler handler = new Handler();

	// 网格控件适配器
	private final Drag_BookGridViewAdapter gridViewAdapter = new Drag_BookGridViewAdapter();

	// 判断横竖屏状态
	@Override
	public void onConfigurationChanged(Configuration config) {
		super.onConfigurationChanged(config);
		if (this.getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE) {
			// 横屏显示
		} else if (this.getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT) {
			// 竖屏显示
		}
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			boolean returnValue = true;

			do {
				if (isSearching) {
					closeSearch();

					returnValue = false;
					break;
				}

				if (isOpeningFolder) {
					closeFolder();

					returnValue = false;
					break;
				}

				showDialogForQuiteApp();
			} while (false);

			return returnValue;
		}

		return super.onKeyDown(keyCode, event);
	}

	// 关闭搜索功能的相关代码
	private void closeSearch() {
		isSearching = false;

		// 删除碎片
		FragmentTransaction fragmentTransaction = getFragmentManager().beginTransaction();
		fragmentTransaction.remove(searchFragment);
		fragmentTransaction.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE);
		fragmentTransaction.commit();

		// UI效果
		final View view = findViewById(R.id.content_parent_layout);
		findViewById(R.id.titlebar_layout).setVisibility(View.VISIBLE);
		AnimatorDirector.animate(new AnimatorBuilder().view(findViewById(R.id.titlebar_layout)).y(0).setDuration(600));
		AnimatorDirector.animate(new AnimatorBuilder().view(view).y((Float) view.getTag(R.id.last_y)).setDuration(650));
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		DebugLog.i(TAG, "onCreate");
		super.onCreate(savedInstanceState);
		// 获取标识位，用来设置是否支持屏幕翻转(手机不支持屏幕翻转)
		if (!getResources().getBoolean(R.bool.isLarge)) {
			setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
		}
		setContentView(R.layout.activity_book_shelf);
		ThemeBean themeBean = null;
		if (ThemeManagerSingleton.getInstance.getNewThemeModel() != null) {
			themeBean = ThemeManagerSingleton.getInstance.getNewThemeModel().getThemeBean();
		} else {
			DebugLog.e(TAG, "ThemeManagerSingleton.getInstance.getNewThemeModel()---->null !!!");
		}

		mainLayout = (RelativeLayout) findViewById(R.id.bookShelf_mainLayout);

		checkSoftwareUpdate();

		// 文件名相关布局
		folderNameLayout = (FolderNameLayout) findViewById(R.id.folderName);
		folderNameEditText = folderNameLayout.getNameEditText();
		folderCancelButton = folderNameLayout.getCancelButton();

		folderNameEditText.setOnTouchListener(new OnTouchListener() {
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				if (event.getAction() == MotionEvent.ACTION_DOWN) {
					ThemeBean themeBean = ThemeManagerSingleton.getInstance.getNewThemeModel().getThemeBean();
					Drawable folderNameRightCancelDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(BookShelfActivity.this, themeBean.getFolderNameRightCancel());
					folderCancelButton.setBackgroundDrawable(folderNameRightCancelDrawable);
				}

				return false;
			}
		});
		folderNameEditText.setOnKeyListener(new OnKeyListener() {
			@Override
			/**
			 * 编辑文件夹名称,这里会进行结构层的操作,即序列化结构层文件
			 */
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				if (event.getKeyCode() == KeyEvent.KEYCODE_ENTER) {
					if (folderNameEditText.getText().toString().length() == 0) {
						Toast.makeText(getBaseContext(), "请输入文件名。", Toast.LENGTH_SHORT).show();
					} else {
						String newFolderNmae = folderNameEditText.getText().toString();

						// 数据层操作
						BookFolderLayout currentBookFolderLayout = (BookFolderLayout) folderNameEditText.getTag();
						currentBookFolderLayout.setName(newFolderNmae);
						((DragView) currentBookFolderLayout).getBookFile().setDirectoryName(newFolderNmae);

						GlobalDataCacheForNeedSaveToFileSystem.saveBookFileToFileSystem(GlobalDataCacheForMemorySingleton.getInstance.getRootDirectory());
					}
				}

				return false;
			}
		});

		// 退出编辑文件夹
		folderCancelButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (FastDoubleClickTest.isFastDoubleClick()) {
					return;
				}

				folderNameLayout.requestFocus();
				folderNameLayout.requestFocusFromTouch();

				InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
				imm.hideSoftInputFromWindow(folderNameEditText.getWindowToken(), 0);

				ThemeBean themeBean = ThemeManagerSingleton.getInstance.getNewThemeModel().getThemeBean();
				Drawable folderNameRightDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(BookShelfActivity.this, themeBean.getFolderNameRight());
				folderCancelButton.setBackgroundDrawable(folderNameRightDrawable);
			}
		});

		publicBookstore = (TextView) findViewById(R.id.public_bookstore_textView);
		publicBookstore.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				if (FastDoubleClickTest.isFastDoubleClick()) {
					return;
				}

				// 如果有企业账户处于登录中, 就直接跳转到书城界面, 否则弹出登录提示框
				if (GlobalDataCacheForMemorySingleton.getInstance.isLogged()) {
					gotoBookStoreActivity(GlobalDataCacheForMemorySingleton.getInstance.getUsernameForLastSuccessfulLogon(),
							GlobalDataCacheForMemorySingleton.getInstance.getPasswordForLastSuccessfulLogon());

				} else {
					Intent intent = new Intent();
					intent.setClass(BookShelfActivity.this, OptionActivity.class);
					BookShelfActivity.this.startActivity(intent);
					BookShelfActivity.this.finish();
				}
			}
		});

		optionTextView = (TextView) findViewById(R.id.optionButton_textView);
		optionTextView.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				if (FastDoubleClickTest.isFastDoubleClick()) {
					return;
				}

				Intent intent = new Intent();
				intent.setClass(BookShelfActivity.this, OptionActivity.class);
				BookShelfActivity.this.startActivity(intent);
			}
		});

		booksGridLayout = (DragGridLayout) findViewById(R.id.bookshelf_gridLayout);
		booksGridLayout.setAdapter(gridViewAdapter);
		booksGridLayout.setOnItemClickListener(new OnClickListener() {
			// 点击书籍事件
			@Override
			public void onClick(View view) {
				if (FastDoubleClickTest.isFastDoubleClick()) {
					return;
				}

				BookFile bookFile = ((DragView) view).getBookFile();
				Book book = GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList().bookByContentID(bookFile.getContentID());
				bookClickedHandler(book);
			}
		});
		booksGridLayout.setOnFolderClickListener(new OnClickListener() {
			@Override
			public void onClick(View folderView) {
				if (FastDoubleClickTest.isFastDoubleClick()) {
					return;
				}

				isOpeningFolder = true;

				folderContentGridLayout.removeAllViews();

				folderNameLayout.setFocusable(true);
				folderNameLayout.setFocusableInTouchMode(true);

				folderNameEditText.setText(((BookFolderLayout) folderView).getBookFile().getDirectoryName());
				folderNameEditText.setTag(folderView);

				final BookFolderLayout currentClickFolder = (BookFolderLayout) folderView;
				Iterator<View> iterator = currentClickFolder.getChildViewsIterator();
				while (iterator.hasNext()) {
					View view = iterator.next();
					view.setOnDragListener(new OnDragListener() {
						@Override
						public boolean onDrag(final View listenerView, DragEvent event) {
							final View dragView = (View) event.getLocalState();

							// 如果拖动事件遮蔽自己则直接return
							if (listenerView.equals(dragView)) {
								return false;
							}

							Timer timer = (Timer) listenerView.getTag(R.id.timer);

							switch (event.getAction()) {
							case DragEvent.ACTION_DRAG_ENTERED:
								timer.schedule(new TimerTask() {
									@Override
									public void run() {
										handler.post(new Runnable() {
											@Override
											public void run() {
												ViewGroup viewGroup = (ViewGroup) listenerView.getParent();
												int index = viewGroup.indexOfChild(listenerView);

												currentClickFolder.removeChildView(dragView, false);
												currentClickFolder.addChildView(index, dragView);
												currentClickFolder.getBookFile().getListFiles().add(index, ((DragView) dragView).getBookFile());

												// viewGroup.removeView(dragView);//注释此段代码因为在之上的removeChildView()方法中已有相同代码操作了
												viewGroup.addView(dragView, index);
											}
										});
									}
								}, 250);

								return true;
							case DragEvent.ACTION_DRAG_EXITED:
								// 取消定时器,并重置
								timer.cancel();
								listenerView.setTag(R.id.timer, new Timer());

								return true;
							case DragEvent.ACTION_DROP:
								((DragView) dragView).showSelf();

								// 取消定时器,并重置
								timer.cancel();
								listenerView.setTag(R.id.timer, new Timer());

								return false;
							default:

								return true;
							}
						}
					});

					((DragView) view).showSelf();

					folderContentGridLayout.addView(view);
				}

				// 展现文件夹内容以及阻隔层
				if (folderLayout.getTag(R.id.last_y) == null) {
					folderLayout.setTag(R.id.last_y, folderLayout.getY());// 缓存初始y坐标值
				}
				folderLayout.setVisibility(View.VISIBLE);
				folderLayout.setY((Float) folderLayout.getTag(R.id.last_y));
				findViewById(R.id.middle_layout).setVisibility(View.VISIBLE);
			}
		});
		booksGridLayout.setOnLongClickListenerFromActivity(new HandleActivityEvent() {
			// 长按书籍事件,主要用来触发显示删除图标
			@Override
			public void onViewItemLongClick() {
				/**
				 * 以下代码均为删除图标的展现动画效果,勿费精力阅读
				 */
				if (deleteImageView.getTag(R.id.last_x) == null) {
					deleteImageView.setTag(R.id.last_x, deleteImageView.getX());
					deleteImageView.setTag(R.id.last_y, deleteImageView.getY());
				}

				int parentHeight = ((View) deleteImageView.getParent()).getHeight();
				deleteImageView.setVisibility(View.VISIBLE);
				deleteImageView.setX(-deleteImageView.getWidth());
				deleteImageView.setY(parentHeight + deleteImageView.getHeight());

				AnimatorDirector.animate(new AnimatorBuilder().view(deleteImageView).x((Float) deleteImageView.getTag(R.id.last_x)).y((Float) deleteImageView.getTag(R.id.last_y)));// 删除图标展现动画
			}

			@Override
			public void onHandleEnter() {
				closeFolder();
			}
		});

		// 删除图标
		deleteImageView = (ImageView) findViewById(R.id.delete_imageView);
		deleteImageView.setOnDragListener(new OnDragListener() {
			@Override
			public boolean onDrag(View listenerView, DragEvent event) {
				ThemeBean themeBean = ThemeManagerSingleton.getInstance.getNewThemeModel().getThemeBean();

				final View dragView = (View) event.getLocalState();

				final float lastY = (Float) deleteImageView.getTag(R.id.last_y);// 缓存删除图标的Y坐标

				switch (event.getAction()) {
				case DragEvent.ACTION_DRAG_ENTERED:
					Drawable deleteImageViewPressedDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(BookShelfActivity.this, themeBean.getDeleteButtonHighlightedInBookShelf());
					deleteImageView.setImageDrawable(deleteImageViewPressedDrawable);

					break;
				case DragEvent.ACTION_DRAG_EXITED:
					Drawable deleteImageViewDefaultDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(BookShelfActivity.this, themeBean.getDeleteButtonDefaultInBookShelf());
					deleteImageView.setImageDrawable(deleteImageViewDefaultDrawable);

					break;
				case DragEvent.ACTION_DROP:
					((DragView) dragView).showSelf();

					final BookFile rootBookFile = GlobalDataCacheForMemorySingleton.getInstance.getRootDirectory();
					final BookList localBookList = GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList();

					// 获得当前dragView对应的BookFile对象、book对象以及名称
					final BookFile bookFile = ((DragView) dragView).getBookFile();
					final Book book = localBookList.bookByContentID(bookFile.getContentID());
					String bookName = book != null ? book.getInfo().getName() : bookFile.getDirectoryName();

					// 删除提示框
					AlertDialog.Builder builder = new AlertDialog.Builder(BookShelfActivity.this);
					builder.create();
					builder.setTitle("提示");
					builder.setMessage("是否删除《" + bookName + "》?");
					builder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int which) {
							// 数据层操作
							if (dragView instanceof BookFolderLayout) {
								// 删除文件夹操作,需要迭代删除文件夹里的所有书籍
								for (BookFile bookFileItem : ((DragView) dragView).getBookFile().getListFiles()) {
									Book bookItem = localBookList.bookByContentID(bookFileItem.getContentID());

									switch (bookItem.getState()) {
									case kBookStateEnum_Downloading:// 正在下载中...
										// 正在下载中的书籍需要先暂停
										bookItem.stopDownloadBook();
										break;
									default:
										break;
									}

									localBookList.removeBook(bookItem);
								}

								bookFile.getListFiles().clear();
							} else {
								// 删除单独一本书籍操作
								switch (book.getState()) {
								case kBookStateEnum_Downloading:// 正在下载中...
									// 正在下载中的书籍需要先暂停
									book.stopDownloadBook();
									break;
								default:
									break;
								}

								localBookList.removeBook(book);
							}

							// UI层操作
							if (null == dragView.getTag()) {
								((ViewGroup) dragView.getParent()).removeView(dragView);
								// 结构层操作
								GlobalDataCacheForMemorySingleton.getInstance.getRootDirectory().getListFiles().remove(bookFile);
							} else {
								((BookFolderLayout) dragView.getTag()).removeChildView(dragView, true);
							}

							// 将BookFile添加至队列中异步序列化之
							GlobalDataCacheForNeedSaveToFileSystem.saveBookFileToFileSystem(rootBookFile);
						}
					});

					// 只删除文件夹,保留文件中的书籍操作
					if (dragView instanceof BookFolderLayout) {
						builder.setNeutralButton("保留书籍", new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface dialog, int which) {
								// UI层操作
								ViewGroup rootView = (ViewGroup) dragView.getParent();

								rootView.removeView(dragView);

								Iterator<View> iterator = ((BookFolderLayout) dragView).getChildViewsIterator();
								while (iterator.hasNext()) {
									View view = iterator.next();
									BookShelfBookCell newBookCell = new BookShelfBookCell(MyApplication.getApplication());
									newBookCell.bind(((DragView) view).getBookFile());
									rootView.addView(newBookCell, 0);
								}

								// 结构层操作
								rootBookFile.getListFiles().remove(bookFile);

								for (BookFile bookFileItem : bookFile.getListFiles()) {
									rootBookFile.getListFiles().add(0, bookFileItem);
								}

								// 将BookFile添加至队列中异步序列化之
								GlobalDataCacheForNeedSaveToFileSystem.saveBookFileToFileSystem(rootBookFile);
							}
						});
					}

					builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int which) {
							dialog.cancel();
						}
					});

					builder.show();

					break;
				case DragEvent.ACTION_DRAG_ENDED:
					AnimatorDirector.animate(new AnimatorBuilder().view(deleteImageView).x(-deleteImageView.getWidth()).y(lastY + deleteImageView.getHeight()).setListener(new AnimatorListener() {
						@Override
						public void onAnimationStart(Animator animation) {
						}

						@Override
						public void onAnimationRepeat(Animator animation) {
						}

						@Override
						public void onAnimationEnd(Animator animation) {
							deleteImageView.setVisibility(View.INVISIBLE);
							ThemeBean themeBean = ThemeManagerSingleton.getInstance.getNewThemeModel().getThemeBean();
							Drawable deleteImageViewDefaultDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(BookShelfActivity.this, themeBean.getDeleteButtonDefaultInBookShelf());
							deleteImageView.setImageDrawable(deleteImageViewDefaultDrawable);
						}

						@Override
						public void onAnimationCancel(Animator animation) {
						}
					}));

					// 将BookFile添加至队列中异步序列化之
					GlobalDataCacheForNeedSaveToFileSystem.saveBookFileToFileSystem(GlobalDataCacheForMemorySingleton.getInstance.getRootDirectory());

					break;
				}

				return true;
			}
		});

		folderLayout = (LinearLayout) findViewById(R.id.folder_layout_LinearLayout);

		folderContentGridLayout = (AutoColumnGridLayout) findViewById(R.id.folder_content_GridLayout);
		folderContentGridLayout.calculateParmenters();
		folderContentGridLayout.setOnDragListener(new OnDragListener() {
			@Override
			public boolean onDrag(View v, DragEvent event) {
				switch (event.getAction()) {
				case DragEvent.ACTION_DROP:
					final View dragView = (View) event.getLocalState();
					((DragView) dragView).showSelf();

					float x = dragView.getX();
					float y = dragView.getY();

					float e_x = event.getX() - dragView.getWidth() / 2;
					float e_y = event.getY() - dragView.getHeight() / 2;

					dragView.setX(e_x);
					dragView.setY(e_y);

					AnimatorDirector.animate(new AnimatorBuilder().view(dragView).x(x).y(y).setDuration(450));

					break;
				case DragEvent.ACTION_DRAG_EXITED:
					closeFolder();

					break;
				default:
					break;
				}

				return true;
			}
		});

		// 阻隔层添加点击事件,用以关闭文件夹以及关闭输入法
		findViewById(R.id.middle_layout).setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (FastDoubleClickTest.isFastDoubleClick()) {
					return;
				}

				closeFolder();
			}
		});

		searchResultLayout = findViewById(R.id.search_result_layout);
		// 搜索按钮
		searchButton = (ShelfSearchButtonLayout) findViewById(R.id.search_button);
		searchButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				if (FastDoubleClickTest.isFastDoubleClick()) {
					return;
				}

				isSearching = true;

				Bundle bundle = new Bundle();
				bundle.putSerializable("type", SearchTypeEnum.BookShelf);
				searchFragment = new BookSearchFragment();
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

				// 添加碎片
				FragmentTransaction fragmentTransaction = getFragmentManager().beginTransaction();
				fragmentTransaction.add(R.id.search_result_layout, searchFragment);
				fragmentTransaction.setTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE);
				fragmentTransaction.commit();

				// UI效果
				View contentParent = findViewById(R.id.content_parent_layout);
				if (contentParent.getTag(R.id.last_y) == null) {
					contentParent.setTag(R.id.last_y, contentParent.getY());// 备份中间层的当前Y值
				}
				searchResultLayout.setY(contentParent.getY());
				AnimatorDirector.animate(new AnimatorBuilder().view(searchResultLayout).y(0).setDuration(670));
				AnimatorDirector.animate(new AnimatorBuilder().view(contentParent).y(0).setDuration(670));
				AnimatorDirector.animate(new AnimatorBuilder().view(findViewById(R.id.titlebar_layout)).y(-100).setDuration(700));
			}
		});

	}

	/**
	 * 皮肤设置
	 */
	private void setThemes() {
		ThemeBean themeBean = ThemeManagerSingleton.getInstance.getNewThemeModel().getThemeBean();

		// 查询按钮皮肤
		Drawable searchButtonLeftDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(this, themeBean.getShelfSearchLeft());
		Drawable searchButtonMiddleDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(this, themeBean.getShelfSearchMiddle());
		Drawable searchButtonRightDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(this, themeBean.getShelfSearchRight());
		Drawable searchButtonLeftPressedDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(this, themeBean.getShelfSearchPressedLeft());
		Drawable searchButtonMiddlePressedDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(this, themeBean.getShelfSearchPressedMiddle());
		Drawable searchButtonRightPressedDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(this, themeBean.getShelfSearchPressedRight());

		searchButton.setLeftViewImage(searchButtonLeftDrawable);
		searchButton.setMiddleViewImage(searchButtonMiddleDrawable);
		searchButton.setRightViewImage(searchButtonRightDrawable);
		searchButton.setLeftViewImagePressed(searchButtonLeftPressedDrawable);
		searchButton.setMiddleViewImagePressed(searchButtonMiddlePressedDrawable);
		searchButton.setRightViewImagePressed(searchButtonRightPressedDrawable);

		// 文件名皮肤
		Drawable folderLineLeftDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(this, themeBean.getFolderLineLeft());
		View folderLineLeft = findViewById(R.id.folderLine_left);
		folderLineLeft.setBackgroundDrawable(folderLineLeftDrawable);

		Drawable folderLineMiddleDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(this, themeBean.getFolderLineMiddle());
		View folderLineMiddle = findViewById(R.id.folderLine_middle);
		folderLineMiddle.setBackgroundDrawable(folderLineMiddleDrawable);

		Drawable folderLineRightDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(this, themeBean.getFolderLineRight());
		View folderLineRight = findViewById(R.id.folderLine_right);
		folderLineRight.setBackgroundDrawable(folderLineRightDrawable);

		Drawable folderNameLeftDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(this, themeBean.getFolderNameLeft());
		folderNameLayout.setLeftViewImage(folderNameLeftDrawable);

		Drawable folderNameMiddleDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(this, themeBean.getFolderNameMiddle());
		folderNameLayout.setMiddleViewImage(folderNameMiddleDrawable);

		Drawable folderNameRightDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(this, themeBean.getFolderNameRight());
		folderNameLayout.setRightViewImage(folderNameRightDrawable);

		folderNameEditText.setTextColor(Color.parseColor(themeBean.getMainTextColor()));

		// 主界面背景皮肤
		Drawable mainLayoutBackGroung = ThemeBuilder.getBitmapAccordingScreenDensity(this, themeBean.getMainBackgroundImage());
		if (mainLayoutBackGroung != null) {
			mainLayout.setBackgroundDrawable(mainLayoutBackGroung);
		} else {
			ThemeBuilder.replaceControlsBackgroung(themeBean.getMainBackgroundColor(), mainLayout);
		}

		// 标题栏的各个皮肤
		ThemeBuilder.replaceControlsBackgroung(themeBean.getMainColor(), findViewById(R.id.titlebar_layout));

		final int textDefaultColor = Color.parseColor(themeBean.getMainTextColor());
		final int textProssedColor = Color.parseColor(themeBean.getMainTextColorHighlighted());

		TextView titleTextView = (TextView) findViewById(R.id.bookShelf_titleText_textView);
		titleTextView.setTextColor(textDefaultColor);
		publicBookstore.setTextColor(textDefaultColor);
		optionTextView.setTextColor(textDefaultColor);

		publicBookstore.setOnTouchListener(new OnTouchListener() {
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					publicBookstore.setTextColor(textProssedColor);
					break;
				case MotionEvent.ACTION_UP:
					publicBookstore.setTextColor(textDefaultColor);
					break;
				default:
					break;
				}

				return false;
			}
		});

		optionTextView.setOnTouchListener(new OnTouchListener() {
			@Override
			public boolean onTouch(View v, MotionEvent event) {
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					optionTextView.setTextColor(textProssedColor);
					break;
				case MotionEvent.ACTION_UP:
					optionTextView.setTextColor(textDefaultColor);
					break;
				default:
					break;
				}

				return false;
			}
		});

		Drawable deleteImageViewDefaultDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(BookShelfActivity.this, themeBean.getDeleteButtonDefaultInBookShelf());
		deleteImageView.setImageDrawable(deleteImageViewDefaultDrawable);
	}

	@Override
	protected void onNewIntent(Intent intent) {
		super.onNewIntent(intent);

		setThemes();
	}

	/**
	 * 关闭文件夹操作
	 */
	private void closeFolder() {
		isOpeningFolder = false;

		folderContentGridLayout.removeAllViews();// 清空文件夹内容

		View middleView = findViewById(R.id.middle_layout);

		middleView.setVisibility(View.GONE);

		AnimatorDirector.animate(new AnimatorBuilder().view(folderLayout).y(getResources().getDisplayMetrics().heightPixels + 10).setDuration(650));

		InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
		imm.hideSoftInputFromWindow(folderNameEditText.getWindowToken(), 0);

		ThemeBean themeBean = ThemeManagerSingleton.getInstance.getNewThemeModel().getThemeBean();
		Drawable folderNameRightDrawable = ThemeBuilder.getBitmapAccordingScreenDensity(BookShelfActivity.this, themeBean.getFolderNameRight());
		folderCancelButton.setBackgroundDrawable(folderNameRightDrawable);
	}

	@Override
	protected void onDestroy() {
		DebugLog.i(TAG, "onDestroy");

		super.onDestroy();
	}

	@Override
	protected void onPause() {
		DebugLog.i(TAG, "onPause");
		super.onPause();
		MobclickAgent.onPause(this);
		closeFolder();
	}

	@Override
	protected void onRestart() {
		DebugLog.i(TAG, "onRestart");
		super.onRestart();
	}

	@Override
	protected void onResume() {
		DebugLog.i(TAG, "onResume");
		super.onResume();
		MobclickAgent.onResume(this);

		setThemes();

		// 刷新网格控件。
		gridViewAdapter.changeDataSource(GlobalDataCacheForMemorySingleton.getInstance.getRootDirectory().getListFiles());
	}

	@Override
	public void onWindowFocusChanged(boolean arg0) {
		super.onWindowFocusChanged(arg0);

		if (folderLayout.getTag(R.id.last_y) == null) {
			folderLayout.setTag(R.id.last_y, folderLayout.getY());// 缓存文件夹层的初始y坐标值
		}

		/**
		 * 自动拉长书架网格长度,详情可参考DragGridLayout.onLayout()方法
		 */
		if (booksGridLayout.getChildCount() > 0) {
			// 书架网格的父布局
			ViewGroup parentGroup = (ViewGroup) booksGridLayout.getParent();

			// 书架网格中的最后一个子元素
			View lastView = booksGridLayout.getChildAt(booksGridLayout.getChildCount() - 1);

			if (lastView.getBottom() + 70 >= booksGridLayout.getBottom()) {
				LinearLayout.LayoutParams layoutParams = (LinearLayout.LayoutParams) booksGridLayout.getLayoutParams();
				layoutParams.height = lastView.getBottom() + 70;
				parentGroup.updateViewLayout(booksGridLayout, layoutParams);
			}
		}
	}

	@Override
	protected void onStart() {
		DebugLog.i(TAG, "onStart");
		super.onStart();
	}

	@Override
	protected void onStop() {
		DebugLog.i(TAG, "onStop");
		netRequestHandleForUpdate.cancel();
		super.onStop();
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {

		return true;
	}

	private void bookClickedHandler(Book book) {
		switch (book.getState()) {
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
		case kBookStateEnum_Unziping:// 解压书籍zip资源包中....
			Toast.makeText(BookShelfActivity.this, "正在解压中, 请稍等.", Toast.LENGTH_SHORT).show();
			break;
		case kBookStateEnum_Installed:// 已安装(已经解压开的书籍, 可以正常阅读了)
			openThisBookByPath(book.bookSaveDirPath());
			break;
		default:

			break;
		}
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

	/**
	 * 显示 "退出应用程序" 提示框
	 */
	private void showDialogForQuiteApp() {
		AlertDialog.Builder builder = new Builder(BookShelfActivity.this);
		builder.setTitle("提示");
		builder.setMessage("是否退出应用");
		builder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.cancel();

				// 先暂停用于异步保存LocalBook的线程
				GlobalDataCacheForNeedSaveToFileSystem.stopSaveLocalBookThread();

				// 这里需要先将所有正在下载的书籍暂停，然后才能退出应用
				List<Book> books = GlobalDataCacheForMemorySingleton.getInstance.getLocalBookList().getShallowCopyBookList();
				for (Book book : books) {
					book.stopDownloadBook();
				}
				CircleProgressObservable.INSTANCE.stop();
				// 停止一个服务
				Intent intent = new Intent(BookShelfActivity.this, UpdateSoftwareService.class);
				// 采用 startService() 启动的服务, 必须主动调用 stopService()来主动停止
				stopService(intent);
				// 退出应用
				ToolsFunctionForThisProgect.quitApp(BookShelfActivity.this);
			}
		});
		builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.cancel();
			}
		});
		AlertDialog dialog = builder.create();
		dialog.show();
	}

	public void onClickForNothing(View view) {
		// 阻隔点击事件
	}

	private void checkSoftwareUpdate() {
		if (ToolsFunctionForThisProgect.isServiceRunning(BookShelfActivity.this, UpdateSoftwareService.class.getName().toString())) {
			return;
		}
		CheckSoftwareUpdateRequestBean netRequestDomainBean = new CheckSoftwareUpdateRequestBean();
		netRequestHandleForUpdate = SimpleNetworkEngineSingleton.getInstance.requestDomainBean(netRequestDomainBean, new IDomainBeanAsyncHttpResponseListener() {

			@Override
			public void onSuccess(Object respondDomainBean) {
				if (respondDomainBean != null) {
					final CheckSoftwareUpdateRespondBean respondBean = (CheckSoftwareUpdateRespondBean) respondDomainBean;
					do {
						// 判断是否升级
						if (!respondBean.isUpdate()) {
							break;
						}
						// 判断版本是否更新
						if (respondBean.getVersion().equals(GlobalDataCacheForMemorySingleton.getInstance.getVersionName())) {
							break;
						}
						// 表示需要验证
						final File file = new File(getApplicationContext().getFilesDir() + "/DreamBook_update.apk");
						if (!file.exists()) {
							// 创建对话框
							AlertDialog.Builder builder = new Builder(BookShelfActivity.this);
							builder.setTitle("版本升级提示");
							builder.setMessage("服务器有新版本是否升级版本");
							builder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog, int which) {
									dialog.cancel();
									// 启动一个服务, 用于预加载相关数据
									Intent intent = new Intent(BookShelfActivity.this, UpdateSoftwareService.class);
									intent.putExtra(UpdateSoftwareService.TRACK_VIEW_URL, respondBean.getTrackViewUrl());
									intent.putExtra(UpdateSoftwareService.MD5, respondBean.getmD5String());

									// 采用 startService() 启动的服务, 必须主动调用 stopService()
									// 来主动停止
									startService(intent);
								}
							});
							builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog, int which) {
									dialog.cancel();
								}
							});
							AlertDialog dialog = builder.create();
							dialog.show();
						} else {
							String fileMd5String = MD5Util.md5sum(getApplicationContext().getFilesDir() + "/DreamBook_update.apk");
							// 这里使用equalsIgnoreCase是因为使用MD5工具类转出来的MD5码是大写
							// 如果使用equals则会区分大小写,equalsIgnoreCase是不区分大小写的
							if (!respondBean.getmD5String().equalsIgnoreCase(fileMd5String)) {
								// 创建对话框
								AlertDialog.Builder builder = new Builder(BookShelfActivity.this);
								builder.setTitle("文件MD5码校验错误");
								builder.setMessage("请点击确定重新下载文件");
								builder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
									@Override
									public void onClick(DialogInterface dialog, int which) {
										dialog.cancel();
										// 启动一个服务, 用于预加载相关数据
										Intent intent = new Intent(BookShelfActivity.this, UpdateSoftwareService.class);
										intent.putExtra(UpdateSoftwareService.TRACK_VIEW_URL, respondBean.getTrackViewUrl());
										intent.putExtra(UpdateSoftwareService.MD5, respondBean.getmD5String());

										// 采用 startService() 启动的服务, 必须主动调用 stopService()
										// 来主动停止
										startService(intent);
									}
								});
								builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
									@Override
									public void onClick(DialogInterface dialog, int which) {
										dialog.cancel();
									}
								});
								AlertDialog dialog = builder.create();
								dialog.getWindow().setType((WindowManager.LayoutParams.TYPE_SYSTEM_ALERT));
								dialog.show();
								return;
							} else {
								// TODO 安装本地版本
								// 创建对话框
								AlertDialog.Builder newBuilder = new Builder(BookShelfActivity.this);
								newBuilder.setTitle("本地有新版本");
								newBuilder.setMessage("是否安装新版本");
								newBuilder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
									@Override
									public void onClick(DialogInterface dialog, int which) {
										// 下载完成点击通知进入安装
										Uri uri = Uri.fromFile(new File(getApplicationContext().getFilesDir() + "/DreamBook_update.apk"));
										Intent installIntent = new Intent(Intent.ACTION_VIEW);
										installIntent.setDataAndType(uri, "application/vnd.android.package-archive");
										String[] args1 = { "chmod", "705", getApplicationContext().getFilesDir().toString() };
										UpdateSoftwareService.exec(args1);
										// [文件604:-rw----r--]
										String[] args2 = { "chmod", "604", getApplicationContext().getFilesDir() + "/DreamBook_update.apk" };
										UpdateSoftwareService.exec(args2);
										// 这里必须要加Context中有一个startActivity方法，Activity继承自Context，
										// 重载了startActivity方法。如果使用Activity的startActivity方法，不会有任何限制，
										// 而如果使用Context的startActivity方法的话，就需要开启一个新的task，遇到上面那个异常的，都是因为使用了Context的startActivity方法。解决办法是，加一个flag。
										installIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
										startActivity(installIntent);
									}
								});
								newBuilder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
									@Override
									public void onClick(DialogInterface dialog, int which) {
										dialog.cancel();
									}
								});
								AlertDialog newDialog = newBuilder.create();
								newDialog.getWindow().setType((WindowManager.LayoutParams.TYPE_SYSTEM_ALERT));
								newDialog.show();
							}
						}

					} while (false);

				}

			}

			@Override
			public void onFailure(MyNetRequestErrorBean error) {
				Toast.makeText(BookShelfActivity.this, error.getErrorMessage(), Toast.LENGTH_SHORT).show();

			}
		});

	}

	/**
	 * 跳转到 "书城" 界面
	 * 
	 * @param username
	 * @param password
	 */
	private void gotoBookStoreActivity(String username, String password) {
		// 企业用户登录成功
		LoginNetRespondBean privateAccountLogonNetRespondBean = new LoginNetRespondBean();
		privateAccountLogonNetRespondBean.setName(username);
		privateAccountLogonNetRespondBean.setPassword(password);
		GlobalDataCacheForMemorySingleton.getInstance.noteSignInSuccessfulInfo(privateAccountLogonNetRespondBean, username, password);
		Intent intent = new Intent(this, BookStoreActivity.class);
		BookShelfActivity.this.startActivity(intent);
	}
}
